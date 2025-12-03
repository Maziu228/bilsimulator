#include "car.h"
#include <threepp/threepp.hpp>
#include <threepp/loaders/OBJLoader.hpp>
#include <iostream>
#include <cmath>

Car::Car() {

    // Car body
    threepp::OBJLoader loader;
    auto carBody = loader.load("assets/cars/simple_car/simple_car_body.obj");
    carBody->scale.set(1, 1, 1);
    carBody->position.set(0, -0.5, 0);
    carBody->rotation.y = threepp::math::degToRad(90);
    this->add(carBody);

    // Wheel model
    auto wheelModel = loader.load("assets/cars/simple_car/simple_car_wheels3.obj");
    wheelModel->scale.set(1, 1, 1);
   wheelModel->rotation.y = threepp::math::degToRad(90);

    // Separate wheel adjustments (4)
    threepp::Vector3 wheelOffsets[4] = {
        { 0.9, -0.1,  1.1 },  // front left
        {-0.9, -0.1,  1.1 },  // front right
        { 0.9, -0.1, -1.1 },  // back left
        {-0.9, -0.1, -1.1 },  // back right
    };
    // Fix wheels position
    for (auto& o : wheelOffsets) {
        float oldX = o.x;
        float oldZ = o.z;
        o.x =  oldZ;
        o.z = -oldX;
    }

    // Clone the wheel 4 times
    for (int i = 0; i < 4; ++i) {
        auto wheel = wheelModel->clone();
        wheel->position.copy(wheelOffsets[i]);

        // Mirror right wheels so all the wheels are not looking same direction
        bool isRight = (i == 1 || i == 3);
        if (isRight) {
            wheel->rotation.y = threepp::math::degToRad(180);
        }

        this->add(wheel);
        wheels_.push_back(wheel.get());
    }

}
// Handling input
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

// Updates
void Car::update(float dt) {

    // Powerup timers
    if (speedBoostTime > 0) {
        speedBoostTime -= dt;
        if (speedBoostTime <= 0) {
            speedMultiplier = 1;
        }
    }

    if (sizeBoostTime > 0) {
        sizeBoostTime -= dt;
        if (sizeBoostTime <= 0) {
            sizeMultiplier = 1;
            this->scale.set(1, 1, 1);
        }
    }

    // Input
    float steerIn    = (isDPressed ? -1 : 0) + (isAPressed ? 1 : 0);
    bool forward  = isWPressed;
    bool backward = isSPressed;

    if (forward && !backward) {

        // Driving specs
        if (currentSpeed < 0) {
            currentSpeed += brakeRate * dt;
            if (currentSpeed > 0) currentSpeed = 0;
        } else {
            currentSpeed += accelRate * speedMultiplier * dt;
        }

    } else if (backward && !forward) {

        if (currentSpeed > 0) {
            currentSpeed -= brakeRate * dt;
            if (currentSpeed < 0) currentSpeed = 0;
        } else {
            currentSpeed -= accelRate * speedMultiplier * dt;
        }

    } else {
        // Slow down
        if (currentSpeed > 0) {
            currentSpeed -= brakeRate * 0.5 * dt;
            if (currentSpeed < 0) currentSpeed = 0;
        } else if (currentSpeed < 0) {
            currentSpeed += brakeRate * 0. * dt;
            if (currentSpeed > 0) currentSpeed = 0;
        }
    }

    // Speed cap
    if (currentSpeed >  maxSpeed * speedMultiplier)
        currentSpeed =  maxSpeed * speedMultiplier;

    if (currentSpeed < -maxReverseSpeed * speedMultiplier)
        currentSpeed = -maxReverseSpeed * speedMultiplier;

    // Handling settings
    const float wheelbase = 2.5;
    float maxSteer = 0.6;

    if (currentSpeed > maxSteer * 30) maxSteer *= 0.5;

    float turningRadius = std::tan(steerIn * maxSteer) / wheelbase;

    rotation.y += currentSpeed * turningRadius * dt;

    this->position.x += std::cos(rotation.y) * currentSpeed * dt;
    this->position.z += std::sin(rotation.y) * -currentSpeed * dt;

    // Wheels rotation and steering
    const float wheelRadius = 0.35;

    float angularSpeed = (wheelRadius > 0)
        ? currentSpeed / wheelRadius
        : 0.f;

    wheelRotation_ -= angularSpeed * dt;

    float steerAngle = steerIn * maxSteer;

    for (int i = 0; i < wheels_.size(); ++i) {

        auto* w = wheels_[i];
        if (!w) continue;

        bool isFront = (i < 2);
        bool isRight = (i == 1 || i == 3);

        // Base yaw: 0 for left wheels 180 for right wheels
        float baseYaw = isRight ? threepp::math::degToRad(180) : 0.f;

        // Front wheels turn around base yaw, back wheels stay still
        if (isFront) {
            w->rotation.y = steerAngle + baseYaw;
        } else {
            w->rotation.y = baseYaw;
        }
        // Wheel rotation direction (visual)
        float angle = wheelRotation_;
        if (isRight) angle = -angle;
        w->rotation.z = angle;
    }
}

// Helpers
void Car::setW(bool down) { isWPressed = down; }
void Car::setS(bool down) { isSPressed = down; }
void Car::setA(bool down) { isAPressed = down; }
void Car::setD(bool down) { isDPressed = down; }

bool Car::steeringRight() const { return isDPressed; }
bool Car::steeringLeft()  const { return isAPressed; }

bool Car::isReversing() const {return currentSpeed < -0.1;}

threepp::Box3 Car::getBoundingBox() const {
    threepp::Vector3 min(position.x - halfExtents.x,
                         position.y - halfExtents.y,
                         position.z - halfExtents.z);

    threepp::Vector3 max(position.x + halfExtents.x,
                         position.y + halfExtents.y,
                         position.z + halfExtents.z);

    return threepp::Box3(min, max);
}
// Speed boost
void Car::applySpeedBoost(float factor, float duration) {
    speedMultiplier = factor;
    speedBoostTime  = duration;
}
// Size boost
void Car::applySizeBoost(float factor, float duration) {
    sizeMultiplier = factor;
    sizeBoostTime  = duration;
    this->scale.set(factor, factor, factor);
}

// Car states when reseted
void Car::resetState() {
    currentSpeed    = 0;

    speedMultiplier = 1;
    speedBoostTime  = 0;

    sizeMultiplier  = 1;
    sizeBoostTime   = 0;

    this->scale.set(1, 1, 1); // removes size boost
}

