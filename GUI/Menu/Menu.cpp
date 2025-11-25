/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** Menu.cpp
*/

#include "Menu/Menu.hpp"
#include <thread>
#include <chrono>
#include <iostream>

Menu::Menu(sf::RenderWindow* window, sf::Font& font, float yOffset, const std::string& musicFile)
    : m_window(window),
      m_font(font),
      m_yOffset(yOffset),
      m_currentMusicFile(musicFile)
{
    m_leftArrow.setFont(m_font);
    m_leftArrow.setString(">");
    m_leftArrow.setCharacterSize(48);
    m_leftArrow.setFillColor(HOVER_COLOR);

    m_rightArrow.setFont(m_font);
    m_rightArrow.setString("<");
    m_rightArrow.setCharacterSize(48);
    m_rightArrow.setFillColor(HOVER_COLOR);

    m_startButton.setFont(m_font);
    m_startButton.setString("Start");
    m_startButton.setCharacterSize(48);
    m_startButton.setFillColor(NORMAL_COLOR);
    {
        sf::FloatRect bounds = m_startButton.getLocalBounds();
        float x = (m_window->getSize().x - bounds.width) / 2.0f;
        float y = 250.f + m_yOffset;
        m_startButton.setPosition(x, y);
    }
    m_settingsButton.setFont(m_font);
    m_settingsButton.setString("Settings");
    m_settingsButton.setCharacterSize(48);
    m_settingsButton.setFillColor(NORMAL_COLOR);
    {
        sf::FloatRect bounds = m_settingsButton.getLocalBounds();
        float x = (m_window->getSize().x - bounds.width) / 2.0f;
        float y = 350.f + m_yOffset;
        m_settingsButton.setPosition(x, y);
    }
    m_changeThemeButton.setFont(m_font);
    m_changeThemeButton.setString("Change Theme");
    m_changeThemeButton.setCharacterSize(36);
    m_changeThemeButton.setFillColor(NORMAL_COLOR);
    {
        sf::FloatRect bounds = m_changeThemeButton.getLocalBounds();
        float x = (m_window->getSize().x - bounds.width) / 2.0f;
        float y = 550.f + m_yOffset;
        m_changeThemeButton.setPosition(x, y);
    }
    m_exitButton.setFont(m_font);
    m_exitButton.setString("Exit");
    m_exitButton.setCharacterSize(48);
    m_exitButton.setFillColor(NORMAL_COLOR);
    {
        sf::FloatRect bounds = m_exitButton.getLocalBounds();
        float x = (m_window->getSize().x - bounds.width) / 2.0f;
        float y = 450.f + m_yOffset;
        m_exitButton.setPosition(x, y);
    }
    if (m_bk_texture.loadFromFile("Assets/MAIN_VILLAGE/ZAPPY.png")) {
        m_bk_sprite.setTexture(m_bk_texture);
    }
    if (!m_music.openFromFile(m_currentMusicFile)) {
        std::cerr << "Erreur : impossible de charger la musique '" << m_currentMusicFile << "'\n";
    } else {
        m_music.play();
        m_music.setLoop(true);
    }
}

MenuAction Menu::show()
{
    while (m_window->isOpen()) {
        updateHoverStates();
        MenuAction action = handleInput();
        if (action != ACTION_NONE) {
            return action;
        }
        draw();
        m_window->display();
    }
    return ACTION_EXIT;
}

void Menu::updateHoverStates()
{
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(
        sf::Mouse::getPosition(*m_window)
    );
    m_hoverStart    = m_startButton.getGlobalBounds().contains(mousePos);
    m_hoverSettings = m_settingsButton.getGlobalBounds().contains(mousePos);
    m_hoverChangeTheme = m_changeThemeButton.getGlobalBounds().contains(mousePos);
    m_hoverExit     = m_exitButton.getGlobalBounds().contains(mousePos);
}

void Menu::draw()
{
    m_window->clear(sf::Color::Black);

    // Affiche le fond si dispo
    if (m_bk_texture.getSize().x > 0) {
        float scaleX = static_cast<float>(m_window->getSize().x) / m_bk_texture.getSize().x;
        float scaleY = static_cast<float>(m_window->getSize().y) / m_bk_texture.getSize().y;
        m_bk_sprite.setScale(scaleX, scaleY);
        m_bk_sprite.setPosition(0.f, 0.f);
        m_window->draw(m_bk_sprite);
    }

    // Boutons et survols
    std::vector<sf::Text*> buttons = { &m_startButton, &m_settingsButton, &m_changeThemeButton, &m_exitButton };
    std::vector<bool> hoverStates = { m_hoverStart, m_hoverSettings, m_hoverChangeTheme, m_hoverExit };

    const float spacing = 40.f;

    // Calcule la hauteur totale des éléments
    float totalHeight = m_titleText.getLocalBounds().height + spacing;
    for (const auto& btn : buttons)
        totalHeight += btn->getLocalBounds().height + spacing;

    // Point de départ centré verticalement, avec un léger décalage vers le bas
    float startY = (m_window->getSize().y - totalHeight) / 2.f + 150.f;

    float titleX = (m_window->getSize().x - m_titleText.getLocalBounds().width) / 2.f;
    m_titleText.setPosition(titleX, startY);
    m_window->draw(m_titleText);

    float currentY = startY + m_titleText.getLocalBounds().height + spacing;

    for (size_t i = 0; i < buttons.size(); ++i) {
        sf::Text* btn = buttons[i];
        float posX = (m_window->getSize().x - btn->getLocalBounds().width) / 2.f;
        btn->setPosition(posX, currentY);
        bool isSelected = (m_keyboardMode && i == m_selectedButtonIndex);
        btn->setFillColor((hoverStates[i] || isSelected) ? HOVER_COLOR : NORMAL_COLOR);
        m_window->draw(*btn);

        if (isSelected) {
            float leftArrowX = posX - 60.f;  // 60px à gauche du bouton
            float arrowY = currentY + (btn->getLocalBounds().height / 2.f) - (m_leftArrow.getLocalBounds().height / 2.f);
            m_leftArrow.setPosition(leftArrowX, arrowY);
            m_window->draw(m_leftArrow);

            float rightArrowX = posX + btn->getLocalBounds().width + 20.f;  // 20px à droite du bouton
            m_rightArrow.setPosition(rightArrowX, arrowY);
            m_window->draw(m_rightArrow);
        }

        currentY += btn->getLocalBounds().height + spacing;
    }
}


MenuAction Menu::handleInput()
{
    sf::Event event;
    while (m_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window->close();
            return ACTION_EXIT;
        }
        if (event.type == sf::Event::KeyPressed) {
            m_keyboardMode = true;
            if (event.key.code == sf::Keyboard::Up) {
                m_selectedButtonIndex = (m_selectedButtonIndex - 1 + TOTAL_BUTTONS) % TOTAL_BUTTONS;
            }
            else if (event.key.code == sf::Keyboard::Down) {
                m_selectedButtonIndex = (m_selectedButtonIndex + 1) % TOTAL_BUTTONS;
            }
            else if (event.key.code == sf::Keyboard::Return) {
                switch (m_selectedButtonIndex) {
                    case 0: return ACTION_START;
                    case 1: return ACTION_SETTINGS;
                    case 2: return ACTION_CHANGE_THEME;
                    case 3: return ACTION_EXIT;
                }
            }
        }
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(
                sf::Mouse::getPosition(*m_window)
            );
            if (m_startButton.getGlobalBounds().contains(mousePos)) {
                m_startButton.setScale(0.9f, 0.9f);
                m_startButton.setFillColor(CLICK_COLOR);
                m_window->draw(m_startButton);
                m_window->display();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                m_startButton.setScale(1.f, 1.f);
                m_music.stop();
                return ACTION_START;
            }
            if (m_settingsButton.getGlobalBounds().contains(mousePos)) {
                m_settingsButton.setScale(0.9f, 0.9f);
                m_settingsButton.setFillColor(CLICK_COLOR);
                m_window->draw(m_settingsButton);
                m_window->display();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                m_settingsButton.setScale(1.f, 1.f);
                return ACTION_SETTINGS;
            }
            if (m_changeThemeButton.getGlobalBounds().contains(mousePos)) {
                m_changeThemeButton.setScale(0.9f, 0.9f);
                m_changeThemeButton.setFillColor(CLICK_COLOR);
                m_window->draw(m_changeThemeButton);
                m_window->display();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                m_changeThemeButton.setScale(1.f, 1.f);
                return ACTION_CHANGE_THEME;
            }
            if (m_exitButton.getGlobalBounds().contains(mousePos)) {
                m_exitButton.setScale(0.9f, 0.9f);
                m_exitButton.setFillColor(CLICK_COLOR);
                m_window->draw(m_exitButton);
                m_window->display();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                m_exitButton.setScale(1.f, 1.f);
                return ACTION_EXIT;
            }
        }
    }
    return ACTION_NONE;
}

bool Menu::changeTheme(const std::string& musicFile)
{
    if (musicFile == m_currentMusicFile) {
        return true;
    }
    if (m_music.getStatus() == sf::Music::Playing) {
        m_music.stop();
    }
    if (!m_music.openFromFile(musicFile)) {
        std::cerr << "Erreur : impossible de charger le thème '" << musicFile << "'\n";
        return false;
    }
    m_currentMusicFile = musicFile;
    m_music.setLoop(true);
    m_music.play();
    return true;
}
