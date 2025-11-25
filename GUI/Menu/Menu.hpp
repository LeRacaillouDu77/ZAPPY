/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** Menu.hpp
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum MenuAction {
    ACTION_NONE,
    ACTION_START,
    ACTION_SETTINGS,
    ACTION_EXIT,
    ACTION_CHANGE_THEME
};

class Menu {
    public:
        Menu(sf::RenderWindow* window,
            sf::Font& font,
            float yOffset = 0.f,
            const std::string& musicFile = "Assets/MAIN_VILLAGE/basic_music.ogg");
            MenuAction show();
            bool changeTheme(const std::string& musicFile);
    private:
        size_t m_selectedButtonIndex = 0;
        bool m_keyboardMode = true;
        const int TOTAL_BUTTONS = 4;
        sf::Text m_leftArrow;
        sf::Text m_rightArrow;
        sf::RenderWindow* m_window;
        sf::Font&         m_font;
        float             m_yOffset;
        sf::Text    m_titleText;
        sf::Text    m_startButton;
        sf::Text    m_settingsButton;
        sf::Text    m_exitButton;
        sf::Text m_changeThemeButton;
        bool m_hoverStart     = false;
        bool m_hoverSettings  = false;
        bool m_hoverExit      = false;
        bool m_hoverChangeTheme = false;
        const sf::Color NORMAL_COLOR = sf::Color::White;
        const sf::Color HOVER_COLOR  = sf::Color(200, 200, 255);
        const sf::Color CLICK_COLOR  = sf::Color(100, 100, 255);
        sf::Music m_music;
        std::string m_currentMusicFile;
        sf::Texture m_bk_texture;
        sf::Sprite  m_bk_sprite;
        void updateHoverStates();
        void draw();
        MenuAction handleInput();
};
