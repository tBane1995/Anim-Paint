#include "Components/LayersPanel/LayersPanel.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Cursor.hpp"
#include "Window.hpp"
#include "Tools/Selection.hpp"
#include "Components/FramesPanel.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "History.hpp"

LayersPanel::LayersPanel() 
	: Dialog(
		L"Layers",
		sf::Vector2i(192, dialog_title_rect_height + 4 * 32 + 32 + dialog_padding * 3),
		sf::Vector2i(int(mainView.getSize().x) - 160 - dialog_margin, frames_panel->getPosition().y + frames_panel->getSize().y)
		) {

	_add_layer = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\add.png"), getTexture(L"tex\\btn32\\add_hover.png"), getTexture(L"tex\\btn32\\add_press.png"));
	_remove_layer = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\remove.png"), getTexture(L"tex\\btn32\\remove_hover.png"), getTexture(L"tex\\btn32\\remove_press.png"));
	_move_top = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\top.png"), getTexture(L"tex\\btn32\\top_hover.png"), getTexture(L"tex\\btn32\\top_press.png"));
	_move_bottom = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\bottom.png"), getTexture(L"tex\\btn32\\bottom_hover.png"), getTexture(L"tex\\btn32\\bottom_press.png"));
	

	_add_layer->_onclick_func = []() {
		if (getCurrentAnimation()->getLayersCount() >= 4) return;

		std::shared_ptr<Layer> new_layer = std::make_shared<Layer>(
			L"Layer " + std::to_wstring(getCurrentAnimation()->getLayersCount() + 1),
			canvas->_size,
			(getCurrentAnimation()->getLayersCount() == 0) ? sf::Color::White : sf::Color::Transparent
		);

		layers_panel->addLayer(new_layer);
		history->saveStep();
		};
	_add_layer->setTooltip(L"Add Layer", L"Add a new layer above the current layer.");

	_remove_layer->_onclick_func = []() {
		layers_panel->removeLayer(getCurrentAnimation()->getCurrentLayerID());
		history->saveStep();
		};
	_remove_layer->setTooltip(L"Remove Layer", L"Removes the current layer.");

	_move_top->_onclick_func = []() {
		int index = getCurrentAnimation()->getCurrentLayerID();
		if (index >= 0 && index < getCurrentAnimation()->getLayersCount() - 1) {
			std::swap(getCurrentAnimation()->getCurrentFrame()->_layers[index], getCurrentAnimation()->getCurrentFrame()->_layers[index + 1]);
			getCurrentAnimation()->setCurrentLayerID(index + 1);
			layers_panel->loadLayersFromCurrentFrame();
			history->saveStep();
		}
		};
	_move_top->setTooltip(L"Move Layer Up", L"Moves the current layer up in the layer stack.");

	_move_bottom->_onclick_func = []() {
		int index = getCurrentAnimation()->getCurrentLayerID();
		if (index > 0 && index < getCurrentAnimation()->getLayersCount()) {
			std::swap(getCurrentAnimation()->getCurrentFrame()->_layers[index], getCurrentAnimation()->getCurrentFrame()->_layers[index - 1]);
			getCurrentAnimation()->setCurrentLayerID(index - 1);
			layers_panel->loadLayersFromCurrentFrame();
			history->saveStep();
		}
		};
	_move_bottom->setTooltip(L"Move Layer Down", L"Moves the current layer down in the layer stack.");

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

void LayersPanel::addLayer(std::shared_ptr<Layer> layer) {
	auto frame = getCurrentAnimation()->getCurrentFrame();
	if (!frame) return;

	auto& layers = frame->_layers;

	int index = getCurrentAnimation()->getCurrentLayerID();
	if (index < 0) index = -1;
	if (index >= (int)layers.size()) index = layers.size() - 1;

	int insertPos = index + 1;
	if (insertPos > (int)layers.size())
		insertPos = layers.size();

	layers.insert(layers.begin() + insertPos, layer);

	getCurrentAnimation()->setCurrentLayerID(insertPos);

	auto layerBox = std::make_shared<LayerBox>(layer);
	layerBox->_isActive = true;

	if (index >= 0 && index < (int)layersBoxes.size())
		layersBoxes[index]->_isActive = false;

	layersBoxes.insert(layersBoxes.begin() + insertPos, layerBox);

	layerBox->_onclick_func = [layerBox]() {
		auto& boxes = layers_panel->layersBoxes;
		int current = getCurrentAnimation()->getCurrentLayerID();
		if (current >= 0 && current < (int)boxes.size())
			boxes[current]->_isActive = false;

		int newID = -1;
		for (int i = 0; i < (int)boxes.size(); i++) {
			if (boxes[i] == layerBox) {
				newID = i;
				break;
			}
		}

		if (newID >= 0) {
			getCurrentAnimation()->setCurrentLayerID(newID);
			boxes[newID]->_isActive = true;
		}
		};

	setPosition(getPosition());
}

void LayersPanel::removeLayer(int index) {
	std::shared_ptr<Frame> frame = getCurrentAnimation()->getCurrentFrame();
	if (!frame) return;

	auto& layers = frame->_layers;
	if (index < 0 || index >= (int)layers.size()) return;

	layers.erase(layers.begin() + index);

	if (index >= 0 && index < (int)layersBoxes.size())
		layersBoxes.erase(layersBoxes.begin() + index);

	int newID = std::max(0, index - 1);
	getCurrentAnimation()->setCurrentLayerID(newID);

	if (!layersBoxes.empty() && newID >= 0 && newID < (int)layersBoxes.size())
		layersBoxes[newID]->_isActive = true;

	setPosition(getPosition());
}

void LayersPanel::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	for (int i = 0; i < layersBoxes.size(); i++) {
		sf::Vector2i pos;
		pos.x = _position.x + dialog_padding;
		pos.y = _position.y + 32 + dialog_padding + ((int)(layersBoxes.size()) - 1 - i) * 32;
		layersBoxes[i]->setPosition(pos);
	}

	_add_layer->setPosition(getContentPosition() + sf::Vector2i(dialog_padding, getContentSize().y - 32 - dialog_padding));
	_remove_layer->setPosition(getContentPosition() + sf::Vector2i(dialog_padding + 32, getContentSize().y -32 - dialog_padding));
	_move_top->setPosition(getContentPosition() + sf::Vector2i(getSize().x - dialog_padding - 64, getContentSize().y -32 - dialog_padding));
	_move_bottom->setPosition(getContentPosition() + sf::Vector2i(getSize().x - dialog_padding - 32, getContentSize().y -32 - dialog_padding));
}

void LayersPanel::cursorHover() {

	if(!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	if(!static_dialogs.empty() && static_dialogs.front()->_is_moved)
		return;

	if (canvas->_state != CanvasState::Idle)
		return;

	if (!(selection->_state == SelectionState::None || selection->_state == SelectionState::Selected))
		return;

	Dialog::cursorHover();

	for (auto& layerbox : layersBoxes) {
		layerbox->cursorHover();
	}

	_add_layer->cursorHover();
	_remove_layer->cursorHover();
	_move_top->cursorHover();
	_move_bottom->cursorHover();
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

	_add_layer->handleEvent(event);
	_remove_layer->handleEvent(event);
	_move_top->handleEvent(event);
	_move_bottom->handleEvent(event);

}

void LayersPanel::update() {
	Dialog::update();

	for (auto& layerbox : layersBoxes) {
		layerbox->update();
	}

	_add_layer->update();
	_remove_layer->update();
	_move_top->update();
	_move_bottom->update();
}

void LayersPanel::draw() {
	Dialog::draw();

	for (auto& layerbox : layersBoxes) {
		layerbox->draw();
	}

	_add_layer->draw();
	_remove_layer->draw();
	_move_top->draw();
	_move_bottom->draw();
}

std::shared_ptr<LayersPanel> layers_panel;