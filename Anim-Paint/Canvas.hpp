#ifndef Canvas_hpp
#define Canvas_hpp

class Canvas : public ElementGUI {
public:

	sf::Image img;
	sf::Vector2f size;

	Canvas(sf::Vector2f size) : ElementGUI() {
		this->size = size;
		img.create(size.x, size.y, canvas_color);

		sf::Color c1 = sf::Color(64, 64, 64);
		sf::Color c2 = sf::Color(96, 96, 96);

		for (int y = 0; y < size.y; y++) {
			for (int x = 0; x < size.x; x++) {

				int xx = x / 8;
				int yy = y / 8;
				sf::Color c;
				if (yy % 2 == 0) {
					(xx % 2 == 0) ? c = c1 : c = c2;
				}
				else {
					(xx % 2 == 1) ? c = c1 : c = c2;
				}

				img.setPixel(x, y, c);

				
			}
		}

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