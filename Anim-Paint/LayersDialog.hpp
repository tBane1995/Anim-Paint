#ifndef LayersDialog_hpp
#define LayersDialog_hpp

class LayerBox : public ElementGUI {
public:

	sf::RectangleShape rect;			
	Checkbox* visibling;				// is Visible or No on Canvas
	sf::Text textName;					// text for name

	Layer* layer;
	bool isActive;						// active to draw
	std::function<void()> onclick_func;	// onclick func

	LayerBox(std::wstring name, sf::Vector2i size) {

		rect = sf::RectangleShape(sf::Vector2f(160 - 2 * dialog_padding, 32));
		rect.setFillColor(sf::Color::Transparent);

		visibling = new Checkbox();
		visibling->addValue(getTexture(L"tex\\layers\\visible.png"), getTexture(L"tex\\layers\\visible_hover.png"));
		visibling->addValue(getTexture(L"tex\\layers\\unvisible.png"), getTexture(L"tex\\layers\\unvisible_hover.png"));
		visibling->setValue(0);

		textName = sf::Text(name, basicFont, 17);
		textName.setFillColor(normal_text_color);

		// image

		layer = new Layer(name, size);
	}

	~LayerBox() { }

	void setPosition(sf::Vector2f position) {
		visibling->setPosition(position);
		rect.setPosition(position);
		textName.setPosition(position + sf::Vector2f(32 + dialog_padding, (32.0f/2.0f - basicFont.getLineSpacing(17)/2.0f)));
	}

	void cursorHover() {
		visibling->cursorHover();

		if (isActive == true) {
			(rect.getGlobalBounds().contains(worldMousePosition))? rect.setFillColor(sf::Color(80, 64, 64)) : rect.setFillColor(sf::Color(64, 48, 48));
		}
		else {
			(rect.getGlobalBounds().contains(worldMousePosition))? rect.setFillColor(sf::Color(48, 48, 48)) : rect.setFillColor(sf::Color::Transparent);
		}
	}

	void handleEvent(sf::Event& event) {
		visibling->handleEvent(event);


		if (ElementGUI_pressed != visibling) {
			if (rect.getGlobalBounds().contains(worldMousePosition)) {
				if (event.type == event.MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
					onclick_func();
				}
			}
		}

	}

	void update() {
		visibling->update();
	}

	void draw() {
		window->draw(rect);
		visibling->draw();
		window->draw(textName);
	}
};

class LayersDialog : public Dialog {
public:

	std::vector < LayerBox* > layers;
	int currentLayer = 3;

	LayersDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) : Dialog(title, size, position) {

		

		for (int i = 0; i < 4; i++) {
			layers.push_back(new LayerBox(L"Layer " + std::to_wstring(i), sf::Vector2i(16, 16)));
			layers.back()->onclick_func = [this,i]() { 
				layers[currentLayer]->isActive = false;
				currentLayer = i; 
				layers[currentLayer]->isActive = true;
				};
		}

		layers[currentLayer]->isActive = true;

		setPosition(position);
	}

	~LayersDialog() { }

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);


		for (int i = 0; i < layers.size(); i++) {

			sf::Vector2f pos;
			pos = position + sf::Vector2f(dialog_padding, 32 + dialog_padding + (layers.size() - 1 - i) * 32);

			layers[i]->setPosition(pos);
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

LayersDialog* layers_dialog = nullptr;

#endif