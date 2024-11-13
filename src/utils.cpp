

    #include "utils.hpp"
    #include "PCG/pcg_basic.h"
    using namespace sf;
    using namespace std;



    Vector2f WIN_DIMS;
    Vector2f WIN_CENTER;
    void SetWinDims(Vector2f dims) {
        WIN_DIMS = dims;
        WIN_CENTER = dims / 2.f;
    }

    void Print(string t) { cout << t << endl; }



    bool CoinFlip() { return pcg32_random() % 2 == 1; }

    int RdmSign() { return (CoinFlip()) ? -1 : 1; }

    float RdmNorm() {
        return (float)pcg32_random() / (float)UINT32_MAX;
    }

    int Rdm(int min, int max) {
        return min + pcg32_boundedrand((uint32_t)(max - min + 1));
    }

    float Rdm(float min, float max) { return RdmNorm() * (max - min) + min; }



    float Damp(float v, float d, float dt)
    { return v * (1.f - d * dt); }

    Vector2f Damp(Vector2f v, float d, float dt)
    { d = (1.f - d * dt); return Vector2f(v.x *= d, v.y *= d); }



    Every::Every(float t): m_step(t), m_timer(0) {}

    void Every::set(float t) { m_step = t; }

    void Every::add(Every_CB cb) { m_callbacks.push_back(cb); }

    void Every::clean() { m_callbacks.clear(); }

    void Every::update(float dt) {
        if (!m_callbacks.empty()) {
            m_timer += dt;
            while (m_timer >= m_step) {
                m_timer -= m_step;
                m_callbacks.erase(
                    remove_if(
                        m_callbacks.begin(),
                        m_callbacks.end(),
                        [](const Every_CB& cb) { return cb(); }
                    ),
                    m_callbacks.end()
                );
            }
        }
    }

