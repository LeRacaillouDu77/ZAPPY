/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** InGameInfoMenu.cpp
*/

#include "InGameInfoMenu.hpp"
#include "../gui.hpp"
#include <thread>
#include <chrono>
#include <string>
#include <cstring>
#include <algorithm>

InGameInfoMenu::InGameInfoMenu(sf::RenderWindow* window, sf::Font& font)
    : m_window(window), m_font(font), m_pulseAnimation(0.0f)
{
    for (int i = 0; i < 1; i++) {
        m_hoverAnimationProgress[i] = 0.0f;
    }
    initializeUI();
}

void InGameInfoMenu::initializeUI()
{
    m_titleText.setFont(m_font);
    m_titleText.setString("Menu des Informations");
    m_titleText.setCharacterSize(56);
    m_titleText.setFillColor(ACCENT_COLOR);
    m_titleText.setStyle(sf::Text::Bold);
    m_resumeButton.setFont(m_font);
    m_resumeButton.setString("Reprendre la Partie");
    m_resumeButton.setCharacterSize(36);
    m_resumeButton.setFillColor(TEXT_COLOR);

    InitializeQuantityText(m_level1, sf::Vector2f(100, 160));
    InitializeQuantityText(m_level2, sf::Vector2f(100, 235));
    InitializeQuantityText(m_level3, sf::Vector2f(100, 310));
    InitializeQuantityText(m_level4, sf::Vector2f(100, 385));
    InitializeQuantityText(m_level5, sf::Vector2f(100, 460));
    InitializeQuantityText(m_level6, sf::Vector2f(100, 535));
    InitializeQuantityText(m_level7, sf::Vector2f(100, 610));
    InitializeQuantityText(m_level8, sf::Vector2f(100, 685));

    InitializeQuantityText(m_food, sf::Vector2f(300, m_window->getSize().y * 0.74));
    InitializeQuantityText(m_linemate, sf::Vector2f(400, m_window->getSize().y * 0.74));
    InitializeQuantityText(m_deraumere, sf::Vector2f(500, m_window->getSize().y * 0.74));
    InitializeQuantityText(m_sibur, sf::Vector2f(600, m_window->getSize().y * 0.74));
    InitializeQuantityText(m_mendiane, sf::Vector2f(700, m_window->getSize().y * 0.74));
    InitializeQuantityText(m_phiras, sf::Vector2f(800, m_window->getSize().y * 0.74));
    InitializeQuantityText(m_thystame, sf::Vector2f(900, m_window->getSize().y * 0.74));

    InitializeQuantitySprite(m_sprite_level1, m_Texture_level1, sf::Vector2f(25, 150), "Assets/Game/playerlvl_1.png");
    InitializeQuantitySprite(m_sprite_level2, m_Texture_level2, sf::Vector2f(25, 225), "Assets/Game/playerlvl_2.png");
    InitializeQuantitySprite(m_sprite_level3, m_Texture_level3, sf::Vector2f(25, 300), "Assets/Game/playerlvl_3.png");
    InitializeQuantitySprite(m_sprite_level4, m_Texture_level4, sf::Vector2f(25, 375), "Assets/Game/playerlvl_4.png");
    InitializeQuantitySprite(m_sprite_level5, m_Texture_level5, sf::Vector2f(25, 450), "Assets/Game/playerlvl_5.png");
    InitializeQuantitySprite(m_sprite_level6, m_Texture_level6, sf::Vector2f(25, 525), "Assets/Game/playerlvl_6.png");
    InitializeQuantitySprite(m_sprite_level7, m_Texture_level7, sf::Vector2f(25, 600), "Assets/Game/playerlvl_7.png");
    InitializeQuantitySprite(m_sprite_level8, m_Texture_level8, sf::Vector2f(25, 675), "Assets/Game/playerlvl_8.png");

    InitializeItemsSprite(m_sprite_food, m_Texture_food, sf::Vector2f(300, m_window->getSize().y * 0.66), "Assets/Items/food_sprite.png");
    InitializeItemsSprite(m_sprite_linemate, m_Texture_linemate, sf::Vector2f(320, m_window->getSize().y * 0.57), "Assets/Items/linemate_sprite.png");
    InitializeItemsSprite(m_sprite_deraumere, m_Texture_deraumere, sf::Vector2f(415, m_window->getSize().y * 0.57), "Assets/Items/deraumere_sprite.png");
    InitializeItemsSprite(m_sprite_sibur, m_Texture_sibur, sf::Vector2f(515, m_window->getSize().y * 0.57), "Assets/Items/sibur_sprite.png");
    InitializeItemsSprite(m_sprite_mendiane, m_Texture_mendiane, sf::Vector2f(687, m_window->getSize().y * 0.66), "Assets/Items/mendiane_sprite.png");
    InitializeItemsSprite(m_sprite_phiras, m_Texture_phiras, sf::Vector2f(795, m_window->getSize().y * 0.67), "Assets/Items/phiras_sprite.png");
    InitializeItemsSprite(m_sprite_thystame, m_Texture_thystame, sf::Vector2f(890, m_window->getSize().y * 0.67), "Assets/Items/thystame_sprite.png");

    m_sprite_food.setScale(0.1, 0.1);
    m_sprite_linemate.setScale(0.4, 0.4);
    m_sprite_deraumere.setScale(0.4, 0.4);
    m_sprite_sibur.setScale(0.4, 0.4);
    m_sprite_mendiane.setScale(0.1, 0.1);
    m_sprite_phiras.setScale(0.05, 0.05);
    m_sprite_thystame.setScale(0.05, 0.05);

    m_mapDimensionsText.setFont(m_font);
    m_mapDimensionsText.setCharacterSize(24);
    m_mapDimensionsText.setFillColor(ACCENT_COLOR);
    m_mapDimensionsText.setStyle(sf::Text::Bold);
    m_mapDimensionsText.setPosition(400, 160);
    m_mapDimensionsText.setString("Dimensions de la carte: Non disponible");
    m_teamsTitle.setFont(m_font);
    m_teamsTitle.setCharacterSize(28);
    m_teamsTitle.setFillColor(ACCENT_COLOR);
    m_teamsTitle.setStyle(sf::Text::Bold);
    m_teamsTitle.setPosition(400, 210);
    m_teamsTitle.setString("Equipes:");
}

void InGameInfoMenu::InitializeQuantityText(sf::Text &text, sf::Vector2f position)
{
    text.setFont(m_font);
    text.setCharacterSize(26);
    text.setFillColor(ACCENT_COLOR);
    text.setStyle(sf::Text::Bold);
    text.setPosition(position);
    text.setString("0");
}

void InGameInfoMenu::InitializeQuantitySprite(sf::Sprite &sprite, sf::Texture &sprite_texture, sf::Vector2f position, sf::String filename)
{
    sprite_texture.loadFromFile(filename);
    sprite.setPosition(position);
    sprite.setTexture(sprite_texture);
    sprite.setTextureRect(sf::IntRect(0,0, 65, 51));
}

void InGameInfoMenu::InitializeItemsSprite(sf::Sprite &sprite, sf::Texture &sprite_texture, sf::Vector2f position, sf::String filename)
{
    sprite_texture.loadFromFile(filename);
    sprite.setPosition(position);
    sprite.setTexture(sprite_texture);
}

InGameInfoMenuAction InGameInfoMenu::show()
{
    float deltaTime = m_animationClock.restart().asSeconds();

    while (m_window->isOpen()) {
        deltaTime = m_animationClock.restart().asSeconds();
        updateAnimations(deltaTime);
        updateHoverStates();
        InGameInfoMenuAction action = handleInput();
        if (action != INGAME_INFO_NONE)
            return action;
        draw();
        m_window->display();
    }
    return INGAME_INFO_NONE;
}

void InGameInfoMenu::updateAnimations(float deltaTime)
{
    m_pulseAnimation += deltaTime * 2.0f;
    for (int i = 0; i < 1; i++) {
        bool shouldAnimate = false;
        switch (i) {
            case 0: shouldAnimate = m_hoverResume; break;
            case 1: shouldAnimate = (m_selectedSectionIndex == i && m_keyboardMode); break;
        }
        if (shouldAnimate) {
            m_hoverAnimationProgress[i] = std::min(1.0f, m_hoverAnimationProgress[i] + deltaTime * 4.0f);
        } else {
            m_hoverAnimationProgress[i] = std::max(0.0f, m_hoverAnimationProgress[i] - deltaTime * 4.0f);
        }
    }
}

void InGameInfoMenu::updateHoverStates()
{
    if (m_keyboardMode)
        return;
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*m_window));
    m_hoverResume = m_resumeButtonBg.getGlobalBounds().contains(mousePos);
}

void InGameInfoMenu::drawMenuPanel()
{
    sf::Vector2u windowSize = m_window->getSize();
    float panelWidth = 600.0f;
    float panelHeight = 700.0f;
    m_menuPanel.setSize(sf::Vector2f(panelWidth, panelHeight));
    m_menuPanel.setPosition((windowSize.x - panelWidth) / 2.0f, (windowSize.y - panelHeight) / 2.0f);
    m_menuPanel.setFillColor(PANEL_COLOR);
    sf::RectangleShape border = m_menuPanel;
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(2.0f);
    border.setOutlineColor(sf::Color(ACCENT_COLOR.r, ACCENT_COLOR.g, ACCENT_COLOR.b, 120));
    m_window->draw(m_menuPanel);
    m_window->draw(border);
    m_titlePanel.setSize(sf::Vector2f(panelWidth - 20.0f, 100.0f));
    m_titlePanel.setPosition(m_menuPanel.getPosition().x + 10.0f, m_menuPanel.getPosition().y + 10.0f);
    m_titlePanel.setFillColor(TITLE_COLOR);
    m_window->draw(m_titlePanel);
}

void InGameInfoMenu::drawModernButton(sf::Text& button, sf::RectangleShape& buttonBg, bool isActive, bool isHovered)
{
    sf::Vector2f buttonSize = calculateButtonSize(button);
    float padding = 20.0f;
    buttonBg.setSize(sf::Vector2f(buttonSize.x + padding * 2, buttonSize.y + padding));
    buttonBg.setPosition(button.getPosition().x - padding, button.getPosition().y - padding/2);
    sf::Color bgColor = BUTTON_NORMAL;
    if (isActive) {
        bgColor = BUTTON_ACTIVE;
    } else if (isHovered) {
        bgColor = BUTTON_HOVER;
    }
    buttonBg.setScale(1.0f, 1.0f);
    buttonBg.setFillColor(bgColor);
    if (isActive) {
        buttonBg.setOutlineThickness(2.0f);
        buttonBg.setOutlineColor(ACCENT_COLOR);
    } else {
        buttonBg.setOutlineThickness(0.0f);
    }
    m_window->draw(buttonBg);
    m_window->draw(button);
}

sf::Vector2f InGameInfoMenu::calculateButtonSize(const sf::Text& text)
{
    sf::FloatRect bounds = text.getLocalBounds();
    return sf::Vector2f(bounds.width, bounds.height);
}

void InGameInfoMenu::draw()
{
    m_window->setView(m_window->getDefaultView());
    sf::RectangleShape overlay;
    overlay.setSize(static_cast<sf::Vector2f>(m_window->getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    m_window->draw(overlay);
    // drawMenuPanel();
    sf::Vector2f panelPos = m_menuPanel.getPosition();
    float contentX = panelPos.x + 40.0f;
    float currentY = panelPos.y + 30.0f;
    // Titre principal
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setPosition(contentX + (1200.0f - titleBounds.width) / 2.0f, currentY);
    m_window->draw(m_titleText);
    currentY += 150.0f;
    m_window->draw(m_level1);
    m_window->draw(m_level2);
    m_window->draw(m_level3);
    m_window->draw(m_level4);
    m_window->draw(m_level5);
    m_window->draw(m_level6);
    m_window->draw(m_level7);
    m_window->draw(m_level8);
    m_window->draw(m_sprite_level1);
    m_window->draw(m_sprite_level2);
    m_window->draw(m_sprite_level3);
    m_window->draw(m_sprite_level4);
    m_window->draw(m_sprite_level5);
    m_window->draw(m_sprite_level6);
    m_window->draw(m_sprite_level7);
    m_window->draw(m_sprite_level8);
    m_window->draw(m_food);
    m_window->draw(m_linemate);
    m_window->draw(m_deraumere);
    m_window->draw(m_sibur);
    m_window->draw(m_mendiane);
    m_window->draw(m_phiras);
    m_window->draw(m_thystame);
    m_window->draw(m_sprite_food);
    m_window->draw(m_sprite_linemate);
    m_window->draw(m_sprite_deraumere);
    m_window->draw(m_sprite_sibur);
    m_window->draw(m_sprite_mendiane);
    m_window->draw(m_sprite_phiras);
    m_window->draw(m_sprite_thystame);
    m_window->draw(m_mapDimensionsText);
    m_window->draw(m_teamsTitle);
    for (const auto& teamText : m_teamTexts) {
        m_window->draw(teamText);
    }
    for (const auto& playerCountText : m_teamPlayerCountTexts) {
        m_window->draw(playerCountText);
    }
    sf::Vector2u windowSize = m_window->getSize();
    sf::FloatRect buttonBounds = m_resumeButton.getLocalBounds();
    float buttonX = (windowSize.x - buttonBounds.width) / 2.0f;
    float buttonY = windowSize.y - 80.0f;
    m_resumeButton.setPosition(buttonX, buttonY);
    bool backActive = (m_keyboardMode && m_selectedSectionIndex == 1) || m_hoverResume;
    drawModernButton(m_resumeButton, m_resumeButtonBg, backActive, m_hoverResume);
}

InGameInfoMenuAction InGameInfoMenu::handleInput()
{
    sf::Event event;

    while (m_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window->close();
            return INGAME_INFO_RESUME;
        }
        if (event.type == sf::Event::KeyPressed) {
            m_keyboardMode = true;
            if (event.key.code == sf::Keyboard::Escape) {
                return INGAME_INFO_RESUME;
            }
            if (event.key.code == sf::Keyboard::Up) {
                m_selectedSectionIndex = (m_selectedSectionIndex + TOTAL_SECTIONS - 1) % TOTAL_SECTIONS;
            }
            if (event.key.code == sf::Keyboard::Down) {
                m_selectedSectionIndex = (m_selectedSectionIndex + 1) % TOTAL_SECTIONS;
            }
            if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space) {
                switch (m_selectedSectionIndex) {
                    case 0: return INGAME_INFO_RESUME;
                    case 1: return INGAME_INFO_RESUME;
                }
            }
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            m_keyboardMode = false;
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*m_window));
                sf::FloatRect buttonBounds = m_resumeButtonBg.getGlobalBounds();
                sf::FloatRect textBounds = m_resumeButton.getGlobalBounds();
                if (buttonBounds.contains(mousePos) || textBounds.contains(mousePos)) {
                    return INGAME_INFO_RESUME;
                }
            }
        }
    }
    return INGAME_INFO_NONE;
}

void InGameInfoMenu::setquantity(sf::Text &text, int number)
{
    text.setString(std::to_string(number));
}

void InGameInfoMenu::updateTeamsAndMapInfo(const std::vector<Team>& teams, const std::vector<Player>& players, int mapWidth, int mapHeight)
{
    m_mapDimensionsText.setString("Carte: " + std::to_string(mapWidth) + " x " + std::to_string(mapHeight));
    m_teamTexts.clear();
    m_teamPlayerCountTexts.clear();
    float yPosition = 240;
    for (size_t i = 0; i < teams.size(); ++i) {
        const Team& team = teams[i];
        int playerCount = 0;
        for (const Player& player : players) {
            if (player.teamName == team.name) {
                playerCount++;
            }
        }
        sf::Text teamNameText;
        teamNameText.setFont(m_font);
        teamNameText.setCharacterSize(20);
        teamNameText.setFillColor(TEXT_COLOR);
        teamNameText.setStyle(sf::Text::Bold);
        teamNameText.setPosition(450, yPosition + 20);
        teamNameText.setString("->   " + team.name);
        m_teamTexts.push_back(teamNameText);
        sf::Text playerCountText;
        playerCountText.setFont(m_font);
        playerCountText.setCharacterSize(18);
        playerCountText.setFillColor(ACCENT_COLOR);
        playerCountText.setPosition(600, yPosition + 20);
        playerCountText.setString("(" + std::to_string(playerCount) + " joueurs)");
        m_teamPlayerCountTexts.push_back(playerCountText);
        yPosition += 30;
    }
}