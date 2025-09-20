#ifndef Checkbox_hpp
#define Checkbox_hpp

enum class CheckboxState { Idle, Hover, Pressed };

class Checkbox : public ElementGUI {
public:
	int value = -1;
	std::vector < Texture* > textures;
	std::vector < Texture* > hoverTextures;
	sf::Sprite sprite;

	sf::Vector2f position;
	
	CheckboxState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;

	Checkbox() {
		value = -1;
		textures.clear();
		hoverTextures.clear();
	}

	~Checkbox() { }

	void setPosition(sf::Vector2f position) {
		this->position = position;
		sprite.setPosition(position);
	}

	void setValue(int value) {
		this->value = value;

		sprite.setTexture(*textures[value]->texture);
	}

	void addValue(Texture* texture, Texture* hoverTexture) {

		textures.push_back(texture);
		hoverTextures.push_back(hoverTexture);
	}

	void unclick() {
		state = CheckboxState::Idle;
		sprite.setTexture(*textures[value]->texture);
	}

	void hover() {
		state = CheckboxState::Hover;
		sprite.setTexture(*hoverTextures[value]->texture);

	}

	void click() {
		state = CheckboxState::Pressed;

		value += 1;
		if (value >= textures.size())
			value = 0;

		sprite.setTexture(*hoverTextures[value]->texture);
		clickTime = currentTime;
	}

	void cursorHover() {

		if (value > -1) {
			if (sprite.getGlobalBounds().contains(worldMousePosition)) {
				ElementGUI_hovered = this;
			}
		}

	}

	void handleEvent(sf::Event& event) {
		if (sprite.getGlobalBounds().contains(worldMousePosition)) {
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

		if (state == CheckboxState::Pressed) {
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
		window->draw(sprite);
	}

};

#endif