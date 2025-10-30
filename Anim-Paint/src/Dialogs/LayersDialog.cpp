#include "Dialogs/LayersDialog.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Mouse.hpp"
#include "Window.hpp"

LayerBox::LayerBox(std::shared_ptr<Layer> layer) : ElementGUI() {

	_layer = layer;

	_isActive = false;

	_rect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(160 - 2 * dialog_padding, 32));

	_visibling = std::make_shared<Checkbox>(getTexture(L"tex\\layers\\visible.png"), getTexture(L"tex\\layers\\visible_hover.png"));
	_visibling->addValue(getTexture(L"tex\\layers\\unvisible.png"), getTexture(L"tex\\layers\\unvisible_hover.png"));
	_visibling->setValue(0);
}

LayerBox::~LayerBox() {


}

void LayerBox::setPosition(sf::Vector2i position) {
	_visibling->setPosition(position);
	_rect.position = position;
}

void LayerBox::cursorHover() {

	if (!dialogs.empty())
		return;

	_visibling->cursorHover();

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void LayerBox::handleEvent(const sf::Event& event) {

	if (!dialogs.empty())
		return;

	_visibling->handleEvent(event);

	if (ElementGUI_pressed != _visibling.get()) {
		if (_rect.contains(cursor->_worldMousePosition)) {
			if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
				ElementGUI_pressed == this;
				_onclick_func();
			}
		}
	}

	



}

void LayerBox::update() {

	_visibling->update();
}

void LayerBox::draw() {
	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	if (_isActive) {
		if (ElementGUI_pressed == this)
			rect.setFillColor(sf::Color(127, 63, 63));
		else if (ElementGUI_hovered == this)
			rect.setFillColor(sf::Color(95, 63, 63));
		else
			rect.setFillColor(sf::Color(63, 47, 47));
	}
	else {
		if (ElementGUI_pressed == this)
			rect.setFillColor(sf::Color(95, 95, 95));
		else if (ElementGUI_hovered == this)
			rect.setFillColor(sf::Color(63, 63, 63));
		else
			rect.setFillColor(sf::Color(47, 47, 47));
	}
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);

	_visibling->draw();

	if (_textName == nullptr) {
		_textName = std::make_unique<sf::Text>(basicFont, _layer->_name, 17);
		_textName->setFillColor(normal_text_color);
	}
	_textName->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(32 + dialog_padding, (32.0f / 2.0f - basicFont.getLineSpacing(17) / 2.0f)));
	
	window->draw(*_textName);
}

/////////////////////////////////////////////////////////////

LayersDialog::LayersDialog(std::wstring title, sf::Vector2i size, sf::Vector2i position) : Dialog(title, size, position) {

	loadLayersFromCurrentFrame();
}

LayersDialog::~LayersDialog() {}

void LayersDialog::loadLayersFromCurrentFrame() {

	layersBoxes.clear();

	int current_frame = animation->getCurrentFrameID();
	int count_layers = animation->getLayersCount();


	for (int i = 0; i < count_layers; i++) {
		layersBoxes.push_back(std::make_shared<LayerBox>(animation->getLayer(i)));
		layersBoxes.back()->_onclick_func = [this, i]() {
			layersBoxes[animation->getCurrentLayerID()]->_isActive = false;
			animation->setCurrentLayerID(i);
			layersBoxes[i]->_isActive = true;
			};
	}


	int id = animation->getCurrentLayerID();
	if (id > 0 && id < animation->getLayersCount()) {
		layersBoxes[id]->_isActive = true;
	}

	setPosition(this->getPosition());
}

void LayersDialog::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);


	for (int i = 0; i < layersBoxes.size(); i++) {

		sf::Vector2i pos;
		pos = _position + sf::Vector2i(dialog_padding, 32 + dialog_padding + (layersBoxes.size() - 1 - i) * 32);

		layersBoxes[i]->setPosition(pos);
	}

}

void LayersDialog::cursorHover() {
	Dialog::cursorHover();

	for (auto& layerbox : layersBoxes) {
		layerbox->cursorHover();
	}
}

void LayersDialog::handleEvent(const sf::Event& event) {
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