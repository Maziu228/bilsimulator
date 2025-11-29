
#pragma once
#include <btBulletDynamicsCommon.h>

struct PhysicsWorld {
    btDefaultCollisionConfiguration* config;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* world;

    PhysicsWorld() {
        config     = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(config);
        broadphase = new btDbvtBroadphase();
        solver     = new btSequentialImpulseConstraintSolver();

        world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);
        world->setGravity(btVector3(0, -9.81f, 0));
    }

    ~PhysicsWorld() {
        delete world;
        delete solver;
        delete broadphase;
        delete dispatcher;
        delete config;
    }
};
