#ifndef Dialog_hpp
#define Dialog_hpp

enum class DialogState { Idle, ToClose };

class Dialog : public ElementGUI {
public:
	sf::Vector2f position;
	sf::RectangleShape dialog_rect;
	sf::RectangleShape title_rect;
	sf::Text title_text;
	sf::RectangleShape content_rect;
	NormalButton* close_btn;

	DialogState state;

	bool is_moved;
	sf::Vector2f offset;

	Dialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(-1, -1)) {

		state = DialogState::Idle;

		dialog_rect = sf::RectangleShape(size);
		dialog_rect.setFillColor(dialog_border_color);

		title_rect = sf::RectangleShape(sf::Vector2f(size.x - 2 * dialog_border_width, dialog_title_rect_height));
		title_rect.setFillColor(dialog_title_rect_color);

		title_text = sf::Text(title, basicFont, dialog_title_font_size);
		title_text.setFillColor(dialog_title_text_color);

		content_rect = sf::RectangleShape(sf::Vector2f(size.x - 2 * dialog_border_width, size.y - 2 * dialog_border_width - title_rect.getSize().y));
		content_rect.setFillColor(dialog_content_rect_color);

		close_btn = new NormalButton(getTexture(L"tex\\dialog\\close.png"), getTexture(L"tex\\dialog\\close_hover.png"));
		close_btn->onclick_func = [this]() {
			state = DialogState::ToClose;
			};

		if (position == sf::Vector2f(-1,-1)){
			sf::Vector2f pos; 
			pos.x = window->getSize().x / 2 - size.x/2;
			pos.y = window->getSize().y / 2 - size.y/2;
			setPosition(pos);
		}else
			setPosition(position);

		is_moved = false;
		offset = sf::Vector2f(0, 0);
	}

	virtual ~Dialog() {}

	sf::Vector2f getPosition() {
		return dialog_rect.getPosition();
	}

	sf::Vector2f setSize(sf::Vector2f size) {
	
	}

	sf::Vector2f getSize() {
		return dialog_rect.getSize();
	}

	virtual void setPosition(sf::Vector2f position) {

		this->position = sf::Vector2f(sf::Vector2i(position) / 8 * 8);

		dialog_rect.setPosition(this->position);
		title_rect.setPosition(this->position + sf::Vector2f(dialog_border_width, dialog_border_width));
		close_btn->setPosition(this->position + sf::Vector2f(this->getSize().x - dialog_border_width - 32, dialog_border_width));
		sf::Vector2f pos(this->position + sf::Vector2f(dialog_border_width + (title_rect.getSize().y - title_text.getFont()->getLineSpacing(dialog_title_font_size)) / 2.0f, dialog_border_width + (title_rect.getSize().y - title_text.getFont()->getLineSpacing(dialog_title_font_size)) / 2.0f));
		title_text.setPosition(pos);
		content_rect.setPosition(this->position + sf::Vector2f(dialog_border_width, dialog_border_width + title_rect.getSize().y));
	}

	virtual void cursorHover() {
		if (dialog_rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}

		close_btn->cursorHover();
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
		
		close_btn->handleEvent(event);
	}

	virtual void update() {
		if (is_moved) {
			setPosition(worldMousePosition + offset);
		}

		close_btn->update();
	}
	
	virtual void draw() {
		window->draw(dialog_rect);
		window->draw(title_rect);
		close_btn->draw();
		window->draw(title_text);
		window->draw(content_rect);
	}
};

std::vector < Dialog* > dialogs;

#endif