#ifndef Canvas_hpp
#define Canvas_hpp



class Canvas : public ElementGUI {
public:

	// background
	sf::Image bg_image;
	sf::Texture bg_texture;
	sf::Sprite bg_sprite;

	sf::Vector2f size;

	int pixel_size;

	float zoom;			// in percentage
	float zoom_delta;	// const
	float min_zoom;
	float max_zoom;

	Canvas(sf::Vector2f size) : ElementGUI() {
		this->size = size;

		this->pixel_size = 8.0f;

		this->zoom = 1.0f;			// 100%
		this->zoom_delta = 16.0f;
		this->min_zoom = 0.25;
		this->max_zoom = 2.0f;

		generateBackground();

	}

	~Canvas() { }

	void updateBackgroundSprite() {

		sf::Vector2f s = sf::Vector2f(int(size.x * zoom_delta * zoom), int(size.y * zoom_delta * zoom));

		bg_texture = sf::Texture();
		bg_texture.loadFromImage(bg_image);

		bg_sprite = sf::Sprite(bg_texture);
		bg_sprite.setPosition(sf::Vector2f(window->getSize()) / 2.0f - s / 2.0f);
	}

	void generateBackground() {

		bg_image = sf::Image();
		float whole_zoom = zoom_delta * zoom;
		sf::Vector2f s = sf::Vector2f(int(size.x * whole_zoom), int(size.y * whole_zoom));
		bg_image.create(s.x, s.y, canvas_color);

		sf::Color c1 = sf::Color(64, 64, 64);
		sf::Color c2 = sf::Color(96, 96, 96);

		for (int y = 0; y < s.y; y++) {
			for (int x = 0; x < s.x; x++) {

				int xx = x / pixel_size;
				int yy = y / pixel_size;

				sf::Color c;

				if (yy % 2 == 0)
					(xx % 2 == 0) ? c = c1 : c = c2;
				else
					(xx % 2 == 1) ? c = c1 : c = c2;

				bg_image.setPixel(x, y, c); // tu jest błąd - wychodzi poza zakres

			}
		}

		updateBackgroundSprite();
	}

	float getZoom() {
		return this->zoom;
	}

	void setZoom(float zoom) {
		this->zoom = zoom;
		generateBackground();
		updateBackgroundSprite();
	}

	void setPixel(sf::Vector2f worldMousePosition, sf::Color color) {

		float whole_zoom = zoom_delta * zoom;
		sf::Vector2f s = (worldMousePosition - bg_sprite.getPosition());
		s.x = int(s.x) / 8 * 8;
		s.y = int(s.y) / 8 * 8;
		std::cout << s.x << ", " << s.y << "\n";

		for (int y = 0; y < pixel_size; y++) {
			for (int x = 0; x < pixel_size; x++) {
				bg_image.setPixel(s.x+x, s.y+y, color);
			}
		}

		updateBackgroundSprite();

	}

	void cursorHover() {
		if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}

	void handleEvent(sf::Event& event) {
		if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				ElementGUI_pressed = this;

				setPixel(worldMousePosition, sf::Color::Black);
			}

			if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				setPixel(worldMousePosition, sf::Color::Black);
			}

			if (event.type == sf::Event::MouseWheelScrolled) {
				float zoom = getZoom();
				zoom += 0.25f * event.mouseWheelScroll.delta;

				if (zoom > max_zoom)
					zoom = max_zoom;

				if (zoom < min_zoom)
					zoom = min_zoom;

				setZoom(zoom);
				
			}
		}
	}

	void update() {

	}

	void draw() {
		
		window->draw(bg_sprite);
	}

};

Canvas* canvas = nullptr;

#endif