#pragma once

#include "uima/api.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class ConllReader : public uima::Annotator {
private:
    // Logic variables

    bool _eof;
    std::string _filename;
    std::ifstream _stream;

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

    // Annotation structure

    struct ConllAnnotation {
        int start_pos, end_pos;
        int id;
        int last_id;
        icu::UnicodeString form;
        icu::UnicodeString lemma;
        icu::UnicodeString upostag;
        icu::UnicodeString xpostag;
        std::unordered_map<std::string, icu::UnicodeString> feats;
        std::unordered_map<std::string, icu::UnicodeString> misc;
        std::vector<ConllAnnotation> children;
    };

    // Exceptions

    class BadFormat : public std::exception {
    public:
        const char *what() {
            return "Bad file format";
        }
    };

    class EndOfFile : public std::exception {
    public:
        const char *what() {
            return "End of file reached";
        }
    };

    class EndOfSentence : public std::exception {
    public:
        const char *what() {
            return "End of sentence reached";
        }
    };

    // Processors

    std::vector<ConllAnnotation> fetchSentence();

    void fetchLine(
        ConllAnnotation &dest);

    icu::UnicodeString restoreDocumentText(
        std::vector<std::vector<ConllAnnotation>> &sentences);

    void fillFeatureStructure(
        uima::FeatureStructure &fs,
        const ConllAnnotation &annotation,
        uima::CAS &cas);

    void setStringMapValue(
        uima::FeatureStructure &fs,
        const uima::Feature &feature,
        const std::unordered_map<std::string, icu::UnicodeString> &value,
        uima::CAS &cas);

    void setConllArrayValue(
        uima::FeatureStructure &fs,
        const uima::Feature &feature,
        const std::vector<ConllAnnotation> &value,
        uima::CAS &cas);

    void addSentenceToCAS(
        const std::vector<ConllAnnotation> &annotations,
        uima::CAS &cas);

    // Value parsers

    void readId(
        int &dest_id,
        int &dest_last_id,
        const icu::UnicodeString &value);

    void readStringMap(
        std::unordered_map<std::string, icu::UnicodeString> &dest,
        const icu::UnicodeString &value);

    // Helpers

    static std::vector<icu::UnicodeString> splitString(
        const icu::UnicodeString &str,
        UChar delim);

    static int strToInt(
        const icu::UnicodeString &value);

public:
    ConllReader() = default;

    uima::TyErrorId destroy() override {
        return (uima::TyErrorId)UIMA_ERR_NONE;
    }

    uima::TyErrorId initialize(
        uima::AnnotatorContext &rclAnnotatorContext) override;

    uima::TyErrorId typeSystemInit(
        uima::TypeSystem const & crTypeSystem) override;

    uima::TyErrorId process(
        uima::CAS &cas,
        uima::ResultSpecification const &crResultSpecification) override;

};

using uima::Annotator;
MAKE_AE(ConllReader);
