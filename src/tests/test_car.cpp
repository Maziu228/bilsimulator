// tests/test_car.cpp
#include <catch2/catch_all.hpp>
#include "car.h"

TEST_CASE("Car accelerates forward when W is pressed") {
    Car car;

    // Hold W for some time
    car.setW(true);
    for (int i = 0; i < 20; ++i) { // 20 * 0.1s = 2 seconds
        car.update(0.1f);
    }
    car.setW(false);

    REQUIRE(car.getSpeed() > 0.f);    // speed increased
    REQUIRE(car.getSpeed() <= 30.f);  // not above maxSpeed
}

TEST_CASE("Car brakes and then reverses when holding S") {
    Car car;

    // First accelerate forward a bit
    car.setW(true);
    for (int i = 0; i < 20; ++i) {
        car.update(0.1f);
    }
    car.setW(false);

    REQUIRE(car.getSpeed() > 0.f);    // we are moving forward

    // Now hold S to brake and then reverse
    car.setS(true);
    bool wentNegative = false;

    for (int i = 0; i < 50; ++i) {
        car.update(0.1f);
        if (car.getSpeed() < 0.f) {
            wentNegative = true;
            break;
        }
    }

    REQUIRE(wentNegative);     // eventually moving backwards
    REQUIRE(car.isReversing()); // helper reports reverse
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
