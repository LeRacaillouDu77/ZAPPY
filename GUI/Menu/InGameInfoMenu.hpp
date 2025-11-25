#pragma once
#include <SFML/Graphics.hpp>
#include "Menu/Menu.hpp"
#include "Settings/Setting.hpp"
#include <functional>
#include <cmath>
#include <vector>

// Déclarations forward pour éviter les dépendances circulaires
class Team;
class Player;

enum InGameInfoMenuAction {
    INGAME_INFO_NONE,
    INGAME_INFO_RESUME,
};

class InGameInfoMenu {
    public:
        InGameInfoMenu(sf::RenderWindow* window, sf::Font& font);
        InGameInfoMenuAction show();
        void setVolumePointer(int* sharedVolumePtr);
        void setCommandSender(std::function<void(const std::string&)> func);
        void setquantity(sf::Text &text, int number);
        void updateTeamsAndMapInfo(const std::vector<Team>& teams, const std::vector<Player>& players, int mapWidth, int mapHeight);

        sf::Text m_level1;
        sf::Text m_level2;
        sf::Text m_level3;
        sf::Text m_level4;
        sf::Text m_level5;
        sf::Text m_level6;
        sf::Text m_level7;
        sf::Text m_level8;

        sf::Text m_food;
        sf::Text m_linemate;
        sf::Text m_deraumere;
        sf::Text m_sibur;
        sf::Text m_mendiane;
        sf::Text m_phiras;
        sf::Text m_thystame;

        // Nouveaux éléments pour les équipes et dimensions
        sf::Text m_mapDimensionsText;
        sf::Text m_teamsTitle;
        std::vector<sf::Text> m_teamTexts;
        std::vector<sf::Text> m_teamPlayerCountTexts;

    private:
        std::function<void(const std::string&)> m_sendCommandToServer;
        void updateHoverStates();
        void draw();
        void drawModernButton(sf::Text& button, sf::RectangleShape& buttonBg, bool isActive, bool isHovered);
        void drawSlider(sf::Vector2f position, float width, float value, float maxValue, const std::string& label, bool isActive);
        void drawMenuPanel();
        void updateAnimations(float deltaTime);
        InGameInfoMenuAction handleInput();
        void changeVolume(int delta);
        void updateVolumeDisplay();
        void updateSliderFromMouse(sf::Vector2f mousePos, bool isVolume);
        sf::Vector2f calculateButtonSize(const sf::Text& text);
        void initializeUI();
        bool setSystemVolume(int newVol);

        sf::RenderWindow* m_window;
        sf::Font& m_font;

        sf::RectangleShape m_menuPanel;
        sf::RectangleShape m_titlePanel;
        sf::RectangleShape m_resumeButtonBg;
        sf::RectangleShape m_settingsButtonBg;
        sf::RectangleShape m_backToMenuButtonBg;

        sf::Text m_resumeButton;
        sf::Text m_settingsButton;

        sf::Text m_volumeLabel;
        sf::Text m_freqLabel;
        sf::Text m_freqValueText;

        sf::Clock m_animationClock;
        float m_hoverAnimationProgress[6];
        float m_pulseAnimation;

        const sf::Color PANEL_COLOR = sf::Color(25, 25, 35, 240);
        const sf::Color TITLE_COLOR = sf::Color(45, 45, 65, 255);
        const sf::Color BUTTON_NORMAL = sf::Color(60, 60, 80, 200);
        const sf::Color BUTTON_HOVER = sf::Color(85, 170, 255, 220);
        const sf::Color BUTTON_ACTIVE = sf::Color(120, 200, 255, 255);
        const sf::Color TEXT_COLOR = sf::Color(220, 220, 230, 255);
        const sf::Color ACCENT_COLOR = sf::Color(100, 200, 255, 255);
        const sf::Color SLIDER_BG = sf::Color(40, 40, 50, 200);
        const sf::Color SLIDER_FILL = sf::Color(85, 170, 255, 255);

        sf::Text m_backToMenuButton;
        sf::Text m_volumeValueText;
        sf::Text m_leftArrow;
        sf::Text m_rightArrow;
        sf::Text m_titleText;

        bool m_keyboardMode = true;
        int m_selectedSectionIndex = 0;
        static const int TOTAL_SECTIONS = 6;

        // États de survol modernisés
        bool m_hoverResume = false;

        int *m_sharedVolume = nullptr;

        int m_serverFrequency = 100;
        sf::Vector2f m_lastMousePos;

        sf::Sprite m_sprite_level8;
        sf::Sprite m_sprite_level7;
        sf::Sprite m_sprite_level6;
        sf::Sprite m_sprite_level5;
        sf::Sprite m_sprite_level4;
        sf::Sprite m_sprite_level3;
        sf::Sprite m_sprite_level2;
        sf::Sprite m_sprite_level1;

        sf::Texture m_Texture_level8;
        sf::Texture m_Texture_level7;
        sf::Texture m_Texture_level6;
        sf::Texture m_Texture_level5;
        sf::Texture m_Texture_level4;
        sf::Texture m_Texture_level3;
        sf::Texture m_Texture_level2;
        sf::Texture m_Texture_level1;

        sf::Sprite m_sprite_food;
        sf::Sprite m_sprite_linemate;
        sf::Sprite m_sprite_deraumere;
        sf::Sprite m_sprite_sibur;
        sf::Sprite m_sprite_mendiane;
        sf::Sprite m_sprite_phiras;
        sf::Sprite m_sprite_thystame;

        sf::Texture m_Texture_food;
        sf::Texture m_Texture_linemate;
        sf::Texture m_Texture_deraumere;
        sf::Texture m_Texture_sibur;
        sf::Texture m_Texture_mendiane;
        sf::Texture m_Texture_phiras;
        sf::Texture m_Texture_thystame;

        void InitializeQuantityText(sf::Text &text, sf::Vector2f position);
        void InitializeQuantitySprite(sf::Sprite &sprite, sf::Texture &sprite_texture, sf::Vector2f position, sf::String filename);
        void InitializeItemsSprite(sf::Sprite &sprite, sf::Texture &sprite_texture, sf::Vector2f position, sf::String filename);
};
