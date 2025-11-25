/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** loading.hpp
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class LoadingScreen {
    private:
        sf::RenderWindow* m_window;
        sf::Font* m_font;
        sf::Text m_loadingText;
        sf::Text m_progressText;
        sf::RectangleShape m_progressBar;
        sf::RectangleShape m_progressBarBackground;
        float m_animationTime;
        int m_tilesReceived;
        int m_totalTiles;
    public:
        LoadingScreen(sf::RenderWindow* window, sf::Font* font);
        ~LoadingScreen() = default;
        void setTotalTiles(int total);
        void setReceivedTiles(int received);
        void update(float deltaTime);
        void render();
        bool isComplete() const;
        int getTotalTiles() const { return m_totalTiles; }
    private:
        void updateLayout();
};
