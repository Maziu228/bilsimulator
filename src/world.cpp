#include "world.h"
#include <threepp/threepp.hpp>
#include <threepp/loaders/OBJLoader.hpp>
#include <iostream>
#include <memory>

World::World() {
    threepp::OBJLoader loader;
    auto road = loader.load("assets/objects/road/RoadNetwork3.obj");
    road->scale.set(0.65, 0.65, 0.65);
    road->position.set(-15, -0.4, 0);

    auto house = loader.load("assets/objects/buildings/HouseLowPoly2.obj");
    house->scale.set(0.3, 0.3, 0.3);
    house->position.set(10, -0.6, -10);

    auto finish = loader.load ("assets/objects/misc/finishLine.obj");
    finish->scale.set(1.5, 1.5, 1.5);
    finish->position.set(-60, -0.5, 46.6);
    finish->rotation.y= threepp::math::degToRad(90);

    auto barrier = loader.load("assets/objects/misc/Barrier.obj");
    barrier->scale.set(2, 2, 2);

    auto geometry = threepp::BoxGeometry::create(400, 5, 400);
    auto material = threepp::MeshStandardMaterial::create();
    material->color = threepp::Color(0xCF9E7C);
    auto ground = threepp::Mesh::create(geometry, material);
    ground->position.set(0, -3, 0);

    this->add(road);
    this->add(ground);
    this->add(house);
    this->add(finish);

    // Custom collider for house
    threepp::Box3 houseBox(
    threepp::Vector3(4, -1, -13),
    threepp::Vector3(16, 3, -7)
);
    colliders_.push_back(houseBox);

    // Barricade placements: position + rotation
    struct BarricadeInfo {
        threepp::Vector3 pos;
        float yawDeg;
    };

    std::vector<BarricadeInfo> barricades = {
        //  x      y   z       yaw
        { threepp::Vector3(70, 0.f, 46),   0 },
        { threepp::Vector3(167, 0.f, 4),  60 },
        { threepp::Vector3(82, 0.f, 120),  95 },
        { threepp::Vector3(0, 0.f, 140), 90},
        { threepp::Vector3(-117, 0.f, 140), 90},
        { threepp::Vector3(-15, 0.f, -20), 90},
    };

    for (const auto& info : barricades) {
        // clone visual model
        auto b = barrier->clone();
        b->position.copy(info.pos);
        b->rotation.y = threepp::math::degToRad(info.yawDeg);

        this->add(b);

        // colliders for barricades
        threepp::Box3 box;
        box.setFromObject(*b);
        colliders_.push_back(box);
    }

    // Power ups
    {
        // visual mesh
        auto speedGeo = threepp::SphereGeometry::create(0.7f, 16, 16);
        auto speedMat = threepp::MeshStandardMaterial::create();
        speedMat->color = threepp::Color(0xffff00);

        auto sizeGeo = threepp::SphereGeometry::create(0.7, 16, 16);
        auto sizeMat = threepp::MeshStandardMaterial::create();
        sizeMat->color = threepp::Color(0x00ffff);

        struct PowerUpInfo {
            threepp::Vector3 pos;
            PowerUp::Type type;
        };
        std::vector<PowerUpInfo> infos = {
            { threepp::Vector3(-13.f, 1.f,  -5.f), PowerUp::Type::SpeedX2 },
            { threepp::Vector3( 80, 1.f, 90), PowerUp::Type::SpeedX2 },
            { threepp::Vector3( -100, 1.f,80), PowerUp::Type::SpeedX2 },

            { threepp::Vector3(80, 1.f,  40.f), PowerUp::Type::SizeX2 },
            { threepp::Vector3( -65 , 1.f,  120), PowerUp::Type::SizeX2 },
        };

        for (const auto& info : infos) {
            std::shared_ptr<threepp::Mesh> mesh;

            if (info.type == PowerUp::Type::SpeedX2) {
                mesh = threepp::Mesh::create(speedGeo, speedMat);
            } else { // SizeX2
                mesh = threepp::Mesh::create(sizeGeo, sizeMat);
            }

            mesh->position.copy(info.pos);
            this->add(mesh);

            threepp::Box3 box;
            box.setFromObject(*mesh);

            PowerUp pu;
            pu.type   = info.type;
            pu.box    = box;
            pu.visual = mesh.get();
            powerUps_.push_back(pu);
        }
    }
}