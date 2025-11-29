#include "world.h"
#include <threepp/threepp.hpp>
#include <threepp/loaders/OBJLoader.hpp>
#include <iostream>

#include "../cmake-build-debug/_deps/threepp-src/include/threepp/loaders/OBJLoader.hpp"
#include "threepp/extras/curves/LineCurve.hpp"

World::World() {
    threepp::OBJLoader loader;
    auto road = loader.load("assets/objects/road/RoadNetwork3.obj");
    auto house = loader.load("assets/objects/buildings/HouseLowPoly2.obj");
    auto geometry = threepp::BoxGeometry::create(400, 5, 400);
    auto material = threepp::MeshStandardMaterial::create();
    material->color = threepp::Color(0xCF9E7C);
    auto cube = threepp::Mesh::create(geometry, material);
    cube->position.set(0, -3, 0);
    house->scale.set(0.3, 0.3, 0.3);
    house->position.set(10, -0.6, -10);
    this->add(cube);
    this->add(house);

    // check the size of the model (debug)
    threepp::Box3 box;
    box.setFromObject(*road);
    threepp::Vector3 size;
    box.getSize(size);
    std::cout << "Road model size (x, y, z): "
              << size.x << ", " << size.y << ", " << size.z << std::endl;


    road->scale.set(0.65, 0.65, 0.65);
    road->position.set(-15, -0.4, 0);



    this->add(road);
}
