#pragma once
#include <threepp/threepp.hpp>
#include "btBulletDynamicsCommon.h"
#include "physics.h"

class Car : public threepp::Object3D, public threepp::KeyListener {
public:
    Car();
    // input handlers
    void onKeyPressed(threepp::KeyEvent evt) override;
    void onKeyReleased(threepp::KeyEvent evt) override;

    // update every frame
    void update(float dt);

    // helpers for camera, etc.
    void setW(bool down);
    void setS(bool down);
    void setA(bool down);
    void setD(bool down);

    bool steeringRight() const;
    bool steeringLeft() const;

private:
    // car state and parameters
    float currentSpeed      = 0.f;
    float maxSpeed          = 30.f;
    float maxReverseSpeed   = 10.f;
    float accelRate         = 8.f;
    float brakeRate         = 15.f;

    bool isWPressed = false;
    bool isSPressed = false;
    bool isAPressed = false;
    bool isDPressed = false;
};