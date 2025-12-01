#include <threepp/threepp.hpp>
#include <threepp/canvas/Monitor.hpp>
#include <cmath>
#include <functional>

// Headers
#include "car.h"
#include "world.h"

// Imgui
#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

using namespace threepp;

int main() {

    Canvas canvas("Bilsimulator");
    GLRenderer renderer(canvas.size());
    renderer.setPixelRatio(monitor::contentScale().first);

    // Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    auto* window = static_cast<GLFWwindow*>(canvas.windowPtr());

    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 150");

    Scene scene;
    scene.background = Color::lightgray;
    PerspectiveCamera camera(75, canvas.aspect(), 0.1f, 1000.f);

    Car car;
    Vector3 startPos(-80, 0, 46.6); // Start position
    float   startYaw = 0.f;

    car.position.copy(startPos); // Restart position
    car.rotation.y = startYaw;

    scene.add(car);
    canvas.addKeyListener(car);

    World world;
    scene.add(world);

    // Reset
    auto resetGame = [&]() {
        // resets car
        car.position.copy(startPos);
        car.rotation.y = startYaw;
        car.resetState();

        // Resets all powerups in the world
        auto& powerUps = world.getPowerUps();
        for (auto& pu : powerUps) {
            pu.collected = false;

            if (pu.visual) {
                pu.visual->visible = true;

                // Rebuild trigger box
                threepp::Box3 box;
                box.setFromObject(*pu.visual);
                pu.box = box;
            }
        }
    };

    // Lights
    auto ambient = AmbientLight::create(Color(0xfff2e0), 0.6f);
    auto directional = DirectionalLight::create(Color(0xffe4b5), 1.2f);
    directional->position.set(10, 15, 10);

    auto fill = DirectionalLight::create(Color(0xcfd9ff), 0.3f);
    fill->position.set(-10, 5, -10);

    scene.add(ambient);
    scene.add(directional);
    scene.add(fill);

    // Restart listener
    struct RestartListener : KeyListener {
        std::function<void()> resetGame;

        explicit RestartListener(std::function<void()> fn)
            : resetGame(std::move(fn)) {}

        void onKeyPressed(KeyEvent evt) override {
            if (evt.key == Key::R) {
                resetGame();
            }
        }
    };

    RestartListener restartListener(resetGame);
    canvas.addKeyListener(restartListener);


    // Camera state
    float camYawOffset = 0.f;
    Clock clock;

    // Main loop
    canvas.animate([&] {
        float dt = clock.getDelta();

        Vector3 oldPos = car.position;
        car.update(dt);

        // AABB collision
        {
            Box3 carBox = car.getBoundingBox();
            const auto& colliders = world.getColliders();

            for (const auto& box : colliders) {
                if (carBox.intersectsBox(box)) {
                    car.position.copy(oldPos);
                    break;
                }
            }
        }

        // Power ups
        {
            Box3 carBox = car.getBoundingBox();
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

        // Following camera
        float steerDir = 0.0f;
        if (car.steeringLeft())  steerDir += 1.0f;
        if (car.steeringRight()) steerDir -= 1.0f;

        float targetOffset = steerDir * 0.45f;
        float followSpeed  = 1.5f;
        camYawOffset += (targetOffset - camYawOffset) * followSpeed * dt;

        float camYaw = car.rotation.y + camYawOffset;

        camera.position.set(
            car.position.x - 8 * std::cos(camYaw),
            car.position.y + 4,
            car.position.z + 8 * std::sin(camYaw)
        );

        float lookAheadDist = 3.f;
        float forwardX = std::cos(car.rotation.y);
        float forwardZ = -std::sin(car.rotation.y);

        Vector3 lookTarget(
            car.position.x + forwardX * lookAheadDist,
            car.position.y + 1,
            car.position.z + forwardZ * lookAheadDist
        );

        camera.lookAt(lookTarget);

        // Imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        // Display size check
        {
            auto s = canvas.size();
            ImGuiIO& io2 = ImGui::GetIO();
            io2.DisplaySize = ImVec2(
             static_cast<float>(s.width()),
             static_cast<float>(s.height())
            );

        }

        ImGui::NewFrame();

        // HUD window
        ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize   |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoMove    |
                ImGuiWindowFlags_NoSavedSettings;

        ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGui::Begin("HUD", nullptr, flags);

        // Speedometer
        float speed = car.getSpeedAbs();
        ImGui::Text("Speed: %.1f", speed);

        float maxSpeedVis = 50.f;
        float norm = std::min(speed / maxSpeedVis, 1.0f);
        ImGui::ProgressBar(norm, ImVec2(150, 0), "");

        ImGui::Separator();

        // Powerup indicator
        if (car.hasSpeedBoost()) {
            ImGui::Text("Power-up: SPEED x%.1f", car.getSpeedMultiplier());
        } else if (car.hasSizeBoost()) {
            ImGui::Text("Power-up: SIZE x%.1f", car.getSizeMultiplier());
        } else {
            ImGui::Text("Power-up: none");
        }

        ImGui::Separator();

        // Reset button
        if (ImGui::Button("Restart (R)")) {
    resetGame();
}

        ImGui::End();

        // Render
        renderer.render(scene, camera);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });

    // Imgui quit
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
