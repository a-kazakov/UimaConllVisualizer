#include "uima/api.hpp"

#include <iostream>
#include <memory>

std::string prompt(const std::string &message) {
    std::string result;
    std::cout << message << " ";
    std::getline(std::cin, result);
    return result;
}

int main(int argc, char *argv[]) {
    static const icu::UnicodeString READER_DESCRIPTOR_XML = icu::UnicodeString::fromUTF8("ConllReader.xml");
    static const icu::UnicodeString VISUALIZER_DESCRIPTOR_XML = icu::UnicodeString::fromUTF8("Visualizer.xml");

    (void) uima::ResourceManager::createInstance("test");

    uima::TyErrorId utErrorId; // Variable to store return codes
    uima::ErrorInfo errorInfo;

    std::string filename = argc > 1
        ? argv[1]
        : prompt("Enter filename:");

    uima::NameValuePair fn_pair;
    uima::AnalysisEngineDescription engineDescription;
    uima::TextAnalysisEngineSpecifierBuilder builder;
    fn_pair.define(uima::ConfigurationParameter::STRING, uima::ConfigurationParameter::SINGLE_VALUE);
    fn_pair.setName(icu::UnicodeString::fromUTF8("FileName"));
    fn_pair.setValue(icu::UnicodeString::fromUTF8(filename));
    builder.buildTaeFromFile(engineDescription, READER_DESCRIPTOR_XML);
    engineDescription.setNameValuePair(fn_pair);

    uima::AnalysisEngine *pEngine = uima::Framework::createAnalysisEngine(engineDescription, errorInfo);
    uima::AnalysisEngine *sEngine = uima::Framework::createAnalysisEngine("Visualizer.xml", errorInfo);

    std::cerr << errorInfo.asString() << std::endl;

    uima::CAS *cas = pEngine->newCAS();
    pEngine->process(*cas);
    sEngine->process(*cas);

    return 0;
}
