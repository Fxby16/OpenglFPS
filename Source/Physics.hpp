#pragma once

#include <cstdint>

#include <Jolt.h>
#include <RegisterTypes.h>
#include <Core/Factory.h>
#include <Core/TempAllocator.h>
#include <Core/JobSystemThreadPool.h>
#include <Physics/PhysicsSettings.h>
#include <Physics/PhysicsSystem.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include <Physics/Collision/Shape/SphereShape.h>
#include <Physics/Collision/Shape/MeshShape.h>
#include <Physics/Body/BodyCreationSettings.h>
#include <Physics/Body/BodyActivationListener.h>
#include <Physics/Collision/ObjectLayer.h>

#include <glm.hpp>
#include <gtx/quaternion.hpp>

#include <Log.hpp>

extern void TraceImpl(const char* fmt, ...);

#ifdef JPH_ENABLE_ASSERTS

extern bool AssertFailedImpl(const char* expression, const char* message, const char* file, uint line);

#endif

namespace Layers{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
	{
		switch(inObject1){
			case Layers::NON_MOVING:
				return inObject2 == Layers::MOVING; // Non moving only collides with moving
			case Layers::MOVING:
				return true; // Moving collides with everything
			default:
				JPH_ASSERT(false);
				return false;
		}
	}
};

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface{
public:
	BPLayerInterfaceImpl()
	{
		m_ObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		m_ObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return m_ObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
			case (JPH::BroadPhaseLayer::Type) BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
			case (JPH::BroadPhaseLayer::Type) BroadPhaseLayers::MOVING:		return "MOVING";
			default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer m_ObjectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
			case Layers::NON_MOVING:
				return inLayer2 == BroadPhaseLayers::MOVING;
			case Layers::MOVING:
				return true;
			default:
				JPH_ASSERT(false);
				return false;
		}
	}
};

class MyContactListener : public JPH::ContactListener{
public:
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override
	{
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
	{
	}

	virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
	{
	}

	virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
	{
	}
};

class MyBodyActivationListener : public JPH::BodyActivationListener{
public:
	virtual void OnBodyActivated(const JPH::BodyID &inBodyID, uint64_t inBodyUserData) override
	{
	}

	virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, uint64_t inBodyUserData) override
	{
	}
};

JPH::BodyID CreateBoxShape(glm::vec3 halfExtent, glm::vec3 position, glm::quat rotation, const JPH::ObjectLayer& layer);
JPH::BodyID CreateSphereShape(glm::vec3 center, float radius, const JPH::ObjectLayer& layer);
JPH::BodyID CreateMeshShape(const std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3 position, glm::quat rotation, const JPH::ObjectLayer& layer);

inline JPH::Vec3 operator+(const JPH::Vec3 jphVec, const glm::vec3 glmVec) {
    return JPH::Vec3(jphVec.GetX() + glmVec.x, jphVec.GetY() + glmVec.y, jphVec.GetZ() + glmVec.z);
}

inline glm::vec3 operator+(const glm::vec3 glmVec, const JPH::Vec3 jphVec) {
    return glm::vec3(glmVec.x + jphVec.GetX(), glmVec.y + jphVec.GetY(), glmVec.z + jphVec.GetZ());
}

inline JPH::Vec3 operator-(const JPH::Vec3 jphVec, const glm::vec3 glmVec) {
    return JPH::Vec3(jphVec.GetX() - glmVec.x, jphVec.GetY() - glmVec.y, jphVec.GetZ() - glmVec.z);
}

inline glm::vec3 operator-(const glm::vec3 glmVec, const JPH::Vec3 jphVec) {
    return glm::vec3(glmVec.x - jphVec.GetX(), glmVec.y - jphVec.GetY(), glmVec.z - jphVec.GetZ());
}

inline JPH::Vec3 operator*(const JPH::Vec3 jphVec, float scalar) {
    return JPH::Vec3(jphVec.GetX() * scalar, jphVec.GetY() * scalar, jphVec.GetZ() * scalar);
}

inline glm::vec3 operator*(const glm::vec3 glmVec, float scalar) {
    return glm::vec3(glmVec.x * scalar, glmVec.y * scalar, glmVec.z * scalar);
}

inline glm::vec3 JPHToGLM(JPH::Vec3 jphVec) {
	return glm::vec3(jphVec.GetX(), jphVec.GetY(), jphVec.GetZ());
}

inline glm::quat JPHToGLM(JPH::Quat jphQuat){
	return glm::quat(jphQuat.GetW(), jphQuat.GetX(), jphQuat.GetY(), jphQuat.GetZ());
}

inline JPH::Vec3 GLMToJPH(glm::vec3 glmVec){
	return JPH::Vec3(glmVec.x, glmVec.y, glmVec.z);
}

inline JPH::Quat GLMToJPH(glm::quat glmQuat){
	return JPH::Quat(glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w);
}

extern void InitPhysics();
extern void DeinitPhysics();
extern void OptimizeBroadPhase();
extern JPH::TempAllocatorImpl& GetTempAllocator();
extern JPH::JobSystemThreadPool& GetJobSystem();
extern JPH::PhysicsSystem& GetPhysicsSystem();

extern void SetLinearVelocity(JPH::BodyID body_id, glm::vec3 velocity);
extern void ApplyForce(JPH::BodyID body_id, glm::vec3 force, glm::vec3 position);
extern void ApplyForce(JPH::BodyID body_id, glm::vec3 force);
extern void ApplyTorque(JPH::BodyID body_id, glm::vec3 torque);
extern void SetPosition(JPH::BodyID body_id, glm::vec3 position);
extern void SetRotation(JPH::BodyID body_id, glm::quat rotation);

inline constexpr uint MAX_PHYSICS_THREADS = 4;
inline constexpr uint MAX_PHYSICS_JOBS = 2048;
inline constexpr uint MAX_PHYSICS_BARRIERS = 1024;
inline constexpr uint MAX_BODIES = 1024;
inline constexpr uint NUM_BODY_MUTEXES = 0; // 0 to auto detect
inline constexpr uint MAX_BODY_PAIRS = 1024;
inline constexpr uint MAX_CONTACT_CONSTRAINTS = 1024;