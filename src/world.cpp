#include "world.h"
#include <threepp/threepp.hpp>
#include <threepp/loaders/OBJLoader.hpp>
#include <iostream>
#include <memory>

World::World() {
    threepp::OBJLoader loader;

    // road model
    auto road = loader.load("assets/objects/road/RoadNetwork3.obj");
    road->scale.set(0.65, 0.65, 0.65);
    road->position.set(-15, -0.4, 0);

    // house model
    auto house = loader.load("assets/objects/buildings/HouseLowPoly2.obj");
    house->scale.set(0.3, 0.3, 0.3);
    house->position.set(10, -0.6, -10);

    // finish model
    auto finish = loader.load ("assets/objects/misc/finishLine.obj");
    finish->scale.set(1.5, 1.5, 1.5);
    finish->position.set(-60, -0.5, 46.6);
    finish->rotation.y= threepp::math::degToRad(90);

    // barrier model
    auto barrier = loader.load("assets/objects/misc/Barrier.obj");
    barrier->scale.set(2, 2, 2);

    // ground model
    auto geometry = threepp::BoxGeometry::create(400, 5, 400);
    auto material = threepp::MeshStandardMaterial::create();
    material->color = threepp::Color(0xCF9E7C);
    auto ground = threepp::Mesh::create(geometry, material);
    ground->position.set(0, -3, 0);

    this->add(road);
    this->add(ground);
    this->add(house);
    this->add(finish);

// AABB kolisjoner er delvis inspirert av KI (ChatGPT 5.1) for å forstår hvordan den kan brukes
// Plassering og logikken bak er gjort av meg

    // Custom collider for house
    threepp::Box3 houseColider(
    threepp::Vector3(4, -1, -13),
    threepp::Vector3(16, 3, -7)
);
    colliders_.push_back(houseColider);

    // Barricade placements: position + rotation
    struct BarricadeInfo {
        threepp::Vector3 pos;
        float yawDeg;
    };

    std::vector<BarricadeInfo> barricades = {
        { threepp::Vector3(70, 0, 46),   0 },
        { threepp::Vector3(167, 0, 4),  60 },
        { threepp::Vector3(82, 0, 120),  95 },
        { threepp::Vector3(0, 0, 140), 90},
        { threepp::Vector3(-117, 0, 140), 90},
        { threepp::Vector3(-15, 0, -20), 90},
    };

    for (const auto& info : barricades) {
        // clone visual model
        auto bar = barrier->clone();
        bar->position.copy(info.pos);
        bar->rotation.y = threepp::math::degToRad(info.yawDeg);

        this->add(bar);

        // colliders for barricades
        threepp::Box3 box;
        box.setFromObject(*bar);
        colliders_.push_back(box);
    }

    // Power ups
    {
        // visual mesh
        auto speedPU = threepp::SphereGeometry::create(0.7, 16, 16);
        auto speedCOL = threepp::MeshStandardMaterial::create();
        speedCOL->color = threepp::Color(0xffff00); // yellow

        auto sizePU = threepp::SphereGeometry::create(0.7, 16, 16);
        auto sizeCOL = threepp::MeshStandardMaterial::create();
        sizeCOL->color = threepp::Color(0x00ffff); // cyan

        struct PowerUpInfo {
            threepp::Vector3 pos;
            PowerUp::Type type;
        };
        std::vector<PowerUpInfo> infos = {
            { threepp::Vector3(-13, 1,  -5), PowerUp::Type::SpeedX2 },  // position for speed power ups
            { threepp::Vector3( 80, 1, 90), PowerUp::Type::SpeedX2 },
            { threepp::Vector3( -100, 1,80), PowerUp::Type::SpeedX2 },

            { threepp::Vector3(80, 1,  40), PowerUp::Type::SizeX2 },    // position for size power ups
            { threepp::Vector3( -65 , 1,  120), PowerUp::Type::SizeX2 },
        };

        for (const auto& info : infos) {
            std::shared_ptr<threepp::Mesh> mesh;

            if (info.type == PowerUp::Type::SpeedX2) {
                mesh = threepp::Mesh::create(speedPU, speedCOL); // speed
            } else {
                mesh = threepp::Mesh::create(sizePU, sizeCOL);  // size
            }

            mesh->position.copy(info.pos);
            this->add(mesh);

            threepp::Box3 box;
            box.setFromObject(*mesh);

            PowerUp PU;
            PU.type   = info.type;
            PU.box    = box;
            PU.visual = mesh.get();
            powerUps_.push_back(PU);
        }
    }
}