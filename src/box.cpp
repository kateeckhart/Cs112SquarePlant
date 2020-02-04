#include "box.hpp"
#include <algorithm>
#include <utility>
#include <cassert>

namespace SquarePlant {

Box::Box(std::vector<std::unique_ptr<AbstractPlant>> inPlants) : plants(std::move(inPlants)) {
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
        std::vector<std::unique_ptr<AbstractPlant>> newBox;
        newBox.push_back(std::move(plant));
        ret.push_back(Box(std::move(newBox)));
    }

    for (auto&& plant: plantsBySize[2]) {
        std::vector<std::unique_ptr<AbstractPlant>> newBox;
        newBox.push_back(std::move(plant));
        for (int i = 0; i < 7; i++) {
            if (plantsBySize[0].empty()) break;

            newBox.push_back(std::move(plantsBySize[0].back()));
            plantsBySize[0].pop_back();
        }
        ret.push_back(Box(std::move(newBox)));
    }

    while (!plantsBySize[1].empty()) {
        std::vector<std::unique_ptr<AbstractPlant>> newBox;
        int remainingTwoXTwo = 4;
        while (remainingTwoXTwo > 0 && !plantsBySize[1].empty()) {
            newBox.push_back(std::move(plantsBySize[1].back()));
            plantsBySize[1].pop_back();
            remainingTwoXTwo--;
        }
        int remainingOneXOne = remainingTwoXTwo * 4;
        while (remainingOneXOne > 0 && !plantsBySize[0].empty()) {
            newBox.push_back(std::move(plantsBySize[0].back()));
            plantsBySize[0].pop_back();
            remainingOneXOne--;
        }

        ret.push_back(Box(std::move(newBox)));
    }

    while (!plantsBySize[0].empty()) {
        std::vector<std::unique_ptr<AbstractPlant>> newBox;
        int remainingOneXOne = 16;
        while (remainingOneXOne > 0 && !plantsBySize[0].empty()) {
            newBox.push_back(std::move(plantsBySize[0].back()));
            plantsBySize[0].pop_back();
            remainingOneXOne--;
        }

        ret.push_back(Box(std::move(newBox)));
    }

    return ret;
}

}
