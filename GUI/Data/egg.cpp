/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** egg.cpp
*/

#include "../gui.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <cmath>
#include <algorithm>

void ZappyGUI::get_egg_data(const std::string& message)
{
    std::istringstream iss(message);
    std::string command;
    iss >> command;

    if (command == "pfk") {
        std::string playerIdStr;
        if (!(iss >> playerIdStr)) {
            std::cerr << "[GUI ERROR] Format invalide pour pfk: " << message << std::endl;
            return;
        }
        int playerId = -1;
        if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
            try {
                playerId = std::stoi(playerIdStr.substr(1));
            } catch (const std::exception& e) {
                std::cerr << "[GUI ERROR] ID joueur invalide dans pfk: " << playerIdStr << std::endl;
                return;
            }
        } else {
            std::cerr << "[GUI ERROR] Format ID joueur invalide dans pfk: " << playerIdStr << std::endl;
            return;
        }
        const Player* player = getPlayerById(playerId);
        if (player) {
            std::cout << "[GUI INFO] 🥚 Joueur #" << playerId << " (" << player->teamName 
                      << ") pond un œuf à (" << player->x << "," << player->y << ")" << std::endl;
        } else {
            std::cout << "[GUI WARNING] ⚠️  Joueur #" << playerId << " non trouvé pour ponte d'œuf" << std::endl;
        }

    } else if (command == "enw") {
        std::string eggIdStr, playerIdStr;
        int x, y;
        if (!(iss >> eggIdStr >> playerIdStr >> x >> y)) {
            std::cerr << "[GUI ERROR] Format invalide pour enw: " << message << std::endl;
            return;
        }
        int eggId = -1;
        if (eggIdStr.length() > 1 && eggIdStr[0] == '#') {
            try {
                eggId = std::stoi(eggIdStr.substr(1));
            } catch (const std::exception& e) {
                std::cerr << "[GUI ERROR] ID œuf invalide dans enw: " << eggIdStr << std::endl;
                return;
            }
        } else {
            std::cerr << "[GUI ERROR] Format ID œuf invalide dans enw: " << eggIdStr << std::endl;
            return;
        }
        int playerId = -1;
        if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
            try {
                playerId = std::stoi(playerIdStr.substr(1));
            } catch (const std::exception& e) {
                std::cerr << "[GUI ERROR] ID joueur invalide dans enw: " << playerIdStr << std::endl;
                return;
            }
        } else {
            std::cerr << "[GUI ERROR] Format ID joueur invalide dans enw: " << playerIdStr << std::endl;
            return;
        }
        std::string teamName = "All_teams";
        if (playerId != -1) {
            const Player* player = getPlayerById(playerId);
            if (player) {
                teamName = player->teamName;
            }
        }
        bool eggExists = false;
        for (auto& egg : m_eggs) {
            if (egg.id == eggId) {
                egg.x = x;
                egg.y = y;
                egg.creatorPlayerId = playerId;
                egg.isActive = true;
                egg.teamName = teamName;
                eggExists = true;
                std::cout << "[GUI INFO] 🔄 Œuf #" << eggId << " mis à jour à (" << x << "," << y 
                          << ") équipe=" << teamName << std::endl;
                break;
            }
        }
        if (!eggExists) {
            m_eggs.emplace_back(eggId, playerId, x, y, teamName);
            if (playerId == -1) {
                std::cout << "[GUI INFO] 🐣 Nouvel œuf initial #" << eggId << " créé à (" << x << "," << y 
                          << ") équipe=" << teamName << std::endl;
            } else {
                std::cout << "[GUI INFO] 🥚 Nouvel œuf #" << eggId << " pondu par joueur #" << playerId 
                          << " à (" << x << "," << y << ") équipe=" << teamName << std::endl;
            }
        }
    } else if (command == "ebo") {
        std::string eggIdStr;
        if (!(iss >> eggIdStr)) {
            std::cerr << "[GUI ERROR] Format invalide pour ebo: " << message << std::endl;
            return;
        }
        int eggId = -1;
        if (eggIdStr.length() > 1 && eggIdStr[0] == '#') {
            try {
                eggId = std::stoi(eggIdStr.substr(1));
            } catch (const std::exception& e) {
                std::cerr << "[GUI ERROR] ID œuf invalide dans ebo: " << eggIdStr << std::endl;
                return;
            }
        } else {
            std::cerr << "[GUI ERROR] Format ID œuf invalide dans ebo: " << eggIdStr << std::endl;
            return;
        }
        bool eggFound = false;
        for (auto& egg : m_eggs) {
            if (egg.id == eggId && egg.isActive) {
                egg.isActive = false;
                eggFound = true;
                std::cout << "[GUI INFO] 🐣 Connexion depuis œuf #" << eggId << " à (" 
                          << egg.x << "," << egg.y << ") équipe=" << egg.teamName << std::endl;
                break;
            }
        }
        if (!eggFound) {
            std::cout << "[GUI WARNING] ⚠️  Œuf #" << eggId << " non trouvé pour connexion" << std::endl;
        }

    } else if (command == "edi") {
        std::string eggIdStr;
        if (!(iss >> eggIdStr)) {
            std::cerr << "[GUI ERROR] Format invalide pour edi: " << message << std::endl;
            return;
        }
        int eggId = -1;
        if (eggIdStr.length() > 1 && eggIdStr[0] == '#') {
            try {
                eggId = std::stoi(eggIdStr.substr(1));
            } catch (const std::exception& e) {
                std::cerr << "[GUI ERROR] ID œuf invalide dans edi: " << eggIdStr << std::endl;
                return;
            }
        } else {
            std::cerr << "[GUI ERROR] Format ID œuf invalide dans edi: " << eggIdStr << std::endl;
            return;
        }
        bool eggFound = false;
        for (auto it = m_eggs.begin(); it != m_eggs.end(); ++it) {
            if (it->id == eggId) {
                std::cout << "[GUI INFO] 💔 Œuf #" << eggId << " détruit à ("
                          << it->x << "," << it->y << ") équipe=" << it->teamName << std::endl;
                m_eggs.erase(it);
                eggFound = true;
                break;
            }
        }
        if (!eggFound) {
            std::cout << "[GUI WARNING] ⚠️  Œuf #" << eggId << " non trouvé pour destruction" << std::endl;
        }
    } else {
        std::cerr << "[GUI ERROR] Commande d'œuf non reconnue: " << command << std::endl;
    }
}

const Egg* ZappyGUI::getEggById(int eggId) const
{
    for (const auto& egg : m_eggs) {
        if (egg.id == eggId) {
            return &egg;
        }
    }
    return nullptr;
}

sf::Vector2f ZappyGUI::getEggScreenPosition(const Egg& egg) const
{
    if (game_bk_texture.getSize().x == 0)
        return sf::Vector2f(0, 0);
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const float TOP_X = 360.f * scaleX;
    const float TOP_Y = 62.f * scaleY;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    float eggScreenX = TOP_X + (egg.x - egg.y) * (TILE_WIDTH / 2.f);
    float eggScreenY = TOP_Y + (egg.x + egg.y) * (TILE_HEIGHT / 2.f);
    return sf::Vector2f(eggScreenX, eggScreenY);
}

sf::Vector2f ZappyGUI::getEggBaseTilePosition(int x, int y) const
{
    if (game_bk_texture.getSize().x == 0)
        return sf::Vector2f(0, 0);
    return const_cast<ZappyGUI*>(this)->getCachedTilePosition(x, y);
}

sf::Vector2f ZappyGUI::calculateEggOffset(sf::Vector2f basePos, size_t eggIndex, size_t totalEggs) const
{
    if (totalEggs == 1) {
        return basePos;
    }
    const float OFFSET_RADIUS = 15.0f;
    const float OFFSET_SPACING = 12.0f;
    if (totalEggs <= 4) {
        float angle = (2.0f * M_PI * eggIndex) / totalEggs;
        float offsetX = OFFSET_RADIUS * cos(angle);
        float offsetY = OFFSET_RADIUS * sin(angle);
        return sf::Vector2f(basePos.x + offsetX, basePos.y + offsetY);
    } else {
        int cols = static_cast<int>(ceil(sqrt(totalEggs)));
        int row = eggIndex / cols;
        int col = eggIndex % cols;
        float totalWidth = (cols - 1) * OFFSET_SPACING;
        float totalHeight = ((totalEggs - 1) / cols) * OFFSET_SPACING;
        float offsetX = col * OFFSET_SPACING - totalWidth / 2.0f;
        float offsetY = row * OFFSET_SPACING - totalHeight / 2.0f;
        return sf::Vector2f(basePos.x + offsetX, basePos.y + offsetY);
    }
}