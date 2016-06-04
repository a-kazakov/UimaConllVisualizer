#include "ConllReader.h"

#include <fstream>

uima::TyErrorId ConllReader::process(uima::CAS &cas,
                                     uima::ResultSpecification const &crResultSpecification) {
    // Initialization
    _stream.seekg(0);
    _eof = false;
    // Reading
    std::vector<std::vector<ConllAnnotation>> sentences;
    while (!_eof) {
        sentences.push_back(fetchSentence());
        if (sentences.back().empty()) {
            sentences.pop_back();
        }
    }
    // Exporting
    cas.setDocumentText(restoreDocumentText(sentences));
    for (const auto &sentence : sentences) {
        addSentenceToCAS(sentence, cas);
    }
    return (uima::TyErrorId)UIMA_ERR_NONE;
}

uima::TyErrorId ConllReader::initialize(uima::AnnotatorContext &rclAnnotatorContext) {
    rclAnnotatorContext.extractValue(icu::UnicodeString::fromUTF8("FileName"), _filename);
    _stream.open(_filename);
    return (uima::TyErrorId)UIMA_ERR_NONE;
}

uima::TyErrorId ConllReader::typeSystemInit(uima::TypeSystem const &crTypeSystem){
    _t_keyvalue = crTypeSystem.getType("net.akazakov.KeyValue");
    _t_conll    = crTypeSystem.getType("net.akazakov.ConllAnnotation");
    _f_key      = _t_keyvalue.getFeatureByBaseName("Key");
    _f_value    = _t_keyvalue.getFeatureByBaseName("Value");
    _f_form     = _t_conll.getFeatureByBaseName("Form");
    _f_lemma    = _t_conll.getFeatureByBaseName("Lemma");
    _f_upostag  = _t_conll.getFeatureByBaseName("UPosTag");
    _f_xpostag  = _t_conll.getFeatureByBaseName("XPosTag");
    _f_feats    = _t_conll.getFeatureByBaseName("Feats");
    _f_misc     = _t_conll.getFeatureByBaseName("Misc");
    _f_children = _t_conll.getFeatureByBaseName("Children");
    return (uima::TyErrorId)UIMA_ERR_NONE;
}

std::vector<ConllReader::ConllAnnotation> ConllReader::fetchSentence() {
    std::vector <ConllAnnotation> result;
    ConllAnnotation buf;
    int skip_till_id = -1;
    while (true) {
        try {
            fetchLine(buf);
            if (skip_till_id < buf.id) {
                result.push_back(buf);
                if (buf.last_id >= 0) {
                    skip_till_id = buf.last_id;
                }
            } else {
                result.back().children.push_back(buf);
            }
        }
        catch (EndOfSentence &) {
            return result;
        }
        catch (EndOfFile &) {
            _eof = true;
            return result;
        }
    }
}

void ConllReader::fetchLine(ConllAnnotation &dest) {
    std::string raw_line;
    if (!std::getline(_stream, raw_line)) {
        throw EndOfFile();
    }
    icu::UnicodeString line = icu::UnicodeString::fromUTF8(raw_line);
    if (line.length() == 0 || line[0] == '#') {
        throw EndOfSentence();
    }
    std::vector<icu::UnicodeString> chunks = splitString(line, '\t');
    if (chunks.size() != 10) {
        throw BadFormat();
    }
    readId(dest.id, dest.last_id, chunks[0]);
    dest.form       = chunks[1];
    dest.lemma      = chunks[2];
    dest.upostag    = chunks[3];
    dest.xpostag    = chunks[4];
    readStringMap(dest.feats, chunks[5]);
    readStringMap(dest.misc, chunks[9]);
}

icu::UnicodeString ConllReader::restoreDocumentText(std::vector<std::vector<ConllAnnotation>> &sentences) {
    icu::UnicodeString result;
    icu::UnicodeString SPACE = icu::UnicodeString::fromUTF8(" ");
    for (auto &sentence_annotations : sentences) {
        for (auto &annotation : sentence_annotations) {
            annotation.start_pos = result.length();
            annotation.end_pos = result.length() + annotation.form.length();
            result += annotation.form;
            if (!annotation.misc.count("SpaceAfter") || annotation.misc["SpaceAfter"] != "No") {
                result += SPACE;
            }
        }
    }
    return result;
}

void ConllReader::fillFeatureStructure(uima::FeatureStructure &fs,
                                       const ConllReader::ConllAnnotation &annotation,
                                       uima::CAS &cas) {

    fs.setStringValue(_f_form, annotation.form);
    fs.setStringValue(_f_lemma, annotation.lemma);
    fs.setStringValue(_f_upostag, annotation.upostag);
    fs.setStringValue(_f_xpostag, annotation.xpostag);
    setStringMapValue(fs, _f_feats, annotation.feats, cas);
    setStringMapValue(fs, _f_misc, annotation.misc, cas);
    setConllArrayValue(fs, _f_children, annotation.children, cas);
}

void ConllReader::setStringMapValue(uima::FeatureStructure &fs,
                                    const uima::Feature &feature,
                                    const std::unordered_map<std::string, icu::UnicodeString> &value,
                                    uima::CAS &cas) {
    uima::ArrayFS feats_arr = cas.createArrayFS(value.size());
    size_t idx = 0;
    for (const auto &p : value) {
        uima::FeatureStructure element = cas.createFS(_t_keyvalue);
        element.setStringValue(_f_key, p.first.c_str());
        element.setStringValue(_f_value, p.second);
        feats_arr.set(idx++, element);
    }
    fs.setFSValue(feature, feats_arr);
}

void ConllReader::setConllArrayValue(uima::FeatureStructure &fs,
                                     const uima::Feature &feature,
                                     const std::vector<ConllAnnotation> &value,
                                     uima::CAS &cas) {
    uima::ArrayFS arr = cas.createArrayFS(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        uima::FeatureStructure element = cas.createFS(_t_conll);
        fillFeatureStructure(element, value[i], cas);
        arr.set(i, element);
    }
    fs.setFSValue(feature, arr);
}

void ConllReader::addSentenceToCAS(const std::vector<ConllAnnotation> &annotations, uima::CAS &cas) {
    for (const auto &annotation : annotations) {
        uima::AnnotationFS fs = cas.createAnnotation(_t_conll, annotation.start_pos, annotation.end_pos);
        fillFeatureStructure(fs, annotation, cas);
        cas.getIndexRepository().addFS(fs);
    }
}

void ConllReader::readId(int &dest_id,
                         int &dest_last_id,
                         const icu::UnicodeString &value) {
    const std::vector<icu::UnicodeString> temp = splitString(value, '-');
    if (temp.size() == 1) {
        dest_id = strToInt(temp[0]);
        dest_last_id = -1;
    } else if (temp.size() == 2) {
        dest_id = strToInt(temp[0]);
        dest_last_id = strToInt(temp[1]);
    } else {
        throw BadFormat();
    }
}

void ConllReader::readStringMap(std::unordered_map<std::string, icu::UnicodeString> &dest,
                                const icu::UnicodeString &value) {
    dest.clear();
    if (value == "_") {
        return;
    }
    const std::vector<icu::UnicodeString> chunks = splitString(value, '|');
    for (const icu::UnicodeString &chunk : chunks) {
        std::vector<icu::UnicodeString> pair = splitString(chunk, '=');
        if (pair.size() != 2) {
            throw BadFormat();
        }
        std::string tmp;
        dest[pair[0].toUTF8String(tmp)] = pair[1];
    }
}

std::vector<icu::UnicodeString> ConllReader::splitString(const icu::UnicodeString &str,
                                                         UChar delim) {
    std::vector<icu::UnicodeString> result;
    int latest_idx = 0;
    for (int idx = 0; idx < str.length(); ++idx) {
        if (str[idx] == delim) {
            result.push_back(str.tempSubStringBetween(latest_idx, idx));
            latest_idx = idx + 1;
        }
    }
    result.push_back(str.tempSubStringBetween(latest_idx));
    return result;
}

int ConllReader::strToInt(const icu::UnicodeString &value) {
    int result = 0;
    for (int i = 0; i < value.length(); ++i) {
        if (!('0' <= value[i] && value[i] <= '9')) {
            throw BadFormat();
        }
        result *= 10;
        result += value[i] - '0';
    }
    return result;
}

