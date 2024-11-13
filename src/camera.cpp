

    #include "camera.hpp"

    using namespace sf;



    Camera::Camera(): m_shakePos(ZeroVec2f), m_angle(0.f), m_zoom(1.f),
    m_targetMode(false), m_target(ZeroVec2f), m_shakeActive(false),
    m_pos(ZeroVec2f) {}

    void Camera::init(RenderWindow* w, Vector2f dims, Vector2f pos) {
        m_window = w; m_defaultView = w->getDefaultView();
        m_pos = pos; m_dims = dims; m_cameraView.setSize(dims);
    }

    void Camera::setZoom(float z) {
        m_zoom = z; m_cameraView.setSize(m_dims * z);
    }

    const float Camera::getZoom() { return m_zoom; }

    void Camera::setPos(Vector2f pos) { m_pos = pos; }

    const Vector2f Camera::getPos() { return m_pos; }

    void Camera::setAngle(float a) { m_angle = a; }

    float Camera::getAngle() { return m_angle; }

    void Camera::update(float dt) {
        m_cameraView.setRotation(m_angle);
        m_cameraView.setCenter(m_pos + m_shakePos);
        if (m_shakeActive) {
            m_shakePos.x = 0.f;
            m_shakePos.y = 0.f;
            if (m_shakeForce < 0.2f)
            { m_shakeActive = false; } else {
                m_shakePos.x += Rdm(0-m_shakeForce, m_shakeForce);
                m_shakePos.y += Rdm(0-m_shakeForce, m_shakeForce);
                m_shakeForce = Damp(m_shakeForce, m_shakeDamping, dt);
            }
        }
    }

    Vector2f Camera::toCamera(Vector2i p)
    { return m_window->mapPixelToCoords(p, m_cameraView); }

    Vector2i Camera::toScreen(Vector2f p)
    { return m_window->mapCoordsToPixel(p, m_cameraView); }

    void Camera::set() { m_window->setView(m_cameraView); }

    void Camera::unset() { m_window->setView(m_defaultView); }

    void Camera::shake(float f, float d) {
        m_shakePos.x = 0.f, m_shakePos.y = 0.f;
        m_shakeForce = f; m_shakeDamping = d;
        m_shakeActive = true;
    }

    Vector2f Camera::getMouse()
    { return toCamera(Mouse::getPosition(*m_window)); }

