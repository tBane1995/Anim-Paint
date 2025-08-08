#ifndef Layers_hpp
#define Layers_hpp

class Layer : public ElementGUI {
public:

	sf::RectangleShape rect;
	std::wstring name;
	Checkbox* visibling;
	
	Layer(std::wstring name) {

		this->name = name;

		rect = sf::RectangleShape(sf::Vector2f(160 - 2 * dialog_margin, 32));
		rect.setFillColor(sf::Color::Red);

		visibling = new Checkbox();
		visibling->addValue(getTexture(L"tex\\layers\\visible.png"), getTexture(L"tex\\layers\\visible_hover.png"));
		visibling->addValue(getTexture(L"tex\\layers\\unvisible.png"), getTexture(L"tex\\layers\\unvisible_hover.png"));
		visibling->setValue(0);
	}

	~Layer() { }

	void setPosition(sf::Vector2f position) {
		visibling->setPosition(position);
		rect.setPosition(position);
	}

	void cursorHover() {
		visibling->cursorHover();
	}

	void handleEvent(sf::Event& event) {
		visibling->handleEvent(event);
	}

	void update() {
		visibling->update();
	}

	void draw() {
		visibling->draw();
	}
};

class Layers : public Dialog {
public:

	std::vector < Layer* > layers;

	Layers(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) : Dialog(title, size, position) {

		for (int i = 0; i < 4; i++) {
			layers.push_back(new Layer(L"layer 0"));
		}

		setPosition(position);
	}

	~Layers() { }

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);


		for (int i = 0; i < layers.size(); i++) {
			layers[i]->setPosition(position + sf::Vector2f(dialog_padding, 32 + dialog_padding + i * 32));
		}
		
	}

	void cursorHover() {
		Dialog::cursorHover();

		for (auto& layer : layers) {
			layer->cursorHover();
		}
	}

	void handleEvent(sf::Event& event) {
		Dialog::handleEvent(event);

		for (auto& layer : layers) {
			layer->handleEvent(event);
		}
	}

	void update() {
		Dialog::update();
		
		for (auto& layer : layers) {
			layer->update();
		}
	}

	void draw() {
		Dialog::draw();

		for (auto& layer : layers) {
			layer->draw();
		}
	}

};

#endif