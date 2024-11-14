#include <gui.hpp>

using namespace sf;

Element::Element(Vector2f pos)
    : m_visible(true) { m_sprite.setPosition(pos); }

void Element::setPos(Vector2f pos) { m_sprite.setPosition(pos); }

void Element::setColor(Color col) { m_sprite.setColor(col); }

void Element::setVisible(bool v) { m_visible = v; }

const Vector2f Element::getPos() { return m_sprite.getPosition(); }

const bool Element::getVisible() { return m_visible; }

Sprite &Element::getSpr() { return m_sprite; }

const float Element::getSize() { return m_sprite.getScale().x; }

void Element::setSize(float s) { m_sprite.setScale(s, s); }

void Element::draw(RenderWindow &w)
{
    if (m_visible)
    {
        w.draw(m_sprite);
    }
}

Panel::Panel(Vector2f pos, Vector2f dims)
    : Element(pos), m_isHover(false), m_hoverSwitch(false), m_dims(dims)
{
    m_sprite.setOrigin(dims / 2.f);
    m_canvas.create(dims.x, dims.y);
    m_sprite.setTexture(m_canvas.getTexture());
    updateAABB();
}

void Panel::setEvent(GuiEvent name, Event_CB cb) { m_events[name] = cb; }

void Panel::updateAABB() { m_aabb = m_sprite.getGlobalBounds(); }

bool Panel::isHover() { return m_isHover; }

RenderTexture &Panel::getCanvas() { return m_canvas; }

const Vector2f Panel::getDims() { return m_dims; }

void Panel::callHover(bool b)
{
    m_isHover = b;
    m_hoverSwitch = b;
    if (m_haveEvent(GuiEvent::Hover))
    {
        m_events[GuiEvent::Hover](b);
    }
}

void Panel::set() { m_canvas.clear(Color::Transparent); }

void Panel::add(const Drawable &d) { m_canvas.draw(d); }

void Panel::unset() { m_canvas.display(); }

void Panel::update(Vector2f mousepos)
{
    m_isHover = m_aabb.contains(mousepos);
    if (m_isHover && !m_hoverSwitch)
    {
        callHover(true);
    }
    else if (!m_isHover && m_hoverSwitch)
    {
        callHover(false);
    }
}

Button::Button(Vector2f pos, Vector2f dims, bool constant)
    : Panel(pos, dims), m_isPressed(false), m_pressedSwitch(false), m_constant(constant) {}

bool Button::isPressed() { return m_isPressed; }

void Button::callPressed(bool b)
{
    m_isPressed = b;
    if (m_haveEvent(GuiEvent::Pressed))
    {
        m_events[GuiEvent::Pressed](b);
    }
}

void Button::click(bool b)
{
    if (m_isHover && b)
    {
        if (!m_isPressed)
        {
            callPressed(true);
        }
        else if (m_constant)
        {
            callPressed(false);
        }
    }
    else if (!b && m_isPressed && !m_constant)
    {
        callPressed(false);
    }
}
