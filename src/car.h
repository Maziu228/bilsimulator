#pragma once
#include <threepp/threepp.hpp>


class Car : public threepp::Object3D, public threepp::KeyListener {
public:
    Car();
    // input handlers
    void onKeyPressed(threepp::KeyEvent evt) override;
    void onKeyReleased(threepp::KeyEvent evt) override;

    void applySpeedBoost(float factor, float duration);
    void applySizeBoost(float factor, float duration);


    // update every frame
    void update(float dt);

    // helpers for camera, etc.
    void setW(bool down);
    void setS(bool down);
    void setA(bool down);
    void setD(bool down);

    bool steeringRight() const;
    bool steeringLeft() const;

    threepp::Box3 getBoundingBox() const;

    // Ui helpers
    float getSpeed() const { return currentSpeed; }
    float getSpeedAbs() const { return std::abs(currentSpeed); }
    bool hasSpeedBoost() const { return speedBoostTime > 0.f; }
    bool hasSizeBoost()  const { return sizeBoostTime  > 0.f; }
    float getSpeedMultiplier() const { return speedMultiplier; }
    float getSizeMultiplier()  const { return sizeMultiplier; }

    // Reset car state (for restart)
    void resetState();


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

    // car collider box
    threepp::Vector3 halfExtents {1.f, 0.5f, 2.f};

    // powerup state
    float speedMultiplier = 1.f;
    float speedBoostTime  = 0.f;

    float sizeMultiplier  = 1.f;
    float sizeBoostTime   = 0.f;

    std::vector<threepp::Object3D*> wheels_;
    float wheelRotation_ = 0.f;
};