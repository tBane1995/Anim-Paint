#ifndef ColorsDialog_hpp
#define ColorsDialog_hpp

enum class ColorBoxState { Idle, Hover, Pressed };

class ColorBox : public ElementGUI {
public:
	sf::Sprite frame;
	sf::Color color;
	sf::RectangleShape rect;
	ColorBoxState state;
	std::function<void()> onclick_func;
	sf::Time clickTime;

	ColorBox(sf::Color color) {
		this->color = color;

		frame = sf::Sprite(*getTexture(L"tex\\colors\\frame.png")->texture);
		
		rect = sf::RectangleShape(sf::Vector2f(24, 24));
		rect.setFillColor(this->color);

		state = ColorBoxState::Idle;
		onclick_func = { };
		clickTime = currentTime;
		

	}

	~ColorBox() { }

	void setPosition(sf::Vector2f position) {
		frame.setPosition(position);
		rect.setPosition(position + sf::Vector2f(4, 4));
	}

	void unclick() {
		state = ColorBoxState::Idle;
		frame.setTexture(*getTexture(L"tex\\colors\\frame.png")->texture);
	}

	void hover() {
		state = ColorBoxState::Hover;
		frame.setTexture(*getTexture(L"tex\\colors\\frame_hover.png")->texture);

	}

	void click() {
		state = ColorBoxState::Pressed;
		frame.setTexture(*getTexture(L"tex\\colors\\frame_hover.png")->texture);
		clickTime = currentTime;
	}

	void cursorHover() {
		if (frame.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}

	void handleEvent(sf::Event& event) {
		if (frame.getGlobalBounds().contains(worldMousePosition)) {
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				ElementGUI_pressed = this;
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
				if (ElementGUI_pressed == this) {
					click();
				}
			}

		}
	}

	void update() {

		if (state == ColorBoxState::Pressed) {
			if ((currentTime - clickTime).asSeconds() > 0.05f) {
				if (onclick_func) {
					onclick_func();
				}
				ElementGUI_pressed = nullptr;
				unclick();
			}
		}
		else if (ElementGUI_hovered == this) {
			hover();
		}
		else
			unclick();
	}

	void draw() {
		window->draw(frame);
		window->draw(rect);
	}
};

class ColorsDialog : public Dialog {
public:

	std::vector < ColorBox* > colors;
	int current_color_id;

	ColorsDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) : Dialog(title, size, position) {

		colors.clear();

		// black - grey - white
		colors.push_back(new ColorBox(sf::Color(0, 0, 0)));
		colors.push_back(new ColorBox(sf::Color(63, 63, 63)));
		colors.push_back(new ColorBox(sf::Color(191, 191, 191)));
		colors.push_back(new ColorBox(sf::Color(255, 255, 255)));

		// red
		colors.push_back(new ColorBox(sf::Color(63, 0, 0)));
		colors.push_back(new ColorBox(sf::Color(127, 0, 0)));
		colors.push_back(new ColorBox(sf::Color(191, 0, 0)));
		colors.push_back(new ColorBox(sf::Color(255, 0, 0)));

		// green
		colors.push_back(new ColorBox(sf::Color(0, 63, 0)));
		colors.push_back(new ColorBox(sf::Color(0, 127, 0)));
		colors.push_back(new ColorBox(sf::Color(0, 191, 0)));
		colors.push_back(new ColorBox(sf::Color(0, 255, 0)));

		// blue
		colors.push_back(new ColorBox(sf::Color(0, 0, 63)));
		colors.push_back(new ColorBox(sf::Color(0, 0, 127)));
		colors.push_back(new ColorBox(sf::Color(0, 0, 191)));
		colors.push_back(new ColorBox(sf::Color(0, 0, 255)));

		// yellow
		colors.push_back(new ColorBox(sf::Color(63, 63, 0)));
		colors.push_back(new ColorBox(sf::Color(127, 127, 0)));
		colors.push_back(new ColorBox(sf::Color(191, 191, 0)));
		colors.push_back(new ColorBox(sf::Color(255, 255, 0)));

		// magenta
		colors.push_back(new ColorBox(sf::Color(63, 0, 63)));
		colors.push_back(new ColorBox(sf::Color(127, 0, 127)));
		colors.push_back(new ColorBox(sf::Color(191, 0, 191)));
		colors.push_back(new ColorBox(sf::Color(255, 0, 255)));

		// cyan
		colors.push_back(new ColorBox(sf::Color(0, 63, 63)));
		colors.push_back(new ColorBox(sf::Color(0, 127, 127)));
		colors.push_back(new ColorBox(sf::Color(0, 191, 191)));
		colors.push_back(new ColorBox(sf::Color(0, 255, 255)));

		current_color_id = 0;
		for (int i = 0; i < colors.size(); i++) {
			colors[i]->onclick_func = [this, i]() {
				current_color_id = i;
				};
		}

		setPosition(position);
	}

	~ColorsDialog() { }

	sf::Color getCurrentColor() {
		return colors[current_color_id]->color;
	}

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);

		sf::Vector2f pos;
		for (int i = 0; i < colors.size(); i++) {
			pos.x = this->position.x + dialog_padding + (colors_dialog_margin) * (i%4) + 32 * (i%4);
			pos.y = this->position.y + 32 + dialog_padding + (colors_dialog_margin) * (i/4) + 32 * (i/4);

			colors[i]->setPosition(pos);
		}
	}

	void cursorHover() {
		Dialog::cursorHover();

		for (auto& color : colors)
			color->cursorHover();

	}

	void handleEvent(sf::Event& event) {
		Dialog::handleEvent(event);
		for (auto& color : colors)
			color->handleEvent(event);

	}

	void update() {
		Dialog::update();
		for (auto& color : colors)
			color->update();
	}

	void draw() {
		Dialog::draw();

		for (auto& color : colors) {
			color->draw();
		}

	}

};

ColorsDialog* colors_dialog = nullptr;

#endif