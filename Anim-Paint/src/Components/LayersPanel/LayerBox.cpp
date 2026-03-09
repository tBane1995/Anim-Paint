#include "Components/LayersPanel/LayersPanel.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Cursor.hpp"
#include "Window.hpp"
#include "Tools/Selection.hpp"
#include "Components/FramesPanel.hpp"

LayerBox::LayerBox(std::shared_ptr<Layer> layer) : Element() {

	_layer = layer;

	_isActive = false;

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(160 - 2 * dialog_padding, 32));

	_textName = std::make_unique<sf::Text>(basicFont, _layer->_name, 17);
	_textName->setFillColor(normal_text_color);

	_visibling = std::make_shared<Checkbox>(getTexture(L"tex\\btn32\\visible.png"), getTexture(L"tex\\btn32\\visible_hover.png"));
	_visibling->addValue(getTexture(L"tex\\btn32\\unvisible.png"), getTexture(L"tex\\btn32\\unvisible_hover.png"));
	_visibling->setValue(0);
	_visibling->_onclick_func = [this]() {
		};
}

LayerBox::~LayerBox() {


}

void LayerBox::setPosition(sf::Vector2i position) {
	_visibling->setPosition(position);
	_rect.position = position;
	sf::Vector2f textNamePosition;
	textNamePosition.x = (float)(position.x + 32 + dialog_padding);
	textNamePosition.y = (float)(position.y + (32.0f / 2.0f - basicFont.getLineSpacing(17) / 2.0f));
	_textName->setPosition(textNamePosition);
}

void LayerBox::cursorHover() {

	if (!dialogs.empty())
		return;

	_visibling->cursorHover();

	if (_rect.contains(cursor->_position)) {
		Element_hovered = this->shared_from_this();
	}
}

void LayerBox::handleEvent(const sf::Event& event) {

	_visibling->handleEvent(event);

	if (Element_pressed != _visibling) {
		if (_rect.contains(cursor->_position)) {
			if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
				Element_pressed = this->shared_from_this();
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
		if (Element_pressed.get() == this)
			rect.setFillColor(sf::Color(127, 63, 63));
		else if (Element_hovered.get() == this)
			rect.setFillColor(sf::Color(95, 63, 63));
		else
			rect.setFillColor(sf::Color(63, 47, 47));
	}
	else {
		if (Element_pressed.get() == this)
			rect.setFillColor(sf::Color(95, 95, 95));
		else if (Element_hovered.get() == this)
			rect.setFillColor(sf::Color(63, 63, 63));
		else
			rect.setFillColor(sf::Color(47, 47, 47));
	}
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);

	_visibling->draw();

	window->draw(*_textName);
}
