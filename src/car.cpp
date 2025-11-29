#include "car.h"
#include <threepp/threepp.hpp>
#include <threepp/loaders/OBJLoader.hpp>
#include <iostream>
#include <cmath>

Car::Car()
    // 3d model
    { threepp::OBJLoader loader;
        auto carModel = loader.load("assets/cars/simple_car/free_car_001.obj");
        carModel->scale.set(1, 1, 1);
        carModel->position.set(0, -0.5, 0);
        carModel->rotation.y = threepp::math::degToRad(90);
        this->add(carModel);
    }
// create key event handlers
void Car::onKeyPressed(threepp::KeyEvent evt) {
    if (evt.key == threepp::Key::W)      isWPressed = true;
    else if (evt.key == threepp::Key::S) isSPressed = true;
    else if (evt.key == threepp::Key::A) isAPressed = true;
    else if (evt.key == threepp::Key::D) isDPressed = true;
}

void Car::onKeyReleased(threepp::KeyEvent evt) {
    if (evt.key == threepp::Key::W)      isWPressed = false;
    else if (evt.key == threepp::Key::S) isSPressed = false;
    else if (evt.key == threepp::Key::A) isAPressed = false;
    else if (evt.key == threepp::Key::D) isDPressed = false;
}

void Car::update(float dt) {
    // inputs
    float steerIn    = (isDPressed ? -1.f : 0.f) + (isAPressed ? 1.f : 0.f);
    float throttleIn = (isWPressed ? +1.f : 0.f) + (isSPressed ? -1.f : 0.f);

    // accelerate forward
    if (throttleIn > 0) {
        currentSpeed += accelRate * dt;
    }
    // reverse
    else if (throttleIn < 0) {
        currentSpeed -= accelRate * dt;
    }
    // no input = slow down
    else {
        if (currentSpeed > 0) {
            currentSpeed -= brakeRate * dt;
            if (currentSpeed < 0) currentSpeed = 0;
        } else if (currentSpeed < 0) {
            currentSpeed += brakeRate * dt;
            if (currentSpeed > 0) currentSpeed = 0;
        }
    }

    // max speed cap
    if (currentSpeed >  maxSpeed)        currentSpeed =  maxSpeed;
    if (currentSpeed < -maxReverseSpeed) currentSpeed = -maxReverseSpeed;

    // turning
    const float wheelbase = 2.5f;
    float maxSteer = 0.6f;
    if (currentSpeed > maxSteer * 30.f) maxSteer = maxSteer / 2.f; // less steering at high speed
    const float turningRadius = std::tan(steerIn * maxSteer) / wheelbase;
    const float v = currentSpeed; // units per second

    // steering a/d
    rotation.y += v * turningRadius * dt;

    // move w/s
    this->position.x += std::cos(this->rotation.y) * currentSpeed * dt;
    this->position.z += std::sin(this->rotation.y) * -currentSpeed * dt;
}

// helpers
void Car::setW(bool down) { isWPressed = down; }
void Car::setS(bool down) { isSPressed = down; }
void Car::setA(bool down) { isAPressed = down; }
void Car::setD(bool down) { isDPressed = down; }

bool Car::steeringRight() const { return isDPressed; }
bool Car::steeringLeft()  const { return isAPressed; }
