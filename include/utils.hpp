

    #pragma once
    #include <cstdint>
    #include <iostream>
    #include <functional>
    #include "tweeny.hpp"
    #include "SFML/System/Vector2.hpp"
    #include "box2d/math_functions.h"



    extern sf::Vector2f WIN_DIMS;
    extern sf::Vector2f WIN_CENTER;
    void SetWinDims(sf::Vector2f dims);

    template <typename T> void PrintT(T t)
    { std::cout << std::to_string(t) << std::endl; }
    void Print(std::string t);

    #define ZeroVec2f (sf::Vector2f){0.f, 0.f}
    #define PI b2_pi
    constexpr float PI2 = PI * 2.f;
    constexpr float RAD90 = PI / 2.f;
    constexpr float RAD45 = PI / 4.f;
    #define TODEG(rad) ((rad) * (180.f/PI))
    #define TORAD(deg) ((deg) * (PI/180.f))

    bool CoinFlip();
    int RdmSign();
    float RdmNorm();
    int Rdm(int min, int max);
    float Rdm(float min, float max);

    float Damp(float value, float damping, float dt);
    sf::Vector2f Damp(sf::Vector2f vec, float damping, float dt);



    using Every_CB = std::function<bool()>;

    class Every {
        public:
            Every(float t = 1.f);
        
            void set(float t);

            void add(Every_CB cb);

            void clean();
        
            void update(float dt);
        private:
            float m_step, m_timer;
            std::vector<Every_CB> m_callbacks;
    };

    template<typename... Ts>
    class Tweener {
        public:
            Tweener(): m_active(false) {}

            Tweener& operator=(const tweeny::tween<Ts...>& t) {
                m_tween = t;
                m_active = true;
                return *this;
            }
        
            bool isActive() { return m_active; }
        
            const auto update(int32_t step = 1) {
                auto v = m_tween.step(step);
                m_active = m_tween.progress() != 1;
                return v;
            }
        private:
            tweeny::tween<Ts...> m_tween;
            bool m_active;
    };

