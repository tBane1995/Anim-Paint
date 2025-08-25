#ifndef Slider_hpp
#define Slider_hpp

enum class SliderState { Idle, Changed };

class Slider : ElementGUI {
public:
	int min_value;
	int max_value;
	int current_value;

	Texture* bar_texture;
	Texture* slider_texture;

	sf::Sprite bar_sprite;
	sf::Sprite slider_sprite;

	SliderState state;
	sf::Vector2f offset;

	Slider(int min_value, int max_value) : ElementGUI() {
		this->min_value = min_value;
		this->max_value = max_value;
		current_value = min_value;

		bar_texture = getTexture(L"tex\\slider\\bar.png");
		bar_sprite = sf::Sprite(*bar_texture->texture);
		bar_sprite.setScale(128 / 8, 1.0f);

		slider_texture = getTexture(L"tex\\slider\\slider.png");
		slider_sprite = sf::Sprite(*slider_texture->texture);

		state = SliderState::Idle;
	}

	~Slider() { }

	void setValue(int value) {
		current_value = value;

		if (current_value < min_value)
			current_value = min_value;
		else if (current_value > max_value)
			current_value = max_value;

		slider_sprite.setPosition(getSliderPosition());

	}

	int getValue() {
		return current_value;
	}

	sf::Vector2f getSliderPosition() {
		float line_length = bar_sprite.getGlobalBounds().getSize().x - 16;

		sf::Vector2f result;
		result.x = bar_sprite.getPosition().x + float(current_value - min_value) * line_length / float(max_value - min_value);
		result.y = bar_sprite.getPosition().y - 4;

		return result;
	}

	void setPosition(sf::Vector2f position) {
		bar_sprite.setPosition(position);
		slider_sprite.setPosition(getSliderPosition());
	}

	void cursorHover() { 
		
	}
	void handleEvent(sf::Event& event) {

		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			if (slider_sprite.getGlobalBounds().contains(worldMousePosition)) {
				ElementGUI_pressed = this;
				offset = worldMousePosition - slider_sprite.getPosition();
				state = SliderState::Changed;

			}
			
		}
		else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			if (ElementGUI_pressed == this) {
				ElementGUI_pressed = nullptr;
				state = SliderState::Idle;
			}
		}

	}

	void update() {
		if (state == SliderState::Changed) {
			sf::Vector2f newPos = worldMousePosition - offset;
			newPos.y = bar_sprite.getPosition().y - 4;
			newPos.x = std::clamp(newPos.x, bar_sprite.getPosition().x, bar_sprite.getPosition().x + bar_sprite.getGlobalBounds().getSize().x - 16);
			slider_sprite.setPosition(newPos);

			current_value = float(slider_sprite.getPosition().x - bar_sprite.getPosition().x)/ (bar_sprite.getGlobalBounds().getSize().x-16) * (float(max_value - min_value)) + min_value;
		}
	}

	
	
	void draw() {
		window->draw(bar_sprite);
		window->draw(slider_sprite);
	}
};

#endif