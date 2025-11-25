#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class ZappyGUI;

class Setting {
    public:
        Setting(sf::RenderWindow* renderWindow, sf::Font& font, int* sharedVolume);
        void show();

    private:
        size_t m_currentSection = 0;
        size_t m_currentButtonInSection = 0;
        bool m_keyboardMode = true;
        std::vector<size_t> m_buttonsPerSection = {2, 3, 1};

        sf::Text m_leftArrow;
        sf::Text m_rightArrow;
        const sf::Color SelectedBtnColor = sf::Color(150, 255, 150);

        // Window and font references
        sf::RenderWindow* window;
        sf::Font* font;

        // Shared volume reference
        int* m_sharedVolume = nullptr;

        // UI Text elements
        sf::Text volumeLabel;
        sf::Text volumeValueText;
        sf::Text windowLabel;

        // Button structure
        struct Button {
            sf::RectangleShape shape;
            sf::Text text;
            bool hover = false;
        };

        // UI Buttons
        Button volUpButton;
        Button volDownButton;
        Button res1080Button;
        Button res720Button;
        Button res900Button;
        Button backButton;

        // Background
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;

        // Window size
        unsigned int m_winW = 0;
        unsigned int m_winH = 0;

        // Visual effects
        sf::Vector2f shadowOffset = { 4.f, 4.f };

        // Private methods
        void updateLayout();
        void updateHoverStates();
        bool handleInput();
        void draw();
        void drawSelectionArrows();
        bool handleKeyboardSelection();
        void configureButton(Button& btn, const std::string& label, float cx, float cy, float w, float h);
        bool setSystemVolume(int newVol);

        // Color constants
        sf::Color NormalBtnColor = sf::Color::White;
        sf::Color HoverBtnColor  = sf::Color(200, 200, 255);
        sf::Color ClickBtnColor  = sf::Color(100, 100, 255);
        sf::Color ShadowColor    = sf::Color(50, 50, 50);
        sf::Color TextColor      = sf::Color::Black;
};
