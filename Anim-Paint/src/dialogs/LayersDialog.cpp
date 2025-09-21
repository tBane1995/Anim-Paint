#include "Dialogs/LayersDialog.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Mouse.hpp"
#include "Window.hpp"

LayerBox::LayerBox(Layer* layer) : ElementGUI() {

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

LayerBox::~LayerBox() {}

void LayerBox::setPosition(sf::Vector2f position) {
	visibling->setPosition(position);
	rect.setPosition(position);
	textName.setPosition(position + sf::Vector2f(32 + dialog_padding, (32.0f / 2.0f - basicFont.getLineSpacing(17) / 2.0f)));
}

void LayerBox::cursorHover() {
	visibling->cursorHover();

	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void LayerBox::handleEvent(sf::Event& event) {
	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		if (event.type == event.MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			ElementGUI_pressed == this;
		}
	}

	visibling->handleEvent(event);



}

void LayerBox::rect_coloring() {
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

void LayerBox::update() {

	visibling->update();
	rect_coloring();
}

void LayerBox::draw() {
	window->draw(rect);
	visibling->draw();
	window->draw(textName);
}

/////////////////////////////////////////////////////////////

LayersDialog::LayersDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position) : Dialog(title, size, position) {

	loadLayersFromCurrentFrame();
}

LayersDialog::~LayersDialog() {}

void LayersDialog::loadLayersFromCurrentFrame() {

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

void LayersDialog::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);


	for (int i = 0; i < layersBoxes.size(); i++) {

		sf::Vector2f pos;
		pos = this->position + sf::Vector2f(dialog_padding, 32 + dialog_padding + (layersBoxes.size() - 1 - i) * 32);

		layersBoxes[i]->setPosition(pos);
	}

}

void LayersDialog::cursorHover() {
	Dialog::cursorHover();

	for (auto& layerbox : layersBoxes) {
		layerbox->cursorHover();
	}
}

void LayersDialog::handleEvent(sf::Event& event) {
	Dialog::handleEvent(event);

	for (auto& layerbox : layersBoxes) {
		layerbox->handleEvent(event);
	}

}

void LayersDialog::update() {
	Dialog::update();

	for (auto& layerbox : layersBoxes) {
		layerbox->update();
	}
}

void LayersDialog::draw() {
	Dialog::draw();

	for (auto& layerbox : layersBoxes) {
		layerbox->draw();
	}
}

LayersDialog* layers_dialog = nullptr;