#include "Dialogs/LayersDialog.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Mouse.hpp"
#include "Window.hpp"

LayerBox::LayerBox(Layer* layer) : ElementGUI() {

	_layer = layer;

	_isActive = false;

	_rect = sf::RectangleShape(sf::Vector2f(160 - 2 * dialog_padding, 32));
	_rect.setFillColor(sf::Color::Transparent);

	_visibling = new Checkbox();
	_visibling->addValue(getTexture(L"tex\\layers\\visible.png"), getTexture(L"tex\\layers\\visible_hover.png"));
	_visibling->addValue(getTexture(L"tex\\layers\\unvisible.png"), getTexture(L"tex\\layers\\unvisible_hover.png"));
	_visibling->setValue(0);

	_textName = sf::Text(layer->_name, basicFont, 17);
	_textName.setFillColor(normal_text_color);

	rect_coloring();
}

LayerBox::~LayerBox() {}

void LayerBox::setPosition(sf::Vector2f position) {
	_visibling->setPosition(position);
	_rect.setPosition(position);
	_textName.setPosition(position + sf::Vector2f(32 + dialog_padding, (32.0f / 2.0f - basicFont.getLineSpacing(17) / 2.0f)));
}

void LayerBox::cursorHover() {

	if (!dialogs.empty())
		return;

	_visibling->cursorHover();

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void LayerBox::handleEvent(sf::Event& event) {

	if (!dialogs.empty())
		return;

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		if (event.type == event.MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			ElementGUI_pressed == this;
		}
	}

	_visibling->handleEvent(event);



}

void LayerBox::rect_coloring() {
	if (_isActive) {

		if (ElementGUI_pressed == this) {
			_rect.setFillColor(sf::Color(127, 63, 63));
		}
		else if (ElementGUI_hovered == this) {
			_rect.setFillColor(sf::Color(95, 63, 63));
		}
		else {
			_rect.setFillColor(sf::Color(63, 47, 47));
		}

	}
	else {
		if (ElementGUI_pressed == this) {
			_rect.setFillColor(sf::Color(95, 95, 95));
		}
		else if (ElementGUI_hovered == this) {
			_rect.setFillColor(sf::Color(63, 63, 63));
		}
		else {
			_rect.setFillColor(sf::Color(47, 47, 47));
		}
	}
}

void LayerBox::update() {

	_visibling->update();
	rect_coloring();
}

void LayerBox::draw() {
	window->draw(_rect);
	_visibling->draw();
	window->draw(_textName);
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
	int count_layers = animation->getLayersCount();
	for (int i = 0; i < count_layers; i++) {
		layersBoxes.push_back(new LayerBox(animation->getLayer(i)));
		layersBoxes.back()->_onclick_func = [this, i]() {
			layersBoxes[animation->getCurrentLayerID()]->_isActive = false;
			animation->setCurrentLayerID(i);
			layersBoxes[i]->_isActive = true;
			};
	}

	layersBoxes[animation->getCurrentLayerID()]->_isActive = true;

	for (auto& l : layersBoxes)
		l->rect_coloring();



	setPosition(this->getPosition());
}

void LayersDialog::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);


	for (int i = 0; i < layersBoxes.size(); i++) {

		sf::Vector2f pos;
		pos = _position + sf::Vector2f(dialog_padding, 32 + dialog_padding + (layersBoxes.size() - 1 - i) * 32);

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