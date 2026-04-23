#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

int main() {
	srand(static_cast<unsigned int>(time(nullptr)));

	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Weldom - Craft Captcha");
	window.setFramerateLimit(60);

	// ========== ЗАГРУЗКА ТЕКСТУР ==========
	sf::Texture craftBgTex;
	if (!craftBgTex.loadFromFile("craft_bg.png")) { std::cerr << "Error: craft_bg.png" << std::endl; return 1; }
	sf::Sprite craftBg(craftBgTex);
	craftBg.setScale({ static_cast<float>(window.getSize().x) / craftBgTex.getSize().x,
					  static_cast<float>(window.getSize().y) / craftBgTex.getSize().y });

	sf::Texture stickTex;
	if (!stickTex.loadFromFile("stick.png")) { std::cerr << "Error: stick.png" << std::endl; return 1; }

	sf::Texture diamondTex;
	if (!diamondTex.loadFromFile("diamond.png")) { std::cerr << "Error: diamond.png" << std::endl; return 1; }

	sf::Texture pickaxeTex;
	if (!pickaxeTex.loadFromFile("pickaxe.png")) { std::cerr << "Error: pickaxe.png" << std::endl; return 1; }
	sf::Sprite pickaxe(pickaxeTex);
	pickaxe.setScale({ 0.8f, 0.8f });
	pickaxe.setOrigin({ 32.0f, 32.0f });
	pickaxe.setPosition({ static_cast<float>(window.getSize().x) / 2.0f,
						 static_cast<float>(window.getSize().y) / 2.0f + 100.0f });

	sf::Texture chickenTex;
	if (!chickenTex.loadFromFile("roasted_chicken.png")) { std::cerr << "Error: roasted_chicken.png" << std::endl; return 1; }

	sf::Texture duckBgTex;
	if (!duckBgTex.loadFromFile("duck_stage_bg.png")) { std::cerr << "Error: duck_stage_bg.png" << std::endl; return 1; }
	sf::Sprite duckBg(duckBgTex);
	duckBg.setScale({ static_cast<float>(window.getSize().x) / duckBgTex.getSize().x,
					 static_cast<float>(window.getSize().y) / duckBgTex.getSize().y });

	sf::Texture eyeBgTex;
	if (!eyeBgTex.loadFromFile("eye_exam_bg.png")) { std::cerr << "Error: eye_exam_bg.png" << std::endl; return 1; }
	sf::Sprite eyeBg(eyeBgTex);
	eyeBg.setScale({ static_cast<float>(window.getSize().x) / eyeBgTex.getSize().x,
					static_cast<float>(window.getSize().y) / eyeBgTex.getSize().y });

	sf::Font font;
	if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) { std::cerr << "Warning: font not loaded" << std::endl; }
	sf::Text text(font, "", 25);
	text.setFillColor(sf::Color::Black);
	text.setPosition({ 50.0f, 50.0f });

	sf::Text trollText(font, "АХАХАХАХА\nНИКАКОЙ ИГРЫ НЕТ\nЛОХ", 60);
	trollText.setOutlineThickness(3.0f);
	sf::FloatRect bounds = trollText.getLocalBounds();
	trollText.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
						 bounds.position.y + bounds.size.y / 2.0f });
	trollText.setPosition({ static_cast<float>(window.getSize().x) / 2.0f,
						   static_cast<float>(window.getSize().y) / 2.0f });

	sf::CircleShape clickCircle(15.0f);
	clickCircle.setFillColor(sf::Color(139, 69, 19, 128));
	clickCircle.setOrigin({ 15.0f, 15.0f });
	clickCircle.setPosition({ -100.0f, -100.0f });

	// ========== ЭТАП 1: ПРЕДМЕТЫ ==========
	std::vector<sf::Sprite> sticks;
	std::vector<bool> stickCollected;
	sf::Vector2f stickPositions[2] = { {200.0f, 300.0f}, {600.0f, 400.0f} };
	for (int i = 0; i < 2; i++) {
		sf::Sprite s(stickTex);
		s.setOrigin({ 32.0f, 32.0f });
		s.setPosition(stickPositions[i]);
		sticks.push_back(s);
		stickCollected.push_back(false);
	}

	std::vector<sf::Sprite> diamonds;
	std::vector<bool> diamondCollected;
	sf::Vector2f diamondPositions[3] = { {300.0f, 200.0f}, {500.0f, 250.0f}, {400.0f, 450.0f} };
	for (int i = 0; i < 3; i++) {
		sf::Sprite d(diamondTex);
		d.setOrigin({ 32.0f, 32.0f });
		d.setPosition(diamondPositions[i]);
		diamonds.push_back(d);
		diamondCollected.push_back(false);
	}

	// ========== ЭТАП 2: 9 КУРИЦ ==========
	struct Chicken {
		sf::Sprite sprite;
		bool caught;
		float vx, vy;
	};
	std::vector<Chicken> chickens;
	float chickenSpeed = 120.0f;

	for (int i = 0; i < 9; i++) {
		Chicken c;
		c.sprite.setTexture(chickenTex);
		c.sprite.setOrigin({ 32.0f, 32.0f });
		float x = 50.0f + (rand() % 700);
		float y = 50.0f + (rand() % 500);
		c.sprite.setPosition({ x, y });
		c.caught = false;
		float angle = (rand() % 360) * 3.14159f / 180.0f;
		c.vx = cos(angle) * chickenSpeed;
		c.vy = sin(angle) * chickenSpeed;
		chickens.push_back(c);
	}

	// ========== ЭТАП 3: ПРОВЕРКА ЗРЕНИЯ ==========
	sf::FloatRect eyeTarget(350.0f, 250.0f, 100.0f, 100.0f);
	bool eyePassed = false;

	// ========== СОСТОЯНИЯ ==========
	enum GameState { STAGE1_COLLECT, STAGE2_CHICKENS, STAGE3_EYE, STAGE4_TROLL };
	GameState state = STAGE1_COLLECT;

	int sticksCount = 0;
	int diamondsCount = 0;
	int chickensRemaining = 9;
	bool craftDone = false;

	sf::Clock deltaClock;
	sf::Clock trollClock;
	sf::Clock blinkClock;
	sf::Clock shakeClock;

	float trollStartX = static_cast<float>(window.getSize().x) / 2.0f;
	float trollStartY = static_cast<float>(window.getSize().y) / 2.0f;

	while (window.isOpen()) {
		float dt = deltaClock.restart().asSeconds();
		if (dt > 0.033f) dt = 0.033f;

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Button::Left) {
					sf::Vector2f clickPos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
					clickCircle.setPosition(clickPos);

					if (state == STAGE1_COLLECT && !craftDone) {
						for (int i = 0; i < 2; i++) {
							if (!stickCollected[i] && sticks[i].getGlobalBounds().contains(clickPos)) {
								stickCollected[i] = true;
								sticksCount++;
							}
						}
						for (int i = 0; i < 3; i++) {
							if (!diamondCollected[i] && diamonds[i].getGlobalBounds().contains(clickPos)) {
								diamondCollected[i] = true;
								diamondsCount++;
							}
						}
						if (sticksCount >= 2 && diamondsCount >= 3 && !craftDone) {
							craftDone = true;
							text.setString("Stage 2: Catch all 9 chickens!");
							state = STAGE2_CHICKENS;
						}
					}
					else if (state == STAGE2_CHICKENS) {
						for (int i = 0; i < 9; i++) {
							if (!chickens[i].caught && chickens[i].sprite.getGlobalBounds().contains(clickPos)) {
								chickens[i].caught = true;
								chickensRemaining--;
							}
						}
						if (chickensRemaining == 0) {
							text.setString("Stage 3: Click on the hidden cat!");
							state = STAGE3_EYE;
						}
						else {
							text.setString("Stage 2: Catch chickens! Remaining: " + std::to_string(chickensRemaining));
						}
					}
					else if (state == STAGE3_EYE && !eyePassed) {
						if (eyeTarget.contains(clickPos)) {
							eyePassed = true;
							state = STAGE4_TROLL;
							trollClock.restart();
							blinkClock.restart();
							shakeClock.restart();
						}
					}
				}
			}
		}

		if (state == STAGE1_COLLECT && !craftDone) {
			text.setString("Stage 1: Collect 2 Sticks and 3 Diamonds!\nSticks: " + std::to_string(sticksCount) +
				"/2   Diamonds: " + std::to_string(diamondsCount) + "/3");
		}

		if (state == STAGE2_CHICKENS) {
			for (int i = 0; i < 9; i++) {
				if (!chickens[i].caught) {
					float x = chickens[i].sprite.getPosition().x + chickens[i].vx * dt;
					float y = chickens[i].sprite.getPosition().y + chickens[i].vy * dt;

					if (x < 40.0f) { x = 40.0f; chickens[i].vx = -chickens[i].vx; }
					if (x > static_cast<float>(window.getSize().x) - 40.0f) { x = static_cast<float>(window.getSize().x) - 40.0f; chickens[i].vx = -chickens[i].vx; }
					if (y < 40.0f) { y = 40.0f; chickens[i].vy = -chickens[i].vy; }
					if (y > static_cast<float>(window.getSize().y) - 40.0f) { y = static_cast<float>(window.getSize().y) - 40.0f; chickens[i].vy = -chickens[i].vy; }

					chickens[i].sprite.setPosition({ x, y });
				}
			}
		}

		if (state == STAGE4_TROLL) {
			float blinkTime = blinkClock.getElapsedTime().asSeconds();
			float alpha = std::abs(std::sin(blinkTime * 15.0f));
			int colorMode = static_cast<int>(blinkTime * 10) % 3;

			if (colorMode == 0) {
				trollText.setFillColor(sf::Color(255, 0, 0, static_cast<unsigned char>(alpha * 255)));
				trollText.setOutlineColor(sf::Color::Black);
			}
			else if (colorMode == 1) {
				trollText.setFillColor(sf::Color(255, 255, 255, static_cast<unsigned char>(alpha * 255)));
				trollText.setOutlineColor(sf::Color::Red);
			}
			else {
				trollText.setFillColor(sf::Color(255, 200, 0, static_cast<unsigned char>(alpha * 255)));
				trollText.setOutlineColor(sf::Color::Black);
			}

			float shakeTime = shakeClock.getElapsedTime().asSeconds();
			if (shakeTime < 3.0f) {
				float offsetX = static_cast<float>(rand() % 31) - 15.0f;
				float offsetY = static_cast<float>(rand() % 31) - 15.0f;
				trollText.setPosition({ trollStartX + offsetX, trollStartY + offsetY });
			}

			float scale = 1.0f + std::abs(std::sin(blinkTime * 20.0f)) * 0.3f;
			trollText.setScale({ scale, scale });

			if (trollClock.getElapsedTime().asSeconds() >= 3.0f) {
				window.close();
			}
		}

		window.clear();

		if (state == STAGE4_TROLL) {
			window.clear(sf::Color::Black);
			window.draw(trollText);
		}
		else if (state == STAGE3_EYE) {
			window.draw(eyeBg);
			window.draw(text);
			window.draw(clickCircle);
		}
		else if (state == STAGE2_CHICKENS) {
			window.draw(duckBg);
			for (int i = 0; i < 9; i++) {
				if (!chickens[i].caught) {
					window.draw(chickens[i].sprite);
				}
			}
			window.draw(clickCircle);
			window.draw(text);
		}
		else {
			window.draw(craftBg);
			for (int i = 0; i < 2; i++) {
				if (!stickCollected[i]) window.draw(sticks[i]);
			}
			for (int i = 0; i < 3; i++) {
				if (!diamondCollected[i]) window.draw(diamonds[i]);
			}
			if (craftDone) window.draw(pickaxe);
			window.draw(clickCircle);
			window.draw(text);
		}

		window.display();
	}

	return 0;
}