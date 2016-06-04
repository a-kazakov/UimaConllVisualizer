#include "Visualizer.h"

#include <unordered_map>

uima::TyErrorId Visualizer::process(uima::CAS &cas,
                                    uima::ResultSpecification const &crResultSpecification) {
    size_t latest_pos = 0;
    icu::UnicodeString content;
    icu::UnicodeString document_text = dereferenceString(cas.getDocumentText());
    std::set<std::string> upostags;
    for (auto it = cas.getAnnotationIndex(_t_conll).iterator(); it.isValid(); it.moveToNext()) {
        const auto &fs = it.get();
        if (latest_pos < fs.getBeginPosition()) {
            content += document_text.tempSubStringBetween(latest_pos, fs.getBeginPosition());
        }
        content += makeTag(fs);
        latest_pos = fs.getEndPosition();
        // Process UPosTag
        std::string upostag;
        dereferenceString(fs.getStringValue(_f_upostag)).toUTF8String(upostag);
        upostags.insert(upostag);
    }
    saveResult(content, makeControlsSet(upostags));
    return (uima::TyErrorId)UIMA_ERR_NONE;
}

uima::TyErrorId Visualizer::initialize(uima::AnnotatorContext &rclAnnotatorContext) {
    rclAnnotatorContext.extractValue(icu::UnicodeString::fromUTF8("FileName"), _filename);
    return (uima::TyErrorId)UIMA_ERR_NONE;
}

uima::TyErrorId Visualizer::typeSystemInit(uima::TypeSystem const &crTypeSystem) {
    _t_conll    = crTypeSystem.getType("net.akazakov.ConllAnnotation");
    _t_keyvalue = crTypeSystem.getType("net.akazakov.KeyValue");
    _f_form     = _t_conll.getFeatureByBaseName("Form");
    _f_lemma    = _t_conll.getFeatureByBaseName("Lemma");
    _f_upostag  = _t_conll.getFeatureByBaseName("UPosTag");
    _f_xpostag  = _t_conll.getFeatureByBaseName("XPosTag");
    _f_feats    = _t_conll.getFeatureByBaseName("Feats");
    _f_misc     = _t_conll.getFeatureByBaseName("Misc");
    _f_children = _t_conll.getFeatureByBaseName("Children");
    _f_key      = _t_keyvalue.getFeatureByBaseName("Key");
    _f_value    = _t_keyvalue.getFeatureByBaseName("Value");
    return (uima::TyErrorId)UIMA_ERR_NONE;
}

icu::UnicodeString Visualizer::makeCardContent(const uima::FeatureStructure &fs) {
    // Definitions
    static const icu::UnicodeString PART1 = "<div class=\"subcard\">";
    static const icu::UnicodeString PART2 = "</div>";
    // Implementation
    icu::UnicodeString result = "<table>";
    result += makeStringCardRow("Word form", fs, _f_form);
    result += makeStringCardRow("Lemma of word form", fs, _f_lemma);
    result += makeStringCardRow("Universal part-of-speech tag", fs, _f_upostag);
    result += makeStringCardRow("Language-specific part-of-speech tag", fs, _f_xpostag);
    result += makeKeyValueCardBlock("Morphological features", fs, _f_feats);
    result += makeKeyValueCardBlock("Other annotations", fs, _f_misc);
    result += "</table>";
    uima::ArrayFS children = fs.getArrayFSValue(_f_children);
    for (size_t i = 0; i < children.size(); ++i) {
        result += PART1 + makeCardContent(children.get(i)) + PART2;
    }
    return result;
}

icu::UnicodeString Visualizer::makeStringCardRow(const icu::UnicodeString &caption,
                                                 const uima::FeatureStructure &fs,
                                                 const uima::Feature &feature) {
    // Definitions
    static const icu::UnicodeString EMPTY = "_";
    static const icu::UnicodeString PART1 = "<tr><th>";
    static const icu::UnicodeString PART2 = "</th><td>";
    static const icu::UnicodeString PART3 = "</td></tr>";
    // Implementation
    auto value_ref = fs.getStringValue(feature);
    if (value_ref == EMPTY) {
        return "";
    }
    return PART1 + caption + PART2 + dereferenceString(value_ref) + PART3;
}

icu::UnicodeString Visualizer::makeKeyValueCardBlock(const icu::UnicodeString &caption,
                                                     const uima::FeatureStructure &fs,
                                                     const uima::Feature &feature) {
    // Definitions
    static const icu::UnicodeString HEADER1 = "<tr><th class=\"title\" colspan=\"2\">";
    static const icu::UnicodeString HEADER2 = "</th</tr>";
    static const icu::UnicodeString PART1 = "<tr><th>";
    static const icu::UnicodeString PART2 = "</th><td>";
    static const icu::UnicodeString PART3 = "</td></tr>";
    // Implementation
    uima::ArrayFS arr = fs.getArrayFSValue(feature);
    if (arr.size() == 0) {
        return "";
    }
    icu::UnicodeString result = HEADER1 + caption + HEADER2;
    for (size_t i = 0; i < arr.size(); ++i) {
        uima::FeatureStructure element = arr.get(i);
        result += PART1 + dereferenceString(element.getStringValue(_f_key)) +
                  PART2 + dereferenceString(element.getStringValue(_f_value)) +
                  PART3;
    }
    return result;
}

icu::UnicodeString Visualizer::makeTag(const uima::AnnotationFS &fs) {
    // Definitions
    static const icu::UnicodeString PART1 = "<span class=\"a a-";
    static const icu::UnicodeString PART2 = "\">";
    static const icu::UnicodeString PART3 = "<div class=\"card\">";
    static const icu::UnicodeString PART4 = "</div></span>";
    // Implementation
    icu::UnicodeString result;
    UErrorCode err;
    return PART1 + dereferenceString(fs.getStringValue(_f_upostag)) +
           PART2 + dereferenceString(fs.getCoveredText()) +
           PART3 + makeCardContent(fs) +
           PART4;
}

icu::UnicodeString Visualizer::makeControlsSet(const std::set<std::string> &values) {
    icu::UnicodeString result;
    for (const auto &value : values) {
        result += makeControl(value);
    }
    return result;
}

icu::UnicodeString Visualizer::makeControl(const std::string &value) {
    // Definitions
    static const std::unordered_map<std::string, icu::UnicodeString> ALIASES {
        std::make_pair<std::string, icu::UnicodeString>("_"     , "Untyped / multiple types"),
        std::make_pair<std::string, icu::UnicodeString>("ADJ"   , "Adjectives"),
        std::make_pair<std::string, icu::UnicodeString>("ADP"   , "Adpositions"),
        std::make_pair<std::string, icu::UnicodeString>("ADV"   , "Adverbs"),
        std::make_pair<std::string, icu::UnicodeString>("AUX"   , "Auxiliary verbs"),
        std::make_pair<std::string, icu::UnicodeString>("CONJ"  , "Coordinating conjunctions"),
        std::make_pair<std::string, icu::UnicodeString>("DET"   , "Determiners"),
        std::make_pair<std::string, icu::UnicodeString>("INTJ"  , "Interjections"),
        std::make_pair<std::string, icu::UnicodeString>("NOUN"  , "Nouns"),
        std::make_pair<std::string, icu::UnicodeString>("NUM"   , "Numerals"),
        std::make_pair<std::string, icu::UnicodeString>("PART"  , "Particles"),
        std::make_pair<std::string, icu::UnicodeString>("PRON"  , "Pronouns"),
        std::make_pair<std::string, icu::UnicodeString>("PROPN" , "Proper nouns"),
        std::make_pair<std::string, icu::UnicodeString>("PUNCT" , "Punctuation"),
        std::make_pair<std::string, icu::UnicodeString>("SCONJ" , "Subordinating conjunctions"),
        std::make_pair<std::string, icu::UnicodeString>("SYM"   , "Symbols"),
        std::make_pair<std::string, icu::UnicodeString>("VERB"  , "Verbs"),
        std::make_pair<std::string, icu::UnicodeString>("X"     , "Others")
    };
    static const icu::UnicodeString PART1 = "<label><input type=\"checkbox\" checked class=\"control-cb\" data-value=\"";
    static const icu::UnicodeString PART2 = "\">";
    static const icu::UnicodeString PART3 = "</label><br>\n";
    // Implementation
    icu::UnicodeString label;
    if (ALIASES.count(value) == 0) {
        label = icu::UnicodeString::fromUTF8(value);
    } else {
        label = ALIASES.at(value);
    }
    return PART1 + icu::UnicodeString::fromUTF8(value) + PART2 + label + PART3;
}

void Visualizer::saveResult(const icu::UnicodeString &content,
                            const icu::UnicodeString &controls) {
    std::string buf, output;
    std::ofstream out_stream(_filename);
    std::ifstream template_stream("_visualizer_template.html");
    while (std::getline(template_stream, buf)) {
        if (buf == "#content") {
            out_stream << content.toUTF8String(output);
            output.clear();
        } else if (buf == "#controls") {
            out_stream << controls.toUTF8String(output);
            output.clear();
        } else {
            out_stream << buf;
        }
        out_stream << "\n";
    }
}

icu::UnicodeString Visualizer::dereferenceString(const uima::UnicodeStringRef &ref) {
    return icu::UnicodeString(ref.getBuffer(), ref.length());
}
