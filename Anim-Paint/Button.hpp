#ifndef Button_hpp
#define Button_hpp

enum class ButtonState { Idle, Hover, Pressed };

class Button : public ElementGUI {
public:
	
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;

	sf::Vector2f position;
	
	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	Button(Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0,0)) {

		this->texture = texture;
		this->hoverTexture = hoverTexture;
		sprite = sf::Sprite(*texture->texture);

		this->position = position;
		setPosition(position);

		state = ButtonState::Idle;

		hover_func = { };
		onclick_func = { };
		clickTime = currentTime;

	}

	~Button() { }

	void setPosition(sf::Vector2f position) {
		this->position = position;
		sprite.setPosition(position);
	}

	void unclick() {
		state = ButtonState::Idle;
		sprite.setTexture(*texture->texture);
	}

	void hover() {
		state = ButtonState::Hover;
		sprite.setTexture(*hoverTexture->texture);
		
	}

	void click() {
		state = ButtonState::Pressed;
		sprite.setTexture(*hoverTexture->texture);
		clickTime = currentTime;
	}


	void cursorHover() {

		if (sprite.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
			
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

		if (state == ButtonState::Pressed) {
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