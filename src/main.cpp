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

constexpr png_color BOARDER_COLOR = {255, 255, 255};
constexpr png_color NOPLANT_COLOR = {0, 0, 0};

void printUsage(const char* name) {
    std::cerr << "Usage " << name << " [-o output.png] input" << std::endl;
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
        else if (name.size() == 1) {
            if (name[0] == "onion") {
                plantTemplate = std::make_unique<SquarePlant::Onion>();
            }
            else if (name[0] == "garlic") {
                plantTemplate = std::make_unique<SquarePlant::Garlic>();
            }
        }
        else if (name.size() == 2) {
            if (name[0] == "grape" && name[1] == "vine") {
                plantTemplate = std::make_unique<SquarePlant::GrapeVine>();
            }
            else if (name[0] == "berry" && name[1] == "bush") {
                plantTemplate = std::make_unique<SquarePlant::BerryBush>();
            }
            else if (name[0] == "orange" && name[1] == "tree") {
                plantTemplate = std::make_unique<SquarePlant::OrangeTree>();
            }
            else if (name[0] == "lemon" && name[1] == "tree") {
                plantTemplate = std::make_unique<SquarePlant::LemonTree>();
            }
            else if (name[0] == "apple" && name[1] == "tree") {
                plantTemplate = std::make_unique<SquarePlant::AppleTree>();
            }
            else if (name[0] == "lime" && name[1] == "tree") {
                plantTemplate = std::make_unique<SquarePlant::LimeTree>();
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

png_byte lookupPalette(const SquarePlant::Box& box, std::vector<png_color>& palette, int i, int j) {
    png_byte plantColor;
    SquarePlant::AbstractPlant* plant = box.getPlants()[(i - 10) / 10][j / 10].get();
    if (plant) {
        std::array<png_byte, 3> rawPlantColor = plant->getColor();
        png_color pngPlantColor;
        memcpy(&pngPlantColor, &rawPlantColor, sizeof(png_color));

        bool colorFound = false;
        for (unsigned i = 0; i < palette.size(); i++) {
            if (memcmp(&pngPlantColor, &palette[i], sizeof(png_color)) == 0) {
                plantColor = i;
                colorFound = true;
                break;
            }
        }
        if (!colorFound) {
            if (palette.size() >= 16) {
                std::cerr << "Too many colors" << std::endl;
                abort();
            }
            plantColor = palette.size();
            palette.push_back(pngPlantColor);
        }
    }
    else {
        plantColor = 0;
    }

    return plantColor;
}

void drawBox(const SquarePlant::Box& box, int widthOffset, png_byte** rows, std::vector<png_color>& palette) {
    for (int i = 0; i < 10; i++) {
        for (int j = widthOffset; j < 60 / 2 + widthOffset; j++) {
            rows[i][j] = 0x11; //0x11 sets two adjacent pixels to boarder
        }
    }
    for (int i = 10; i < 50; i++) {
        for (int j = widthOffset; j < 10 / 2 + widthOffset; j++) {
            rows[i][j] = 0x11;
        }

        for (int j = 10 / 2 + widthOffset; j < 50 / 2 + widthOffset; j++) {
            png_byte plantColorOne = lookupPalette(box, palette, i, (j - widthOffset - 5) * 2);
            png_byte plantColorTwo = lookupPalette(box, palette, i, (j - widthOffset - 5) * 2 + 1);
            png_byte plantColor = plantColorOne << 4;
            plantColor |= plantColorTwo;

            rows[i][j] = plantColor;
        }

        for (int j = 50 / 2 + widthOffset; j < 60 / 2 + widthOffset; j++) {
            rows[i][j] = 0x11;
        }
    }
    for (int i = 50; i < 60; i++) {
        for (int j = widthOffset; j < 60 / 2 + widthOffset; j ++) {
            rows[i][j] = 0x11;
        }
    }

}

png_byte** genImage(const std::vector<SquarePlant::Box>& boxes, int& width, int& height, std::vector<png_color>& palette) {
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
    palette.push_back(NOPLANT_COLOR);
    palette.push_back(BOARDER_COLOR);
    png_byte** rows = new png_byte*[height];
    for (int i = 0; i < height; i++) {
        rows[i] = new png_byte[width / 2];
        memset(rows[i], 0, width / 2); // Set empty space to black.
    }
    boxCurrentH = boxCurrentV = 0;
    for (auto&& box: boxes) {
        drawBox(box, boxCurrentH * 30, rows + (boxCurrentV * 60), palette);
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
    png_byte** rows = nullptr;
    int width, height;
    std::vector<png_color> palette;
    png_struct* png = nullptr;
    png_info* info = nullptr;

    if (!outputFile) {
        perror("Output file could not be opened");
        goto error;
    }

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "png_create_write_struct failed" << std::endl;
        goto error;
    }

    info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "png_create_info_struct failed" << std::endl;
        goto error;
    }

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Error during init_io" << std::endl;
        goto error;
    }

    png_init_io(png, outputFile);

    rows = genImage(boxes, width, height, palette);

    if(setjmp(png_jmpbuf(png))) {
        std::cerr << "Error while writing header" << std::endl;
        goto error;
    }

    png_set_IHDR(png, info, width, height, 4, 
            PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, 
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_set_PLTE(png, info, palette.data(), palette.size());

    png_color_16 black;
    black.index = 0;
    png_set_bKGD(png, info, &black);

    png_write_info(png, info);

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Error while writing image data" << std::endl;
        goto error;
    }

    png_write_image(png, rows);

    for (int i = 0; i < height; i++) {
        delete[] rows[i];
    }
    delete[] rows;
    rows = nullptr;

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Error while writing end" << std::endl;
        goto error;
    }

    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
    fclose(outputFile);

    std::cout << "You need " << boxes.size() << " boxes." << std::endl;
    return 0;
error:
    png_destroy_write_struct(&png, &info);
    fclose(outputFile);
    if (rows) {
        for (int i = 0; i < height; i++) {
            delete[] rows[i];
        }
    }
    delete[] rows;
    return 1;
}
