#ifndef Canvas_hpp
#define Canvas_hpp

class Canvas : public ElementGUI {
public:

	sf::Image img;
	sf::Vector2f size;

	Canvas(sf::Vector2f size) : ElementGUI() {
		this->size = size;
		img.create(size.x, size.y, canvas_color);

	}

	~Canvas() { }


	void cursorHover() {
	}

	void handleEvent(sf::Event& event) {

	}

	void update() {
	}

	void draw() {
		sf::Texture tex;
		tex.loadFromImage(img);
		sf::Sprite sprite;
		sprite.setTexture(tex);
		sprite.setPosition(sf::Vector2f(window->getSize()) / 2.0f - size/2.0f);
		window->draw(sprite);
	}

};

Canvas* canvas = nullptr;

#endif