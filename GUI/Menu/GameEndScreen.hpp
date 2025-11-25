/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** GameEndScreen.hpp
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum GameEndAction {
    GAME_END_NONE,
    GAME_END_BACK_TO_MENU,
    GAME_END_CLOSE
};

class GameEndScreen {
    public:
        GameEndScreen(sf::RenderWindow* window, sf::Font& font);
        ~GameEndScreen();
        GameEndAction show();
        void setWinningTeam(const std::string& teamName);
    private:
        void initializeUI();
        void updateAnimations(float deltaTime);
        void draw();
        GameEndAction handleInput();
        void drawModernButton(sf::Text& button, sf::RectangleShape& buttonBg, bool isHovered);
        sf::Vector2f calculateButtonSize(const sf::Text& text);
        sf::RenderWindow* m_window;
        sf::Font& m_font;

        // UI Elements
        sf::RectangleShape m_backgroundOverlay;
        sf::RectangleShape m_mainPanel;
        sf::RectangleShape m_titlePanel;
        sf::RectangleShape m_backToMenuButtonBg;

        sf::Text m_titleText;
        sf::Text m_winnerText;
        sf::Text m_congratsText;
        sf::Text m_backToMenuButton;

        sf::Clock m_animationClock;
        float m_fadeAnimation;
        float m_scaleAnimation;
        float m_pulseAnimation;
        std::string m_winningTeam;
        bool m_keyboardMode;

        static const sf::Color OVERLAY_COLOR;
        static const sf::Color PANEL_COLOR;
        static const sf::Color TITLE_COLOR;
        static const sf::Color BUTTON_NORMAL;
        static const sf::Color BUTTON_HOVER;
        static const sf::Color TEXT_COLOR;
        static const sf::Color ACCENT_COLOR;
        static const sf::Color WINNER_COLOR;
};

