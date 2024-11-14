#include <state.hpp>

using namespace sf;
using namespace std;

State::State(StateManager &state_man, ResourceManager &res_man, RenderWindow &window, const bool replace)
    : m_state_man(state_man), m_res_man(res_man), m_window(window), m_replacing(replace) {}

unique_ptr<State> State::next() { return move(m_next); }

bool State::isReplacing() { return m_replacing; }

StateManager::StateManager() : m_resume{false}, m_running{false}
{
    cout << "StateManager Init\n";
}

void StateManager::run(unique_ptr<State> state)
{
    m_states.push(move(state));
    m_running = true;
}

void StateManager::nextState()
{
    if (m_resume)
    {
        // Cleanup the current state
        if (!m_states.empty())
        {
            m_states.pop();
        }
        // Resume previous state
        if (!m_states.empty())
        {
            m_states.top()->resume();
        }
        m_resume = false;
    }
    if (!m_states.empty())
    {
        auto temp = m_states.top()->next();
        if (temp != nullptr)
        {
            if (temp->isReplacing())
            {
                // Replace the running state
                m_states.pop();
            }
            else
            {
                // Pause the running state
                m_states.top()->pause();
            }
            m_states.push(move(temp));
        }
    }
}

void StateManager::lastState() { m_resume = true; }

void StateManager::handleEvents(Event &event)
{
    m_states.top()->handleEvents(event);
}

void StateManager::update(float dt) { m_states.top()->update(dt); }

void StateManager::draw() { m_states.top()->draw(); }

bool StateManager::running() { return m_running; }

void StateManager::quit() { m_running = false; }