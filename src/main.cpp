#include <threepp/threepp.hpp>
#include <threepp/canvas/Monitor.hpp>

using namespace threepp;

int main() {
    Canvas canvas("bilsimulator");
    GLRenderer renderer(canvas.size());

    renderer.setPixelRatio(monitor::contentScale().first); //sentrere bilde for mac

    Scene scene;
    scene.background = Color::aliceblue;

    PerspectiveCamera camera(55, canvas.aspect(), 0.1f, 1000.f);
    camera.position.z = 8;
    camera.lookAt({0,0,0});

    canvas.onWindowResize([&](const WindowSize& size){
        camera.aspect = size.aspect();
        camera.updateProjectionMatrix();
    });
    // Lage veien
    auto roadGeom = BoxGeometry::create(10.f, 0.1f, 50.f);
    auto roadMat = MeshBasicMaterial::create();
    roadMat->color = Color::gray;
    auto road = Mesh::create(roadGeom, roadMat);
    road->position.set(0.f, -0.5f, 0.f);
    scene.add(road);

    renderer.render(scene, camera);
    return 0;
}
