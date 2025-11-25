/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** caca.cpp
*/

#include "gui.hpp"

Menu* ZappyGUI::getMenu() {
    return m_menu.get();
}

const Menu* ZappyGUI::getMenu() const {
    return m_menu.get();
}

void ZappyGUI::setMenu(std::unique_ptr<Menu> menu) {
    m_menu = std::move(menu);
}

Theme* ZappyGUI::getTheme() {
    return m_theme.get();
}

const Theme* ZappyGUI::getTheme() const {
    return m_theme.get();
}

void ZappyGUI::setTheme(std::unique_ptr<Theme> theme) {
    m_theme = std::move(theme);
}

Setting* ZappyGUI::getSetting() {
    return m_setting.get();
}

const Setting* ZappyGUI::getSetting() const {
    return m_setting.get();
}

void ZappyGUI::setSetting(std::unique_ptr<Setting> setting) {
    m_setting = std::move(setting);
}

MenuAction ZappyGUI::getCurrentAction() const {
    return m_currentAction;
}

void ZappyGUI::setCurrentAction(MenuAction action) {
    m_currentAction = action;
}

Themes_enum ZappyGUI::getCurrentTheme() const {
    if (m_theme) {
        return m_theme->getCurrentTheme();
    }
    return Themes_enum::THEME_BASE; // Default theme if not set
}

void ZappyGUI::setCurrentTheme(Themes_enum theme) {
    m_currentTheme = theme;
}

float ZappyGUI::getIncantationAnimationDuration() const
{
    int actualServerFrequency = 100;

    if (m_network) {
        actualServerFrequency = m_network->getServerFrequency();
    }
    float duration = 300.0f / static_cast<float>(actualServerFrequency);

    return std::max(1.0f, std::min(300.0f, duration));
}

int ZappyGUI::getCurrentVolume() const {
    return m_currentVolume;
}

void ZappyGUI::setCurrentVolume(int vol) {
    m_currentVolume = std::clamp(vol, 0, 100);
}

void ZappyGUI::updateLoadingProgress() {
    if (m_loadingScreen && m_isLoading) {
        m_loadingScreen->setReceivedTiles(m_receivedTiles);
    }
}

void ZappyGUI::onConnectionStatusChanged(bool connected, const std::string& statusMessage)
{
    m_connected = connected;
    m_statusText.setString(statusMessage);
    m_statusText.setFillColor(connected ? sf::Color::Green : sf::Color::Red);
}

void ZappyGUI::onLoadingStatusChanged(bool loading, int received, int expected)
{
    m_isLoading = loading;
    m_receivedTiles = received;
    m_expectedTiles = expected;
    if (m_loadingScreen) {
        m_loadingScreen->setTotalTiles(expected);
        m_loadingScreen->setReceivedTiles(received);
    }
}

bool ZappyGUI::isLoadingComplete() const
{
    return !m_isLoading && m_expectedTiles > 0 && m_receivedTiles >= m_expectedTiles;
}

void ZappyGUI::sendMessageToServer(const std::string& message)
{
    if (m_network && m_network->isConnected()) {
        m_network->sendMessage(message);
    }
}