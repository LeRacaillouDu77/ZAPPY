/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** Theme.hpp
*/
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum Themes_enum {
	THEME_BASE,
	THEME_WAR,
	THEME_NIGHT,
	THEME_CAPITAL
};

class Theme {
	public:
		Theme(sf::RenderWindow* renderWindow, sf::Font& font);
		Themes_enum getCurrentTheme();
		void show();

	private:
		size_t m_currentSection = 0;
		size_t m_currentButtonInSection = 0;
		bool m_keyboardMode = true;
		std::vector<size_t> m_buttonsPerSection = {3, 1};
		sf::Text m_leftArrow;
		sf::Text m_rightArrow;
		bool handleKeyboardSelection();
		void drawSelectionArrows();
		// Window and font references
		sf::RenderWindow* window;
		sf::Font* font;

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
		Button warButton;
		Button nightButton;
		Button capitalButton;
		Button backButton;

		// Background elements
		sf::Texture backgroundTexture;
		sf::Sprite backgroundSprite;

		// Themes state
		int volume = 50;
		unsigned int m_winW = 0;
		unsigned int m_winH = 0;

		// Visual effects
		sf::Vector2f shadowOffset = { 4.f, 4.f };

		// Private methods
		void updateLayout();
		void updateHoverStates();
		bool handleInput();
		void draw();
		void configureButton(Button& btn, const std::string& label, float cx, float cy, float w, float h);
		bool setSystemVolume(int newVol);

		// Color constants
		sf::Color NormalBtnColor = sf::Color::White;
		const sf::Color HoverBtnColor = sf::Color(200, 200, 255);
		const sf::Color ClickBtnColor = sf::Color(100, 100, 255);
		const sf::Color ShadowColor = sf::Color(50, 50, 50);
		sf::Color TextColor = sf::Color::Black;
};
