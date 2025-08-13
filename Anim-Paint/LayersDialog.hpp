#ifndef LayersDialog_hpp
#define LayersDialog_hpp

class Canvas;

class LayerBox : public ElementGUI {
public:

	sf::RectangleShape rect;			
	Checkbox* visibling;				// is Visible or No on Canvas
	sf::Text textName;					// text for name

	Layer* layer;
	bool isActive;						// active to draw
	std::function<void()> onclick_func;	// onclick func

	LayerBox(Layer* layer) {

		this->layer = layer;

		rect = sf::RectangleShape(sf::Vector2f(160 - 2 * dialog_padding, 32));
		rect.setFillColor(sf::Color::Transparent);

		visibling = new Checkbox();
		visibling->addValue(getTexture(L"tex\\layers\\visible.png"), getTexture(L"tex\\layers\\visible_hover.png"));
		visibling->addValue(getTexture(L"tex\\layers\\unvisible.png"), getTexture(L"tex\\layers\\unvisible_hover.png"));
		visibling->setValue(0);

		textName = sf::Text(layer->name, basicFont, 17);
		textName.setFillColor(normal_text_color);

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

	Canvas* canvas;

	std::vector < LayerBox* > layersBoxes;
	int currentLayerId = 3;

	LayersDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) : Dialog(title, size, position) {

		loadLayersFromCurrentFrame();
		canvas = nullptr;
	}

	~LayersDialog() { }

	void setCanvas(Canvas* canvas) {
		this->canvas = canvas;
	}

	Layer* getCurrentLayer() {
		return layersBoxes[currentLayerId]->layer;
	}

	void loadLayersFromCurrentFrame() {

		for (auto& layerBox : layersBoxes) {
			delete layerBox;
		}

		layersBoxes.clear();

		int current_frame = frames_dialog->currentFrameId;
		int count_layers = frames_dialog->getCurrentFrame()->layers.size();
		for (int i = 0; i < count_layers; i++) {
			layersBoxes.push_back(new LayerBox(frames_dialog->getCurrentFrame()->layers[i]));
			layersBoxes.back()->onclick_func = [this, i]() {
				layersBoxes[currentLayerId]->isActive = false;
				currentLayerId = i;
				layersBoxes[currentLayerId]->isActive = true;
				};
		}

		setPosition(this->getPosition());

		layersBoxes[currentLayerId]->isActive = true;
	}

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);


		for (int i = 0; i < layersBoxes.size(); i++) {

			sf::Vector2f pos;
			pos = this->position + sf::Vector2f(dialog_padding, 32 + dialog_padding + (layersBoxes.size() - 1 - i) * 32);

			layersBoxes[i]->setPosition(pos);
		}
		
	}

	void cursorHover() {
		Dialog::cursorHover();

		for (auto& layerbox : layersBoxes) {
			layerbox->cursorHover();
		}
	}

	void handleEvent(sf::Event& event) {
		Dialog::handleEvent(event);

		for (auto& layerbox : layersBoxes) {
			layerbox->handleEvent(event);
		}

	}

	void update() {
		Dialog::update();
		
		// TO-DO - not optimize
		if (layersBoxes.size() > 0 && layersBoxes[0]->layer != frames_dialog->getCurrentFrame()->layers[0]) {
			loadLayersFromCurrentFrame();
		}

		for (auto& layerbox : layersBoxes) {
			layerbox->update();
		}
	}

	void draw() {
		Dialog::draw();

		for (auto& layerbox : layersBoxes) {
			layerbox->draw();
		}
	}

};

LayersDialog* layers_dialog = nullptr;

#endif