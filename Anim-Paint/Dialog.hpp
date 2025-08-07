#ifndef Dialog_hpp
#define Dialog_hpp



class Dialog: public ElementGUI {
public:

	sf::RectangleShape dialog_rect;
	sf::RectangleShape title_rect;
	sf::Text title_text;
	sf::RectangleShape content_rect;

	bool is_moved;
	sf::Vector2f offset;
	
	Dialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0,0)) {

		dialog_rect = sf::RectangleShape(size);
		dialog_rect.setFillColor(dialog_rect_color);

		title_rect = sf::RectangleShape(sf::Vector2f(size.x-2* dialog_border_width, 32));
		title_rect.setFillColor(dialog_title_rect_color);

		title_text = sf::Text(title, basicFont, 17);
		title_text.setFillColor(dialog_title_text_color);

		content_rect = sf::RectangleShape(sf::Vector2f(size.x-2* dialog_border_width, size.y-2* dialog_border_width-title_rect.getSize().y));
		content_rect.setFillColor(dialog_content_rect_color);

		setPosition(position);

		is_moved = false;
		offset = sf::Vector2f(0, 0);
	}

	~Dialog() {}

	sf::Vector2f getPosition() {
		return dialog_rect.getPosition();
	}

	sf::Vector2f getSize() {
		return dialog_rect.getSize();
	}

	virtual void setPosition(sf::Vector2f position) {
		dialog_rect.setPosition(position);
		title_rect.setPosition(position + sf::Vector2f(dialog_border_width, dialog_border_width));
		sf::Vector2f pos(position + sf::Vector2f(dialog_border_width + (title_rect.getSize().y - title_text.getFont()->getLineSpacing(dialog_title_font_size)) / 2.0f, dialog_border_width + (title_rect.getSize().y - title_text.getFont()->getLineSpacing(dialog_title_font_size)) / 2.0f));
		title_text.setPosition(pos);
		content_rect.setPosition(position + sf::Vector2f(dialog_border_width, dialog_border_width + title_rect.getSize().y));
	}

	virtual void cursorHover() {
		if (dialog_rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}

	virtual void handleEvent(sf::Event& event) {
		if (event.type == sf::Event::MouseButtonPressed) {
			if (ElementGUI_hovered == this && title_rect.getGlobalBounds().contains(worldMousePosition)) {
				is_moved = true;
				offset = title_rect.getPosition() - worldMousePosition;
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased) {
			is_moved = false;
		}
		
	}

	virtual void update() {
		if (is_moved) {
			setPosition(worldMousePosition + offset);
		}
	}
	
	virtual void draw() {
		window->draw(dialog_rect);
		window->draw(title_rect);
		window->draw(title_text);
		window->draw(content_rect);
	}
};

std::vector < Dialog* > dialogs;

#endif