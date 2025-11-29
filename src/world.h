#pragma once
#include <threepp/threepp.hpp>

class World : public threepp::Object3D {
public:
    World();

private:
    void createRoad();
};
