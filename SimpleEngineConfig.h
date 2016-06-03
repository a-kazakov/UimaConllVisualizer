#pragma once

#include "uima/api.hpp"

class SimpleEngineConfig {
public:
    SimpleEngineConfig(
        const icu::UnicodeString &base_xml);

    void addStringParameter(
        const icu::UnicodeString &key,
        const icu::UnicodeString &value);

    uima::AnalysisEngineDescription &getConfig() const;

private:
    std::vector<std::pair<icu::UnicodeString, icu::UnicodeString>> _data;
    const icu::UnicodeString _xml_filename;
    mutable std::shared_ptr<uima::AnalysisEngineDescription> _result;
};
