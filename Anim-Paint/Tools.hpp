#ifndef Tools_hpp
#define Tools_hpp

class Separator {
public:
	sf::RectangleShape rect;
	Separator() { 
		rect = sf::RectangleShape(sf::Vector2f(2, tools_separator_height));
		rect.setFillColor(tools_separator_color);
	}

	~Separator() { }

	void setPosition(sf::Vector2f position) {
		rect.setPosition(position + sf::Vector2f(0, (tools_height - tools_separator_height)/2));
	}

	sf::Vector2f getSize() {
		return rect.getSize();
	}

	void draw() {
		window->draw(rect);
	}
};

class ColorButton : public NormalButton {
public:

	std::wstring name;
	sf::Color color;
	sf::RectangleShape rect;

	ColorButton(std::wstring color_name, sf::Color color) : NormalButton(getTexture(L"tex\\colors\\frame.png"), getTexture(L"tex\\colors\\frame_hover.png")) {
		this->name = color_name;
		this->color = color;

		rect = sf::RectangleShape(sf::Vector2f(24, 24));
		rect.setFillColor(color);
	}

	void setPosition(sf::Vector2f position) {
		NormalButton::setPosition(position);
		rect.setPosition(position + sf::Vector2f(4, 4));

	}

	void draw() {
		NormalButton::draw();
		window->draw(rect);

	}
};


class Tools : public ElementGUI {
public:
	sf::RectangleShape rect;

	std::vector < Separator* > separators;

	std::vector < Button* > clipboard;
	sf::Text clipboard_text;
	ButtonWithBottomText* btn_paste;
	ButtonWithBottomText* btn_cut;
	ButtonWithBottomText* btn_copy;
	ButtonWithBottomText* btn_select;
	
	sf::Text colors_text;
	std::vector < ColorButton* > colors;


	Tools() : ElementGUI() {
		rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, tools_height));
		rect.setFillColor(tools_bar_color);
		
		// clipboard
		clipboard_text = sf::Text(L"clipboard", basicFont, 13);
		clipboard_text.setFillColor(tools_text_color);
		btn_paste = new ButtonWithBottomText(L"paste", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_paste.png"), getTexture(L"tex\\tools\\btn_paste_hover.png"));
		btn_cut = new ButtonWithBottomText(L"cut", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_cut.png"), getTexture(L"tex\\tools\\btn_cut_hover.png"));
		btn_copy = new ButtonWithBottomText(L"copy", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_copy.png"), getTexture(L"tex\\tools\\btn_copy_hover.png"));
		btn_select = new ButtonWithBottomText(L"select", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_select.png"), getTexture(L"tex\\tools\\btn_select_hover.png"));

		clipboard.clear();
		clipboard.push_back(btn_paste);
		clipboard.push_back(btn_cut);
		clipboard.push_back(btn_copy);
		clipboard.push_back(btn_select);
		separators.push_back(new Separator());

		// colors
		colors_text = sf::Text(L"colors", basicFont, 13);
		colors_text.setFillColor(tools_text_color);
		 
		// black - grey - white
		colors.push_back(new ColorButton(L"black", sf::Color(0, 0, 0)));
		colors.push_back(new ColorButton(L"white", sf::Color(255, 255, 255)));

		// greys
		colors.push_back(new ColorButton(L"dark grey", sf::Color(63, 63, 63)));
		colors.push_back(new ColorButton(L"light grey", sf::Color(191, 191, 191)));

		// red
		colors.push_back(new ColorButton(L"dark red", sf::Color(63, 0, 0)));
		colors.push_back(new ColorButton(L"light red", sf::Color(191, 0, 0)));

		// green
		colors.push_back(new ColorButton(L"dark green", sf::Color(0, 63, 0)));
		colors.push_back(new ColorButton(L"light green", sf::Color(0, 191, 0)));

		// blue
		colors.push_back(new ColorButton(L"dark blue", sf::Color(0, 0, 63)));
		colors.push_back(new ColorButton(L"light blue", sf::Color(0, 0, 191)));

		// yellow
		colors.push_back(new ColorButton(L"dark yellow", sf::Color(63, 63, 0)));
		colors.push_back(new ColorButton(L"light yellow", sf::Color(191, 191, 0)));

		// magenta
		colors.push_back(new ColorButton(L"dark magenta", sf::Color(63, 0, 63)));
		colors.push_back(new ColorButton(L"light magenta", sf::Color(191, 0, 191)));

		// cyan
		colors.push_back(new ColorButton(L"dark cyan", sf::Color(0, 63, 63)));
		colors.push_back(new ColorButton(L"light cyan", sf::Color(0, 191, 191)));

		setPosition(sf::Vector2f(0, 0));
	}

	void setPosition(sf::Vector2f position) {
		rect.setPosition(position + sf::Vector2f(0, menu_height));

		// clipboard
		int x = 0;

		for (int i = 0; i < clipboard.size(); i++) {
			clipboard[i]->setPosition(position + sf::Vector2f(x, menu_height));
			x += clipboard[i]->getSize().x;
		}

		clipboard_text.setPosition(x / 2 - clipboard_text.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);

		separators[0]->setPosition(sf::Vector2f(x, menu_height));
		x += separators[0]->getSize().x;

		// colors
		
		int old_x = x;
		int y = menu_height + 4;
		for (int i = 0; i < colors.size(); i+=2) {
			colors[i]->setPosition(sf::Vector2f(x + 6, y));
			colors[i+1]->setPosition(sf::Vector2f(x + 6, y+32));
			x += 32;
		}

		colors_text.setPosition((x+old_x) / 2 - colors_text.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);

	}

	void cursorHover() {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}

		for (auto& tool : clipboard)
			tool->cursorHover();

		for (auto& color : colors)
			color->cursorHover();

	}

	void handleEvent(sf::Event& event) {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				ElementGUI_pressed = this;
			}
		}

		for (auto& tool : clipboard)
			tool->handleEvent(event);

		for (auto& color : colors)
			color->handleEvent(event);
	}

	void update() {
		for (auto& tool : clipboard)
			tool->update();

		for (auto& color : colors)
			color->update();
	}

	void draw() {
		window->draw(rect);

		for (auto& separator : separators) {
			separator->draw();
		}

		for(auto& tool : clipboard)
			tool->draw();

		for (auto& col : colors)
			col->draw();

		window->draw(clipboard_text);
		window->draw(colors_text);
	}
};

Tools* tools = nullptr;


#endif