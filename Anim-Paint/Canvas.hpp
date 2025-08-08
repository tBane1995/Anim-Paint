#ifndef Canvas_hpp
#define Canvas_hpp



class Canvas : public ElementGUI {
public:

	// background
	sf::Image bg_image;
	sf::Texture bg_texture;
	sf::Sprite bg_sprite;

	sf::Vector2f size;
	float zoom;

	Canvas(sf::Vector2f size) : ElementGUI() {
		this->size = size;
		this->zoom = 16;

		generateBackground();

	}

	~Canvas() { }

	void setZoom(float zoom) {
		this->zoom = zoom;
	}

	void generateBackground() {

		bg_image = sf::Image();
		bg_image.create(size.x * zoom, size.y * zoom, canvas_color);

		sf::Color c1 = sf::Color(64, 64, 64);
		sf::Color c2 = sf::Color(96, 96, 96);

		for (int y = 0; y < size.y * zoom; y++) {
			for (int x = 0; x < size.x * zoom; x++) {

				int xx = x / 8;
				int yy = y / 8;

				sf::Color c;

				if (yy % 2 == 0)
					(xx % 2 == 0) ? c = c1 : c = c2;
				else
					(xx % 2 == 1) ? c = c1 : c = c2;

				bg_image.setPixel(x, y, c);

			}
		}

		bg_texture = sf::Texture();
		bg_texture.loadFromImage(bg_image);

		bg_sprite.setTexture(bg_texture);
		bg_sprite.setPosition(sf::Vector2f(window->getSize()) / 2.0f - size*zoom / 2.0f);
	}

	void cursorHover() {
	}

	void handleEvent(sf::Event& event) {

	}

	void update() {

	}

	void draw() {
		
		window->draw(bg_sprite);
	}

};

Canvas* canvas = nullptr;

#endif