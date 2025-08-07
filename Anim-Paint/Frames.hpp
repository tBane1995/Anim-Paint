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

		first_btn.setPosition(position + sf::Vector2f(16, 48));
		prev_btn.setPosition(position + sf::Vector2f(16+32, 48));
		next_btn.setPosition(position + sf::Vector2f(16+96, 48));
		last_btn.setPosition(position + sf::Vector2f(16+128, 48));
	}

	void cursorHover() {
		Dialog::cursorHover();
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