#include "physics.hpp"
#include "enkiTS/TaskScheduler_c.h"
//test
using namespace sf;

#define MAX_TASKS 128
#define WORKER_COUNT 4
#define MAYBE_UNUSED(x) ((void)(x))
#define RL_WORLD_POINT(body_id, point) toRLVec(b2Body_GetWorldPoint(body_id, point))

float PhysicStep;
int SubStepCount;
float Accumulator;
b2WorldId WorldID;
b2JointId GrabJointId;

b2Polygon NewRect(float w, float h)
{
    return b2MakeBox(w / 2.f, h / 2.f);
}

b2Polygon NewRoundRect(float w, float h, float r)
{
    return b2MakeRoundedBox(w / 2.f, h / 2.f, r);
}

b2Polygon NewBox(float side)
{
    return b2MakeSquare(side / 2.f);
}

b2Polygon NewRoundBox(float side, float r)
{
    return b2MakeRoundedBox(side / 2.f, side / 2.f, r);
}

b2Circle NewCirc(float radius)
{
    return {{0.f, 0.f}, radius};
}

b2Circle NewOffsetCirc(float cx, float cy, float radius)
{
    return {{cx, cy}, radius};
}

b2Capsule NewCapsule(float r, float d, float a)
{
    float x, y;
    x = cosf(a) * d;
    y = sinf(a) * d;
    return {{x, y}, {-x, -y}, r};
}

b2BodyId NewDefaultBody()
{
    b2BodyDef body_def = b2DefaultBodyDef();
    return b2CreateBody(WorldID, &body_def);
}

b2BodyId NewBody(b2BodyType type, b2Vec2 pos, float angle)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = type;
    body_def.position = pos;
    body_def.rotation = b2MakeRot(angle);
    return b2CreateBody(WorldID, &body_def);
}

void AddPoly(b2BodyId id, b2ShapeDef *def, b2Polygon shape)
{
    b2CreatePolygonShape(id, def, &shape);
}

void AddCirc(b2BodyId id, b2ShapeDef *def, b2Circle shape)
{
    b2CreateCircleShape(id, def, &shape);
}

void AddCaps(b2BodyId id, b2ShapeDef *def, b2Capsule shape)
{
    b2CreateCapsuleShape(id, def, &shape);
}

void LinearStiffness(float *stiffness, float *damping,
                     float frequencyHertz, float dampingRatio,
                     float massA, float massB)
{
    float mass;
    if (massA > 0.0f && massB > 0.0f)
    {
        mass = massA * massB / (massA + massB);
    }
    else if (massA > 0.0f)
    {
        mass = massA;
    }
    else
    {
        mass = massB;
    }
    float omega = 2.0f * b2_pi * frequencyHertz;
    *stiffness = mass * omega * omega;
    *damping = 2.0f * mass * dampingRatio * omega;
}

typedef struct
{
    b2Vec2 point;
    b2BodyId bodyId;
} QueryContext;

bool QueryCallback(b2ShapeId shapeId, void *context)
{
    QueryContext *queryContext = (QueryContext *)context;
    b2BodyId bodyId = b2Shape_GetBody(shapeId);
    b2BodyType bodyType = b2Body_GetType(bodyId);
    if (bodyType != b2_dynamicBody)
    {
        return true;
    }
    if (b2Shape_TestPoint(shapeId, queryContext->point))
    {
        queryContext->bodyId = bodyId;
        return false;
    }
    return true;
}

bool CanGrab() { return !B2_IS_NON_NULL(GrabJointId); }

void TryGrab(Vector2f pos)
{
    b2Vec2 p = toB2Vec(pos);
    b2Vec2 d = {0.001f, 0.001f};
    b2AABB box = {b2Sub(p, d), b2Add(p, d)};
    QueryContext context = {p, b2_nullBodyId};
    b2World_OverlapAABB(WorldID, box, b2DefaultQueryFilter(), QueryCallback, &context);
    if (B2_IS_NON_NULL(context.bodyId))
    {
        b2MouseJointDef jdef = b2DefaultMouseJointDef();
        jdef.target = p;
        jdef.bodyIdA = NewDefaultBody();
        jdef.bodyIdB = context.bodyId;
        float massA = b2Body_GetMass(jdef.bodyIdA);
        float massB = b2Body_GetMass(jdef.bodyIdB);
        jdef.maxForce = 50000.f * massB;
        LinearStiffness(&jdef.hertz, &jdef.dampingRatio, 1.5f, 1.5f, massA, massB);
        GrabJointId = b2CreateMouseJoint(WorldID, &jdef);
        b2Body_SetAwake(context.bodyId, true);
    }
}

void UpdateGrab(Vector2f pos)
{
    if (CanGrab())
    {
        return;
    }
    if (b2Joint_IsValid(GrabJointId) == false)
    {
        GrabJointId = b2_nullJointId;
        return;
    }
    b2MouseJoint_SetTarget(GrabJointId, toB2Vec(pos));
    b2Body_SetAwake(b2Joint_GetBodyB(GrabJointId), true);
}

void StopGrab()
{
    if (CanGrab())
    {
        return;
    }
    if (b2Joint_IsValid(GrabJointId) == false)
    {
        GrabJointId = b2_nullJointId;
        return;
    }
    b2BodyId GroundId = b2Joint_GetBodyA(GrabJointId);
    b2DestroyJoint(GrabJointId);
    GrabJointId = b2_nullJointId;
    b2DestroyBody(GroundId);
}

typedef struct
{
    b2TaskCallback *box2dTask;
    void *box2dContext;
} TaskData;

enkiTaskScheduler *Scheduler;
enkiTaskSet *Tasks[MAX_TASKS];
TaskData TasksData[MAX_TASKS];
int TaskCount;

void ExecuteRangeTask(uint32_t start, uint32_t end, uint32_t threadIndex, void *context)
{
    auto data = (TaskData *)context;
    data->box2dTask(start, end, threadIndex, data->box2dContext);
}

static void *EnqueueTask(b2TaskCallback *box2dTask, int itemCount, int minRange, void *box2dContext, void *userContext)
{
    MAYBE_UNUSED(userContext);
    if (TaskCount < MAX_TASKS)
    {

        enkiTaskSet *task = Tasks[TaskCount];
        TaskData *data = &TasksData[TaskCount];
        data->box2dTask = box2dTask;
        data->box2dContext = box2dContext;

        struct enkiParamsTaskSet params;
        params.minRange = minRange;
        params.setSize = itemCount;
        params.pArgs = data;
        params.priority = 0;

        enkiSetParamsTaskSet(task, params);
        enkiAddTaskSet(Scheduler, task);
        ++TaskCount;
        return task;
    }
    else
    {

        box2dTask(0, itemCount, 0, box2dContext);
        return NULL;
    }
}

static void FinishTask(void *userTask, void *userContext)
{
    MAYBE_UNUSED(userContext);
    enkiWaitForTaskSet(Scheduler, (enkiTaskSet *)userTask);
}

void InitWorld(float gx, float gy, float step, int substep)
{
    Scheduler = enkiNewTaskScheduler();
    struct enkiTaskSchedulerConfig config = enkiGetTaskSchedulerConfig(Scheduler);
    config.numTaskThreadsToCreate = WORKER_COUNT - 1;
    enkiInitTaskSchedulerWithConfig(Scheduler, config);
    for (int i = 0; i < MAX_TASKS; ++i)
    {
        Tasks[i] = enkiCreateTaskSet(Scheduler, ExecuteRangeTask);
    }

    b2SetLengthUnitsPerMeter(B2_LENGTH_UNIT);
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity.x = gx * B2_LENGTH_UNIT;
    worldDef.gravity.y = gy * B2_LENGTH_UNIT;
    worldDef.enqueueTask = EnqueueTask;
    worldDef.finishTask = FinishTask;
    worldDef.workerCount = WORKER_COUNT;
    worldDef.enableSleep = true;
    WorldID = b2CreateWorld(&worldDef);

    SetWorldStep(step, substep);
    Accumulator = 0.f;
}

void SetWorldStep(float step, int substep)
{
    PhysicStep = step;
    SubStepCount = substep;
}

void UpdateWorld(float dt, float speed)
{
    dt *= speed;
    while (Accumulator >= PhysicStep)
    {
        b2World_Step(WorldID, PhysicStep, SubStepCount);
        TaskCount = 0;
        Accumulator -= PhysicStep;
    }
    Accumulator += dt;
}

void DestroyWorld()
{
    enkiDeleteTaskScheduler(Scheduler);
    b2DestroyWorld(WorldID);
    WorldID = b2_nullWorldId;
}
