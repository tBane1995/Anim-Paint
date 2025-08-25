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

class NormalButtonWithText : public Button {
public:

	sf::RectangleShape rect;
	sf::Text text;
	sf::Color normal_color;
	sf::Color hover_color;
	sf::Color press_color;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	NormalButtonWithText(std::wstring text, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) {

		normal_color = button_normal_color;
		hover_color = button_hover_color;
		press_color = button_press_color;

		rect = sf::RectangleShape(size);
		rect.setFillColor(normal_color);
		rect.setOutlineThickness(dialog_border_width);
		rect.setOutlineColor(sf::Color(dialog_border_color));

		this->text = sf::Text(text, basicFont, 13);
		this->text.setFillColor(button_text_color);

		setPosition(position);

		state = ButtonState::Idle;

		hover_func = { };
		onclick_func = { };
		clickTime = currentTime;

	}

	~NormalButtonWithText() {}

	sf::Vector2f getSize() {
		return rect.getSize();
	}

	void setPosition(sf::Vector2f position) {
		this->position = position;
		rect.setPosition(position);
		sf::Vector2f text_pos;
		text_pos.x = rect.getPosition().x + rect.getSize().x / 2 - text.getGlobalBounds().width/2;
		text_pos.y = rect.getPosition().y + rect.getSize().y / 2 - basicFont.getLineSpacing(13) / 2;
		text.setPosition(text_pos);
	}

	void unclick() {
		state = ButtonState::Idle;
		rect.setFillColor(normal_color);
	}

	void hover() {
		state = ButtonState::Hover;
		rect.setFillColor(hover_color);
	}

	void click() {
		state = ButtonState::Pressed;
		rect.setFillColor(press_color);
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
		window->draw(text);
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

class Option : public Button {
public:

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


	Option(std::wstring text, Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0, 0)) {

		this->text = sf::Text(text, basicFont, 13);
		this->text.setFillColor(menu_text_color);

		this->texture = texture;
		this->hoverTexture = hoverTexture;
		sprite = sf::Sprite(*texture->texture);

		rect = sf::RectangleShape(sf::Vector2f(32 + this->text.getGlobalBounds().width + 8, 32));
		rect.setFillColor(optionbox_idle_color);
		setPosition(position);

		state = ButtonState::Idle;

		hover_func = { };
		onclick_func = { };
		clickTime = currentTime;

	}

	~Option() {}

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
		rect.setFillColor(optionbox_idle_color);
		text.setFillColor(menu_text_color);
	}

	void hover() {
		state = ButtonState::Hover;
		sprite.setTexture(*hoverTexture->texture);
		rect.setFillColor(optionbox_hover_color);
		text.setFillColor(menu_text_color);

	}

	void click() {
		state = ButtonState::Pressed;
		sprite.setTexture(*hoverTexture->texture);
		rect.setFillColor(optionbox_press_color);
		text.setFillColor(menu_text_color);
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

	sf::RectangleShape list_rect;
	sf::VertexArray list_border;
	std::vector < Option* > options;
	bool isOpen;

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

		list_rect = sf::RectangleShape();
		list_rect.setFillColor(menuoptions_border_color);
		options.clear();
		isOpen = false;

		state = ButtonState::Idle;

		hover_func = { };
		onclick_func = { };
		clickTime = currentTime;

		addOption(L"paste");
		addOption(L"from file");
		addOption(L"transparency");
	}

	~ButtonWithTopTextAndList() {}

	sf::Vector2f getSize() {
		return rect.getGlobalBounds().getSize();
	}

	

	void addOption(std::wstring text) {
		Option* o = new Option(text, getTexture(L"tex\\tools\\btn_none.png"), getTexture(L"tex\\tools\\btn_none_hover.png"));
		
		if (options.size() == 0) {
			options.push_back(o);
			list_rect.setSize(sf::Vector2f(0, o->rect.getSize().y + menuoptions_border_width));
			return;
		}

		int wdt = (options.back()->rect.getSize().x > o->rect.getSize().x) ? options.back()->rect.getSize().x : o->rect.getSize().x;
		
		list_rect.setSize(sf::Vector2f(wdt+2*menuoptions_border_width, list_rect.getSize().y + o->rect.getSize().y));

		options.push_back(o);
		for (auto& o : options) {
			o->rect.setSize(sf::Vector2f(wdt, 32));
		}
	}

	void setPosition(sf::Vector2f position) {
		this->position = position;
		rect.setPosition(position);
		sprite.setPosition(position+sf::Vector2f(0,16));
		text.setPosition(position + sf::Vector2f(48 / 2 - text.getGlobalBounds().width / 2.0f, 0));

		
		sf::Vector2f pos = rect.getPosition() + sf::Vector2f(0, rect.getSize().y);
		
		this->list_rect.setPosition(pos - sf::Vector2f(menuoptions_border_width, 0));
		for (auto& o : options) {
			o->setPosition(pos);
			pos.y += 32;
		}
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

		if (isOpen) {
			for (auto& option : options)
				option->cursorHover();
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
					isOpen = !isOpen;
				}
			}
		}

		bool clicked_in_menu = false;

		if (isOpen) {
			for (auto& option : options) {
				option->handleEvent(event);
				if (ElementGUI_pressed == option)
					clicked_in_menu = true;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			if (clicked_in_menu == false) {
				isOpen = false;
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

		for (auto& option : options)
			option->update();
	}

	void draw() {
		window->draw(rect);
		window->draw(sprite);
		window->draw(text);

		if (isOpen) {

			window->draw(list_rect);
			for (auto& o : options) {
				o->draw();
			}
			
		}
	}

};
#endif