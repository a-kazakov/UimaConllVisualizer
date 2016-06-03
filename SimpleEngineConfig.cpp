#include "SimpleEngineConfig.h"


SimpleEngineConfig::SimpleEngineConfig(const icu::UnicodeString &base_xml)
    : _xml_filename(base_xml)
{}


void SimpleEngineConfig::addStringParameter(const icu::UnicodeString &key,
                                            const icu::UnicodeString &value) {
    _data.push_back(std::make_pair(key, value));
}

uima::AnalysisEngineDescription &SimpleEngineConfig::getConfig() const {
    _result.reset(new uima::AnalysisEngineDescription);
    uima::TextAnalysisEngineSpecifierBuilder builder;
    builder.buildTaeFromFile(*_result, _xml_filename);
    for (const auto &p : _data) {
        uima::NameValuePair fn_pair;
        fn_pair.define(uima::ConfigurationParameter::STRING, uima::ConfigurationParameter::SINGLE_VALUE);
        fn_pair.setName(p.first);
        fn_pair.setValue(p.second);
        _result->setNameValuePair(fn_pair);
    }
    return *_result;
}
