#ifndef Button_hpp
#define Button_hpp

enum class ButtonState { Idle, Hover, Pressed };

class Button : public ElementGUI {
public:
	Button() : ElementGUI() {

	}

	~Button() { }

	virtual sf::Vector2f getSize() { return sf::Vector2f(0, 0); }
	virtual void setPosition(sf::Vector2f position) { }
	virtual void cursorHover() { }
	virtual void handleEvent(sf::Event& event) { }
	virtual void update() { }
	virtual void draw() { }

};

class NormalButton : public Button {
public:
	
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;

	sf::Vector2f position;
	
	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	NormalButton(Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0,0)) {

		this->texture = texture;
		this->hoverTexture = hoverTexture;
		sprite = sf::Sprite(*texture->texture);

		setPosition(position);

		state = ButtonState::Idle;

		hover_func = { };
		onclick_func = { };
		clickTime = currentTime;

	}

	~NormalButton() { }

	sf::Vector2f getSize() {
		return sprite.getGlobalBounds().getSize();
	}

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

class ButtonWithBottomText : public Button {
public:

	sf::Color textColor;
	sf::Color hoverTextColor;
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;
	sf::Text text;
	sf::RectangleShape rect;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0, 0)) {

		this->textColor = textColor;
		this->hoverTextColor = hoverTextColor;

		this->text = sf::Text(text, basicFont, 13);
		this->text.setFillColor(textColor);

		this->texture = texture;
		this->hoverTexture = hoverTexture;
		sprite = sf::Sprite(*texture->texture);

		rect = sf::RectangleShape(sf::Vector2f(48, 64));
		rect.setFillColor(rectColor);
		setPosition(position);

		state = ButtonState::Idle;

		hover_func = { };
		onclick_func = { };
		clickTime = currentTime;

	}

	~ButtonWithBottomText() {}

	sf::Vector2f getSize() {
		return rect.getGlobalBounds().getSize();
	}

	void setPosition(sf::Vector2f position) {
		this->position = position;
		rect.setPosition(position);
		sprite.setPosition(position);
		text.setPosition(position + sf::Vector2f(48/2 - text.getGlobalBounds().width/2.0f, rect.getSize().y - basicFont.getLineSpacing(13) - 4));
	}

	void unclick() {
		state = ButtonState::Idle;
		sprite.setTexture(*texture->texture);
		text.setFillColor(textColor);
	}

	void hover() {
		state = ButtonState::Hover;
		sprite.setTexture(*hoverTexture->texture);
		text.setFillColor(hoverTextColor);

	}

	void click() {
		state = ButtonState::Pressed;
		sprite.setTexture(*hoverTexture->texture);
		text.setFillColor(hoverTextColor);
		clickTime = currentTime;
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
		window->draw(rect);
		window->draw(sprite);
		window->draw(text);
	}

};

class ButtonWithRightText : public Button {
public:

	sf::Color textColor;
	sf::Color hoverTextColor;
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;
	sf::Text text;
	sf::RectangleShape rect;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	ButtonWithRightText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0, 0)) {

		this->textColor = textColor;
		this->hoverTextColor = hoverTextColor;

		this->text = sf::Text(text, basicFont, 13);
		this->text.setFillColor(textColor);

		this->texture = texture;
		this->hoverTexture = hoverTexture;
		sprite = sf::Sprite(*texture->texture);

		rect = sf::RectangleShape(sf::Vector2f(64, 32));
		rect.setFillColor(rectColor);
		setPosition(position);

		state = ButtonState::Idle;

		hover_func = { };
		onclick_func = { };
		clickTime = currentTime;

	}

	~ButtonWithRightText() {}

	sf::Vector2f getSize() {
		return rect.getGlobalBounds().getSize();
	}

	void setPosition(sf::Vector2f position) {
		this->position = position;
		rect.setPosition(position);
		sprite.setPosition(position);
		text.setPosition(position + sf::Vector2f(32, 24 - basicFont.getLineSpacing(13)));
	}

	void unclick() {
		state = ButtonState::Idle;
		sprite.setTexture(*texture->texture);
		text.setFillColor(textColor);
	}

	void hover() {
		state = ButtonState::Hover;
		sprite.setTexture(*hoverTexture->texture);
		text.setFillColor(hoverTextColor);

	}

	void click() {
		state = ButtonState::Pressed;
		sprite.setTexture(*hoverTexture->texture);
		text.setFillColor(hoverTextColor);
		clickTime = currentTime;
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
		window->draw(rect);
		window->draw(sprite);
		window->draw(text);
	}

};

class ButtonWithTopTextAndList : public Button {
public:

	sf::Color textColor;
	sf::Color hoverTextColor;
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;
	sf::Text text;
	sf::RectangleShape rect;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	ButtonWithTopTextAndList(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, sf::Vector2f position = sf::Vector2f(0, 0)) {

		this->textColor = textColor;
		this->hoverTextColor = hoverTextColor;

		this->text = sf::Text(text, basicFont, 13);
		this->text.setFillColor(textColor);

		this->texture = getTexture(L"tex\\tools\\bottom_arrow.png");
		this->hoverTexture = getTexture(L"tex\\tools\\bottom_arrow_hover.png");
		sprite = sf::Sprite(*texture->texture);

		rect = sf::RectangleShape(sf::Vector2f(48, 32));
		rect.setFillColor(rectColor);
		setPosition(position);

		state = ButtonState::Idle;

		hover_func = { };
		onclick_func = { };
		clickTime = currentTime;

	}

	~ButtonWithTopTextAndList() {}

	sf::Vector2f getSize() {
		return rect.getGlobalBounds().getSize();
	}

	void setPosition(sf::Vector2f position) {
		this->position = position;
		rect.setPosition(position);
		sprite.setPosition(position+sf::Vector2f(0,16));
		text.setPosition(position + sf::Vector2f(48 / 2 - text.getGlobalBounds().width / 2.0f, 0));
	}

	void unclick() {
		state = ButtonState::Idle;
		sprite.setTexture(*texture->texture);
		text.setFillColor(textColor);
	}

	void hover() {
		state = ButtonState::Hover;
		sprite.setTexture(*hoverTexture->texture);
		text.setFillColor(hoverTextColor);

	}

	void click() {
		state = ButtonState::Pressed;
		sprite.setTexture(*hoverTexture->texture);
		text.setFillColor(hoverTextColor);
		clickTime = currentTime;
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
		window->draw(rect);
		window->draw(sprite);
		window->draw(text);
	}

};
#endif