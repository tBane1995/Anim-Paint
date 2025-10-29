#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "ElementGUI/Checkbox.hpp"
#include "Animation/Layer.hpp"
#include <functional>
#include "Dialog.hpp"


class LayerBox : public ElementGUI {
public:

	sf::RectangleShape _rect;			
	Checkbox* _visibling;				// is Visible or No on Canvas
	sf::Text* _textName;					// text for name

	std::shared_ptr<Layer> _layer;
	bool _isActive;						// active to draw
	std::function<void()> _onclick_func;	// onclick func

	LayerBox(std::shared_ptr<Layer> layer);
	~LayerBox();

	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void rect_coloring();
	void update();
	void draw();
};

class LayersDialog : public Dialog {
public:

	std::vector < LayerBox* > layersBoxes;

	LayersDialog(std::wstring title, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(0, 0));
	~LayersDialog();

	void loadLayersFromCurrentFrame();
	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

extern LayersDialog* layers_dialog;