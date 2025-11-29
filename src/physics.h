// Physics.h
#pragma once
#include <btBulletDynamicsCommon.h>

struct Physics {
    btDefaultCollisionConfiguration* config;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* world;

    Physics();   // creates world + gravity
    ~Physics();  // deletes stuff

    btRigidBody* createStaticBox(const btVector3& halfExtents,
                                 const btVector3& pos);
    btRigidBody* createDynamicBox(const btVector3& halfExtents,
                                  float mass,
                                  const btVector3& pos);
};
