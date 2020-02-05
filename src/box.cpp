#include "box.hpp"
#include <algorithm>
#include <utility>
#include <cassert>

namespace SquarePlant {

Box::Box(std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4> inPlants) : plants(std::move(inPlants)) {
}

const std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4>& Box::getPlants() const {
    return plants;
}

std::vector<Box> Box::packPlants(std::vector<std::unique_ptr<AbstractPlant>> plants) {
    std::vector<Box> ret;

    std::vector<std::unique_ptr<AbstractPlant>> plantsBySize[4];
    for (auto&& plant: plants) {
        int size = plant->getSize();
        assert(size > 0 && size < 5);
        plantsBySize[size - 1].push_back(std::move(plant));
    }

    for (auto&& plant: plantsBySize[3]) {
        std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4> newBox;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                newBox[i][j] = plant->plantCopy();
            }
        }
        ret.push_back(Box(std::move(newBox)));
    }

    for (auto&& plant: plantsBySize[2]) {
        std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4> newBox;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                newBox[i][j] = plant->plantCopy();
            }
        }       
        for (int i = 0; i < 3; i++) {
            if (plantsBySize[0].empty()) break;

            newBox[i][3] = std::move(plantsBySize[0].back());
            plantsBySize[0].pop_back();
        }
        for (int i = 0; i < 4; i++) {
            if (plantsBySize[0].empty()) break;

            newBox[3][i] = std::move(plantsBySize[0].back());
            plantsBySize[0].pop_back();
        }
        ret.push_back(Box(std::move(newBox)));
    }

    while (!plantsBySize[1].empty()) {
        std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4> newBox;
        int collum = 0;
        int row = 0;
        while (row < 4 && !plantsBySize[1].empty()) {
            for (int i = collum; i < collum + 2; i++) {
                for (int j = row; j < row + 2; j++) {
                    newBox[i][j] = plantsBySize[1].back()->plantCopy();
                }
            }
            plantsBySize[1].pop_back();
            collum += 2;
            if (collum == 4) {
                collum = 0;
                row += 2;
            }
        }
        while (row < 4 && !plantsBySize[0].empty()) {
            for (int i = collum; i < collum + 2; i++) {
                for (int j = row; j < row + 2; j++) {
                    if (plantsBySize[0].empty()) break;
                    newBox[i][j] = std::move(plantsBySize[0].back());
                    plantsBySize[0].pop_back();
                }
                if (plantsBySize[0].empty()) break;
            }
            if (plantsBySize[0].empty()) break;
            collum += 2;
            if (collum == 4) {
                collum = 0;
                row += 2;
            }
        }

        ret.push_back(Box(std::move(newBox)));
    }

    while (!plantsBySize[0].empty()) {
        std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4> newBox;
        for (int i = 0; i < 4; i++) {
            if (plantsBySize[0].empty()) break;
            for (int j = 0; j < 4; j++) {
                if (plantsBySize[0].empty()) break;
                newBox[i][j] = std::move(plantsBySize[0].back());
                plantsBySize[0].pop_back();
            }
        }

        ret.push_back(Box(std::move(newBox)));
    }

    return ret;
}

}
