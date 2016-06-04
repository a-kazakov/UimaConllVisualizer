#include "uima/api.hpp"

#include <set>

class Visualizer : public uima::Annotator {
private:
    // Logic variables

    std::string _filename;

    // UIMA type system support

    uima::Type _t_conll;
    uima::Type _t_keyvalue;
    uima::Feature _f_form;
    uima::Feature _f_lemma;
    uima::Feature _f_upostag;
    uima::Feature _f_xpostag;
    uima::Feature _f_feats;
    uima::Feature _f_misc;
    uima::Feature _f_children;
    uima::Feature _f_key;
    uima::Feature _f_value;

    // Processors

    icu::UnicodeString makeCardContent(
        const uima::FeatureStructure &fs);

    icu::UnicodeString makeStringCardRow(
        const icu::UnicodeString &caption,
        const uima::FeatureStructure &fs,
        const uima::Feature &feature);

    icu::UnicodeString makeKeyValueCardBlock(
        const icu::UnicodeString &caption,
        const uima::FeatureStructure &fs,
        const uima::Feature &feature);

    icu::UnicodeString makeTag(
        const uima::AnnotationFS &fs);

    icu::UnicodeString makeControlsSet(
        const std::set<std::string> &values);

    icu::UnicodeString makeControl(
        const std::string &value);

    // Helpers

    void saveResult(
        const icu::UnicodeString &content,
        const icu::UnicodeString &controls);

    static icu::UnicodeString dereferenceString(
        const uima::UnicodeStringRef &ref);

public:
    Visualizer() = default;

    uima::TyErrorId destroy() override {
        return (uima::TyErrorId)UIMA_ERR_NONE;
    }

    uima::TyErrorId initialize(
        uima::AnnotatorContext &rclAnnotatorContext) override;

    uima::TyErrorId typeSystemInit(
        uima::TypeSystem const &crTypeSystem) override;

    uima::TyErrorId process(
        uima::CAS & cas,
        uima::ResultSpecification const &crResultSpecification) override;

};

using uima::Annotator;
MAKE_AE(Visualizer);
