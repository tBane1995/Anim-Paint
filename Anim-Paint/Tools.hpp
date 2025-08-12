#ifndef Tools_hpp
#define Tools_hpp


class Tools : public ElementGUI {
public:
	sf::RectangleShape rect;

	Tools() : ElementGUI() {
		rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, 64));
		rect.setFillColor(sf::Color(31, 31, 31));
		rect.setPosition(sf::Vector2f(0, menu_height));
	}

	void setPosition(sf::Vector2f position) {
		this->rect.setPosition(position);
	}

	void cursorHover() {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}

	}

	void handleEvent(sf::Event& event) {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				ElementGUI_pressed = this;
			}
		}
	}

	void update() {

	}

	void draw() {
		window->draw(rect);
	}
};

Tools* tools = nullptr;


#endif