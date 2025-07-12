#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>

// ========== Pipe Class ==========
class Pipe {
public:
    sf::Texture texture;
    sf::Sprite topPipe;
    sf::Sprite bottomPipe;
    bool passed = false;

    Pipe(float x, float gapY) {
        texture.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\pipe.png");

        topPipe.setTexture(texture);
        bottomPipe.setTexture(texture);

        topPipe.setScale(1.f, -1.f); // Flip top pipe vertically

        float gapSize = 300.f;
        topPipe.setPosition(x, gapY - gapSize / 2.f);
        bottomPipe.setPosition(x, gapY + gapSize / 2.f);
    }

    void move(float dx) {
        topPipe.move(dx, 0);
        bottomPipe.move(dx, 0);
    }

    float getX() const {
        return topPipe.getPosition().x;
    }

    bool isOffScreen() const {
        return getX() + topPipe.getGlobalBounds().width < 0;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(topPipe);
        window.draw(bottomPipe);
    }

    bool checkCollision(const sf::FloatRect& birdBounds) const {
        return topPipe.getGlobalBounds().intersects(birdBounds) ||
            bottomPipe.getGlobalBounds().intersects(birdBounds);
    }
};

// ========== Bird Class ==========
class Bird {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocity = 0;
    float gravity = 1000.0f;
    float flapStrength = -350.0f;
    sf::Sound flapSound;

    Bird() {
        texture.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\raket.png");
        sprite.setTexture(texture);
        sprite.setPosition(100, 300);
        sprite.setScale(0.5f, 0.5f);
        sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2); // Center origin
    }

    void flap() {
        velocity = flapStrength;
        flapSound.play();
    }

    void update(float dt) {
        velocity += gravity * dt;
        sprite.move(0, velocity * dt);
    }

    void setFlapSound(sf::SoundBuffer& buffer) {
        flapSound.setBuffer(buffer);
    }

    void reset() {
        sprite.setPosition(100, 300);
        velocity = 0;
    }
};

// ========== Custom Bird Hitbox ==========
sf::FloatRect getBirdHitbox(const sf::Sprite& sprite) {
    sf::FloatRect bounds = sprite.getGlobalBounds();

    float leftCrop = 60.0f;
    float topCrop = 100.0f;
    float rightCrop = 60.0f;
    float bottomCrop = 55.0f;

    return sf::FloatRect(
        bounds.left + leftCrop,
        bounds.top + topCrop,
        bounds.width - leftCrop - rightCrop,
        bounds.height - topCrop - bottomCrop
    );
}

// ========== Optional: Draw Hitboxes ==========
void drawHitbox(sf::RenderWindow& window, const sf::FloatRect& rect) {
    sf::RectangleShape box;
    box.setPosition(rect.left, rect.top);
    box.setSize({ rect.width, rect.height });
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Red);
    box.setOutlineThickness(2.f);
    window.draw(box);
}

// ========== Game Class ==========
class Game {
private:
    sf::RenderWindow window;

    // Backgrounds
    sf::Texture backgroundTex;
    sf::Sprite background;
    sf::Texture menuBackgroundTex;
    sf::Sprite menuBackground;

    Bird bird;
    std::vector<Pipe> pipes;
    sf::Font font;
    sf::Text scoreText;
    sf::Text deathText;
    sf::Text menuText;

    int score = 0;
    int deathCount = 0;

    sf::Clock clock;
    float spawnTimer = 0;

    sf::SoundBuffer flapBuffer, scoreBuffer, hitBuffer;
    sf::Sound scoreSound, hitSound;

    sf::Music backgroundMusic; // <<== Music object added here

    bool showHitboxes = false;
    bool isMenu = true;
    bool isGameOver = true;
    bool slowMotion = false;

    sf::RectangleShape exitButton;
    sf::Text exitButtonText;

public:
    Game() : window(sf::VideoMode(400, 600), "Flappy Bird - SFML 2.6.2") {
        window.setFramerateLimit(60);

        backgroundTex.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\background.png");
        background.setTexture(backgroundTex);

        menuBackgroundTex.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\menu_background.png");
        menuBackground.setTexture(menuBackgroundTex);

        font.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\FlappyBirdy.ttf");

        scoreText.setFont(font);
        scoreText.setCharacterSize(36);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        deathText.setFont(font);
        deathText.setCharacterSize(24);
        deathText.setFillColor(sf::Color::White);
        deathText.setPosition(10, 50);

        menuText.setFont(font);
        menuText.setCharacterSize(40);
        menuText.setFillColor(sf::Color::Blue);
        menuText.setString("Press Enter to Start");
        sf::FloatRect textRect = menuText.getLocalBounds();
        menuText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        menuText.setScale(0.9f, 0.9f);
        menuText.setPosition(200, 300);

        flapBuffer.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\flap.wav");
        scoreBuffer.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\score.wav");
        hitBuffer.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\TrumpOhnawwh.wav");

        bird.setFlapSound(flapBuffer);
        scoreSound.setBuffer(scoreBuffer);
        hitSound.setBuffer(hitBuffer);

        // Load and play background music
        if (!backgroundMusic.openFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\backgroundMusic.wav")) {
            // Handle error
        }
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(40.f);
        backgroundMusic.play();

        srand(static_cast<unsigned>(time(0)));

        exitButton.setSize({ 150.f, 50.f });
        exitButton.setFillColor(sf::Color(100, 100, 100));
        exitButton.setPosition(400 / 2.f - 75.f, 400.f);

        exitButtonText.setFont(font);
        exitButtonText.setString("Exit");
        exitButtonText.setCharacterSize(24);
        exitButtonText.setFillColor(sf::Color::White);
        sf::FloatRect exitTextRect = exitButtonText.getLocalBounds();
        exitButtonText.setOrigin(exitTextRect.left + exitTextRect.width / 2.0f,
            exitTextRect.top + exitTextRect.height / 2.0f);
        exitButtonText.setPosition(exitButton.getPosition().x + exitButton.getSize().x / 2.f,
            exitButton.getPosition().y + exitButton.getSize().y / 2.f);
    }

    void run() {
        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();
            if (slowMotion) dt *= 0.25f;
            handleEvents();
            if (!isMenu) {
                update(dt);
            }
            draw();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.key.code == sf::Keyboard::C && !isMenu) {
                score += 10;
            }
            if (event.key.code == sf::Keyboard::S && !isMenu) {
                slowMotion = !slowMotion;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (isMenu) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        isMenu = false;
                        if (isGameOver) {
                            resetGame();
                            isGameOver = false;
                        }
                    }
                }
                else {
                    if (event.key.code == sf::Keyboard::Space)
                        bird.flap();
                    if (event.key.code == sf::Keyboard::H)
                        showHitboxes = !showHitboxes;
                    if (event.key.code == sf::Keyboard::P) {
                        isMenu = true;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && isMenu) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
                    if (exitButton.getGlobalBounds().contains(mousePos)) {
                        window.close();
                    }
                }
            }
        }
    }

    void update(float dt) {
        bird.update(dt);

        spawnTimer += dt;
        if (spawnTimer >= 2.0f) {
            float gapY = 200 + rand() % 200;
            pipes.emplace_back(800, gapY);
            spawnTimer = 0;
        }

        for (auto& pipe : pipes)
            pipe.move(-200 * dt);

        for (auto it = pipes.begin(); it != pipes.end();) {
            if (it->isOffScreen()) {
                it = pipes.erase(it);
            }
            else {
                if (!it->passed && it->getX() + 52 < bird.sprite.getPosition().x) {
                    it->passed = true;
                    score++;
                    scoreSound.play();
                }

                if (it->checkCollision(getBirdHitbox(bird.sprite))) {
                    onDeath();
                    return;
                }

                ++it;
            }
        }

        if (bird.sprite.getPosition().y > 600 || bird.sprite.getPosition().y < -100) {
            onDeath();
        }

        scoreText.setString("Score: " + std::to_string(score));
        deathText.setString("Deaths: " + std::to_string(deathCount));
    }

    void draw() {
        window.clear();

        if (isMenu) {
            window.draw(menuBackground);
            window.draw(menuText);

            int displayDeaths = deathCount > 0 ? deathCount - 1 : 0;
            sf::Text menuDeathText;
            menuDeathText.setFont(font);
            menuDeathText.setCharacterSize(24);
            menuDeathText.setFillColor(sf::Color::White);
            menuDeathText.setString("Deaths: " + std::to_string(displayDeaths));
            menuDeathText.setPosition(10, 50);
            window.draw(menuDeathText);

            window.draw(exitButton);
            window.draw(exitButtonText);
        }
        else {
            window.draw(background);
            for (auto& pipe : pipes) {
                pipe.draw(window);
                if (showHitboxes) {
                    drawHitbox(window, pipe.topPipe.getGlobalBounds());
                    drawHitbox(window, pipe.bottomPipe.getGlobalBounds());
                }
            }
            window.draw(bird.sprite);
            if (showHitboxes)
                drawHitbox(window, getBirdHitbox(bird.sprite));
            window.draw(scoreText);
            window.draw(deathText);
        }

        window.display();
    }

    void onDeath() {
        deathCount++;
        hitSound.play();
        pipes.clear();
        bird.reset();
        score = 0;
        isMenu = true;
        isGameOver = true;
    }

    void resetGame() {
        pipes.clear();
        bird.reset();
        score = 0;
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
