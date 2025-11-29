    #include <threepp/threepp.hpp>
    #include <iostream>
    #include <threepp/canvas/Monitor.hpp>
    #include <threepp/loaders/OBJLoader.hpp>
    #include "car.h"
    #include "world.h"
    #include "physics.h"
    #include "../../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreAudio.framework/Headers/AudioHardware.h"

    using namespace threepp;

    int main() {
        Canvas canvas ("Separation of Concerns");
        GLRenderer renderer(canvas.size());

        // center picture in the window
        renderer.setPixelRatio(monitor::contentScale().first);

        // add scene
        Scene scene;
        scene.background = Color::lightgray;
        PerspectiveCamera camera(75, canvas.aspect(), 0.1f, 1000.f);

        // add ground in form of grid
        auto grid = GridHelper::create(200, 200);
        grid->position.y = -1;
        scene.add(grid);

        // add car in scene
        Car car;
        scene.add(car);
        canvas.addKeyListener(car);

        // connect keylistener to car
        canvas.addKeyListener(car);

        // add physics world
        Physics physics;

        //add world
        World world;
        scene.add(world);


        // add lights
        auto ambient = threepp::AmbientLight::create(threepp::Color(0xfff2e0), 0.6);

        auto directional = threepp::DirectionalLight::create(threepp::Color(0xffe4b5), 1.2);
        directional->position.set(10, 15, 10);

        auto fill = threepp::DirectionalLight::create(threepp::Color(0xcfd9ff), 0.3);
        fill->position.set(-10, 5, -10);

        scene.add(ambient);
        scene.add(directional);
        scene.add(fill);



        // camera variables
        float camYawOffset = 0;

        Clock clock;
        canvas.animate([&]{
        float dt = clock.getDelta();
        car.update(dt);

        // ---- CAMERA STEERING OFFSET ----
        // -1 = steer right, +1 = steer left, 0 = straight
        float steerDir = 0.0f;
        if (car.steeringLeft())  steerDir += 1.0f;
        if (car.steeringRight()) steerDir -= 1.0f;

        // target yaw offset for the camera (in radians)
        float targetOffset = steerDir * 0.16; // 0.35 is ~20 degrees, tweak if you want more/less

        // smooth the camera yaw offset so it "follows" the steering
        float followSpeed = 1.5; // higher = snappier camera
        camYawOffset += (targetOffset - camYawOffset) * followSpeed * dt;

        // use the car's rotation plus the camera yaw offset
        float camYaw = car.rotation.y + camYawOffset;

        // place camera behind and above the car, but using camYaw
        camera.position.set(
            car.position.x - 8 * std::cos(camYaw),
            car.position.y + 4,
            car.position.z + 8 * std::sin(camYaw)
        );

        // look slightly ahead in the direction the car is facing
        float lookAheadDist = 3; // how far ahead of the car to look
        float forwardX = std::cos(car.rotation.y);
        float forwardZ = -std::sin(car.rotation.y);

        threepp::Vector3 lookTarget(
            car.position.x + forwardX * lookAheadDist,
            car.position.y + 1,
            car.position.z + forwardZ * lookAheadDist
        );

        camera.lookAt(lookTarget);

        // render
        renderer.render(scene, camera);
    });
    }