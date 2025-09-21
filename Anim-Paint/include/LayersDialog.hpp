#pragma once
#include "ElementGUI.hpp"
#include "Checkbox.hpp"
#include "Layer.hpp"
#include <functional>
#include "Dialog.hpp"


class LayerBox : public ElementGUI {
public:

	sf::RectangleShape rect;			
	Checkbox* visibling;				// is Visible or No on Canvas
	sf::Text textName;					// text for name

	Layer* layer;
	bool isActive;						// active to draw
	std::function<void()> onclick_func;	// onclick func

	LayerBox(Layer* layer);
	~LayerBox();

	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void rect_coloring();
	void update();
	void draw();
};

class LayersDialog : public Dialog {
public:

	std::vector < LayerBox* > layersBoxes;

	LayersDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0));
	~LayersDialog();

	void loadLayersFromCurrentFrame();
	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};

extern LayersDialog* layers_dialog;