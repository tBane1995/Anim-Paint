#ifndef Frames_hpp
#define Frames_hpp

class Frames : public Dialog {
public:

	Button* first_btn;
	Button* prev_btn;
	Button* next_btn;
	Button* last_btn;

	int current_frame;

	Frames(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) : Dialog(title, size, position) {

		first_btn = new Button(getTexture(L"tex\\frames\\first.png"), getTexture(L"tex\\frames\\first_hover.png"));
		prev_btn = new Button(getTexture(L"tex\\frames\\prev.png"), getTexture(L"tex\\frames\\prev_hover.png"));
		next_btn = new Button(getTexture(L"tex\\frames\\next.png"), getTexture(L"tex\\frames\\next_hover.png"));
		last_btn = new Button(getTexture(L"tex\\frames\\last.png"), getTexture(L"tex\\frames\\last_hover.png"));

		setPosition(position);

		current_frame = 0;

	}

	~Frames() { }

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);

		first_btn->setPosition(position + sf::Vector2f(dialog_padding, 32 + dialog_padding));
		prev_btn->setPosition(position + sf::Vector2f(dialog_padding +32, 32 + dialog_padding));
		next_btn->setPosition(position + sf::Vector2f(getSize().x - dialog_padding - 64, 32 + dialog_padding));
		last_btn->setPosition(position + sf::Vector2f(getSize().x - dialog_padding - 32, 32 + dialog_padding));
	}

	void cursorHover() {
		Dialog::cursorHover();

		first_btn->cursorHover();
		prev_btn->cursorHover();
		next_btn->cursorHover();
		last_btn->cursorHover();


	}

	void handleEvent(sf::Event& event) {
		Dialog::handleEvent(event);

		first_btn->handleEvent(event);
		prev_btn->handleEvent(event);
		next_btn->handleEvent(event);
		last_btn->handleEvent(event);

	}

	void update() {
		Dialog::update();

		first_btn->update();
		prev_btn->update();
		next_btn->update();
		last_btn->update();
	}

	void draw() {
		Dialog::draw();

		first_btn->draw();
		prev_btn->draw();
		next_btn->draw();
		last_btn->draw();
	}

};

#endif