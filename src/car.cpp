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
        { 0.9f, -0.1f,  1.1f },  // front left
        {-0.9f, -0.1f,  1.1f },  // front right
        { 0.9f, -0.1f, -1.1f },  // back left
        {-0.9f, -0.1f, -1.1f },  // back right
    };

    for (auto& o : wheelOffsets) {
        float oldX = o.x;
        float oldZ = o.z;
        o.x =  oldZ;
        o.z = -oldX;
    }

    // Clone 4 wheels
    for (int i = 0; i < 4; ++i) {
        auto wheel = wheelModel->clone();
        wheel->position.copy(wheelOffsets[i]);

        // Mirror right wheels
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
    if (speedBoostTime > 0.f) {
        speedBoostTime -= dt;
        if (speedBoostTime <= 0.f) {
            speedMultiplier = 1;
        }
    }

    if (sizeBoostTime > 0.f) {
        sizeBoostTime -= dt;
        if (sizeBoostTime <= 0.f) {
            sizeMultiplier = 1.f;
            this->scale.set(1.f, 1.f, 1.f);
        }
    }

    // Input
    float steerIn    = (isDPressed ? -1.f : 0.f) + (isAPressed ? 1.f : 0.f);
    bool forward  = isWPressed;
    bool backward = isSPressed;

    if (forward && !backward) {

        // Driving specs
        if (currentSpeed < 0.f) {
            currentSpeed += brakeRate * dt;
            if (currentSpeed > 0.f) currentSpeed = 0.f;
        } else {
            currentSpeed += accelRate * speedMultiplier * dt;
        }

    } else if (backward && !forward) {

        if (currentSpeed > 0.f) {
            currentSpeed -= brakeRate * dt;
            if (currentSpeed < 0.f) currentSpeed = 0.f;
        } else {
            currentSpeed -= accelRate * speedMultiplier * dt;
        }

    } else {
        // Slow down
        if (currentSpeed > 0.f) {
            currentSpeed -= brakeRate * 0.5f * dt;
            if (currentSpeed < 0.f) currentSpeed = 0.f;
        } else if (currentSpeed < 0.f) {
            currentSpeed += brakeRate * 0.5f * dt;
            if (currentSpeed > 0.f) currentSpeed = 0.f;
        }
    }

    // Speed cap
    if (currentSpeed >  maxSpeed * speedMultiplier)
        currentSpeed =  maxSpeed * speedMultiplier;

    if (currentSpeed < -maxReverseSpeed * speedMultiplier)
        currentSpeed = -maxReverseSpeed * speedMultiplier;

    // Speed and position
    const float wheelbase = 2.5f;
    float maxSteer = 0.6f;

    if (currentSpeed > maxSteer * 30.f) maxSteer *= 0.5f;

    float turningRadius = std::tan(steerIn * maxSteer) / wheelbase;

    rotation.y += currentSpeed * turningRadius * dt;

    this->position.x += std::cos(rotation.y) * currentSpeed * dt;
    this->position.z += std::sin(rotation.y) * -currentSpeed * dt;

    // Wheels rotation and steering
    const float wheelRadius = 0.35f;

    float angularSpeed = (wheelRadius > 0.f)
        ? currentSpeed / wheelRadius
        : 0.f;

    wheelRotation_ -= angularSpeed * dt;

    float steerAngle = steerIn * maxSteer;

    for (int i = 0; i < wheels_.size(); ++i) {

        auto* w = wheels_[i];
        if (!w) continue;

        bool isFront = (i < 2);
        bool isRight = (i == 1 || i == 3);

        // base yaw: 0 for left wheels, 180 for right wheels
        float baseYaw = isRight ? threepp::math::degToRad(180) : 0.f;

        // front wheels steer around that base yaw, rears just keep base yaw
        if (isFront) {
            w->rotation.y = steerAngle + baseYaw;
        } else {
            w->rotation.y = baseYaw;
        }

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

bool Car::isReversing() const {return currentSpeed < -0.1f;}

threepp::Box3 Car::getBoundingBox() const {
    threepp::Vector3 min(position.x - halfExtents.x,
                         position.y - halfExtents.y,
                         position.z - halfExtents.z);

    threepp::Vector3 max(position.x + halfExtents.x,
                         position.y + halfExtents.y,
                         position.z + halfExtents.z);

    return threepp::Box3(min, max);
}

void Car::applySpeedBoost(float factor, float duration) {
    speedMultiplier = factor;
    speedBoostTime  = duration;
}

void Car::applySizeBoost(float factor, float duration) {
    sizeMultiplier = factor;
    sizeBoostTime  = duration;
    this->scale.set(factor, factor, factor);
}

void Car::resetState() {
    currentSpeed    = 0.f;

    speedMultiplier = 1.f;
    speedBoostTime  = 0.f;

    sizeMultiplier  = 1.f;
    sizeBoostTime   = 0.f;

    this->scale.set(1.f, 1.f, 1.f); // removes size boost
}

