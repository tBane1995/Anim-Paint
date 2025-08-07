#ifndef Window_hpp
#define Window_hpp

sf::View view(sf::FloatRect(0, 0, 800, 600));

sf::RenderWindow* window = new sf::RenderWindow(
	sf::VideoMode(view.getSize().x, view.getSize().y), 
	"Anim Paint", 
	sf::Style::Titlebar | sf::Style::Close);



#endif
