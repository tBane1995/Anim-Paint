#pragma once
#include "Element.hpp"
#include "Controls/Checkbox.hpp"
#include "Animation/Layer.hpp"
#include <functional>
#include "Dialogs/Dialog.hpp"
#include "Components/LayersPanel/LayerBox.hpp"

class LayersPanel : public Dialog {
public:

	std::vector<std::shared_ptr<LayerBox>> layersBoxes;
	std::shared_ptr<ButtonWithSprite> _add_layer;
	std::shared_ptr<ButtonWithSprite> _remove_layer;
	std::shared_ptr<ButtonWithSprite> _move_top;
	std::shared_ptr<ButtonWithSprite> _move_bottom;

	LayersPanel();
	~LayersPanel();

	void loadLayersFromCurrentFrame();
	void addLayer(std::shared_ptr<Layer> layerm);
	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

extern std::shared_ptr<LayersPanel> layers_panel;