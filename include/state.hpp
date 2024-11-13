

	#pragma once
    #include <stack>
    #include <memory>
    #include <iostream>
    #include <stdexcept>
    #include <unordered_map>
    #include "SFML/Graphics.hpp"



    class ResourceManager {
        public:
            template <typename ResType>
            void load(std::string id, const std::string& path) {
                if (m_resources.count(id) > 0) { return; }
                auto res = new ResType();
                if (!res->loadFromFile("res/" + path)) {
                    throw std::runtime_error("Failed to load : res/" + path);
                } m_resources[id] = res;
            }

            template <typename ResType>
            ResType* get(std::string id) {
                if (m_resources.count(id) > 0)
                { return (ResType*)m_resources[id]; }
                return nullptr;
            }

            template <typename ResType>
            void unload(std::string id) {
                if (m_resources.count(id) > 0) {
                    auto it = m_resources.find(id);
                    delete (ResType*)it->second;
                    m_resources.erase(it);
                }
            }
        private:
            std::unordered_map<std::string, void*> m_resources;
    };



    class StateManager;

    class State {
        public:
            State(StateManager& state_man, ResourceManager& res_man , sf::RenderWindow& window, bool replace = true);

            virtual ~State() = default;
        
            State(const State&) = delete;

            State& operator=(const State&) = delete;
        
            virtual void pause() = 0;

            virtual void resume() = 0;
        
            virtual void handleEvents(sf::Event& event) = 0;

            virtual void update(float dt) = 0;

            virtual void draw() = 0;
        
            std::unique_ptr<State> next();

            bool isReplacing();
        protected:
            StateManager& m_state_man;
            ResourceManager& m_res_man;
            sf::RenderWindow& m_window;
            std::unique_ptr<State> m_next;
            bool m_replacing;
    };



    class StateManager {
        public:
            StateManager();

            void run(std::unique_ptr<State> state);

            void nextState();

            void lastState();

            void handleEvents(sf::Event& event);

            void update(float dt);

            void draw();

            bool running();

            void quit();

            template <typename T>
            static std::unique_ptr<T> build(StateManager& state_man, ResourceManager& res_man , sf::RenderWindow& window, bool replace) {
                auto new_state = std::unique_ptr<T>{ nullptr };
                try { new_state = std::make_unique<T>(state_man, res_man, window, replace); }
                catch(std::runtime_error& exception) {
                    std::cout << "Creation of new state was unsuccessful\n";
                    std::cout << exception.what() << std::endl;
                } return new_state;
            }
        private:
            std::stack<std::unique_ptr<State>> m_states;
            bool m_resume, m_running;
    };

