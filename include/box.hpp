#ifndef SQUARE_PLANT_BOX_H
#define SQUARE_PLANT_BOX_H

#include <vector>
#include <memory>
#include "plant.hpp"

namespace SquarePlant {

class Box {
public:
    Box() = default;
    static std::vector<Box> packPlants(std::vector<std::unique_ptr<AbstractPlant>>);
private:
    explicit Box(std::vector<std::unique_ptr<AbstractPlant>>);
    std::vector<std::unique_ptr<AbstractPlant>> plants;
};

}

#endif

