#ifndef MainMenu_hpp
#define MainMenu_hpp

class OptionBox : public ElementGUI {
public:
	sf::RectangleShape rect;
	sf::Text text;
	ButtonState state;
	std::function<void()> onclick_func;
	sf::Time clickTime;

	OptionBox(std::wstring text) {
		this->text = sf::Text(text, basicFont, menu_font_size);
		this->text.setFillColor(menu_text_color);
		rect = sf::RectangleShape(sf::Vector2f(this->text.getGlobalBounds().width + 2 * menu_horizontal_margin, menu_height));
		rect.setFillColor(optionbox_idle_color);
		state = ButtonState::Idle;
		onclick_func = { };
	}

	~OptionBox() { }

	void setPosition(sf::Vector2f position) {
		this->rect.setPosition(position);
		this->text.setPosition(position + sf::Vector2f(menu_horizontal_margin, (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
	}

	void unclick() {
		state = ButtonState::Idle;
		rect.setFillColor(optionbox_idle_color);
	}

	void hover() {
		state = ButtonState::Hover;
		rect.setFillColor(optionbox_hover_color);

	}

	void click() {
		state = ButtonState::Pressed;
		rect.setFillColor(optionbox_press_color);
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

class MenuBox : public ElementGUI {
public:
	sf::RectangleShape rect;
	sf::Text text;
	ButtonState state;
	std::function<void()> onclick_func;
	sf::Time clickTime;
	std::vector < OptionBox* > options;
	bool isOpen;

	MenuBox(std::wstring text) : ElementGUI() {
		this->text = sf::Text(text, basicFont, menu_font_size);
		this->text.setFillColor(menu_text_color);
		rect = sf::RectangleShape(sf::Vector2f(this->text.getGlobalBounds().width + 2*menu_horizontal_margin, menu_height));

		isOpen = false;
		options.clear();
	}

	void addOption(OptionBox* option) {
		options.push_back(option);

		int max_wdt = 0;
		for (auto& o : options) {
			if (o->rect.getSize().x > max_wdt)
				max_wdt = o->rect.getSize().x;
		}

		for (auto& o : options) {
			o->rect.setSize(sf::Vector2f(max_wdt, menu_height));
		}


	}

	void setPosition(sf::Vector2f position) {
		this->rect.setPosition(position);
		this->text.setPosition(position + sf::Vector2f(menu_horizontal_margin, (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
		
		for (int i = 0; i < options.size(); i++) {
			options[i]->setPosition(this->rect.getPosition() + sf::Vector2f(0, this->rect.getSize().y + i*menu_height));
		}
	}

	void unclick() {
		state = ButtonState::Idle;
		rect.setFillColor(menubox_idle_color);
	}

	void hover() {
		state = ButtonState::Hover;
		rect.setFillColor(menubox_hover_color);

	}

	void click() {
		state = ButtonState::Pressed;
		rect.setFillColor(menubox_press_color);
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
				}
			}

		}

		if (isOpen) {
			for (auto& option : options)
				option->handleEvent(event);
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
		window->draw(text);

		if (isOpen) {
			for (auto& option : options)
				option->draw();
		}
	}
};

class MainMenu : public ElementGUI {
public:

	sf::RectangleShape rect;
	sf::Sprite logo;
	std::vector < MenuBox* > menu_boxes;
	MenuBox* open_menu_box;

	MainMenu() : ElementGUI() {
		rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, menu_height));
		rect.setFillColor(menu_rect_color);
		rect.setPosition(0, 0);

		logo = sf::Sprite(*getTexture(L"tex\\logo\\small_logo.png")->texture);

		MenuBox* file = new MenuBox(L"file");
		OptionBox* newFile = new OptionBox(L"new file");
		OptionBox* save = new OptionBox(L"save");
		OptionBox* saveAs = new OptionBox(L"save as");
		OptionBox* open = new OptionBox(L"open");
		OptionBox* exp = new OptionBox(L"export");
		OptionBox* imp = new OptionBox(L"import");
		
		file->addOption(newFile);
		file->addOption(save);
		file->addOption(saveAs);
		file->addOption(open);
		file->addOption(exp);
		file->addOption(imp);

		menu_boxes.push_back(file);


		MenuBox* settings = new MenuBox(L"settings");
		menu_boxes.push_back(settings);

		file->onclick_func = [this, file](){ 
			if (open_menu_box != nullptr)
				open_menu_box->isOpen = false;

			open_menu_box = file;
			open_menu_box->isOpen = true;
			};

		settings->onclick_func = [this, settings]() {
			if (open_menu_box != nullptr)
				open_menu_box->isOpen = false;

			open_menu_box = settings;
			open_menu_box->isOpen = true;
			};

		open_menu_box = nullptr;
		setPosition(sf::Vector2f(0, 0));
	}

	~MainMenu() {}

	sf::Vector2f getSize() {
		return rect.getSize();
	}

	void setPosition(sf::Vector2f position) {
		rect.setPosition(position);
		logo.setPosition(sf::Vector2f(0, (menu_height- logo.getGlobalBounds().getSize().y) / 2.0f));

		int x = 24;
		int y = position.y + menu_padding;
		for (int i = 0; i < menu_boxes.size(); i++) {
			menu_boxes[i]->setPosition(sf::Vector2f(position.x + x, y));
			x = x + menu_boxes[i]->rect.getSize().x;
		}
	}

	void cursorHover() {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}

		for (auto& mb : menu_boxes)
			mb->cursorHover();

	}

	void handleEvent(sf::Event& event) {

		bool clicked_in_menu = false;

		for (auto& mb : menu_boxes) {
			mb->handleEvent(event);
			if (ElementGUI_pressed == mb) {
				clicked_in_menu;
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			if (clicked_in_menu == false) {
				if (open_menu_box != nullptr)
					open_menu_box->isOpen = false;
				open_menu_box = nullptr;
			}
		}
	}

	void update() {
		for (auto& mb : menu_boxes)
			mb->update();
	}

	void draw() {
		window->draw(rect);
		window->draw(logo);

		if (open_menu_box != nullptr) {
			if (open_menu_box->options.size() > 0) {
				sf::RectangleShape rect(sf::Vector2f(open_menu_box->options.front()->rect.getSize().x, open_menu_box->options.size() * menu_height));
				rect.setPosition(open_menu_box->options.front()->rect.getPosition());
				rect.setOutlineThickness(menuoptions_border_width);
				rect.setOutlineColor(menuoptions_border_color);
				window->draw(rect);
			}
		}
		
		for (auto& mb : menu_boxes)
			mb->draw();
	}
};

MainMenu* main_menu = nullptr;

#endif