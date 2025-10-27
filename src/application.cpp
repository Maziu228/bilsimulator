#include <threepp/threepp.hpp>
#include <iostream>
#include <threepp/canvas/Monitor.hpp>

using namespace threepp;

class Car: public Object3D, public KeyListener {

public:
    //create car
    Car() {
        auto boxGeometry = BoxGeometry::create(1, 1, 1);
        auto boxMaterial = MeshBasicMaterial::create();
        boxMaterial->color = Color::red;
        auto boxMesh = Mesh::create(boxGeometry, boxMaterial);
        this->add(boxMesh);
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
        if (currentSpeed > maxSteer*40) maxSteer = maxSteer/3; // less steering at high speed
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
    camera.position.set(0,8,-12);
    camera.lookAt({0,0,0});

// add ground in form of grid
    auto grid = GridHelper::create(200, 200);
    grid->position.y = -1;
    scene.add(grid);

    // add car in scene
    Car car;
    scene.add(car);

// connect keylistener to car
    canvas.addKeyListener(car);

    Clock clock;
    canvas.animate([&]{
        float dt = clock.getDelta();

        car.update(dt);
        renderer.render(scene, camera);
    });
}