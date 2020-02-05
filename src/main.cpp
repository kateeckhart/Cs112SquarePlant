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
#include <cmath>
#include <cstring>
#include <cstdio>
#include <csetjmp>
#include <unistd.h>
#include <png.h>

constexpr std::array<png_byte, 3> BORADER_COLOR = {255, 255, 255};
constexpr std::array<png_byte, 3> NOPLANT_COLOR = {0, 0, 0};

void printUsage(const char* name) {
    std::cerr << "Usage " << name << " [-o output] input" << std::endl;
}

bool parseCommandLine(int argc, char **argv, std::string& inputName, std::string& outputName) {
    std::optional<std::string> outputNameOrNone; 
    bool parsing = true;
    opterr = 0;
    while (parsing) {
        switch (getopt(argc, argv, "+o:")) {
            case -1:
                parsing = false;
                break;
            case 'o':
                if (outputNameOrNone) {
                    std::cerr << "Only one output name allowed" << std::endl;
                    printUsage(argv[0]);
                    return false;
                }
                outputNameOrNone = optarg;
                break;
            default:
                printUsage(argv[0]);
                return false;
        }
    }

    if (optind != argc - 1) {
        printUsage(argv[0]);
        return false;
    }
    inputName = argv[optind];
    if (!outputNameOrNone) {
        std::cerr << "Output name required." << std::endl;
        printUsage(argv[0]);
        return false;
    }
    outputName = *outputNameOrNone;

    return true;
}

std::optional<std::vector<SquarePlant::Box>> parseInputFile(const std::string& inputName) {
    std::vector<std::unique_ptr<SquarePlant::AbstractPlant>> plants;
    std::ifstream inFile(inputName);
    if (inFile.fail()) {
        std::cerr << inputName << " could not be opened" << std::endl;
        return {};
    }
    while(true) {
        std::string line;
        std::getline(inFile, line);
        if (inFile.eof()) break;
        if (inFile.bad()) {
            std::cerr << "Input error" << std::endl;
            return {};
        }
        std::istringstream lineStream(line);
        unsigned count;
        lineStream >> count;
        if (lineStream.eof()) continue;
        if (lineStream.fail()) {
            std::cerr << "Lines must be in format \"[Number] [Plant Name]\"" << std::endl; 
            return {};
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
            return {};
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
            return {};
        }

        for (unsigned x = 0; x < count; x++) {
            plants.push_back(plantTemplate->plantCopy());
        }

    }

    return SquarePlant::Box::packPlants(std::move(plants));
}

void drawBox(const SquarePlant::Box& box, int widthOffset, png_byte** rows) {
    for (int i = 0; i < 10; i++) {
        for (int j = widthOffset; j < (60 * 3) + widthOffset; j += 3) {
            rows[i][j] = BORADER_COLOR[0];
            rows[i][j + 1] = BORADER_COLOR[1];
            rows[i][j + 2] = BORADER_COLOR[2];
        }
    }
    for (int i = 10; i < 50; i++) {
        for (int j = widthOffset; j < (10 * 3) + widthOffset; j += 3) {
            rows[i][j] = BORADER_COLOR[0];
            rows[i][j + 1] = BORADER_COLOR[1];
            rows[i][j + 2] = BORADER_COLOR[2];
        }

        for (int j = (10 * 3) + widthOffset; j < (50 * 3) + widthOffset; j += 3) {
            SquarePlant::AbstractPlant* plant = box.getPlants()[(i - 10) / 10][(j - widthOffset - 30) / 30].get();
            std::array<png_byte, 3> plantColor;
            if (plant) {
                plantColor = plant->getColor();
            }
            else {
                plantColor = NOPLANT_COLOR;
            }
            rows[i][j] = plantColor[0];
            rows[i][j + 1] = plantColor[1];
            rows[i][j + 2] = plantColor[2];
        }

        for (int j = (50 * 3) + widthOffset; j < (60 * 3) + widthOffset; j += 3) {
            rows[i][j] = BORADER_COLOR[0];
            rows[i][j + 1] = BORADER_COLOR[1];
            rows[i][j + 2] = BORADER_COLOR[2];
        }
    }
    for (int i = 50; i < 60; i++) {
        for (int j = widthOffset; j < (60 * 3) + widthOffset; j += 3) {
            rows[i][j] = BORADER_COLOR[0];
            rows[i][j + 1] = BORADER_COLOR[1];
            rows[i][j + 2] = BORADER_COLOR[2];
        }
    }

}

png_byte** genImage(const std::vector<SquarePlant::Box>& boxes, int& width, int& height) {
    int boxHCount = 1;
    int boxResetH = 0;
    int boxVCount = 1;
    int boxCurrentH = 0;
    int boxCurrentV = 0;
    for (unsigned i = 0; i < boxes.size(); i++) {
        boxCurrentH++;
        if (boxCurrentH == boxHCount) {
            boxCurrentH = boxResetH;
            boxCurrentV++;
            if (boxCurrentV == boxVCount) {
                if (boxHCount > boxVCount) {
                    boxVCount++;
                    boxCurrentH = boxResetH = 0;
                }
                else {
                    boxHCount++;
                    boxCurrentV = 0;
                    boxCurrentH = boxResetH = boxHCount - 1;
                }
            }
        }
    }

    width = boxHCount * 60;
    height = boxVCount * 60;
    png_byte** rows = new png_byte*[height];
    for (int i = 0; i < height; i++) {
        rows[i] = new png_byte[width * 3];
        memset(rows[i], 0, width * 3); // Set empty space to black.
    }
    boxCurrentH = boxCurrentV = 0;
    for (auto&& box: boxes) {
        drawBox(box, boxCurrentH * 60 * 3, rows + (boxCurrentV * 60));
        boxCurrentH++;
        if (boxCurrentH == boxHCount) {
            boxCurrentH = 0;
            boxCurrentV++;
        }
    }

    return rows;
}

int main(int argc, char **argv) {
    std::string inputName, outputName;
    if (!parseCommandLine(argc, argv, inputName, outputName)) return 1;
    std::optional<std::vector<SquarePlant::Box>> boxesOrNone = parseInputFile(inputName);
    if (!boxesOrNone) return 1;
    std::vector<SquarePlant::Box> boxes = std::move(*boxesOrNone);

    FILE *outputFile = fopen(outputName.c_str(), "wb");
    if (!outputFile) {
        perror("Output file could not open");
        return 1;
    }

    png_struct* png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "png_create_write_struct failed" << std::endl;
        return 1;
    }

    png_info* info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        std::cerr << "png_create_info_struct failed" << std::endl;
        return 1;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(outputFile);
        std::cerr << "Error during init_io" << std::endl;
        return 1;
    }

    png_init_io(png, outputFile);

    int width, height;
    png_byte** rows = genImage(boxes, width, height);

    if(setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(outputFile);
        for (int i = 0; i < height; i++) {
            delete[] rows[i];
        }
        delete rows;
        std::cerr << "Error while writing header" << std::endl;
        return 1;
    }

    png_set_IHDR(png, info, width, height, 8, 
            PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, 
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(outputFile);
        for (int i = 0; i < height; i++) {
            delete[] rows[i];
        }
        delete rows;
        std::cerr << "Error while writing image data" << std::endl;
        return 1;
    }

    png_write_image(png, rows);

    for (int i = 0; i < height; i++) {
        delete[] rows[i];
    }
    delete[] rows;

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(outputFile);
        std::cerr << "Error while writing end" << std::endl;
        return 1;
    }

    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
    fclose(outputFile);

    std::cout << "You need " << boxes.size() << " boxes." << std::endl;
}
