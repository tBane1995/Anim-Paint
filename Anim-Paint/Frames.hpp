#ifndef Frames_hpp
#define Frames_hpp

class Frames : public Dialog {
public:

	sf::Sprite first_btn;
	sf::Sprite prev_btn;
	sf::Sprite next_btn;
	sf::Sprite last_btn;

	Frames(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) : Dialog(title, size, position) {

		first_btn = sf::Sprite(*getTexture(L"tex\\frames\\first.png")->texture);
		prev_btn = sf::Sprite(*getTexture(L"tex\\frames\\prev.png")->texture);
		next_btn = sf::Sprite(*getTexture(L"tex\\frames\\next.png")->texture);
		last_btn = sf::Sprite(*getTexture(L"tex\\frames\\last.png")->texture);

		setPosition(position);
	}

	~Frames() { }

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);

		first_btn.setPosition(position + sf::Vector2f(dialog_padding, 32 + dialog_padding));
		prev_btn.setPosition(position + sf::Vector2f(dialog_padding +32, 32 + dialog_padding));
		next_btn.setPosition(position + sf::Vector2f(getSize().x - dialog_padding - 64, 32 + dialog_padding));
		last_btn.setPosition(position + sf::Vector2f(getSize().x - dialog_padding - 32, 32 + dialog_padding));
	}

	void cursorHover() {
		Dialog::cursorHover();

		if (first_btn.getGlobalBounds().contains(worldMousePosition))
			first_btn.setTexture(*getTexture(L"tex\\frames\\first_hover.png")->texture);
		else
			first_btn.setTexture(*getTexture(L"tex\\frames\\first.png")->texture);

		if (prev_btn.getGlobalBounds().contains(worldMousePosition))
			prev_btn.setTexture(*getTexture(L"tex\\frames\\prev_hover.png")->texture);
		else
			prev_btn.setTexture(*getTexture(L"tex\\frames\\prev.png")->texture);

		if (next_btn.getGlobalBounds().contains(worldMousePosition))
			next_btn.setTexture(*getTexture(L"tex\\frames\\next_hover.png")->texture);
		else
			next_btn.setTexture(*getTexture(L"tex\\frames\\next.png")->texture);

		if (last_btn.getGlobalBounds().contains(worldMousePosition))
			last_btn.setTexture(*getTexture(L"tex\\frames\\last_hover.png")->texture);
		else
			last_btn.setTexture(*getTexture(L"tex\\frames\\last.png")->texture);

	}

	void handleEvent(sf::Event& event) {
		Dialog::handleEvent(event);

	}

	void update() {
		Dialog::update();
	}

	void draw() {
		Dialog::draw();

		window->draw(first_btn);
		window->draw(prev_btn);
		window->draw(next_btn);
		window->draw(last_btn);
	}

};

#endif