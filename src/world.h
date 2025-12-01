#pragma once
#include <threepp/threepp.hpp>
#include "vector"

// Powerups
struct PowerUp {
    enum class Type {SpeedX2, SizeX2};
    threepp::Box3 box;
    Type type;
    bool collected = false;
    threepp::Object3D*visual = nullptr;
};

class World : public threepp::Object3D {
public:
    World();

    const std::vector<threepp::Box3>& getColliders() const { return colliders_; }
    std::vector<PowerUp>& getPowerUps() { return powerUps_; }
    const std::vector<PowerUp>& getPowerUps() const { return powerUps_;}


private:
    void createRoad();
    std::vector<threepp::Box3> colliders_;
    std::vector<PowerUp> powerUps_;
};
