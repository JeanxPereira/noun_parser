#include "parser.h"
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

struct ProgramOptions {
    std::string inputFile;
    std::string outputFile;
    bool showHelp = false;
};

std::vector<uint8_t> readBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    
    return buffer;
}

void printHelp(const char* programName) {
    std::cout << "Darkspore Noun File Parser [EXPERIMENTAL]\n\n"
              << "Usage: " << programName << " <input_file> [options]\n\n"
              << "Options:\n"
              << "  -h, --help     Display this help message\n"
              << "  -o <file>      Specify output file (default: same as input with .xml extension)\n\n"
              << "Example:\n"
              << "  " << programName << " input.noun.bin -o output.xml\n"
              << std::endl;
}

std::string getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of(".");
    return pos != std::string::npos ? filename.substr(pos) : "";
}

std::string getFileNameWithoutExtension(const std::string& filename) {
    size_t pos = filename.find_last_of(".");
    return pos != std::string::npos ? filename.substr(0, pos) : filename;
}

ProgramOptions parseCommandLine(int argc, char* argv[]) {
    ProgramOptions options;
    
    if (argc < 2) {
        options.showHelp = true;
        return options;
    }
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
            return options;
        } else if (arg == "-o") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                throw std::runtime_error("Output file path missing after -o option");
            }
        } else if (options.inputFile.empty()) {
            options.inputFile = arg;
        } else {
            throw std::runtime_error("Unexpected argument: " + arg);
        }
    }
    
    if (options.inputFile.empty()) {
        throw std::runtime_error("Input file not specified");
    }
    
    return options;
}

int main(int argc, char* argv[]) {
    try {
        ProgramOptions options = parseCommandLine(argc, argv);
        
        if (options.showHelp) {
            printHelp(argv[0]);
            return 0;
        }
        
        if (getFileExtension(options.inputFile) != ".bin") {
            throw std::runtime_error("Input file must have .bin extension");
        }

        if (options.outputFile.empty()) {
            options.outputFile = getFileNameWithoutExtension(options.inputFile) + ".xml";
        }
        
        std::vector<uint8_t> nounData = readBinaryFile(options.inputFile);
        NounData parsedNoun = NounParser::ParseNounFile(nounData);
        
        std::ofstream outFile(options.outputFile.c_str());
        if (!outFile.is_open()) {
            throw std::runtime_error("Could not create output file: " + options.outputFile);
        }
        
        outFile << NounToXML(parsedNoun);
        outFile.close();
        
        std::cout << "File processed successfully!\n"
                  << "Result saved to: " << options.outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}