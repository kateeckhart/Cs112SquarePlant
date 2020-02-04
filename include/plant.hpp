#ifndef SQUARE_PLANT_PLANT_H
#define SQUARE_PLANT_PLANT_H

#include <string>
#include <memory>

namespace SquarePlant {

class AbstractPlant {
public:
    virtual int getSize() const = 0;
    virtual std::string getName() const = 0;
    virtual std::unique_ptr<AbstractPlant> plantCopy() const = 0;
    virtual ~AbstractPlant() = default;
};

template <typename Self>
class Plant : public virtual AbstractPlant {
    std::unique_ptr<AbstractPlant> plantCopy() const override {
        return std::make_unique<Self>(*static_cast<const Self*>(this));
    }
};

class Onion : public Plant<Onion> {
public:
    int getSize() const override {
        return 1;
    }

    std::string getName() const override {
        return "Onion";
    }
};

class GrapeVine : public Plant<GrapeVine> {
public:
    int getSize() const override {
        return 2;
    }

    std::string getName() const override {
        return "Grape Vine";
    }
};

class OrangeTree : public Plant<OrangeTree> {
public:
    int getSize() const override {
        return 3;
    }

    std::string getName() const override {
        return "Orange Tree";
    }
};

class AppleTree : public Plant<AppleTree> {
public:
    int getSize() const override {
        return 4;
    }

    std::string getName() const override {
        return "Apple Tree";
    }
};

}

#endif

