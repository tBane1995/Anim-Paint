#ifndef MainMenu_hpp
#define MainMenu_hpp

enum class MenuBoxState { Idle, Hover, Pressed };


class MenuBox : public ElementGUI {
public:
	sf::RectangleShape rect;
	sf::Text text;
	MenuBoxState state;
	std::function<void()> onclick_func;
	sf::Time clickTime;

	MenuBox(std::wstring text) : ElementGUI() {
		this->text = sf::Text(text, basicFont, menu_font_size);
		this->text.setFillColor(menu_text_color);
		int w = this->text.getGlobalBounds().width + (menu_height - basicFont.getLineSpacing(menu_font_size))/2;
		rect = sf::RectangleShape(sf::Vector2f(w, menu_height-(2*menu_padding)));
	
	}

	void setPosition(sf::Vector2f position) {
		this->rect.setPosition(position + sf::Vector2f(menu_padding, 0));
		this->text.setPosition(position + sf::Vector2f(menu_padding, -menu_padding + (menu_height - basicFont.getLineSpacing(menu_font_size))/2));
	}

	void unclick() {
		state = MenuBoxState::Idle;
		rect.setFillColor(menubox_idle_color);
	}

	void hover() {
		state = MenuBoxState::Hover;
		rect.setFillColor(menubox_hover_color);

	}

	void click() {
		state = MenuBoxState::Pressed;
		rect.setFillColor(menubox_press_color);
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

		if (state == MenuBoxState::Pressed) {
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

class MainMenu : public ElementGUI {
public:

	sf::RectangleShape rect;
	sf::Sprite logo;
	std::vector < MenuBox* > options;

 	MainMenu() : ElementGUI() {
		rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, menu_height));
		rect.setFillColor(menu_rect_color);
		rect.setPosition(0, 0);

		logo = sf::Sprite(*getTexture(L"tex\\logo\\small_logo.png")->texture);

		options.push_back(new MenuBox(L"file"));
		options.push_back(new MenuBox(L"settings"));

		setPosition(sf::Vector2f(0, 0));
	}

	~MainMenu() { }

	sf::Vector2f getSize() {
		return rect.getSize();
	}

	void setPosition(sf::Vector2f position) {
		rect.setPosition(position);
		logo.setPosition(position + sf::Vector2f(0, menu_padding));

		int x = 24;
		int dist = 8;
		int y = position.y + menu_padding;
		for (int i = 0; i < options.size(); i++) {
			options[i]->setPosition(sf::Vector2f(position.x + x, y));
			x = x + options[i]->rect.getSize().x + dist * (i+1);
		}
	}

	void cursorHover() {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}

		for (auto& option : options)
			option->cursorHover();
		
	}

	void handleEvent(sf::Event& event) {
		for (auto& option : options)
			option->handleEvent(event);
	}

	void update() {
		for (auto& option : options)
			option->update();
	}

	void draw() {
		window->draw(rect);
		window->draw(logo);

		for (auto& option : options)
			option->draw();
	}
};

MainMenu* main_menu = nullptr;

#endif