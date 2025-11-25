/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** InGameMenu.cpp
*/

#include "InGameMenu.hpp"
#include <thread>
#include <chrono>
#include <string>
#include <cstring>
#include <algorithm>

InGameMenu::InGameMenu(sf::RenderWindow* window, sf::Font& font)
    : m_window(window), m_font(font), m_pulseAnimation(0.0f)
{
    for (int i = 0; i < 6; i++) {
        m_hoverAnimationProgress[i] = 0.0f;
    }
    initializeUI();
}

void InGameMenu::initializeUI()
{
    m_titleText.setFont(m_font);
    m_titleText.setString("Menu de Jeu");
    m_titleText.setCharacterSize(56);
    m_titleText.setFillColor(ACCENT_COLOR);
    m_titleText.setStyle(sf::Text::Bold);
    m_resumeButton.setFont(m_font);
    m_resumeButton.setString("Reprendre la Partie");
    m_resumeButton.setCharacterSize(36);
    m_resumeButton.setFillColor(TEXT_COLOR);
    m_settingsButton.setFont(m_font);
    m_settingsButton.setString("Reprendre la Partie");
    m_settingsButton.setCharacterSize(36);
    m_settingsButton.setFillColor(TEXT_COLOR);
    m_backToMenuButton.setFont(m_font);
    m_backToMenuButton.setString("Menu Principal");
    m_backToMenuButton.setCharacterSize(36);
    m_backToMenuButton.setFillColor(TEXT_COLOR);
    m_volumeLabel.setFont(m_font);
    m_volumeLabel.setString("Volume Audio");
    m_volumeLabel.setCharacterSize(32);
    m_volumeLabel.setFillColor(TEXT_COLOR);
    m_volumeValueText.setFont(m_font);
    m_volumeValueText.setCharacterSize(28);
    m_volumeValueText.setFillColor(ACCENT_COLOR);
    m_volumeValueText.setStyle(sf::Text::Bold);
    m_freqLabel.setFont(m_font);
    m_freqLabel.setString("Frequence Serveur");
    m_freqLabel.setCharacterSize(32);
    m_freqLabel.setFillColor(TEXT_COLOR);
    m_freqValueText.setFont(m_font);
    m_freqValueText.setCharacterSize(28);
    m_freqValueText.setFillColor(ACCENT_COLOR);
    m_freqValueText.setStyle(sf::Text::Bold);
    m_volumeSliderHandle.setRadius(12.0f);
    m_volumeSliderHandle.setFillColor(ACCENT_COLOR);
    m_volumeSliderHandle.setOrigin(12.0f, 12.0f);
    m_freqSliderHandle.setRadius(12.0f);
    m_freqSliderHandle.setFillColor(ACCENT_COLOR);
    m_freqSliderHandle.setOrigin(12.0f, 12.0f);
}

InGameMenuAction InGameMenu::show()
{
    float deltaTime = m_animationClock.restart().asSeconds();

    while (m_window->isOpen()) {
        deltaTime = m_animationClock.restart().asSeconds();
        updateAnimations(deltaTime);
        updateHoverStates();
        updateVolumeDisplay();
        InGameMenuAction action = handleInput();
        if (action != INGAME_NONE)
            return action;
        draw();
        m_window->display();
    }
    return INGAME_NONE;
}

void InGameMenu::updateAnimations(float deltaTime)
{
    m_pulseAnimation += deltaTime * 2.0f;
    for (int i = 0; i < 6; i++) {
        bool shouldAnimate = false;
        switch (i) {
            case 0: shouldAnimate = m_hoverResume; break;
            case 1: shouldAnimate = m_hoverSettings; break;
            case 2: shouldAnimate = m_hoverBackToMenu; break;
            case 3: shouldAnimate = m_hoverVolumeSlider; break;
            case 4: shouldAnimate = m_hoverFreqSlider; break;
            case 5: shouldAnimate = (m_selectedSectionIndex == i && m_keyboardMode); break;
        }
        if (shouldAnimate) {
            m_hoverAnimationProgress[i] = std::min(1.0f, m_hoverAnimationProgress[i] + deltaTime * 4.0f);
        } else {
            m_hoverAnimationProgress[i] = std::max(0.0f, m_hoverAnimationProgress[i] - deltaTime * 4.0f);
        }
    }
}

void InGameMenu::updateHoverStates()
{
    if (m_keyboardMode)
        return;
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*m_window));
    m_hoverResume = m_resumeButtonBg.getGlobalBounds().contains(mousePos);
    m_hoverSettings = m_settingsButtonBg.getGlobalBounds().contains(mousePos);
    m_hoverBackToMenu = m_backToMenuButtonBg.getGlobalBounds().contains(mousePos);
    m_hoverVolumeSlider = m_volumeSliderBg.getGlobalBounds().contains(mousePos);
    m_hoverFreqSlider = m_freqSliderBg.getGlobalBounds().contains(mousePos);
}

void InGameMenu::drawMenuPanel()
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

void InGameMenu::drawModernButton(sf::Text& button, sf::RectangleShape& buttonBg, bool isActive, bool isHovered)
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

sf::Vector2f InGameMenu::calculateButtonSize(const sf::Text& text)
{
    sf::FloatRect bounds = text.getLocalBounds();
    return sf::Vector2f(bounds.width, bounds.height);
}

void InGameMenu::drawSlider(sf::Vector2f position, float width, float value, float maxValue, const std::string& label, bool isActive)
{
    float sliderHeight = 8.0f;
    float handleRadius = 12.0f;
    sf::RectangleShape sliderBg;
    sliderBg.setSize(sf::Vector2f(width, sliderHeight));
    sliderBg.setPosition(position);
    sliderBg.setFillColor(SLIDER_BG);
    if (label == "Volume") {
        m_volumeSliderBg = sliderBg;
    } else if (label == "Frequency") {
        m_freqSliderBg = sliderBg;
    }
    float fillWidth = (value / maxValue) * width;
    sf::RectangleShape sliderFill;
    sliderFill.setSize(sf::Vector2f(fillWidth, sliderHeight));
    sliderFill.setPosition(position);
    sliderFill.setFillColor(SLIDER_FILL);
    float handleX = position.x + fillWidth;
    float handleY = position.y + sliderHeight / 2.0f;
    sf::CircleShape handle;
    handle.setRadius(handleRadius);
    handle.setOrigin(handleRadius, handleRadius);
    handle.setPosition(handleX, handleY);
    sf::Color handleColor = ACCENT_COLOR;
    if (isActive) {
        handleColor.a = 255;
        handle.setRadius(handleRadius * 1.2f);
        handle.setOrigin(handleRadius * 1.2f, handleRadius * 1.2f);
    } else {
        handleColor.a = 200;
    }
    handle.setFillColor(handleColor);
    handle.setOutlineThickness(2.0f);
    handle.setOutlineColor(sf::Color::White);
    m_window->draw(sliderBg);
    m_window->draw(sliderFill);
    m_window->draw(handle);
}

void InGameMenu::draw()
{
    m_window->setView(m_window->getDefaultView());
    sf::RectangleShape overlay;
    overlay.setSize(static_cast<sf::Vector2f>(m_window->getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    m_window->draw(overlay);
    drawMenuPanel();
    sf::Vector2f panelPos = m_menuPanel.getPosition();
    float contentX = panelPos.x + 40.0f;
    float currentY = panelPos.y + 30.0f;
    // Titre principal
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setPosition(contentX + (520.0f - titleBounds.width) / 2.0f, currentY);
    m_window->draw(m_titleText);
    currentY += 150.0f;
    // Section Volume
    m_volumeLabel.setPosition(contentX, currentY);
    m_window->draw(m_volumeLabel);
    currentY += 60.0f;
    // Slider Volume
    float sliderWidth = 400.0f;
    float volumeValue = m_sharedVolume ? *m_sharedVolume : 50;
    drawSlider(sf::Vector2f(contentX + 60.0f, currentY), sliderWidth, volumeValue, 100.0f, "Volume", 
               (m_keyboardMode && m_selectedSectionIndex == 1) || m_hoverVolumeSlider);
    m_volumeValueText.setString(std::to_string((int)volumeValue) + "%");
    m_volumeValueText.setPosition(contentX + sliderWidth + 80.0f, currentY - 15.0f);
    m_window->draw(m_volumeValueText);
    currentY += 80.0f;
    m_freqLabel.setPosition(contentX, currentY);
    m_window->draw(m_freqLabel);
    currentY += 60.0f;
    drawSlider(sf::Vector2f(contentX + 60.0f, currentY), sliderWidth, m_serverFrequency, 1000.0f, "Frequency",
        (m_keyboardMode && m_selectedSectionIndex == 2) || m_hoverFreqSlider);
    m_freqValueText.setString(std::to_string(m_serverFrequency));
    m_freqValueText.setPosition(contentX + sliderWidth + 80.0f, currentY - 15.0f);
    m_window->draw(m_freqValueText);
    currentY += 100.0f;
    // Bouton Paramètres
    m_settingsButton.setPosition(contentX + 50.0f, currentY);
    bool settingsActive = (m_keyboardMode && m_selectedSectionIndex == 3) || m_hoverSettings;
    drawModernButton(m_settingsButton, m_settingsButtonBg, settingsActive, m_hoverSettings);
    currentY += 80.0f;
    // Bouton Retour au menu
    m_backToMenuButton.setPosition(contentX + 110.0f, currentY);
    bool backActive = (m_keyboardMode && m_selectedSectionIndex == 4) || m_hoverBackToMenu;
    drawModernButton(m_backToMenuButton, m_backToMenuButtonBg, backActive, m_hoverBackToMenu);
}

InGameMenuAction InGameMenu::handleInput()
{
    sf::Event event;

    while (m_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window->close();
            return INGAME_BACK_TO_MENU;
        }
        if (event.type == sf::Event::KeyPressed) {
            m_keyboardMode = true;
            if (event.key.code == sf::Keyboard::Escape) {
                return INGAME_RESUME;
            }
            if (event.key.code == sf::Keyboard::Up) {
                m_selectedSectionIndex = (m_selectedSectionIndex + TOTAL_SECTIONS - 1) % TOTAL_SECTIONS;
            }
            if (event.key.code == sf::Keyboard::Down) {
                m_selectedSectionIndex = (m_selectedSectionIndex + 1) % TOTAL_SECTIONS;
            }
            if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space) {
                switch (m_selectedSectionIndex) {
                    case 0: return INGAME_RESUME;
                    case 3: return INGAME_RESUME;
                    case 4: return INGAME_BACK_TO_MENU;
                }
            }
            if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
                int delta = (event.key.code == sf::Keyboard::Right) ? 1 : -1;
                if (m_selectedSectionIndex == 1 && m_sharedVolume != nullptr) {
                    *m_sharedVolume = std::max(0, std::min(100, *m_sharedVolume + delta * 5));
                    setSystemVolume(*m_sharedVolume);
                } else if (m_selectedSectionIndex == 2) {
                    m_serverFrequency = std::max(1, std::min(1000, m_serverFrequency + delta * 10));
                    if (m_sendCommandToServer) {
                        m_sendCommandToServer("sst " + std::to_string(m_serverFrequency) + "\\n");
                    }
                }
            }
        }
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*m_window));
            if (m_resumeButtonBg.getGlobalBounds().contains(mousePos)) {
                return INGAME_RESUME;
            }
            if (m_settingsButtonBg.getGlobalBounds().contains(mousePos)) {
                return INGAME_RESUME;
            }
            if (m_backToMenuButtonBg.getGlobalBounds().contains(mousePos)) {
                return INGAME_BACK_TO_MENU;
            }
            if (m_volumeSliderBg.getGlobalBounds().contains(mousePos)) {
                m_isDraggingVolume = true;
                updateSliderFromMouse(mousePos, true);
            }
            if (m_freqSliderBg.getGlobalBounds().contains(mousePos)) {
                m_isDraggingFreq = true;
                updateSliderFromMouse(mousePos, false);
            }
        }
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            m_isDraggingVolume = false;
            m_isDraggingFreq = false;
        }
        if (event.type == sf::Event::MouseMoved) {
            m_keyboardMode = false;
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*m_window));
            if (m_isDraggingVolume) {
                updateSliderFromMouse(mousePos, true);
            }
            if (m_isDraggingFreq) {
                updateSliderFromMouse(mousePos, false);
            }
        }
    }
    return INGAME_NONE;
}

void InGameMenu::updateSliderFromMouse(sf::Vector2f mousePos, bool isVolume)
{
    sf::RectangleShape& slider = isVolume ? m_volumeSliderBg : m_freqSliderBg;
    sf::FloatRect bounds = slider.getGlobalBounds();
    float relativeX = mousePos.x - bounds.left;
    float percentage = std::max(0.0f, std::min(1.0f, relativeX / bounds.width));
    if (isVolume && m_sharedVolume != nullptr) {
        *m_sharedVolume = static_cast<int>(percentage * 100);
        setSystemVolume(*m_sharedVolume);
    } else if (!isVolume) {
        m_serverFrequency = static_cast<int>(percentage * 999) + 1;
        if (m_sendCommandToServer) {
            m_sendCommandToServer("sst " + std::to_string(m_serverFrequency) + "\\n");
        }
    }
}

void InGameMenu::setVolumePointer(int* sharedVolumePtr)
{
    m_sharedVolume = sharedVolumePtr;
}

void InGameMenu::setCommandSender(std::function<void(const std::string&)> func)
{
    m_sendCommandToServer = func;
}

void InGameMenu::changeVolume(int delta)
{
    if (m_sharedVolume != nullptr) {
        *m_sharedVolume = std::max(0, std::min(100, *m_sharedVolume + delta));
        setSystemVolume(*m_sharedVolume);
    }
}

void InGameMenu::updateVolumeDisplay()
{
    if (m_sharedVolume != nullptr) {
        m_volumeValueText.setString(std::to_string(*m_sharedVolume) + "%");
    } else {
        m_volumeValueText.setString("--");
    }
    m_freqValueText.setString(std::to_string(m_serverFrequency));
}

bool InGameMenu::setSystemVolume(int newVol)
{
    #if defined(__APPLE__)
        std::string cmd = "osascript -e 'set volume output volume " + std::to_string(newVol) + "'";
        return (system(cmd.c_str()) == 0);
    #elif defined(__linux__)
        std::string cmd = "amixer set Master " + std::to_string(newVol) + "% > /dev/null 2>&1";
        return (system(cmd.c_str()) == 0);
    #else
        return false;
    #endif
}
