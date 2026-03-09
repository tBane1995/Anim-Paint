#pragma once
#include "Controls/Element.hpp"
#include "Controls/Checkbox.hpp"
#include "Animation/Layer.hpp"
#include <functional>
#include "Dialogs/Dialog.hpp"


class LayerBox : public Element {
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
