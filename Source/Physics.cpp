#include <Physics.hpp>

#include <cstdio>
#include <cstdarg>

void TraceImpl(const char* fmt, ...)
{
    va_list list;
	va_start(list, fmt);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), fmt, list);
	va_end(list);

    printf("%s\n", buffer);
}

#ifdef JPH_ENABLE_ASSERTS

bool AssertFailedImpl(const char* expression, const char* message, const char* file, uint line)
{
    printf("%s:%d: (%s) %s\n", file, line, expression, (message != nullptr) ? message : "");
    return true;
}

#endif

static JPH::PhysicsSystem physicsSystem;
static MyBodyActivationListener bodyActivationListener;
static MyContactListener contactListener;
static BPLayerInterfaceImpl broadphaseLayerInterface;
static ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
static ObjectLayerPairFilterImpl objectVSobjectLayerFilter;
static JPH::TempAllocatorImpl* tempAllocator = nullptr;
static JPH::JobSystemThreadPool* jobSystem = nullptr;

void InitPhysics()
{
    JPH::RegisterDefaultAllocator();

    JPH::Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

    //this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
    JPH::Factory::sInstance = new JPH::Factory();

    //Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
    JPH::RegisterTypes();

    tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10 MB

    assert(MAX_PHYSICS_THREADS < std::thread::hardware_concurrency());

    jobSystem = new JPH::JobSystemThreadPool(MAX_PHYSICS_JOBS, MAX_PHYSICS_BARRIERS, MAX_PHYSICS_THREADS);

    physicsSystem.Init(MAX_BODIES, NUM_BODY_MUTEXES, MAX_BODY_PAIRS, MAX_CONTACT_CONSTRAINTS, broadphaseLayerInterface, objectVsBroadPhaseLayerFilter, objectVSobjectLayerFilter);

    physicsSystem.SetBodyActivationListener(&bodyActivationListener);
    physicsSystem.SetContactListener(&contactListener);
}

void DeinitPhysics()
{
    delete jobSystem;
    jobSystem = nullptr;
    delete tempAllocator;
    tempAllocator = nullptr;

    JPH::UnregisterTypes();

    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void OptimizeBroadPhase()
{
    GetPhysicsSystem().OptimizeBroadPhase();
}

JPH::BodyID CreateBoxShape(glm::vec3 halfExtent, glm::vec3 position, glm::quat rotation, const JPH::ObjectLayer& layer)
{
    JPH::BodyInterface &bodyInterface = GetPhysicsSystem().GetBodyInterface();

    JPH::BoxShapeSettings settings(GLMToJPH(halfExtent));
    settings.SetEmbedded();

    JPH::ShapeSettings::ShapeResult result = settings.Create();

    if(result.HasError()){
        LogError("Error creating box shape: %s", result.GetError().c_str());
        return JPH::BodyID();
    }

    JPH::ShapeRefC shape = result.Get();

    JPH::EMotionType motionType = layer == Layers::NON_MOVING ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic;
    JPH::BodyCreationSettings bodySettings(shape, GLMToJPH(position), GLMToJPH(rotation), motionType, layer);

    JPH::Body* body = bodyInterface.CreateBody(bodySettings);

    bodyInterface.AddBody(body->GetID(), JPH::EActivation::DontActivate);

    return body->GetID();
}

JPH::BodyID CreateSphereShape(glm::vec3 center, float radius, const JPH::ObjectLayer& layer)
{
    JPH::BodyInterface &bodyInterface = GetPhysicsSystem().GetBodyInterface();

    JPH::SphereShapeSettings settings(radius);
    settings.SetEmbedded();

    JPH::ShapeSettings::ShapeResult result = settings.Create();

    if(result.HasError()){
        LogError("Error creating sphere shape: %s", result.GetError().c_str());
        return JPH::BodyID();
    }

    JPH::ShapeRefC shape = result.Get();

    JPH::EMotionType motionType = layer == Layers::NON_MOVING ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic;
    JPH::BodyCreationSettings bodySettings(shape, GLMToJPH(center), JPH::Quat::sIdentity(), motionType, layer);

    JPH::Body* body = bodyInterface.CreateBody(bodySettings);

    bodyInterface.AddBody(body->GetID(), JPH::EActivation::DontActivate);

    return body->GetID();
}

JPH::BodyID CreateMeshShape(const std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, glm::vec3 position, glm::quat rotation, const JPH::ObjectLayer& layer)
{
    JPH::BodyInterface &bodyInterface = GetPhysicsSystem().GetBodyInterface();

    JPH::VertexList vertexList(vertices.size());
    JPH::IndexedTriangleList triangleList(indices.size() / 3);

    for(int i = 0; i < vertices.size(); i++){
        vertexList[i].x = vertices[i].x;
        vertexList[i].y = vertices[i].y;
        vertexList[i].z = vertices[i].z;
    }

    for(int i = 0; i < indices.size(); i += 3){
        triangleList[i / 3] = JPH::IndexedTriangle(indices[i], indices[i + 1], indices[i + 2]);
    }

    JPH::MeshShapeSettings settings(vertexList, triangleList);
    settings.SetEmbedded();

    JPH::ShapeSettings::ShapeResult result = settings.Create();

    if(result.HasError()){
        LogError("Error creating mesh shape: %s", result.GetError().c_str());
        return JPH::BodyID();
    }

    JPH::ShapeRefC shape = result.Get();

    JPH::EMotionType motionType = layer == Layers::NON_MOVING ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic;
    JPH::BodyCreationSettings bodySettings(shape, GLMToJPH(position), GLMToJPH(rotation), motionType, layer);

    JPH::Body* body = bodyInterface.CreateBody(bodySettings);

    bodyInterface.AddBody(body->GetID(), JPH::EActivation::DontActivate);

    return body->GetID();
}

JPH::TempAllocatorImpl& GetTempAllocator()
{
    return *tempAllocator;
}

JPH::JobSystemThreadPool& GetJobSystem()
{
    return *jobSystem;
}

JPH::PhysicsSystem& GetPhysicsSystem()
{
    return physicsSystem;
}

void SetLinearVelocity(JPH::BodyID body_id, glm::vec3 velocity)
{
    GetPhysicsSystem().GetBodyInterface().SetLinearVelocity(body_id, GLMToJPH(velocity));
    printf("Linear velocity set\n");
}

void ApplyForce(JPH::BodyID body_id, glm::vec3 force, glm::vec3 position)
{
    GetPhysicsSystem().GetBodyInterface().AddForce(body_id, GLMToJPH(force), GLMToJPH(position));
    printf("Force applied\n");
}

void ApplyForce(JPH::BodyID body_id, glm::vec3 force)
{
    GetPhysicsSystem().GetBodyInterface().AddForce(body_id, GLMToJPH(force));
    printf("Force applied\n");
}

void ApplyTorque(JPH::BodyID body_id, glm::vec3 torque)
{
    GetPhysicsSystem().GetBodyInterface().AddTorque(body_id, GLMToJPH(torque));
    printf("Torque applied\n");
}

void SetPosition(JPH::BodyID body_id, glm::vec3 position)
{
    GetPhysicsSystem().GetBodyInterface().SetPosition(body_id, GLMToJPH(position), JPH::EActivation::Activate);
    printf("Position set\n");
}

void SetRotation(JPH::BodyID body_id, glm::quat rotation)
{
    GetPhysicsSystem().GetBodyInterface().SetRotation(body_id, GLMToJPH(rotation), JPH::EActivation::Activate);
    printf("Rotation set\n");
}