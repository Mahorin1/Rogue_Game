#pragma once
#include <functional>
#include "utils.hpp"
#include "SFML/Graphics.hpp"

class Camera
{
public:
    Camera();

    void init(sf::RenderWindow *w, sf::Vector2f dims, sf::Vector2f pos = ZeroVec2f);

    void shake(float force = 10.f, float damping = 1.f);

    sf::Vector2f toCamera(sf::Vector2i pos);

    sf::Vector2i toScreen(sf::Vector2f pos);

    void set();

    void unset();

    void update(float dt);

    sf::Vector2f getMouse();

    void setZoom(float z);

    const float getZoom();

    void setPos(sf::Vector2f p);

    const sf::Vector2f getPos();

    void setAngle(float a);

    float getAngle();

    void target(sf::Vector2f p);

    sf::Vector2f getTarget();

    void enableTarget(bool b = true);

private:
    float m_zoom;
    float m_angle;
    sf::Vector2f m_dims;
    sf::Vector2f m_pos;
    sf::View m_defaultView;
    sf::View m_cameraView;
    sf::RenderWindow *m_window;

    sf::Vector2f m_shakePos;
    float m_shakeDamping;
    float m_shakeForce;
    bool m_shakeActive;

    bool m_targetMode;
    sf::Vector2f m_target;
};
