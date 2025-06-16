#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1024, 576 }), "Flappy bird");
    sf::Texture background;
    background.loadFromFile("C:\\Users\\User\\Desktop\\Finaltry\\Flapper birds\\Flappy Birds\\x64\\Debug\\bg_2.jpg");
    sf::Sprite backgroundsprite(background);


    // Main Game Loop
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.setSize({ 600, 800 });
        window.draw(backgroundsprite);

        window.display();

















    }
}
