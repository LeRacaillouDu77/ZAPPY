/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** GameEndScreen.cpp
*/

#include "GameEndScreen.hpp"
#include <iostream>
#include <cmath>

const sf::Color GameEndScreen::OVERLAY_COLOR = sf::Color(0, 0, 0, 200);
const sf::Color GameEndScreen::PANEL_COLOR = sf::Color(25, 25, 35, 240);
const sf::Color GameEndScreen::TITLE_COLOR = sf::Color(45, 45, 65, 255);
const sf::Color GameEndScreen::BUTTON_NORMAL = sf::Color(60, 60, 80, 200);
const sf::Color GameEndScreen::BUTTON_HOVER = sf::Color(85, 170, 255, 220);
const sf::Color GameEndScreen::TEXT_COLOR = sf::Color(220, 220, 230, 255);
const sf::Color GameEndScreen::ACCENT_COLOR = sf::Color(100, 200, 255, 255);
const sf::Color GameEndScreen::WINNER_COLOR = sf::Color(255, 215, 0, 255);

GameEndScreen::GameEndScreen(sf::RenderWindow* window, sf::Font& font)
    : m_window(window), m_font(font), m_fadeAnimation(0.0f), m_scaleAnimation(0.0f),
      m_pulseAnimation(0.0f), m_winningTeam("Equipe Inconnue"), m_keyboardMode(true)
{
    initializeUI();
}

GameEndScreen::~GameEndScreen() = default;

void GameEndScreen::initializeUI()
{
    sf::Vector2u windowSize = m_window->getSize();
    m_backgroundOverlay.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    m_backgroundOverlay.setFillColor(OVERLAY_COLOR);
    // Main panel
    sf::Vector2f panelSize(600.0f, 400.0f);
    m_mainPanel.setSize(panelSize);
    m_mainPanel.setPosition((windowSize.x - panelSize.x) / 2.0f, (windowSize.y - panelSize.y) / 2.0f);
    m_mainPanel.setFillColor(PANEL_COLOR);
    // Title panel
    m_titlePanel.setSize(sf::Vector2f(panelSize.x, 80.0f));
    m_titlePanel.setPosition(m_mainPanel.getPosition().x, m_mainPanel.getPosition().y);
    m_titlePanel.setFillColor(TITLE_COLOR);
    // Title text
    m_titleText.setFont(m_font);
    m_titleText.setString(" PARTIE TERMINEE ");
    m_titleText.setCharacterSize(32);
    m_titleText.setFillColor(ACCENT_COLOR);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setPosition(
        m_titlePanel.getPosition().x + (m_titlePanel.getSize().x - titleBounds.width) / 2.0f,
        m_titlePanel.getPosition().y + (m_titlePanel.getSize().y - titleBounds.height) / 2.0f - 5.0f
    );
    m_congratsText.setFont(m_font);
    m_congratsText.setString("Felicitations ! ");
    m_congratsText.setCharacterSize(24);
    m_congratsText.setFillColor(TEXT_COLOR);
    sf::FloatRect congratsBounds = m_congratsText.getLocalBounds();
    m_congratsText.setPosition(
        m_mainPanel.getPosition().x + (m_mainPanel.getSize().x - congratsBounds.width) / 2.0f,
        m_mainPanel.getPosition().y + 120.0f
    );
    m_winnerText.setFont(m_font);
    m_winnerText.setCharacterSize(28);
    m_winnerText.setFillColor(WINNER_COLOR);
}

void GameEndScreen::setWinningTeam(const std::string& teamName)
{
    m_winningTeam = teamName;
    m_winnerText.setString("L'equipe \"" + teamName + "\" a gagne !");
    sf::FloatRect winnerBounds = m_winnerText.getLocalBounds();
    m_winnerText.setPosition(
        m_mainPanel.getPosition().x + (m_mainPanel.getSize().x - winnerBounds.width) / 2.0f,
        m_mainPanel.getPosition().y + 180.0f
    );
}

GameEndAction GameEndScreen::show()
{
    float deltaTime = m_animationClock.restart().asSeconds();
    updateAnimations(deltaTime);
    GameEndAction action = handleInput();
    draw();
    return action;
}

void GameEndScreen::updateAnimations(float deltaTime)
{
    if (m_fadeAnimation < 1.0f) {
        m_fadeAnimation += deltaTime * 2.0f;
        if (m_fadeAnimation > 1.0f) m_fadeAnimation = 1.0f;
    }
    if (m_scaleAnimation < 1.0f) {
        m_scaleAnimation += deltaTime * 3.0f;
        if (m_scaleAnimation > 1.0f) m_scaleAnimation = 1.0f;
    }
    m_pulseAnimation += deltaTime * 3.0f;
    float pulse = (std::sin(m_pulseAnimation) + 1.0f) * 0.5f;
    sf::Uint8 alpha = static_cast<sf::Uint8>(200 + pulse * 55);
    m_winnerText.setFillColor(sf::Color(255, 215, 0, alpha));
}

void GameEndScreen::draw()
{
    sf::Uint8 fadeAlpha = static_cast<sf::Uint8>(m_fadeAnimation * 255);
    sf::Color overlayColor = OVERLAY_COLOR;
    overlayColor.a = static_cast<sf::Uint8>(overlayColor.a * m_fadeAnimation);
    m_backgroundOverlay.setFillColor(overlayColor);
    m_window->draw(m_backgroundOverlay);
    float scale = 0.5f + (m_scaleAnimation * 0.5f);
    sf::Transform transform;
    sf::Vector2f panelCenter = m_mainPanel.getPosition() + m_mainPanel.getSize() / 2.0f;
    transform.translate(panelCenter);
    transform.scale(scale, scale);
    transform.translate(-panelCenter);
    sf::Color panelColor = PANEL_COLOR;
    panelColor.a = static_cast<sf::Uint8>(panelColor.a * m_fadeAnimation);
    m_mainPanel.setFillColor(panelColor);
    m_window->draw(m_mainPanel, transform);
    sf::Color titleColor = TITLE_COLOR;
    titleColor.a = static_cast<sf::Uint8>(titleColor.a * m_fadeAnimation);
    m_titlePanel.setFillColor(titleColor);
    m_window->draw(m_titlePanel, transform);
    sf::Color textColor = TEXT_COLOR;
    textColor.a = fadeAlpha;
    sf::Color accentColor = ACCENT_COLOR;
    accentColor.a = fadeAlpha;
    m_titleText.setFillColor(accentColor);
    m_window->draw(m_titleText, transform);
    m_congratsText.setFillColor(textColor);
    m_window->draw(m_congratsText, transform);
    sf::Color winnerColor = m_winnerText.getFillColor();
    winnerColor.a = static_cast<sf::Uint8>(winnerColor.a * m_fadeAnimation);
    m_winnerText.setFillColor(winnerColor);
    m_window->draw(m_winnerText, transform);
}

GameEndAction GameEndScreen::handleInput()
{
    sf::Event event;

    while (m_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            return GAME_END_CLOSE;
        }
        if (event.type == sf::Event::KeyPressed) {
            m_keyboardMode = true;
            if (event.key.code == sf::Keyboard::Escape ||
                event.key.code == sf::Keyboard::Return ||
                event.key.code == sf::Keyboard::Space) {
                return GAME_END_BACK_TO_MENU;
            }
        }
        if (event.type == sf::Event::MouseMoved) {
            m_keyboardMode = false;
        }
    }
    return GAME_END_NONE;
}

void GameEndScreen::drawModernButton(sf::Text& button, sf::RectangleShape& buttonBg, bool isHovered)
{
    if (isHovered) {
        buttonBg.setFillColor(BUTTON_HOVER);
        button.setFillColor(sf::Color::White);
    } else {
        buttonBg.setFillColor(BUTTON_NORMAL);
        button.setFillColor(TEXT_COLOR);
    }
}

sf::Vector2f GameEndScreen::calculateButtonSize(const sf::Text& text)
{
    sf::FloatRect bounds = text.getLocalBounds();
    return sf::Vector2f(bounds.width + 40.0f, bounds.height + 20.0f);
}
