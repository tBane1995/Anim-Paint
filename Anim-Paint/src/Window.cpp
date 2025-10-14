#include "Window.hpp"
#include "SFML/Graphics.hpp"

sf::RenderWindow* window = nullptr;
sf::View mainView = sf::View(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(800, 600 )));