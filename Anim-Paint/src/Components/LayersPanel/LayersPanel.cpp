#include "Components/LayersPanel/LayersPanel.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Cursor.hpp"
#include "Window.hpp"
#include "Tools/Selection.hpp"
#include "Components/FramesPanel.hpp"
#include "Components/MainMenu/MainMenu.hpp"

LayersPanel::LayersPanel() 
	: Dialog(
		L"Layers",
		sf::Vector2i(160, dialog_title_rect_height + 4 * 32 + 32 + dialog_padding * 2),
		sf::Vector2i(int(mainView.getSize().x) - 160 - dialog_margin, frames_panel->getPosition().y + frames_panel->getSize().y + dialog_margin)
		) {

	loadLayersFromCurrentFrame();
}

LayersPanel::~LayersPanel() {

}

void LayersPanel::loadLayersFromCurrentFrame() {

	layersBoxes.clear();

	int current_frame = getCurrentAnimation()->getCurrentFrameID();
	int count_layers = getCurrentAnimation()->getLayersCount();


	for (int i = 0; i < count_layers; i++) {
		layersBoxes.push_back(std::make_shared<LayerBox>(getCurrentAnimation()->getLayer(i)));
		layersBoxes.back()->_onclick_func = [this, i]() {
			layersBoxes[getCurrentAnimation()->getCurrentLayerID()]->_isActive = false;
			getCurrentAnimation()->setCurrentLayerID(i);
			layersBoxes[i]->_isActive = true;
			};
	}


	int id = getCurrentAnimation()->getCurrentLayerID();
	if (id >= 0 && id < getCurrentAnimation()->getLayersCount()) {
		layersBoxes[id]->_isActive = true;
	}

	setPosition(this->getPosition());
}

void LayersPanel::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	for (int i = 0; i < layersBoxes.size(); i++) {
		sf::Vector2i pos;
		pos.x = _position.x + dialog_padding;
		pos.y = _position.y + 32 + dialog_padding + ((int)(layersBoxes.size()) - 1 - i) * 32;
		layersBoxes[i]->setPosition(pos);
	}

}

void LayersPanel::cursorHover() {

	if(!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	if (canvas->_state != CanvasState::Idle)
		return;

	if (!(selection->_state == SelectionState::None || selection->_state == SelectionState::Selected))
		return;

	Dialog::cursorHover();

	for (auto& layerbox : layersBoxes) {
		layerbox->cursorHover();
	}
}

void LayersPanel::handleEvent(const sf::Event& event) {

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	if (!dialogs.empty())
		return;

	Dialog::handleEvent(event);

	for (auto& layerbox : layersBoxes) {
		layerbox->handleEvent(event);
	}

}

void LayersPanel::update() {
	Dialog::update();

	for (auto& layerbox : layersBoxes) {
		layerbox->update();
	}
}

void LayersPanel::draw() {
	Dialog::draw();

	for (auto& layerbox : layersBoxes) {
		layerbox->draw();
	}
}

std::shared_ptr<LayersPanel> layers_panel;