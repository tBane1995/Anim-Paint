#include "Components/PreviewAnimationPanel.hpp"
#include "Theme.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Time.hpp"
#include "DebugLog.hpp"
#include "Window.hpp"
#include "Components/Canvas.hpp"
#include "Tools/Selection.hpp"

PreviewAnimationPanel::PreviewAnimationPanel() :
	Dialog(
		L"Preview Animation",
		sf::Vector2i(192, 192 + dialog_title_rect_height),
		sf::Vector2i(dialog_padding, int(main_menu->getSize().y) + toolbar->_rect.size.y + dialog_margin)
	) {

	_currentFrame = 0;
	_timer = currentTime;

}

PreviewAnimationPanel::~PreviewAnimationPanel() {

}

void PreviewAnimationPanel::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);
}

void PreviewAnimationPanel::cursorHover() {

	if (!dialogs.empty())
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

}

void PreviewAnimationPanel::handleEvent(const sf::Event& event) {
	if (!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	Dialog::handleEvent(event);
}

void PreviewAnimationPanel::update() {
	Dialog::update();

	if((currentTime - _timer).asSeconds() >= 0.25f) {
		_currentFrame = (_currentFrame + 1);
		_timer = currentTime;
	}

	_currentFrame = _currentFrame % getCurrentAnimation()->getFramesCount();
}

void PreviewAnimationPanel::draw() {
	Dialog::draw();

	std::shared_ptr<Animation> animation = getCurrentAnimation();
	std::shared_ptr<Frame> frame = animation->getFrame(_currentFrame);

	sf::Vector2f size = sf::Vector2f(getContentSize());
	sf::RectangleShape rect(size);
	rect.setPosition(sf::Vector2f(getContentPosition()));
	rect.setFillColor(sf::Color(63, 63, 63));
	window->draw(rect);

	for (int i = 0; i < frame->getLayersCount(); i++) {

		sf::Texture tex;
		if (!tex.loadFromImage(frame->getLayer(i)->_image)) {
			DebugError(L"PreviewAnimationPanel::draw: Failed to load texture from image.");
			exit(0);
		}

		sf::Sprite spr(tex);
		spr.setPosition(sf::Vector2f(getContentPosition()));
		sf::Vector2f scale;
		scale.x = size.x / float(frame->getLayer(i)->_image.getSize().x);
		scale.y = size.y / float(frame->getLayer(i)->_image.getSize().y);
		spr.setScale(scale);
		window->draw(spr);
	}

}

std::shared_ptr<PreviewAnimationPanel> preview_animation_panel;