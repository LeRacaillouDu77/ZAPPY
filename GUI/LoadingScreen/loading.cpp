/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** loading.cpp
*/

#include "loading.hpp"
#include <cmath>
#include <sstream>

LoadingScreen::LoadingScreen(sf::RenderWindow* window, sf::Font* font)
    : m_window(window), m_font(font), m_animationTime(0.0f), m_tilesReceived(0), m_totalTiles(0)
{
    m_loadingText.setFont(*m_font);
    m_loadingText.setString("Loading...");
    m_loadingText.setCharacterSize(48);
    m_loadingText.setFillColor(sf::Color::White);
    m_progressText.setFont(*m_font);
    m_progressText.setCharacterSize(24);
    m_progressText.setFillColor(sf::Color::White);
    m_progressBarBackground.setSize(sf::Vector2f(400.0f, 20.0f));
    m_progressBarBackground.setFillColor(sf::Color(50, 50, 50));
    m_progressBarBackground.setOutlineThickness(2.0f);
    m_progressBarBackground.setOutlineColor(sf::Color::White);
    m_progressBar.setSize(sf::Vector2f(0.0f, 16.0f));
    m_progressBar.setFillColor(sf::Color(0, 150, 255));
    updateLayout();
}

void LoadingScreen::setTotalTiles(int total)
{
    m_totalTiles = total;
    updateLayout();
}

void LoadingScreen::setReceivedTiles(int received)
{
    m_tilesReceived = received;
    updateLayout();
}

void LoadingScreen::update(float deltaTime)
{
    m_animationTime += deltaTime;
    int dots = static_cast<int>(m_animationTime * 2.0f) % 4;
    std::string loadingStr = "Loading";
    for (int i = 0; i < dots; ++i) {
        loadingStr += ".";
    }
    m_loadingText.setString(loadingStr);
    updateLayout();
}

void LoadingScreen::render()
{
    m_window->clear(sf::Color::Black);
    m_window->draw(m_loadingText);
    if (m_totalTiles > 0) {
        m_window->draw(m_progressBarBackground);
        m_window->draw(m_progressBar);
        m_window->draw(m_progressText);
    }
    m_window->display();
}

bool LoadingScreen::isComplete() const
{
    return m_totalTiles > 0 && m_tilesReceived >= m_totalTiles;
}

void LoadingScreen::updateLayout()
{
    sf::Vector2u windowSize = m_window->getSize();
    sf::FloatRect loadingBounds = m_loadingText.getLocalBounds();
    m_loadingText.setPosition(
        (windowSize.x - loadingBounds.width) / 2.0f,
        (windowSize.y - loadingBounds.height) / 2.0f - 50.0f
    );
    if (m_totalTiles > 0) {
        m_progressBarBackground.setPosition(
            (windowSize.x - 400.0f) / 2.0f,
            (windowSize.y / 2.0f) + 20.0f
        );
        float progress = static_cast<float>(m_tilesReceived) / static_cast<float>(m_totalTiles);
        progress = std::min(1.0f, std::max(0.0f, progress));
        m_progressBar.setSize(sf::Vector2f(396.0f * progress, 16.0f));
        m_progressBar.setPosition(
            (windowSize.x - 400.0f) / 2.0f + 2.0f,
            (windowSize.y / 2.0f) + 22.0f
        );
        std::ostringstream oss;
        oss << "Tiles loaded: " << m_tilesReceived << "/" << m_totalTiles
            << " (" << static_cast<int>(progress * 100) << "%)";
        m_progressText.setString(oss.str());
        sf::FloatRect progressBounds = m_progressText.getLocalBounds();
        m_progressText.setPosition(
            (windowSize.x - progressBounds.width) / 2.0f,
            (windowSize.y / 2.0f) + 50.0f
        );
    }
}
