/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** InGameMenu.hpp
*/

#pragma once
#include <SFML/Graphics.hpp>
#include "Menu/Menu.hpp"
#include "Settings/Setting.hpp"
#include <functional>
#include <cmath>

enum InGameMenuAction {
    INGAME_NONE,
    INGAME_RESUME,
    INGAME_VOLUME,
    INGAME_BACK_TO_MENU,
    INGAME_SETTINGS,
    INGAME_SAVE_GAME
};

class InGameMenu {
    public:
        InGameMenu(sf::RenderWindow* window, sf::Font& font);
        InGameMenuAction show();
        void setVolumePointer(int* sharedVolumePtr);
        void setCommandSender(std::function<void(const std::string&)> func);

    private:
        std::function<void(const std::string&)> m_sendCommandToServer;
        void updateHoverStates();
        void draw();
        void drawModernButton(sf::Text& button, sf::RectangleShape& buttonBg, bool isActive, bool isHovered);
        void drawSlider(sf::Vector2f position, float width, float value, float maxValue, const std::string& label, bool isActive);
        void drawMenuPanel();
        void updateAnimations(float deltaTime);
        InGameMenuAction handleInput();
        void changeVolume(int delta);
        void updateVolumeDisplay();
        void updateSliderFromMouse(sf::Vector2f mousePos, bool isVolume);
        sf::Vector2f calculateButtonSize(const sf::Text& text);
        void initializeUI();
        bool setSystemVolume(int newVol);

        sf::RenderWindow* m_window;
        sf::Font& m_font;

        sf::RectangleShape m_menuPanel;
        sf::RectangleShape m_titlePanel;
        sf::RectangleShape m_resumeButtonBg;
        sf::RectangleShape m_settingsButtonBg;
        sf::RectangleShape m_backToMenuButtonBg;

        sf::Text m_resumeButton;
        sf::Text m_settingsButton;

        sf::RectangleShape m_volumeSliderBg;
        sf::RectangleShape m_volumeSliderFill;
        sf::CircleShape m_volumeSliderHandle;

        sf::RectangleShape m_freqSliderBg;
        sf::RectangleShape m_freqSliderFill;
        sf::CircleShape m_freqSliderHandle;

        sf::Text m_volumeLabel;
        sf::Text m_freqLabel;
        sf::Text m_freqValueText;

        sf::Clock m_animationClock;
        float m_hoverAnimationProgress[6];
        float m_pulseAnimation;

        const sf::Color PANEL_COLOR = sf::Color(25, 25, 35, 240);
        const sf::Color TITLE_COLOR = sf::Color(45, 45, 65, 255);
        const sf::Color BUTTON_NORMAL = sf::Color(60, 60, 80, 200);
        const sf::Color BUTTON_HOVER = sf::Color(85, 170, 255, 220);
        const sf::Color BUTTON_ACTIVE = sf::Color(120, 200, 255, 255);
        const sf::Color TEXT_COLOR = sf::Color(220, 220, 230, 255);
        const sf::Color ACCENT_COLOR = sf::Color(100, 200, 255, 255);
        const sf::Color SLIDER_BG = sf::Color(40, 40, 50, 200);
        const sf::Color SLIDER_FILL = sf::Color(85, 170, 255, 255);

        sf::Text m_backToMenuButton;
        sf::Text m_volumeValueText;
        sf::Text m_leftArrow;
        sf::Text m_rightArrow;
        sf::Text m_titleText;

        bool m_keyboardMode = true;
        int m_selectedSectionIndex = 0;
        static const int TOTAL_SECTIONS = 6;

        // États de survol modernisés
        bool m_hoverResume = false;
        bool m_hoverSettings = false;
        bool m_hoverBackToMenu = false;
        bool m_hoverVolumeSlider = false;
        bool m_hoverFreqSlider = false;
        bool m_isDraggingVolume = false;
        bool m_isDraggingFreq = false;

        int *m_sharedVolume = nullptr;

        int m_serverFrequency = 100;
        sf::Vector2f m_lastMousePos;
};
