#ifndef SQUARE_PLANT_BOX_H
#define SQUARE_PLANT_BOX_H

#include <array>
#include <vector>
#include <memory>
#include "plant.hpp"

namespace SquarePlant {

class PlantIter;

class Box {
public:
    Box() = default;
    static std::vector<Box> packPlants(std::vector<std::unique_ptr<AbstractPlant>>);
    const std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4>& getPlants() const;
private:
    explicit Box(std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4>);
    std::array<std::array<std::unique_ptr<AbstractPlant>, 4>, 4> plants;
};

}

#endif

