#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>

class Pipe {
public:
    sf::Texture texture;
    sf::Sprite topPipe, bottomPipe;
    bool passed = false;

    Pipe(float x, float gapY) {
        texture.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\pipe.png");
        topPipe.setTexture(texture);
        bottomPipe.setTexture(texture);

        float pipeHeight = texture.getSize().y;
        float gapSize = 250.f;

        float topHeight = gapY - gapSize / 2.f;
        float bottomHeight = 720.f - (gapY + gapSize / 2.f);

        topPipe.setScale(1.f, -topHeight / pipeHeight);
        topPipe.setPosition(x, gapY - gapSize / 2.f);

        bottomPipe.setScale(1.f, bottomHeight / pipeHeight);
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
        sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
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

sf::FloatRect getBirdHitbox(const sf::Sprite& sprite) {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return sf::FloatRect(bounds.left + 60, bounds.top + 100, bounds.width - 120, bounds.height - 155);
}

void drawHitbox(sf::RenderWindow& window, const sf::FloatRect& rect) {
    sf::RectangleShape box;
    box.setPosition(rect.left, rect.top);
    box.setSize({ rect.width, rect.height });
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Red);
    box.setOutlineThickness(2.f);
    window.draw(box);
}

class Game {
private:
    sf::RenderWindow window;
    sf::Texture backgroundTex, menuBackgroundTex;
    sf::Sprite background, menuBackground;

    Bird bird;
    std::vector<Pipe> pipes;

    sf::Font font;
    sf::Text scoreText, deathText;

    int score = 0, deathCount = 0;
    bool showHitboxes = false, isMenu = true, isGameOver = true, slowMotion = false;

    sf::Clock clock;
    float spawnTimer = 0;

    sf::SoundBuffer flapBuffer, scoreBuffer, hitBuffer;
    sf::Sound scoreSound, hitSound;

    sf::Music backgroundMusic;

    sf::Texture jetTexture;
    sf::Sprite jet;
    float jetSpeed = 450.f; // Faster jet
    sf::SoundBuffer jetSoundBuffer;
    sf::Sound jetSound;

    bool jetSoundPlayed = false;
    float jetTimer = 0.f;     // Controls how often the jet appears
    float jetInterval = 8.f;  // Less frequent flyby

    sf::RectangleShape startButton, resumeButton, exitButton;
    sf::Text startText, resumeText, exitText;

public:
    Game() : window(sf::VideoMode(1080, 720), "Flappy Bird - SFML 2.6.2") {
        window.setFramerateLimit(60);

        backgroundTex.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\background.png");
        background.setTexture(backgroundTex);
        background.setPosition(-825, 0);
        background.scale(1.5f, 1.0f);

        menuBackgroundTex.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\menu_background.png");
        menuBackground.setTexture(menuBackgroundTex);
        menuBackground.setPosition(-825, 0);
        menuBackground.scale(1.5f, 1.0f);

        font.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\FlappyBirdy.ttf");

        scoreText.setFont(font);
        scoreText.setCharacterSize(36);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        deathText.setFont(font);
        deathText.setCharacterSize(24);
        deathText.setFillColor(sf::Color::White);
        deathText.setPosition(10, 50);

        flapBuffer.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\flap.wav");
        scoreBuffer.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\score.wav");
        hitBuffer.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\TrumpOhnawwh.wav");

        bird.setFlapSound(flapBuffer);
        scoreSound.setBuffer(scoreBuffer);
        hitSound.setBuffer(hitBuffer);

        backgroundMusic.openFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\backgroundMusic.wav");
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(40.f);
        backgroundMusic.play();

        jetTexture.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\jet.png");
        jet.setTexture(jetTexture);
        jet.setScale(0.4f, 0.4f);
        jet.setPosition(-300.f, 100.f);

        jetSoundBuffer.loadFromFile("C:\\Users\\User\\Desktop\\FlapperBIRD\\FlappyBirdFinalProject\\assets\\jet_flyby.wav");
        jetSound.setBuffer(jetSoundBuffer);
        jetSound.setVolume(60.f);

        sf::Vector2f buttonSize(300.f, 60.f);
        startButton.setSize(buttonSize);
        startButton.setFillColor(sf::Color(100,250,100));
        startButton.setPosition(390.f, 200.f);

        resumeButton.setSize(buttonSize);
        resumeButton.setFillColor(sf::Color(100,150,250));
        resumeButton.setPosition(390.f, 300.f);

        exitButton.setSize(buttonSize);
        exitButton.setFillColor(sf::Color(250,100,100));
        exitButton.setPosition(390.f, 400.f);

        startText.setFont(font);
        startText.setString("Start Game");
        startText.setCharacterSize(28);
        startText.setFillColor(sf::Color::Black);
        startText.setPosition(420.f, 210.f);

        resumeText.setFont(font);
        resumeText.setString("Resume");
        resumeText.setCharacterSize(28);
        resumeText.setFillColor(sf::Color::Black);
        resumeText.setPosition(450.f, 310.f);

        exitText.setFont(font);
        exitText.setString("Exit");
        exitText.setCharacterSize(28);
        exitText.setFillColor(sf::Color::Black);
        exitText.setPosition(470.f, 410.f);

        srand(static_cast<unsigned>(time(0)));
    }

    void run() {
        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();
            if (slowMotion) dt *= 0.25f;
            handleEvents();
            if (!isMenu) update(dt);
            draw();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space && !isMenu) bird.flap();
                if (event.key.code == sf::Keyboard::P) isMenu = true;
                if (event.key.code == sf::Keyboard::H) showHitboxes = !showHitboxes;
                if (event.key.code == sf::Keyboard::S && !isMenu) slowMotion = !slowMotion;
            }

            if (event.type == sf::Event::MouseButtonPressed && isMenu) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (startButton.getGlobalBounds().contains(mousePos)) { isMenu = false; isGameOver = false; resetGame(); }
                if (resumeButton.getGlobalBounds().contains(mousePos) && !isGameOver) isMenu = false;
                if (exitButton.getGlobalBounds().contains(mousePos)) window.close();
            }
        }
    }

    void update(float dt) {
        bird.update(dt);

        spawnTimer += dt;
        if (spawnTimer >= 2.0f) {
            float gapY = 200 + rand() % 250;
            pipes.emplace_back(1100, gapY);
            spawnTimer = 0;
        }

        for (auto& pipe : pipes)
            pipe.move(-200 * dt);

        for (auto it = pipes.begin(); it != pipes.end();) {
            if (it->isOffScreen()) it = pipes.erase(it);
            else {
                if (!it->passed && it->getX() + 52 < bird.sprite.getPosition().x) { it->passed = true; score++; scoreSound.play(); }
                if (it->checkCollision(getBirdHitbox(bird.sprite))) { onDeath(); return; }
                ++it;
            }
        }

        // Jet logic: flyby less often and faster
        jetTimer += dt;
        if (jetTimer >= jetInterval) {
            jet.move(jetSpeed * dt, 0);
            if (jet.getPosition().x > 0 && !jetSoundPlayed) {
                jetSound.play();
                jetSoundPlayed = true;
            }
            if (jet.getPosition().x > 1200) {
                jet.setPosition(-300.f, 50.f + rand() % 200);
                jetSoundPlayed = false;
                jetTimer = 0.f;
                jetInterval = 6.f + static_cast<float>(rand() % 6); // Randomize next flyby between 6-12 seconds
            }
        }

        if (bird.sprite.getPosition().y > 700 || bird.sprite.getPosition().y < -100)
            onDeath();

        scoreText.setString("Score: " + std::to_string(score));
        deathText.setString("Deaths: " + std::to_string(deathCount));
    }

    void draw() {
        window.clear();
        window.draw(background);
        window.draw(jet);

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

        if (isMenu) {
            window.draw(menuBackground);
            window.draw(startButton); window.draw(startText);
            if (!isGameOver) { window.draw(resumeButton); window.draw(resumeText); }
            window.draw(exitButton); window.draw(exitText);
        }

        window.draw(scoreText);
        window.draw(deathText);
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
