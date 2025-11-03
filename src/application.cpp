#include <threepp/threepp.hpp>
#include <iostream>
#include <threepp/canvas/Monitor.hpp>
#include <threepp/loaders/OBJLoader.hpp>
#include "../../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreAudio.framework/Headers/AudioHardware.h"

using namespace threepp;

class Car: public Object3D, public KeyListener {

public:
    // debug box
    Car() {
        {
            auto boxGeometry = BoxGeometry::create(1, 1, 1);
            auto boxMaterial = MeshBasicMaterial::create();
            boxMaterial->color = Color::red;
            auto boxMesh = Mesh::create(boxGeometry, boxMaterial);
            this->add(boxMesh);
        }
        // real model
        { OBJLoader loader;
            auto carModel = loader.load("assets/cars/test/free_car_001.obj");
            carModel->scale.set(1, 1, 1);
            carModel->position.set(0, -0.5, 0);
            carModel->rotation.y = threepp::math::degToRad(90);
            this->add(carModel);
        }


    }

    // create key event handlers
    void onKeyPressed(KeyEvent evt) override {
        if (evt.key == Key::W) isWPressed = true;
        else if (evt.key == Key::S) isSPressed = true;
        else if (evt.key == Key::A) isAPressed = true;
        else if (evt.key == Key::D) isDPressed = true;
    }
    void onKeyReleased(KeyEvent evt) override {
        if (evt.key == Key::W) isWPressed = false;
        else if (evt.key == Key::S) isSPressed = false;
        else if (evt.key == Key::A) isAPressed = false;
        else if (evt.key == Key::D) isDPressed = false;
    }

    void update(float dt) {
        // inputs
        float steerIn = (isDPressed ? -1 : 0) + (isAPressed ? 1 : 0);
        float throttleIn = (isWPressed ? +1 : 0) + (isSPressed ? -1 : 0);

        // accelerate forward
        if (throttleIn > 0) {
            currentSpeed += accelRate * dt;// accelerate forward
        }
        // reverse
        else if (throttleIn < 0) {
            currentSpeed -= accelRate * dt; // accelerate backward
        }
        // no input = slow down
        else {
            if (currentSpeed > 0) {
                currentSpeed -= brakeRate * dt;
                if (currentSpeed < 0) currentSpeed = 0;
            } else if (currentSpeed < 0) {
                currentSpeed += brakeRate * dt;
                if (currentSpeed > 0) currentSpeed = 0;
            }
        }
        // max speed cap
        if (currentSpeed > maxSpeed)  currentSpeed = maxSpeed;
        if (currentSpeed < -maxReverseSpeed) currentSpeed = -maxReverseSpeed;

        // turning
        const float wheelbase = 2.5;
        float maxSteer = 0.6;
        if (currentSpeed > maxSteer*40) maxSteer = maxSteer/2; // less steering at high speed
        const float turningRadius = std::tan(steerIn*maxSteer)/wheelbase;
        const float v = currentSpeed; // units per second

        // steering a/d
        rotation.y += v * turningRadius *dt;

        // move w/s
        this-> position.x += std::cos(this->rotation.y) * currentSpeed * dt;
        this-> position.z += std::sin(this->rotation.y) * -currentSpeed * dt;
    }

    void setW(bool down) { isWPressed = down; }
    void setS(bool down) { isSPressed = down; }
    void setA(bool down) { isAPressed = down; }
    void setD(bool down) { isDPressed = down; }

    bool steeringRight() const { return isDPressed; }
    bool steeringLeft()  const { return isAPressed; }



private:
    // car state and parameters
    float currentSpeed = 0;
    float maxSpeed = 30;
    float maxReverseSpeed = 10;
    float accelRate = 8;
    float brakeRate = 10;
    bool isWPressed = false;
    bool isSPressed = false;
    bool isAPressed = false;
    bool isDPressed = false;

};

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

// connect keylistener to car
    canvas.addKeyListener(car);

    // add lights
    auto ambient = AmbientLight::create(Color::lightyellow, 0.4f);  // soft overall light
    scene.add(ambient);

    auto dirLight = DirectionalLight::create(Color::lightyellow, 1.0f);
    dirLight->position.set(10, 20, 10);
    scene.add(dirLight);

    Clock clock;
    canvas.animate([&]{
        float dt = clock.getDelta();
        car.update(dt);

        camera.position.set(car.position.x - 5 * std::cos(car.rotation.y),
                            car.position.y + 3,
                            car.position.z + 5  * std::sin(car.rotation.y));
        camera.lookAt(car.position);


        renderer.render(scene, camera);
    });

}