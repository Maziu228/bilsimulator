#include <threepp/threepp.hpp>
#include <iostream>
#include <threepp/canvas/Monitor.hpp>

using namespace threepp;

class Car: public Object3D, public KeyListener {

public:
    Car() {
        auto boxGeometry = BoxGeometry::create(1, 1, 1);
        auto boxMaterial = MeshBasicMaterial::create();
        boxMaterial->color = Color::red;
        auto boxMesh = Mesh::create(boxGeometry, boxMaterial);
        this->add(boxMesh);
    }

    void onKeyPressed(KeyEvent evt) override {
        if (evt.key == Key::W) {
            isWPressed = true;
        } else if (evt.key == Key::S) {
            isSPressed = true;
        } else if (evt.key == Key::A) {
            isAPressed = true;
        } else if (evt.key == Key::D) {
            isDPressed = true;
        }

    }

    void onKeyReleased(KeyEvent evt) override {
        if (evt.key == Key::W) {
            isWPressed = false;
        } else if (evt.key == Key::S) {
            isSPressed = false;
        } else if (evt.key == Key::A) {
            isAPressed = false;
        } else if (evt.key == Key::D) {
            isDPressed = false;
        }
    }

    void update(float dt) {
        if (isWPressed) position.z += speed * dt;
        if (isSPressed) position.z -= speed * dt;
        if (isAPressed) position.x += speed * dt;
        if (isDPressed) position.x -= speed * dt;
    }
private:
    float speed = 4;

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