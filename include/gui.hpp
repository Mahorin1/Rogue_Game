

	#pragma once
    #include <functional>
    #include <unordered_map>
    #include "SFML/Graphics.hpp"

    using Event_CB = std::function<void(bool)>;
	enum class GuiEvent { Hover, Pressed };



    class Element {
        public:
            Element(sf::Vector2f pos);
            
            virtual ~Element() = default;
        
            Element(const Element&) = delete;
            
            Element& operator=(const Element&) = delete;
        
            void setPos(sf::Vector2f pos);
            
            void setColor(sf::Color col);
            
            void setVisible(bool v);
            
            const sf::Vector2f getPos();
            
            const bool getVisible();
            
            sf::Sprite& getSpr();
            
            const float getSize();
            
            void setSize(float s);
            
            void draw(sf::RenderWindow& w);

            virtual void update(sf::Vector2f mousepos) = 0;
        protected:
            bool m_visible;
            sf::Sprite m_sprite;
    };



	class Panel : public Element {
        public:
            Panel(sf::Vector2f pos, sf::Vector2f dims);
        
            void setEvent(GuiEvent name, Event_CB cb);
            
            void updateAABB(); bool isHover();
            
            void callHover(bool b);
            
            const sf::Vector2f getDims();
            
            sf::RenderTexture& getCanvas();
            
            void update(sf::Vector2f mousepos) override;
            
            void set(); void unset();

            void add(const sf::Drawable& d);
        protected:
            bool m_isHover;
            bool m_hoverSwitch;
            sf::Vector2f m_dims;
            sf::FloatRect m_aabb;
            sf::RenderTexture m_canvas;
            std::unordered_map<GuiEvent, Event_CB> m_events;
            bool m_haveEvent(GuiEvent E) { return (m_events.count(E) > 0); }
	};



	class Button : public Panel {
        public:
            Button(sf::Vector2f pos, sf::Vector2f dims, bool constant = false);
        
            bool isPressed();
            
            void click(bool b);
            
            void callPressed(bool b);
        private:
            bool m_constant;
            bool m_isPressed;
            bool m_pressedSwitch;
	};

