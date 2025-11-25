/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** interaction.cpp
*/

#include "interaction.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <vector>

PlayerExpulsion::PlayerExpulsion()
    : m_PlayerId(-1), m_playerX(0), m_playerY(0), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

PlayerExpulsion::PlayerExpulsion(int expellingPlayerId, int playerX, int playerY)
    : m_PlayerId(expellingPlayerId), m_playerX(playerX),
        m_playerY(playerY), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

void PlayerExpulsion::setActive(bool active) {
    m_isActive = active;
}

double PlayerExpulsion::getElapsedTime() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_timestamp);
    return duration.count() / 1000.0;
}

BroadcastBeam::BroadcastBeam()
    : m_senderId(-1), m_posX(0), m_posY(0), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

BroadcastBeam::BroadcastBeam(int senderId, float posX, float posY)
    : m_senderId(senderId), m_posX(posX), m_posY(posY), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

void BroadcastBeam::setActive(bool active) {
    m_isActive = active;
}

double BroadcastBeam::getElapsedTime() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_timestamp);
    return duration.count() / 1000.0;
}

PlayerDeath::PlayerDeath()
    : m_playerId(-1), m_playerX(0), m_playerY(0), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

PlayerDeath::PlayerDeath(int playerId, int playerX, int playerY)
    : m_playerId(playerId), m_playerX(playerX), m_playerY(playerY), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

void PlayerDeath::setActive(bool active) {
    m_isActive = active;
}

double PlayerDeath::getElapsedTime() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_timestamp);
    return duration.count() / 1000.0;
}

PlayerIncantation::PlayerIncantation()
    : m_playerId(-1), m_playerX(0), m_playerY(0), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

PlayerIncantation::PlayerIncantation(int playerId, int playerX, int playerY)
    : m_playerId(playerId), m_playerX(playerX), m_playerY(playerY), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

void PlayerIncantation::setActive(bool active) {
    m_isActive = active;
}

double PlayerIncantation::getElapsedTime() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_timestamp);
    return duration.count() / 1000.0;
}

IncantationFailure::IncantationFailure()
    : m_playerX(0), m_playerY(0), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

IncantationFailure::IncantationFailure(int playerX, int playerY)
    : m_playerX(playerX), m_playerY(playerY), m_timestamp(std::chrono::system_clock::now()), m_isActive(true) {}

void IncantationFailure::setActive(bool active) {
    m_isActive = active;
}

double IncantationFailure::getElapsedTime() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_timestamp);
    return duration.count() / 1000.0;
}

InteractionManager::InteractionManager()
    : m_expulsionDisplayTime(1.5), m_broadcastDisplayTime(10.0), m_deathDisplayTime(3.0), m_incantationDisplayTime(5.0), m_incantationFailureDisplayTime(3.0) {}

void InteractionManager::addExpulsion(int expellingPlayerId, int playerX, int playerY) {
    m_expulsions.emplace_back(expellingPlayerId, playerX, playerY);
    std::cout << "[INTERACTION] Expulsion par le joueur " << expellingPlayerId <<
        " à la position (" << playerX << ", " << playerY << ")" << std::endl;
}

std::vector<PlayerExpulsion> InteractionManager::getActiveExpulsions() const {
    std::vector<PlayerExpulsion> activeExpulsions;
    for (const auto& expulsion : m_expulsions) {
        if (expulsion.isActive()) {
            activeExpulsions.push_back(expulsion);
        }
    }
    return activeExpulsions;
}

void InteractionManager::addBroadcastBeam(int senderId, float posX, float posY) {
    m_broadcastBeams.emplace_back(senderId, posX, posY);
    std::cout << "[INTERACTION] Broadcast créé du joueur " << senderId << " à la position (" << posX << ", " << posY << ")" << std::endl;
}

std::vector<BroadcastBeam> InteractionManager::getActiveBroadcastBeams() const {
    std::vector<BroadcastBeam> activeBeams;
    for (const auto& beam : m_broadcastBeams) {
        if (beam.isActive()) {
            activeBeams.push_back(beam);
        }
    }
    return activeBeams;
}

void InteractionManager::addDeath(int playerId, int playerX, int playerY) {
    m_deaths.emplace_back(playerId, playerX, playerY);
    std::cout << "[INTERACTION] Mort du joueur " << playerId << " à la position (" << playerX << ", " << playerY << ")" << std::endl;
}

std::vector<PlayerDeath> InteractionManager::getActiveDeaths() const {
    std::vector<PlayerDeath> activeDeaths;
    for (const auto& death : m_deaths) {
        if (death.isActive()) {
            activeDeaths.push_back(death);
        }
    }
    return activeDeaths;
}

void InteractionManager::addIncantation(int playerId, int playerX, int playerY) {
    m_incantations.emplace_back(playerId, playerX, playerY);
}

void InteractionManager::endIncantation(int playerId) {
    for (auto& incantation : m_incantations) {
        if (incantation.getPlayerId() == playerId && incantation.isActive()) {
            incantation.setActive(false);
            break;
        }
    }
}

std::vector<PlayerIncantation> InteractionManager::getActiveIncantations() const {
    std::vector<PlayerIncantation> activeIncantations;
    for (const auto& incantation : m_incantations) {
        if (incantation.isActive()) {
            activeIncantations.push_back(incantation);
        }
    }
    return activeIncantations;
}

void InteractionManager::addIncantationFailure(int playerX, int playerY)
{
    m_incantationFailures.emplace_back(playerX, playerY);
}

std::vector<IncantationFailure> InteractionManager::getActiveIncantationFailures() const {
    std::vector<IncantationFailure> activeFailures;
    for (const auto& failure : m_incantationFailures) {
        if (failure.isActive()) {
            activeFailures.push_back(failure);
        }
    }
    return activeFailures;
}

void InteractionManager::updateInteractions()
{
    for (auto& expulsion : m_expulsions) {
        if (expulsion.isActive() && expulsion.getElapsedTime() > m_expulsionDisplayTime) {
            expulsion.setActive(false);
        }
    }
    for (auto& broadcast : m_broadcastBeams) {
        if (broadcast.isActive() && broadcast.getElapsedTime() > m_broadcastDisplayTime) {
            broadcast.setActive(false);
        }
    }
    for (auto& beam : m_broadcastBeams) {
        if (beam.isActive() && beam.getElapsedTime() > m_broadcastDisplayTime) {
            beam.setActive(false);
        }
    }
    for (auto& incantation : m_incantations) {
        if (incantation.isActive() && incantation.getElapsedTime() > m_incantationDisplayTime) {
            incantation.setActive(false);
        }
    }
    for (auto& failure : m_incantationFailures) {
        if (failure.isActive() && failure.getElapsedTime() > m_incantationFailureDisplayTime) {
            failure.setActive(false);
        }
    }
    for (auto& death : m_deaths) {
        if (death.isActive() && death.getElapsedTime() > m_deathDisplayTime) {
            death.setActive(false);
        }
    }
}

void InteractionManager::setIncantationDisplayTime(double duration)
{
    m_incantationDisplayTime = duration;
}
