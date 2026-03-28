//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 28/03/2026.
//

#include <Jolt/Jolt.h>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include "positron/core/log.h"
#include "positron/ecs/component/components.h"
#include "positron/ecs/core/world.h"
#include "positron/ecs/systems/physics_system.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Layers {
    static constexpr JPH::uint8 NON_MOVING = 0;
    static constexpr JPH::uint8 MOVING     = 1;
    static constexpr JPH::uint8 NUM_LAYERS = 2;
} // namespace Layers

namespace BroadPhaseLayers {
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr JPH::uint            NUM_LAYERS = 2;
} // namespace BroadPhaseLayers

class BroadPhaseLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
    BroadPhaseLayerInterfaceImpl() {
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING]     = BroadPhaseLayers::MOVING;
    }

    [[nodiscard]] JPH::uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

    [[nodiscard]] JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    [[nodiscard]] const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
        switch (static_cast<JPH::BroadPhaseLayer::Type>(inLayer)) {
            case static_cast<JPH::BroadPhaseLayer::Type>(0):
                return "NON_MOVING";
            case static_cast<JPH::BroadPhaseLayer::Type>(1):
                return "MOVING";
            default:
                JPH_ASSERT(false);
                return "INVALID";
        }
    }
#endif

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS] = {};
};

class ObjectLayerPairFilterImpl final : public JPH::ObjectLayerPairFilter {
public:
    [[nodiscard]] bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override {
        if (inLayer1 == Layers::NON_MOVING)
            return inLayer2 == Layers::MOVING;
        if (inLayer2 == Layers::NON_MOVING)
            return inLayer1 == Layers::MOVING;
        return true;
    }
};

class ObjectVsBroadPhaseLayerFilterImpl final : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    [[nodiscard]] bool ShouldCollide(JPH::ObjectLayer inLayer, JPH::BroadPhaseLayer inBpLayer) const override {
        switch (inLayer) {
            case Layers::NON_MOVING:
                return inBpLayer == BroadPhaseLayers::MOVING;
            case Layers::MOVING:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
        }
    }
};

using BodyEntityMap = std::unordered_map<uint32_t, Positron::Entity>;

class ContactListenerImpl final : public JPH::ContactListener {
public:
    ContactListenerImpl(Positron::World &world, BodyEntityMap &bodyToEntity) :
        world_(world), bodyToEntity_(bodyToEntity) {}

    JPH::ValidateResult OnContactValidate(const JPH::Body &, const JPH::Body &, JPH::RVec3Arg,
                                          const JPH::CollideShapeResult &) override {
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    void OnContactAdded(const JPH::Body &bodyA, const JPH::Body &bodyB, const JPH::ContactManifold &manifold,
                        JPH::ContactSettings &) override;

    void OnContactPersisted(const JPH::Body &bodyA, const JPH::Body &bodyB, const JPH::ContactManifold &manifold,
                            JPH::ContactSettings &) override;

    void OnContactRemoved(const JPH::SubShapeIDPair &pair) override;

    Positron::World &world_;
    BodyEntityMap   &bodyToEntity_;
};

namespace {
    JPH::Vec3 toJolt(const glm::vec3 &v) { return {v.x, v.y, v.z}; }

    glm::vec3 fromJoltVec3(const JPH::Vec3 &v) { return {v.GetX(), v.GetY(), v.GetZ()}; }

    glm::vec3 fromJoltRVec3(const JPH::RVec3 &v) { return {v.GetX(), v.GetY(), v.GetZ()}; }
} // namespace

namespace Positron {
    static bool sJoltInitialized = false;

    using JoltPhysicsSystem = JPH::PhysicsSystem;

    struct PhysicsSystem::Impl {
        std::unique_ptr<JPH::TempAllocator>                tempAlloc;
        std::unique_ptr<JPH::JobSystem>                    jobSystem;
        std::unique_ptr<BroadPhaseLayerInterfaceImpl>      bpLayerInterface;
        std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> objVsBpFilter;
        std::unique_ptr<ObjectLayerPairFilterImpl>         objLayerFilter;
        std::unique_ptr<JoltPhysicsSystem>                 physicsSystem;
        std::unique_ptr<ContactListenerImpl>               contactListener;
        JPH::BodyInterface                                *bodyInterface        = nullptr;
        bool                                               broadPhaseOptimized_ = false;
        BodyEntityMap                                      bodyToEntity;
    };

    PhysicsSystem::PhysicsSystem() : pImpl_(std::make_unique<Impl>()) {}
    PhysicsSystem::~PhysicsSystem() = default;

    void PhysicsSystem::onInit(World &world) {
        if (!sJoltInitialized) {
            JPH::RegisterDefaultAllocator();
            JPH::Factory::sInstance = new JPH::Factory();
            JPH::RegisterTypes();
            sJoltInitialized = true;
        }
        pImpl_->tempAlloc = std::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);
        pImpl_->jobSystem =
                std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 4);

        pImpl_->bpLayerInterface = std::make_unique<BroadPhaseLayerInterfaceImpl>();
        pImpl_->objVsBpFilter    = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
        pImpl_->objLayerFilter   = std::make_unique<ObjectLayerPairFilterImpl>();
        pImpl_->contactListener  = std::make_unique<ContactListenerImpl>(world, pImpl_->bodyToEntity);
        pImpl_->physicsSystem    = std::make_unique<JoltPhysicsSystem>();

        pImpl_->physicsSystem->Init(1024, 0, 4096, 2048, *pImpl_->bpLayerInterface, *pImpl_->objVsBpFilter,
                                    *pImpl_->objLayerFilter);

        pImpl_->physicsSystem->SetContactListener(pImpl_->contactListener.get());
        pImpl_->physicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));
        pImpl_->bodyInterface = &pImpl_->physicsSystem->GetBodyInterface();

        POSITRON_INFO("Physics initialised");
    }

    void PhysicsSystem::onUpdate(World &world) {
        if (!pImpl_->physicsSystem)
            return;

        if (!world.hasResource<float>())
            return;

        const float dt = world.getResource<float>();

        bool anyNewBodies = false;
        world.each<BoxColliderComponent>([&](const Entity e, const BoxColliderComponent &c) {
            if (c._bodyId == UINT32_MAX) {
                registerOrUpdateBody(e, world);
                anyNewBodies = true;
            }
        });

        world.each<MeshColliderComponent>([&](Entity e, const MeshColliderComponent &c) {
            if (c._bodyId == UINT32_MAX) {
                registerMeshBody(e, world);
                anyNewBodies = true;
            }
        });
        if (anyNewBodies || !pImpl_->broadPhaseOptimized_) {
            pImpl_->physicsSystem->OptimizeBroadPhase();
            pImpl_->broadPhaseOptimized_ = true;
        }

        pImpl_->physicsSystem->Update(dt, subSteps_, pImpl_->tempAlloc.get(), pImpl_->jobSystem.get());

        world.each<TransformComponent, BoxColliderComponent, RigidbodyComponent>(
                [&](Entity, TransformComponent &tf, const BoxColliderComponent &col, RigidbodyComponent &rb) {
                    if (col._bodyId == UINT32_MAX || rb.isKinematic)
                        return;

                    const JPH::BodyID id(col._bodyId);
                    if (!pImpl_->bodyInterface->IsActive(id))
                        return;

                    tf.position        = fromJoltRVec3(pImpl_->bodyInterface->GetPosition(id)) - col.offset;
                    rb.linearVelocity  = fromJoltVec3(pImpl_->bodyInterface->GetLinearVelocity(id));
                    rb.angularVelocity = fromJoltVec3(pImpl_->bodyInterface->GetAngularVelocity(id));

                    const JPH::Quat jq = pImpl_->bodyInterface->GetRotation(id);
                    tf.physicsRotation = glm::quat(jq.GetW(), jq.GetX(), jq.GetY(), jq.GetZ());
                });
    }

    void PhysicsSystem::onShutdown(World &world) {
        if (!pImpl_->physicsSystem)
            return;

        world.each<BoxColliderComponent>([&](Entity, BoxColliderComponent &c) {
            if (c._bodyId != UINT32_MAX) {
                const JPH::BodyID id(c._bodyId);
                pImpl_->bodyInterface->RemoveBody(id);
                pImpl_->bodyInterface->DestroyBody(id);
                pImpl_->bodyToEntity.erase(c._bodyId);
                c._bodyId = UINT32_MAX;
            }
        });

        world.each<MeshColliderComponent>([&](Entity, MeshColliderComponent &c) {
            if (c._bodyId != UINT32_MAX) {
                const JPH::BodyID id(c._bodyId);
                pImpl_->bodyInterface->RemoveBody(id);
                pImpl_->bodyInterface->DestroyBody(id);
                pImpl_->bodyToEntity.erase(c._bodyId);
                c._bodyId = UINT32_MAX;
            }
        });

        pImpl_->physicsSystem.reset();
        pImpl_->jobSystem.reset();
        pImpl_->tempAlloc.reset();
        pImpl_->bodyInterface        = nullptr;
        pImpl_->broadPhaseOptimized_ = false;
    }

    void PhysicsSystem::registerOrUpdateBody(Entity entity, World &world) const {
        auto &col = world.getComponent<BoxColliderComponent>(entity);
        auto &tf  = world.getComponent<TransformComponent>(entity);

        auto             motion = JPH::EMotionType::Static;
        JPH::ObjectLayer layer  = Layers::NON_MOVING;

        if (world.hasComponent<RigidbodyComponent>(entity)) {
            auto &rb = world.getComponent<RigidbodyComponent>(entity);
            motion   = rb.isKinematic ? JPH::EMotionType::Kinematic : JPH::EMotionType::Dynamic;
            layer    = Layers::MOVING;
        }

        JPH::BoxShapeSettings shapeSettings(toJolt(col.halfExtents * tf.scale));

        shapeSettings.mConvexRadius = 0.0f;
        auto shapeResult            = shapeSettings.Create();
        if (shapeResult.HasError()) {
            POSITRON_ERROR("Failed to create box shape: {}", shapeResult.GetError().c_str());
            return;
        }
        const JPH::Ref<JPH::Shape> &shape = shapeResult.Get();

        JPH::RVec3                pos(tf.position.x, tf.position.y, tf.position.z);
        JPH::BodyCreationSettings bcs(shape, pos, JPH::Quat::sIdentity(), motion, layer);

        if (motion == JPH::EMotionType::Dynamic) {
            auto &rb                          = world.getComponent<RigidbodyComponent>(entity);
            bcs.mOverrideMassProperties       = JPH::EOverrideMassProperties::CalculateMassAndInertia;
            bcs.mMassPropertiesOverride.mMass = rb.mass;
            bcs.mRestitution                  = rb.restitution;
            bcs.mFriction                     = rb.friction;
            bcs.mLinearDamping                = rb.linearDamping;
            bcs.mAngularDamping               = rb.angularDamping;
            bcs.mGravityFactor                = rb.useGravity ? 1.0f : 0.0f;
        }

        JPH::Body *body = pImpl_->bodyInterface->CreateBody(bcs);
        if (!body) {
            POSITRON_ERROR("Failed to create physics body");
            return;
        }

        pImpl_->bodyInterface->AddBody(body->GetID(), JPH::EActivation::Activate);
        col._bodyId                       = body->GetID().GetIndexAndSequenceNumber();
        pImpl_->bodyToEntity[col._bodyId] = entity;

        if (motion == JPH::EMotionType::Dynamic) {
            auto &rb = world.getComponent<RigidbodyComponent>(entity);
            if (rb.linearVelocity != glm::vec3(0.f))
                pImpl_->bodyInterface->SetLinearVelocity(body->GetID(), toJolt(rb.linearVelocity));
            if (rb.angularVelocity != glm::vec3(0.f))
                pImpl_->bodyInterface->SetAngularVelocity(body->GetID(), toJolt(rb.angularVelocity));
        }
    }

    void PhysicsSystem::setGravity(const float x, const float y, const float z) const {
        if (pImpl_->physicsSystem)
            pImpl_->physicsSystem->SetGravity(JPH::Vec3(x, y, z));
    }

    void PhysicsSystem::registerMeshBody(Entity entity, World &world) const {
        auto &[geometry, _bodyId] = world.getComponent<MeshColliderComponent>(entity);
        auto &tf                  = world.getComponent<TransformComponent>(entity);

        const auto &verts   = geometry.vertices;
        const auto &indices = geometry.indices;

        if (verts.empty() || indices.size() < 3) {
            POSITRON_ERROR("registerMeshBody: geometry is empty for entity {}", entity);
            return;
        }

        JPH::TriangleList triangles;
        triangles.reserve(indices.size() / 3);

        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            const auto &v0 = verts[indices[i]];
            const auto &v1 = verts[indices[i + 1]];
            const auto &v2 = verts[indices[i + 2]];

            auto scaleVert = [&](const float pos[3]) {
                return JPH::Float3(pos[0] * tf.scale.x, pos[1] * tf.scale.y, pos[2] * tf.scale.z);
            };

            triangles.emplace_back(scaleVert(v2.position), scaleVert(v1.position), scaleVert(v0.position));
        }

        JPH::MeshShapeSettings meshSettings(triangles);
        auto                   shapeResult = meshSettings.Create();
        if (shapeResult.HasError()) {
            POSITRON_ERROR("registerMeshBody: failed to create mesh shape: {}", shapeResult.GetError().c_str());
            return;
        }

        JPH::RVec3                pos(tf.position.x, tf.position.y, tf.position.z);
        JPH::BodyCreationSettings bcs(shapeResult.Get(), pos, JPH::Quat::sIdentity(), JPH::EMotionType::Static,
                                      Layers::NON_MOVING);

        JPH::Body *body = pImpl_->bodyInterface->CreateBody(bcs);
        if (!body) {
            POSITRON_ERROR("registerMeshBody: failed to create physics body");
            return;
        }

        pImpl_->bodyInterface->AddBody(body->GetID(), JPH::EActivation::DontActivate);
        _bodyId                       = body->GetID().GetIndexAndSequenceNumber();
        pImpl_->bodyToEntity[_bodyId] = entity;
    }

    static void
    fireCollisionCallback(World &world, const Entity e, const CollisionInfo &info,
                          const std::function<void(const CollisionInfo &)> CollisionCallbackComponent::*field) {
        if (!world.hasComponent<CollisionCallbackComponent>(e))
            return;
        const auto &cb = world.getComponent<CollisionCallbackComponent>(e);
        if (cb.*field)
            (cb.*field)(info);
    }

} // namespace Positron

void ContactListenerImpl::OnContactAdded(const JPH::Body &bodyA, const JPH::Body &bodyB,
                                         const JPH::ContactManifold &manifold, JPH::ContactSettings &) {
    const uint32_t idA = bodyA.GetID().GetIndexAndSequenceNumber();
    const uint32_t idB = bodyB.GetID().GetIndexAndSequenceNumber();

    const auto itA = bodyToEntity_.find(idA);
    const auto itB = bodyToEntity_.find(idB);
    if (itA == bodyToEntity_.end() || itB == bodyToEntity_.end())
        return;

    const JPH::Vec3 cp            = manifold.GetWorldSpaceContactPointOn1(0);
    const JPH::Vec3 cn            = manifold.mWorldSpaceNormal;
    const glm::vec3 contactPoint  = {cp.GetX(), cp.GetY(), cp.GetZ()};
    const glm::vec3 contactNormal = {cn.GetX(), cn.GetY(), cn.GetZ()};

    Positron::CollisionInfo infoA{};
    infoA.other         = itB->second;
    infoA.contactPoint  = contactPoint;
    infoA.contactNormal = contactNormal;

    Positron::CollisionInfo infoB{};
    infoB.other         = itA->second;
    infoB.contactPoint  = contactPoint;
    infoB.contactNormal = -contactNormal;

    Positron::fireCollisionCallback(world_, itA->second, infoA, &Positron::CollisionCallbackComponent::onContactAdded);
    Positron::fireCollisionCallback(world_, itB->second, infoB, &Positron::CollisionCallbackComponent::onContactAdded);
}

void ContactListenerImpl::OnContactPersisted(const JPH::Body &bodyA, const JPH::Body &bodyB,
                                             const JPH::ContactManifold &manifold, JPH::ContactSettings &) {
    const uint32_t idA = bodyA.GetID().GetIndexAndSequenceNumber();
    const uint32_t idB = bodyB.GetID().GetIndexAndSequenceNumber();

    const auto itA = bodyToEntity_.find(idA);
    const auto itB = bodyToEntity_.find(idB);
    if (itA == bodyToEntity_.end() || itB == bodyToEntity_.end())
        return;

    const JPH::Vec3 cp            = manifold.GetWorldSpaceContactPointOn1(0);
    const JPH::Vec3 cn            = manifold.mWorldSpaceNormal;
    const glm::vec3 contactPoint  = {cp.GetX(), cp.GetY(), cp.GetZ()};
    const glm::vec3 contactNormal = {cn.GetX(), cn.GetY(), cn.GetZ()};

    Positron::CollisionInfo infoA{};
    infoA.other         = itB->second;
    infoA.contactPoint  = contactPoint;
    infoA.contactNormal = contactNormal;

    Positron::CollisionInfo infoB{};
    infoB.other         = itA->second;
    infoB.contactPoint  = contactPoint;
    infoB.contactNormal = -contactNormal;

    Positron::fireCollisionCallback(world_, itA->second, infoA,
                                    &Positron::CollisionCallbackComponent::onContactPersisted);
    Positron::fireCollisionCallback(world_, itB->second, infoB,
                                    &Positron::CollisionCallbackComponent::onContactPersisted);
}

void ContactListenerImpl::OnContactRemoved(const JPH::SubShapeIDPair &pair) {
    const uint32_t idA = pair.GetBody1ID().GetIndexAndSequenceNumber();
    const uint32_t idB = pair.GetBody2ID().GetIndexAndSequenceNumber();

    const auto itA = bodyToEntity_.find(idA);
    const auto itB = bodyToEntity_.find(idB);
    if (itA == bodyToEntity_.end() || itB == bodyToEntity_.end())
        return;

    Positron::CollisionInfo infoA{};
    infoA.other = itB->second;

    Positron::CollisionInfo infoB{};
    infoB.other = itA->second;

    Positron::fireCollisionCallback(world_, itA->second, infoA,
                                    &Positron::CollisionCallbackComponent::onContactRemoved);
    Positron::fireCollisionCallback(world_, itB->second, infoB,
                                    &Positron::CollisionCallbackComponent::onContactRemoved);
}
