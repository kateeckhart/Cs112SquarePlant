#include "plant.hpp"
#include "box.hpp"
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <utility>
#include <optional>
#include <unistd.h>

void printUsage(const char* name) {
    std::cerr << "Usage " << name << " [-o output] input" << std::endl;
}

int main(int argc, char **argv) {
    std::optional<std::string> outputName; 
    bool parsing = true;
    opterr = 0;
    while (parsing) {
        switch (getopt(argc, argv, "+o:")) {
            case -1:
                parsing = false;
                break;
            case 'o':
                if (outputName) {
                    std::cerr << "Only one output name allowed" << std::endl;
                    printUsage(argv[0]);
                    return 1;
                }
                outputName = optarg;
                break;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }

    if (optind != argc - 1) {
        printUsage(argv[0]);
        return 1;
    }
    std::string inputName = argv[optind];
    if (!outputName) {
        printUsage(argv[0]);
        return 1;
    }
    

    std::vector<std::unique_ptr<SquarePlant::AbstractPlant>> plants;
    std::ifstream inFile(inputName);
    while(true) {
        std::string line;
        std::getline(inFile, line);
        if (inFile.eof()) break;
        if (inFile.bad()) {
            std::cerr << "Input error" << std::endl;
            return 1;
        }
        std::istringstream lineStream(line);
        unsigned count;
        lineStream >> count;
        if (lineStream.eof()) continue;
        if (lineStream.fail()) {
            std::cerr << "Lines must be in format \"[Number] [Plant Name]\"" << std::endl; 
            return 1;
        }

        std::vector<std::string> name;
        while (!lineStream.eof()) {
            std::string nameComponent;
            lineStream >> nameComponent;
            if (nameComponent.empty()) break;
            for (auto&& letter: nameComponent) {
                letter = tolower(letter);
            }
            name.push_back(std::move(nameComponent));
        }

        std::unique_ptr<SquarePlant::AbstractPlant> plantTemplate;
        if (name.empty()) {
            std::cerr << "Lines must be in format \"[Number] [Plant Name]\"" << std::endl; 
            return 1;
        }
        else if (name.size() == 1 && name[0] == "onion") {
            plantTemplate = std::make_unique<SquarePlant::Onion>();
        }
        else if (name.size() == 2) {
            if (name[0] == "grape" && name[1] == "vine") {
                plantTemplate = std::make_unique<SquarePlant::GrapeVine>();
            }
            if (name[0] == "orange" && name[1] == "tree") {
                plantTemplate = std::make_unique<SquarePlant::OrangeTree>();
            }
            if (name[0] == "apple" && name[1] == "tree") {
                plantTemplate = std::make_unique<SquarePlant::AppleTree>();
            }
        }

        if (!plantTemplate) {
            std::cerr << "Plant name not supported" << std::endl;
            return 1;
        }

        for (unsigned x = 0; x < count; x++) {
            plants.push_back(plantTemplate->plantCopy());
        }

    }

    std::vector<SquarePlant::Box> boxes = SquarePlant::Box::packPlants(std::move(plants));
    std::cout << "You need " << boxes.size() << " boxes." << std::endl;
}
