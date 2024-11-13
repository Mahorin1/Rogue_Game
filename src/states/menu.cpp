

    #include "states/menu.hpp"

    using namespace sf;
    using namespace std;



    MenuState::MenuState(StateManager& state_man, ResourceManager& res_man, RenderWindow& window, const bool replace)
    : State(state_man, res_man, window, replace) {
        m_background_color.r = 35;
        m_background_color.g = 40;
        m_background_color.b = 50;
    
        m_titleAnimPhase = false;
        m_titlezoom_min = 1.f;
		m_titlezoom_max = 1.55f;
        m_canUseUI = true;
    
        m_blackScreen.setSize(WIN_DIMS);
        m_blackScreenCol = Color::Black;
        m_blackScreenCol.a = 0;
        m_blackScreen.setFillColor(m_blackScreenCol);
    
        m_res_man.load<Font>("Font", "fnt/Font.ttf");
        m_res_man.load<SoundBuffer>("Snd1", "sfx/UI_Click.wav");
        m_res_man.load<SoundBuffer>("Snd2", "sfx/UI_Hover.wav");
        m_res_man.load<SoundBuffer>("Snd3", "sfx/UI_unHover.wav");

        InitSoundVec(30);
    
        m_bufferPtr1 = m_res_man.get<SoundBuffer>("Snd1");
        m_bufferPtr2 = m_res_man.get<SoundBuffer>("Snd2");
        m_bufferPtr3 = m_res_man.get<SoundBuffer>("Snd3");
    
        m_text.setFont(*m_res_man.get<Font>("Font"));
        m_text.setString("Welcome To Ultra Sandbox !");
        m_text.setFillColor(Color::Red);
        m_text.setPosition(WIN_CENTER.x, WIN_CENTER.y - 270);
        m_text.setCharacterSize(44U);
        m_text.setLetterSpacing(3.f);
        m_text.setLineSpacing(1.4f);
        m_text.setOutlineColor(Color::Black);
        m_text.setOutlineThickness(4.f);
        m_text.setOrigin(m_text.getLocalBounds().getSize()/2.f);
        m_text.setScale(m_titlezoom_min, m_titlezoom_min);

        m_debugtext.setFont(*m_res_man.get<Font>("Font"));
        m_debugtext.setPosition(10, 10);
        m_debugtext.setCharacterSize(20U);
        m_debugtext.setFillColor(Color::White);
    
        Vector2f dims = Vector2f(185.f, 82.f);
        Vector2f pos1 = Vector2f(WIN_CENTER.x, WIN_CENTER.y);
        Vector2f pos2 = Vector2f(WIN_CENTER.x, WIN_CENTER.y + dims.y*1.5f);
        Vector2f pos3 = Vector2f(WIN_CENTER.x, WIN_CENTER.y + (dims.y*2.f)*1.5f);
        m_playButton = new Button(pos1, dims);
        m_optionButton = new Button(pos2, dims);
        m_quitButton = new Button(pos3, dims);
    
        music.openFromFile("res/msc/Wii.mp3");
        music.setLoop(true); //music.play();
        music.setVolume(60.f);
    
        Color panelCol(255, 255, 255, 50);
        RectangleShape PanelShape1(dims);
        RectangleShape PanelShape2(dims);
        PanelShape1.setFillColor(panelCol);
        PanelShape2.setFillColor(panelCol);
        PanelShape1.setOutlineColor(Color::White);
        PanelShape2.setOutlineColor(Color::Black);
        PanelShape1.setOutlineThickness(-8.f);
        PanelShape2.setOutlineThickness(-5.f);
    
        Text PlayText("Play", *m_res_man.get<Font>("Font"), 40U);
        PlayText.setOutlineThickness(5.f);
        PlayText.setOrigin(PlayText.getLocalBounds().getSize()/2.f);
        PlayText.setOutlineColor(Color::Black);
        PlayText.setPosition(dims.x/2.f, dims.y/2.f-1.f);
    
        Text OPtionText("Options", *m_res_man.get<Font>("Font"), 40U);
        OPtionText.setOutlineThickness(5.f);
        OPtionText.setOrigin(OPtionText.getLocalBounds().getSize()/2.f);
        OPtionText.setOutlineColor(Color::Black);
        OPtionText.setPosition(dims.x/2.f, dims.y/2.f-2.f);
    
        Text QuitText("Quit", *m_res_man.get<Font>("Font"), 40U);
        QuitText.setOutlineThickness(5.f);
        QuitText.setOrigin(QuitText.getLocalBounds().getSize()/2.f);
        QuitText.setOutlineColor(Color::Black);
        QuitText.setPosition(dims.x/2.f, dims.y/2.f-4.f);
    
        m_playButton->set();
            m_playButton->add(PanelShape1);
            m_playButton->add(PanelShape2);
            m_playButton->add(PlayText);
        m_playButton->unset();
    
        m_optionButton->set();
            m_optionButton->add(PanelShape1);
            m_optionButton->add(PanelShape2);
            m_optionButton->add(OPtionText);
        m_optionButton->unset();
    
        m_quitButton->set();
            m_quitButton->add(PanelShape1);
            m_quitButton->add(PanelShape2);
            m_quitButton->add(QuitText);
        m_quitButton->unset();
    
        m_playButton->setEvent(GuiEvent::Hover, [this](bool b) -> void {
            if (m_canUseUI) {
                float s = m_playButton->getSize();
                if (b) {
                    m_playButtonTween = tweeny::from(s).to(1.2f).during(850).via(tweeny::easing::elasticOut);
                    PlaySoundVec(m_bufferPtr2, 20.f, 1.1f);
                } else {
                    m_playButtonTween = tweeny::from(s).to(1.f).during(450).via(tweeny::easing::bounceOut);
                    PlaySoundVec(m_bufferPtr3, 40.f);
                    PlaySoundVec(m_bufferPtr3, 40.f);
                }
            }
        });
        
        m_playButton->setEvent(GuiEvent::Pressed, [this](bool b) -> void {
            if (m_canUseUI || !b) {
                float s = m_playButton->getSize();
                if (m_canUseUI && b) {
                    m_playButtonTween = tweeny::from(s).to(0.85f).during(600).via(tweeny::easing::elasticOut);
                    m_playButton->setColor(Color(100,100,100)); m_canUseUI = false;
                    PlaySoundVec(m_bufferPtr1, 60.f);
                    
                } else if (!m_blackScreenTween.isActive()) {
                    m_blackScreenTween = tweeny::from(0.f).to(255.f).during(800).via(tweeny::easing::quadraticOut);
                    m_playButtonTween = tweeny::from(s).to(1.f).during(450).via(tweeny::easing::bounceOut);
                    m_playButton->setColor(Color::White); music.pause();
                }
            }
        });
    
        m_optionButton->setEvent(GuiEvent::Hover, [this](bool b) -> void {
            if (!m_optionButton->isPressed() && m_canUseUI) {
                float s = m_optionButton->getSize();
                if (b) {
                    m_optionButtonTween = tweeny::from(s).to(1.2f).during(850).via(tweeny::easing::elasticOut);
                    PlaySoundVec(m_bufferPtr2, 20.f, 1.1f);
                } else {
                    m_optionButtonTween = tweeny::from(s).to(1.f).during(450).via(tweeny::easing::bounceOut);
                    PlaySoundVec(m_bufferPtr3, 40.f);
                }
            }
        });
        
        m_optionButton->setEvent(GuiEvent::Pressed, [this](bool b) -> void {
            if (m_canUseUI) {
                float s = m_optionButton->getSize();
                if (b) {
                    m_optionButtonTween = tweeny::from(s).to(0.85f).during(600).via(tweeny::easing::elasticOut);
                    m_optionButton->setColor(Color(100,100,100));
                    PlaySoundVec(m_bufferPtr1, 60.f);
                } else {
                    float new_s = (m_optionButton->isHover()) ? 1.2f : 1.f;
                    m_optionButton->setColor(Color::White);
                    m_optionButtonTween = tweeny::from(s).to(new_s).during(830).via(tweeny::easing::elasticOut);
                }
            }
        });
    
        m_quitButton->setEvent(GuiEvent::Hover, [this](bool b) -> void {
            if (!m_quitButton->isPressed() && m_canUseUI) {
                float s = m_quitButton->getSize();
                if (b) {
                    m_quitButtonTween = tweeny::from(s).to(1.2f).during(850).via(tweeny::easing::elasticOut);
                    PlaySoundVec(m_bufferPtr2, 20.f, 1.1f);
                } else {
                    m_quitButtonTween = tweeny::from(s).to(1.f).during(450).via(tweeny::easing::bounceOut);
                    PlaySoundVec(m_bufferPtr3, 40.f);
                }
            }
        });
        
        m_quitButton->setEvent(GuiEvent::Pressed, [this](bool b) -> void {
            if (m_canUseUI || !b) {
                float s = m_quitButton->getSize();
                if (b) {
                    m_quitButtonTween = tweeny::from(s).to(0.85f).during(600).via(tweeny::easing::elasticOut);
                    m_quitButton->setColor(Color(100,100,100)); m_canUseUI = false;
                    PlaySoundVec(m_bufferPtr1, 60.f);
                } else { unload(); m_state_man.quit(); }
            }
        });

        m_tweenHandler.set(0.01f);
        m_tweenHandler.add([this]() -> bool {
            if (m_titleTween.isActive()) {
                float v = m_titleTween.update(10);
                m_text.setScale(v, v);
            } else {
                if (!m_titleAnimPhase) {
                    m_titleTween = tweeny::from(m_titlezoom_min).to(m_titlezoom_max).during(2000).via(tweeny::easing::elasticOut);
                } else {
                    m_titleTween = tweeny::from(m_titlezoom_max).to(m_titlezoom_min).during(1800).via(tweeny::easing::bounceOut);
                } m_titleAnimPhase = !m_titleAnimPhase;
            }
            if (m_blackScreenTween.isActive()) {
                m_blackScreenCol.a = (uint8_t)m_blackScreenTween.update(1);
                m_blackScreen.setFillColor(m_blackScreenCol);
                if (!m_blackScreenTween.isActive()) {
                    //unload();
                    //m_next = StateManager::build<GameState>(m_state_man, m_res_man, m_window, false);
                }
            }
            if (m_playButtonTween.isActive()) {
                auto s = m_playButtonTween.update(10);
                m_playButton->setSize(s); m_playButton->updateAABB();
            }
            if (m_optionButtonTween.isActive()) {
                auto s = m_optionButtonTween.update(10);
                m_optionButton->setSize(s); m_optionButton->updateAABB();
            }
            if (m_quitButtonTween.isActive()) {
                auto s = m_quitButtonTween.update(10);
                m_quitButton->setSize(s); m_quitButton->updateAABB();
            } return false;
        });
    }

    MenuState::~MenuState() {
        delete m_playButton;
        delete m_optionButton;
        delete m_quitButton;
        music.stop();
        Print("Destroy The Menu");
    }

    void MenuState::unload() {
        CleanSoundVec();
        m_res_man.unload<SoundBuffer>("Snd1");
        m_res_man.unload<SoundBuffer>("Snd2");
        m_res_man.unload<SoundBuffer>("Snd3");
    }

    void MenuState::pause() {
        Print("Pause Menu");
    }

    void MenuState::resume() {
        //music.play();
        m_window.setView(m_window.getDefaultView());
        m_blackScreenCol.a = 0; m_canUseUI = true;
        m_blackScreen.setFillColor(m_blackScreenCol);
        m_res_man.load<SoundBuffer>("Snd1", "sfx/UI_Click.wav");
        m_res_man.load<SoundBuffer>("Snd2", "sfx/UI_Hover.wav");
        m_res_man.load<SoundBuffer>("Snd3", "sfx/UI_unHover.wav");
        m_bufferPtr1 = m_res_man.get<SoundBuffer>("Snd1");
        m_bufferPtr2 = m_res_man.get<SoundBuffer>("Snd2");
        m_bufferPtr3 = m_res_man.get<SoundBuffer>("Snd3");
        m_playButton->callHover(false);
        Print("Resume Menu");
    }

    void MenuState::handleEvents(Event& event) {
        switch (event.type) {
            case Event::Closed:
                m_state_man.quit();
                break;
        
            case Event::MouseButtonPressed:
                switch (event.mouseButton.button) {
                
                    case Mouse::Left:
                        m_playButton->click(true);
                        m_optionButton->click(true);
                        m_quitButton->click(true);
                        break;
                
                } break;
        
            case Event::MouseButtonReleased:
                switch (event.mouseButton.button) {
                
                    case Mouse::Left:
                        m_playButton->click(false);
                        m_optionButton->click(false);
                        m_quitButton->click(false);
                        break;
                
                } break;
        }
    }

    void MenuState::update(float dt) {
        auto mousePos = (Vector2f)Mouse::getPosition(m_window);
        m_tweenHandler.update(dt);
        m_debugtext.setString("Nb sound = " + to_string(NbSoundVec()));
        m_playButton->update(mousePos);
        m_optionButton->update(mousePos);
        m_quitButton->update(mousePos);
        UpdateSoundVec();
    }

    void MenuState::draw() {
        m_window.clear(m_background_color);
            m_window.draw(m_text);
            m_window.draw(m_debugtext);
            m_playButton->draw(m_window);
            m_optionButton->draw(m_window);
            m_quitButton->draw(m_window);
            m_window.draw(m_blackScreen);
        m_window.display();
    }

