
// Denne testfilen er delvis bassert på kode generert med KI (ChatGPT 5.1)
// Testlogikken og tilpasningene er gjort av meg.


#include <catch2/catch_all.hpp>
#include "car.h"

TEST_CASE("Car accelerates forward when W is pressed") {
    Car car;

    // Hold W for en stund
    car.setW(true);
    for (int i = 0; i < 20; ++i) {
        car.update(0.1f);
    }
    car.setW(false);

    REQUIRE(car.getSpeed() > 0.f);
    REQUIRE(car.getSpeed() <= 30.f);
}

TEST_CASE("Car brakes and then reverses when holding S") {
    Car car;

    // Bilen kjører litt frem
    car.setW(true);
    for (int i = 0; i < 20; ++i) {
        car.update(0.1f);
    }
    car.setW(false);

    REQUIRE(car.getSpeed() > 0.f);

    // Hold S for brems og reverse deretter
    car.setS(true);
    bool wentNegative = false;

    for (int i = 0; i < 50; ++i) {
        car.update(0.1f);
        if (car.getSpeed() < 0.f) {
            wentNegative = true;
            break;
        }
    }

    REQUIRE(wentNegative);
    REQUIRE(car.isReversing());
    car.setS(false);
}

TEST_CASE("Speed boost changes multiplier") {
    Car car;

    REQUIRE(car.getSpeedMultiplier() == 1.f);

    car.applySpeedBoost(1.5f, 5.f);

    REQUIRE(car.hasSpeedBoost());
    REQUIRE(car.getSpeedMultiplier() == 1.5f);
}

TEST_CASE("Size boost changes scale and multiplier") {
    Car car;

    REQUIRE(car.getSizeMultiplier() == 1.f);
    REQUIRE(car.scale.x == 1.f);
    REQUIRE(car.scale.y == 1.f);
    REQUIRE(car.scale.z == 1.f);

    car.applySizeBoost(2.f, 5.f);

    REQUIRE(car.hasSizeBoost());
    REQUIRE(car.getSizeMultiplier() == 2.f);
    REQUIRE(car.scale.x == 2.f);
    REQUIRE(car.scale.y == 2.f);
    REQUIRE(car.scale.z == 2.f);
}

TEST_CASE("resetState resets speed and powerups") {
    Car car;

    // Make the car move and apply boosts
    car.setW(true);
    for (int i = 0; i < 20; ++i) {
        car.update(0.1f);
    }
    car.setW(false);

    car.applySpeedBoost(1.5f, 5.f);
    car.applySizeBoost(2.f, 5.f);

    REQUIRE(car.getSpeedAbs() > 0.f);
    REQUIRE(car.hasSpeedBoost());
    REQUIRE(car.hasSizeBoost());

    car.resetState();

    REQUIRE(car.getSpeedAbs() == 0.f);
    REQUIRE_FALSE(car.hasSpeedBoost());
    REQUIRE_FALSE(car.hasSizeBoost());
    REQUIRE(car.getSpeedMultiplier() == 1.f);
    REQUIRE(car.getSizeMultiplier() == 1.f);
    REQUIRE(car.scale.x == 1.f);
    REQUIRE(car.scale.y == 1.f);
    REQUIRE(car.scale.z == 1.f);
}
