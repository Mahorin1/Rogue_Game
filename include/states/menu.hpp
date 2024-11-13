

	#pragma once
	#include "state.hpp"
	#include "gui.hpp"
	#include "audio.hpp"
	#include "utils.hpp"



	class MenuState final : public State {
        public:
            MenuState(StateManager& state_man, ResourceManager& res_man , sf::RenderWindow& window, bool replace = true);
            ~MenuState();
        
            void pause() override;

            void resume() override;
        
            void handleEvents(sf::Event& event) override;

            void update(float dt) override;

            void draw() override;

            void unload();
        private:
            sf::Text m_text;
            sf::Text m_debugtext;
            sf::Color m_background_color;
            sf::Music music;
            sf::RectangleShape m_blackScreen;
            sf::Color m_blackScreenCol;
            sf::SoundBuffer* m_bufferPtr1;
            sf::SoundBuffer* m_bufferPtr2;
            sf::SoundBuffer* m_bufferPtr3;

            Every m_tweenHandler;

            Tweener<float> m_titleTween;
            Tweener<float> m_blackScreenTween;
            Tweener<float> m_playButtonTween;
            Tweener<float> m_optionButtonTween;
            Tweener<float> m_quitButtonTween;

            Button* m_playButton;
            Button* m_optionButton;
            Button* m_quitButton;

            bool m_canUseUI;
            bool m_titleAnimPhase;
            float m_titlezoom_min;
            float m_titlezoom_max;
	};

