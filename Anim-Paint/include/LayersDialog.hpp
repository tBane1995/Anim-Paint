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

		isActive = false;

		rect = sf::RectangleShape(sf::Vector2f(160 - 2 * dialog_padding, 32));
		rect.setFillColor(sf::Color::Transparent);

		visibling = new Checkbox();
		visibling->addValue(getTexture(L"tex\\layers\\visible.png"), getTexture(L"tex\\layers\\visible_hover.png"));
		visibling->addValue(getTexture(L"tex\\layers\\unvisible.png"), getTexture(L"tex\\layers\\unvisible_hover.png"));
		visibling->setValue(0);

		textName = sf::Text(layer->name, basicFont, 17);
		textName.setFillColor(normal_text_color);

		rect_coloring();
	}

	~LayerBox() { }

	void setPosition(sf::Vector2f position) {
		visibling->setPosition(position);
		rect.setPosition(position);
		textName.setPosition(position + sf::Vector2f(32 + dialog_padding, (32.0f/2.0f - basicFont.getLineSpacing(17)/2.0f)));
	}

	void cursorHover() {
		visibling->cursorHover();

		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}

	void handleEvent(sf::Event& event) {
		if (rect.getGlobalBounds().contains(worldMousePosition)) {
			if (event.type == event.MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				ElementGUI_pressed == this;
			}
		}

		visibling->handleEvent(event);

		

	}

	void rect_coloring() {
		if (isActive) {

			if (ElementGUI_pressed == this) {
				rect.setFillColor(sf::Color(127, 63, 63));
			}
			else if (ElementGUI_hovered == this) {
				rect.setFillColor(sf::Color(95, 63, 63));
			}
			else {
				rect.setFillColor(sf::Color(63, 47, 47));
			}

		}
		else {
			if (ElementGUI_pressed == this) {
				rect.setFillColor(sf::Color(95, 95, 95));
			}
			else if (ElementGUI_hovered == this) {
				rect.setFillColor(sf::Color(63, 63, 63));
			}
			else {
				rect.setFillColor(sf::Color(47, 47, 47));
			}
		}
	}

	void update() {

		visibling->update();
		rect_coloring();
	}

	void draw() {
		window->draw(rect);
		visibling->draw();
		window->draw(textName);
	}
};

class LayersDialog : public Dialog {
public:

	std::vector < LayerBox* > layersBoxes;

	LayersDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0)) : Dialog(title, size, position) {

		loadLayersFromCurrentFrame();
	}

	~LayersDialog() { }

	void loadLayersFromCurrentFrame() {

		for (auto& layerBox : layersBoxes) {
			delete layerBox;
		}

		layersBoxes.clear();

		int current_frame = animation->getCurrentFrameID();
		int count_layers = animation->getLayersSize();
		for (int i = 0; i < count_layers; i++) {
			layersBoxes.push_back(new LayerBox(animation->getLayer(i)));
			layersBoxes.back()->onclick_func = [this, i]() {
				layersBoxes[animation->getCurrentLayerID()]->isActive = false;
				animation->setCurrentLayerID(i);
				layersBoxes[i]->isActive = true;
				};
		}

		layersBoxes[animation->getCurrentLayerID()]->isActive = true;

		for (auto& l : layersBoxes)
			l->rect_coloring();

		

		setPosition(this->getPosition());
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