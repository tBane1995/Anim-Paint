#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "ElementGUI/Checkbox.hpp"
#include "Animation/Layer.hpp"
#include <functional>
#include "../Dialogs/Dialog.hpp"


class LayerBox : public ElementGUI {
public:

	sf::IntRect _rect;			
	std::shared_ptr<Checkbox> _visibling;				// is Visible or No on Canvas
	std::unique_ptr<sf::Text> _textName;					// text for name

	std::shared_ptr<Layer> _layer;
	bool _isActive;						// active to draw
	std::function<void()> _onclick_func;	// onclick func

	LayerBox(std::shared_ptr<Layer> layer);
	~LayerBox();

	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

class LayersPanel : public Dialog {
public:

	std::vector<std::shared_ptr<LayerBox>> layersBoxes;

	LayersPanel(std::wstring title, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(0, 0));
	~LayersPanel();

	void loadLayersFromCurrentFrame();
	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

extern std::shared_ptr<LayersPanel> layers_panel;