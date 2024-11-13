

    #pragma once
    #include "box2d/box2d.h"
    #include "SFML/System/Vector2.hpp"
    
    #define B2_LENGTH_UNIT 128.f



    static inline sf::Vector2f toSFVec(b2Vec2 b2_vec)
    { return {b2_vec.x, b2_vec.y}; }
    static inline b2Vec2 toB2Vec(sf::Vector2f sf_vec)
    { return {sf_vec.x, sf_vec.y}; }

    extern b2WorldId WorldID;

    b2Polygon NewRect(float width, float height);

    b2Polygon NewRoundRect(float width, float height, float radius);

    b2Polygon NewBox(float side);

    b2Polygon NewRoundBox(float side, float r);

    b2Circle NewCirc(float radius);

    b2Circle NewOffsetCirc(float offsetX, float offsetY, float radius);

    b2Capsule NewCapsule(float radius, float dist, float angle);

    b2BodyId NewDefaultBody();

    b2BodyId NewBody(b2BodyType type, b2Vec2 pos, float angle);

    void AddPoly(b2BodyId body_id, b2ShapeDef* shape_def, b2Polygon poly);

    void AddCirc(b2BodyId body_id, b2ShapeDef* shape_def, b2Circle circ);

    void AddCaps(b2BodyId body_id, b2ShapeDef* shape_def, b2Capsule caps);

    void LinearStiffness( float* stiffness, float* damping, 
                          float frequencyHertz, float dampingRatio,
                          float massA, float massB );

    bool CanGrab();

    void TryGrab(sf::Vector2f pos);

    void UpdateGrab(sf::Vector2f pos);

    void StopGrab();

    void InitWorld(float gx, float gy, float step, int substep);

    void SetWorldStep(float step, int substep);

    void UpdateWorld(float dt, float speed);

    void DestroyWorld();