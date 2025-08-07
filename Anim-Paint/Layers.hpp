#ifndef Layers_hpp
#define Layers_hpp

class Layers : public Dialog {
public:

	std::vector < sf::Sprite > visibles;

	Layers(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) : Dialog(title, size, position) {

		for(int i=0;i<4;i++)
			visibles.push_back(sf::Sprite(*getTexture(L"tex\\layers\\visible.png")->texture));

		setPosition(position);
	}

	~Layers() { }

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);

		for (int i = 0; i < visibles.size(); i++) {
			visibles[i].setPosition(position + sf::Vector2f(8, 32 + 8 + i * 32 + i));
		}
		
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

		for (auto& v : visibles) {
			window->draw(v);
		}
	}

};

#endif