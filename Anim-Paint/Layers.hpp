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
			visibles[i].setPosition(position + sf::Vector2f(dialog_padding, 32 + dialog_padding + i * 32));
		}
		
	}

	void cursorHover() {
		Dialog::cursorHover();

		for (auto& v : visibles) {
			if (v.getGlobalBounds().contains(worldMousePosition))
				v.setTexture(*getTexture(L"tex\\layers\\visible_hover.png")->texture);
			else
				v.setTexture(*getTexture(L"tex\\layers\\visible.png")->texture);
		}
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