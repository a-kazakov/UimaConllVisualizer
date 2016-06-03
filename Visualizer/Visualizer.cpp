#include "Visualizer.h"

icu::UnicodeString Visualizer::dereferenceString(const uima::UnicodeStringRef &ref) {
    return icu::UnicodeString(ref.getBuffer(), ref.length());
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

icu::UnicodeString Visualizer::makeCardContent(const uima::FeatureStructure &fs) {
    // Definitions
    static const icu::UnicodeString PART1 = "<div class=\"subcard\">";
    static const icu::UnicodeString PART2 = "</div>";
    // Implementation
    icu::UnicodeString result = "<table>";
    result += makeStringCardRow("Form",    fs, _f_form);
    result += makeStringCardRow("Lemma",   fs, _f_lemma);
    result += makeStringCardRow("UPosTag", fs, _f_upostag);
    result += makeStringCardRow("XPosTag", fs, _f_xpostag);
    result += makeKeyValueCardBlock("Feats", fs, _f_feats);
    result += "</table>";
    uima::ArrayFS children = fs.getArrayFSValue(_f_children);
    for (size_t i = 0; i < children.size(); ++i) {
        result += PART1 + makeCardContent(children.get(i)) + PART2;
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

void Visualizer::saveResult(const icu::UnicodeString &result) {
    std::string buf, output;
    std::ofstream out_stream(_filename);
    std::ifstream template_stream("_visualizer_template.html");
    while (std::getline(template_stream, buf)) {
        if (buf == "#content") {
            out_stream << result.toUTF8String(output);
        } else {
            out_stream << buf;
        }
        out_stream << "\n";
    }
}

uima::TyErrorId Visualizer::process(uima::CAS &cas, uima::ResultSpecification const &crResultSpecification) {
    size_t latest_pos = 0;
    icu::UnicodeString result;
    icu::UnicodeString document_text = dereferenceString(cas.getDocumentText());
    for (auto it = cas.getAnnotationIndex(_t_conll).iterator(); it.isValid(); it.moveToNext()) {
        const auto &fs = it.get();
        if (latest_pos < fs.getBeginPosition()) {
            result += document_text.tempSubStringBetween(latest_pos, fs.getBeginPosition());
        }
        result += makeTag(fs);
        latest_pos = fs.getEndPosition();
    }
    saveResult(result);
    return (uima::TyErrorId)UIMA_ERR_NONE;
}

uima::TyErrorId Visualizer::initialize(uima::AnnotatorContext &rclAnnotatorContext) {
    rclAnnotatorContext.extractValue(icu::UnicodeString::fromUTF8("FileName"), _filename);
    return (uima::TyErrorId)UIMA_ERR_NONE;
}

uima::TyErrorId Visualizer::typeSystemInit(uima::TypeSystem const &crTypeSystem){
    _t_conll    = crTypeSystem.getType("net.akazakov.ConllAnnotation");
    _t_keyvalue = crTypeSystem.getType("net.akazakov.KeyValue");
    _f_form     = _t_conll.getFeatureByBaseName("Form");
    _f_lemma    = _t_conll.getFeatureByBaseName("Lemma");
    _f_upostag  = _t_conll.getFeatureByBaseName("UPosTag");
    _f_xpostag  = _t_conll.getFeatureByBaseName("XPosTag");
    _f_feats    = _t_conll.getFeatureByBaseName("Feats");
    _f_children = _t_conll.getFeatureByBaseName("Children");
    _f_key      = _t_keyvalue.getFeatureByBaseName("Key");
    _f_value    = _t_keyvalue.getFeatureByBaseName("Value");
    return (uima::TyErrorId)UIMA_ERR_NONE;
}
