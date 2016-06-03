#include "uima/api.hpp"

#include <iostream>
#include <memory>

#include "SimpleEngineConfig.h"


std::string prompt(const std::string &message) {
    std::string result;
    std::cout << message << " ";
    std::getline(std::cin, result);
    return result;
}

std::string getArgument(int argc, char *argv[], int arg_pos, const std::string &prompt_message) {
    return argc > arg_pos
        ? argv[arg_pos]
        : prompt(prompt_message);
}

int main(int argc, char *argv[]) {
    static const icu::UnicodeString READER_DESCRIPTOR_XML = icu::UnicodeString::fromUTF8("ConllReader.xml");
    static const icu::UnicodeString VISUALIZER_DESCRIPTOR_XML = icu::UnicodeString::fromUTF8("Visualizer.xml");

    (void) uima::ResourceManager::createInstance("test");

    uima::TyErrorId utErrorId; // Variable to store return codes
    uima::ErrorInfo errorInfo;

    std::string input_filename = getArgument(argc, argv, 1, "Enter input file name:");
    std::string output_filename = getArgument(argc, argv, 2, "Enter output file name:");

    SimpleEngineConfig readEngineConfig(READER_DESCRIPTOR_XML);
    readEngineConfig.addStringParameter("FileName", input_filename.c_str());

    SimpleEngineConfig visEngineConfig(VISUALIZER_DESCRIPTOR_XML);
    visEngineConfig.addStringParameter("FileName", output_filename.c_str());

    uima::AnalysisEngine *readEngine = uima::Framework::createAnalysisEngine(readEngineConfig.getConfig(), errorInfo);
    uima::AnalysisEngine *visEngine = uima::Framework::createAnalysisEngine(visEngineConfig.getConfig(), errorInfo);

    uima::CAS *cas = readEngine->newCAS();

    readEngine->process(*cas);
    visEngine->process(*cas);

    std::cerr << "Successfully saved result to " << output_filename << std::endl;

    return 0;
}
