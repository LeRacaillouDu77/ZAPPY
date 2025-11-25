/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** gui.hpp
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <set>
#include "Menu/Menu.hpp"
#include "Themes/Theme.hpp"
#include "Settings/Setting.hpp"
#include "LoadingScreen/loading.hpp"
#include "Data/interaction.hpp"
#include "Data/player_sprite.hpp"
#include "World/world.hpp"
#include "Menu/InGameMenu.hpp"
#include "Menu/InGameInfoMenu.hpp"
#include "Menu/GameEndScreen.hpp"
#include "Network/network.hpp"

#define ICON_PATH "Assets/zappy_icon.png"

class Egg {
    public:
        int id;
        int x, y;
        int creatorPlayerId; // ID du joueur qui a pondu l'œuf (-1 si œuf initial)
        bool isActive;
        std::string teamName;
        sf::Sprite sprite;

        Egg() : id(-1), x(0), y(0), creatorPlayerId(-1), isActive(false) {}
        Egg(int eggId, int creatorId, int posX, int posY, const std::string& team)
            : id(eggId), x(posX), y(posY), creatorPlayerId(creatorId), isActive(true), teamName(team) {}
};

struct PlayerInventory {
    int food;
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;

    PlayerInventory() : food(0), linemate(0), deraumere(0), sibur(0), mendiane(0), phiras(0), thystame(0) {}
    PlayerInventory(int f, int l, int d, int s, int m, int p, int t)
        : food(f), linemate(l), deraumere(d), sibur(s), mendiane(m), phiras(p), thystame(t) {}
};

class Player
{
    public:
        int id;
        int x, y;
        int orientation;
        int level;
        std::string teamName;
        PlayerInventory inventory;
        PlayerSprite sprite;

        Player() = default;
        Player(int playerId, int posX, int posY, int orient, int lvl, const std::string& team)
            : id(playerId), x(posX), y(posY), orientation(orient), level(lvl), teamName(team) {}
};

class Team
{
    public:
        std::string name;
        std::set<int> playerIds;
        Team() = default;
        Team(const std::string& teamName) : name(teamName) {}
};

class Camera
{
    private:
        sf::Vector2f position;
        float zoomLevel;
        sf::Vector2f size;
        sf::View camera_view;
    public:
        Camera();
        Camera(sf::Vector2f base_position, float base_zoom, sf::Vector2f size);
        void setPosition(float x, float y);
        void setZoom(float zoom);
        void move(sf::Vector2f offset);
        float getZoom() const;
        sf::Vector2f getPosition() const;
        sf::Vector2f getSize() const;
        const sf::View& getView() const;
};

class ZappyGUI {
    public:
        ZappyGUI();
        ~ZappyGUI();
        bool parseArguments(int argc, char **argv);
        bool connectToServer();
        void run();
        void displayUsage(const std::string &programName);
        sf::RenderWindow* getWindow() { return m_window.get(); }
        sf::Font& getFont() { return m_font; }

        Menu* getMenu();
        const Menu* getMenu() const;

        Theme* getTheme();
        const Theme* getTheme() const;

        Setting* getSetting();
        const Setting* getSetting() const;

        MenuAction getCurrentAction() const;

        Themes_enum getCurrentTheme() const;
        // Setters
        void setMenu(std::unique_ptr<Menu> menu);
        void setTheme(std::unique_ptr<Theme> theme);
        void setSetting(std::unique_ptr<Setting> setting);
        void setCurrentAction(MenuAction action);
        void setCurrentTheme(Themes_enum theme);
        //Volume centralisé
        void setCurrentVolume(int vol);
        int getCurrentVolume() const;
        
        // Callbacks pour la communication avec Network
        void onConnectionStatusChanged(bool connected, const std::string& message);
        void onLoadingStatusChanged(bool loading, int received, int expected);
        
        // Méthodes pour gérer les données du monde
        void get_world_data(const std::string& message);
        void get_team_data(const std::string& message);
        void get_player_data(const std::string& message);
        void get_player_level_data(const std::string& message);
        void get_player_inventory_data(const std::string& message);
        void get_egg_data(const std::string& message);
        
        // Gestion de la fin de partie
        void showGameEndScreen(const std::string& winningTeam);

        // Méthodes spécialisées pour la gestion des joueurs
        void handle_player_new(const std::string& message);
        void handle_player_position(const std::string& message);
        int parse_player_id(const std::string& playerIdStr, const std::string& command);
        bool update_existing_player(int playerId, int x, int y, int orientation, int level, const std::string& teamName);
        void create_new_player(int playerId, int x, int y, int orientation, int level, const std::string& teamName);
        void remove_player(int playerId);
        World& getWorld() { return m_world; }
        const World& getWorld() const { return m_world; }

        const std::vector<Team>& getTeams() const { return m_teams; }
        const Team* getTeamByName(const std::string& name) const;
        const std::vector<Player>& getPlayers() const { return m_players; }
        const Player* getPlayerById(int playerId) const;
        const std::vector<Egg>& getEggs() const { return m_eggs; }
        const Egg* getEggById(int eggId) const;

        float getIncantationAnimationDuration() const;

        InteractionManager& getInteractionManager() { return m_interactionManager; }
        const InteractionManager& getInteractionManager() const { return m_interactionManager; }
        void renderExplosions();
        void renderBroadcastBeams();
        void renderIncantations();
        void renderIncantationFailures();
        void renderDeaths();
        void renderPlayers();
        void updatePlayerAnimations(float deltaTime);
        void loadSprite(const std::string& game_image_File);
        bool isLoadingComplete() const;
        void updateLoadingProgress();
        void sendMessageToServer(const std::string& message);

    private:
        void initializeWindow();
        void loadResources();
        void handleEvents();
        void update();
        void render();
        void displayWelcomeScreen();
        void displayConnectionError();
        
        void RenderTiles();
        void draw_tile(Tile& tile, sf::Vector2f tile_pos, sf::Vector2f tile_half_size = sf::Vector2f(0.f, 0.f));
        void handlePlayerClick(sf::Vector2f mousePos);
        void renderPlayerStatsOverlay();
        void initializePlayerStatsUI();
        void handleTileClick(sf::Vector2f mousePos);
        void renderTileStatsOverlay();
        void initializeTileStatsUI();
        sf::Vector2f getPlayerScreenPosition(const Player& player) const;
        void renderEggs();
        sf::Vector2f getEggScreenPosition(const Egg& egg) const;
        sf::Vector2f getEggBaseTilePosition(int x, int y) const;
        sf::Vector2f calculateEggOffset(sf::Vector2f basePos, size_t eggIndex, size_t totalEggs) const;
        sf::Vector2f getPlayerBaseTilePosition(int x, int y) const;
        sf::Vector2f calculatePlayerOffset(sf::Vector2f basePos, size_t playerIndex, size_t totalPlayers, float spriteScale) const;

        sf::FloatRect getVisibleBounds() const;
        bool isPositionVisible(int x, int y) const;
        bool isSpriteVisible(const sf::Vector2f& position, const sf::Vector2f& size) const;

        std::unique_ptr<sf::RenderWindow> m_window;
        sf::Font m_font;
        sf::Text m_welcomeText;
        sf::Text m_statusText;

        std::string m_hostname;
        int m_port;
        bool m_running;

        // Gestion réseau déléguée à la classe Network
        std::unique_ptr<Network> m_network;

        static const int WINDOW_WIDTH = 1290;
        static const int WINDOW_HEIGHT = 800;

        std::unique_ptr<Menu> m_menu;
        std::unique_ptr<Theme> m_theme;
        std::unique_ptr<Setting> m_setting;
        std::unique_ptr<LoadingScreen> m_loadingScreen;
        MenuAction m_currentAction = ACTION_NONE;

        sf::Clock m_loadingClock;
        sf::Clock m_animationClock;

        World m_world;
        std::vector<Team> m_teams;
        std::vector<Player> m_players;
        std::vector<Egg> m_eggs;
        InteractionManager m_interactionManager;
        sf::Texture game_bk_texture;
        sf::Sprite game_bk_sprite;

        sf::Texture m_campfireTexture;
        sf::Sprite m_campfireSprite;

        sf::Texture m_explosionTexture;
        sf::Sprite m_explosionSprite;

        sf::Texture m_skullTexture;
        sf::Sprite m_skullSprite;

        sf::Texture m_broadcastTexture;
        sf::Sprite m_broadcastSprite;

        sf::Texture m_incantationTexture;
        sf::Sprite m_incantationSprite;

        sf::Texture m_nopeTexture;
        sf::Sprite m_nopeSprite;

        sf::Texture m_eggTexture;
        sf::Sprite m_eggSprite;

        std::unique_ptr<InGameMenu> m_inGameMenu;
        std::unique_ptr<InGameInfoMenu> m_inGameInfoMenu;
        std::unique_ptr<GameEndScreen> m_gameEndScreen;
        bool m_showInGameMenu = false;
        bool m_showInGameInfoMenu = false;
        bool m_showGameEndScreen = false;
        std::string m_winningTeam = "";
        sf::Text m_menuButtonText;
        sf::Text m_menuInfoButtonText;

        Themes_enum m_currentTheme = THEME_BASE;
        Camera m_camera;
        int m_currentVolume = 50;
        int m_serverFrequency = 100;
        bool m_showPlayerStats = false;
        int m_selectedPlayerId = -1;
        sf::RectangleShape m_playerStatsBackground;
        sf::Text m_playerStatsTitle;
        sf::Text m_playerTeamText;
        sf::Text m_playerLevelText;
        sf::Text m_playerInventoryText;
        sf::Text m_closeButtonText;

        bool m_showTileStats = false;
        int m_selectedTileX = -1;
        int m_selectedTileY = -1;
        sf::RectangleShape m_tileStatsBackground;
        sf::Text m_tileStatsTitle;
        sf::Text m_tilePositionText;
        sf::Text m_tileResourcesText;
        sf::Text m_tileCloseButtonText;

        struct RenderCache {
            bool isValid = false;
            float scaleX = 0.0f;
            float scaleY = 0.0f;
            float TOP_X = 0.0f;
            float TOP_Y = 0.0f;
            float TILE_WIDTH = 0.0f;
            float TILE_HEIGHT = 0.0f;
            float START_X = 0.0f;
            float START_Y = 0.0f;
            sf::Vector2u lastWindowSize = {0, 0};
            sf::Vector2u lastTextureSize = {0, 0};
            int lastWorldWidth = 0;
            int lastWorldHeight = 0;
            std::map<std::pair<int, int>, sf::Vector2f> tilePositions;
        } m_renderCache;
        void updateRenderCache();
        void invalidateRenderCache();
        sf::Vector2f getCachedTilePosition(int x, int y);
        sf::Image m_icon;
        void update_info_page();
        const sf::Texture& getGameBackgroundTexture() const { return game_bk_texture; }

    private:
        friend class Network;
        bool m_connected = false;
        bool m_isLoading = false;
        int m_expectedTiles = 0;
        int m_receivedTiles = 0;
};