    #include <threepp/threepp.hpp>
    #include <iostream>
    #include <threepp/canvas/Monitor.hpp>
    #include "car.h"
    #include "world.h"
    #include "../../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreAudio.framework/Headers/AudioHardware.h"

    using namespace threepp;

    int main() {
        Canvas canvas ("Separation of Concerns");
        GLRenderer renderer(canvas.size());

        // center picture in the window
        renderer.setPixelRatio(monitor::contentScale().first);

        // scene
        Scene scene;
        scene.background = Color::lightgray;
        PerspectiveCamera camera(75, canvas.aspect(), 0.1f, 1000.f);

        // car
        Car car;
        car.position.set(-80, 0, 46.6);
        scene.add(car);
        canvas.addKeyListener(car);

        // world
        World world;
        scene.add(world);

        // lighting
        auto ambient = threepp::AmbientLight::create(threepp::Color(0xfff2e0), 0.6);
        auto directional = threepp::DirectionalLight::create(threepp::Color(0xffe4b5), 1.2);
        directional->position.set(10, 15, 10);
        auto fill = threepp::DirectionalLight::create(threepp::Color(0xcfd9ff), 0.3);
        fill->position.set(-10, 5, -10);

        scene.add(ambient);
        scene.add(directional);
        scene.add(fill);

        float camYawOffset = 0;
        Clock clock;
        canvas.animate([&]{
        float dt = clock.getDelta();

            // save previous position before moving
        threepp::Vector3 oldPos = car.position;
        car.update(dt);

            // AABB collision
       {
           threepp::Box3 carBox = car.getBoundingBox();
           const auto& colliders = world.getColliders();

           for (const auto& box : colliders) {
               if (carBox.intersectsBox(box)) {
                   car.position = oldPos;
                   break;
               }
           }
       }

            // powerups
            {
    threepp::Box3 carBox = car.getBoundingBox();
    auto& powerUps = world.getPowerUps();

    for (auto& pu : powerUps) {
        if (pu.collected) continue;

        if (carBox.intersectsBox(pu.box)) {
            pu.collected = true;

            if (pu.visual) pu.visual->visible = false;

            switch (pu.type) {
                case PowerUp::Type::SpeedX2:
                    car.applySpeedBoost(1.5, 5);
                    break;
                case PowerUp::Type::SizeX2:
                    car.applySizeBoost(1.5, 5);
                    break;
            }
        }
    }
}




        // camera follow logic
        float steerDir = 0.0f;
        if (car.steeringLeft())  steerDir += 1.0f;
        if (car.steeringRight()) steerDir -= 1.0f;

        // target yaw offset for the camera (in radians)
        float targetOffset = steerDir * 0.45;

        // smooth the camera yaw offset so it "follows" the steering
        float followSpeed = 1.5;
        camYawOffset += (targetOffset - camYawOffset) * followSpeed * dt;

        // use the car's rotation plus the camera yaw offset
        float camYaw = car.rotation.y + camYawOffset;

        camera.position.set(
            car.position.x - 8 * std::cos(camYaw),
            car.position.y + 4,
            car.position.z + 8 * std::sin(camYaw)
        );

        // look slightly ahead in the direction the car is facing
        float lookAheadDist = 3;
        float forwardX = std::cos(car.rotation.y);
        float forwardZ = -std::sin(car.rotation.y);

        threepp::Vector3 lookTarget(
            car.position.x + forwardX * lookAheadDist,
            car.position.y + 1,
            car.position.z + forwardZ * lookAheadDist
        );

        camera.lookAt(lookTarget);
        renderer.render(scene, camera);
    });
    }