#ifndef Tools_hpp
#define Tools_hpp


class Tools : public ElementGUI {
public:
	sf::RectangleShape rect;

	std::vector < Button* > clipboard;
	sf::Text clipboard_text;
	ButtonWithBottomText* btn_paste;
	ButtonWithBottomText* btn_cut;
	ButtonWithBottomText* btn_copy;
	ButtonWithBottomText* btn_select;
	

	Tools() : ElementGUI() {
		rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, tools_height));
		rect.setFillColor(tools_bar_color);
		
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
		setPosition(sf::Vector2f(0, 0));
	}

	void setPosition(sf::Vector2f position) {
		rect.setPosition(position + sf::Vector2f(0, menu_height));

		int x = 0;

		for (int i = 0; i < clipboard.size(); i++) {
			clipboard[i]->setPosition(position + sf::Vector2f(x, menu_height));
			x += clipboard[i]->getSize().x;
		}

		clipboard_text.setPosition(x / 2 - clipboard_text.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);


			
	}

	void cursorHover() {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}

		for (auto& tool : clipboard)
			tool->cursorHover();

	}

	void handleEvent(sf::Event& event) {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				ElementGUI_pressed = this;
			}
		}

		for (auto& tool : clipboard)
			tool->handleEvent(event);
	}

	void update() {
		for (auto& tool : clipboard)
			tool->update();
	}

	void draw() {
		window->draw(rect);

		for(auto& tool : clipboard)
			tool->draw();

		window->draw(clipboard_text);
	}
};

Tools* tools = nullptr;


#endif