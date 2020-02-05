#ifndef SQUARE_PLANT_PLANT_H
#define SQUARE_PLANT_PLANT_H

#include <array>
#include <cstdint>
#include <string>
#include <memory>

namespace SquarePlant {

class AbstractPlant {
public:
    virtual int getSize() const = 0;
    virtual std::string getName() const = 0;
    virtual std::array<uint8_t, 3> getColor() const = 0;
    virtual std::unique_ptr<AbstractPlant> plantCopy() const = 0;
    virtual ~AbstractPlant() = default;
};

template <typename Self>
class Plant : public virtual AbstractPlant {
    std::unique_ptr<AbstractPlant> plantCopy() const override {
        return std::make_unique<Self>(*static_cast<const Self*>(this));
    }
};

class OneByOne : public virtual AbstractPlant {
    int getSize() const override {
        return 1;
    }
};

class Onion : public Plant<Onion>, public OneByOne {
public:
    std::array<uint8_t, 3> getColor() const {
        return {161, 71, 43};
    }

    std::string getName() const override {
        return "Onion";
    }
};

class Garlic : public Plant<Garlic>, public OneByOne {
public:
    std::array<uint8_t, 3> getColor() const {
        return {227, 222, 157};
    }

    std::string getName() const override {
        return "Garlic";
    }
};

class TwoByTwo : public virtual AbstractPlant {
    int getSize() const override {
        return 2;
    }
};

class GrapeVine : public Plant<GrapeVine>, public TwoByTwo {
public:
    std::array<uint8_t, 3> getColor() const {
        return {87, 16, 107};
    }

    std::string getName() const override {
        return "Grape Vine";
    }
};

class BerryBush : public Plant<BerryBush>, public TwoByTwo {
public:
    std::array<uint8_t, 3> getColor() const {
        return {13, 19, 209};
    }

    std::string getName() const override {
        return "Berry Bush";
    }
};

class ThreeByThree : public virtual AbstractPlant {
    int getSize() const override {
        return 3;
    }
};

class OrangeTree : public Plant<OrangeTree>, public ThreeByThree {
public:
    std::array<uint8_t, 3> getColor() const {
        return {230, 152, 18};
    }

    std::string getName() const override {
        return "Orange Tree";
    }
};

class LemonTree : public Plant<LemonTree>, public ThreeByThree {
public:
    std::array<uint8_t, 3> getColor() const {
        return {255, 255, 0};
    }

    std::string getName() const override {
        return "Lemon Tree";
    }
};

class FourByFour : public virtual AbstractPlant {
    int getSize() const override {
        return 4;
    }
};

class AppleTree : public Plant<AppleTree>, public FourByFour {
public:
    std::array<uint8_t, 3> getColor() const {
        return {230, 18, 18};
    }

    std::string getName() const override {
        return "Apple Tree";
    }
};

class LimeTree : public Plant<LimeTree>, public FourByFour {
public:
    std::array<uint8_t, 3> getColor() const {
        return {35, 222, 51};
    }

    std::string getName() const override {
        return "Lime Tree";
    }
};

}

#endif

