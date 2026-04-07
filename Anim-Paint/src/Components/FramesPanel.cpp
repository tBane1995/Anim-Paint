#include "Components/FramesPanel.hpp"
#include "Animation/Animation.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Components/LayersPanel/LayersPanel.hpp"
#include "Tools/Selection.hpp"
#include "Components/AnimationsPanel.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"
#include "History.hpp"

FramesPanel::FramesPanel() 
	: Dialog(
		L"Frames", 
		sf::Vector2i(192, dialog_title_rect_height + 32 + 32 + dialog_padding * 2),
		sf::Vector2i(int(mainView.getSize().x) - 192 - dialog_margin, animations_panel->getPosition().y + animations_panel->getSize().y)
	) {


	_text = std::make_unique<sf::Text>(basicFont, std::to_wstring(getCurrentAnimation()->getCurrentFrameID() + 1) + L"/" + std::to_wstring(getCurrentAnimation()->getFramesCount()), 17);

	_first_btn = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\first.png"), getTexture(L"tex\\btn32\\first_hover.png"), getTexture(L"tex\\btn32\\first_press.png"));
	_prev_btn = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\prev.png"), getTexture(L"tex\\btn32\\prev_hover.png"), getTexture(L"tex\\btn32\\prev_press.png"));
	_next_btn = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\next.png"), getTexture(L"tex\\btn32\\next_hover.png"), getTexture(L"tex\\btn32\\next_press.png"));
	_last_btn = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\last.png"), getTexture(L"tex\\btn32\\last_hover.png"), getTexture(L"tex\\btn32\\last_press.png"));

	_first_btn->_onclick_func = [this]() {
		getCurrentAnimation()->firstFrame();
		layers_panel->loadLayersFromCurrentFrame();
		updateText();
		};
	_first_btn->setTooltip(L"First frame", L"Go to the first frame");

	_prev_btn->_onclick_func = [this]() {
		getCurrentAnimation()->prevFrame();
		layers_panel->loadLayersFromCurrentFrame();
		updateText();
		};
	_prev_btn->setTooltip(L"Previous frame", L"Go to the previous frame");

	_next_btn->_onclick_func = [this]() {
		getCurrentAnimation()->nextFrame();
		layers_panel->loadLayersFromCurrentFrame();
		updateText();
		};
	_next_btn->setTooltip(L"Next frame", L"Go to the next frame");

	_last_btn->_onclick_func = [this]() {
		getCurrentAnimation()->lastFrame();
		layers_panel->loadLayersFromCurrentFrame();
		updateText();
	};
	_last_btn->setTooltip(L"Last frame", L"Go to the last frame");

	_add_frame = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\add.png"), getTexture(L"tex\\btn32\\add_hover.png"), getTexture(L"tex\\btn32\\add_press.png"));
	_remove_frame = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\remove.png"), getTexture(L"tex\\btn32\\remove_hover.png"), getTexture(L"tex\\btn32\\remove_press.png"));
	_move_back = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\move_back.png"), getTexture(L"tex\\btn32\\move_back_hover.png"), getTexture(L"tex\\btn32\\move_back_press.png"));
	_move_next = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\btn32\\move_next.png"), getTexture(L"tex\\btn32\\move_next_hover.png"), getTexture(L"tex\\btn32\\move_next_press.png"));

	_add_frame->_onclick_func = [this]() {
		if (getCurrentAnimation()->getFramesCount() < maxFramesCount) {
			getCurrentAnimation()->addEmptyFrame(canvas->_size);
			getCurrentAnimation()->nextFrame();
			updateText();
			layers_panel->loadLayersFromCurrentFrame();
			history->saveStep();
		}
		};
	_add_frame->setTooltip(L"Add frame", L"Add a new frame after the current one");

	_remove_frame->_onclick_func = [this]() {
		if (getCurrentAnimation()->getFramesCount() > 0) {
			getCurrentAnimation()->subFrame();
			getCurrentAnimation()->prevFrame();
			updateText();
			layers_panel->loadLayersFromCurrentFrame();
			history->saveStep();
		}
		};
	_remove_frame->setTooltip(L"Remove frame", L"Remove the current frame");

	_move_back->_onclick_func = [this]() {
		if (getCurrentAnimation()->moveBackFrame()) {
			getCurrentAnimation()->prevFrame();
			updateText();
			layers_panel->loadLayersFromCurrentFrame();
			history->saveStep();
		}
		
		};
	_move_back->setTooltip(L"Move frame back", L"Move the current frame one position back");

	_move_next->_onclick_func = [this]() {
		if (getCurrentAnimation()->moveNextFrame()) {
			getCurrentAnimation()->nextFrame();
			updateText();
			layers_panel->loadLayersFromCurrentFrame();
			history->saveStep();
		}
		
		};
	_move_next->setTooltip(L"Move frame next", L"Move the current frame one position forward");

	setPosition(_position);
}

FramesPanel::~FramesPanel() {

}

void FramesPanel::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	_first_btn->setPosition(_position + sf::Vector2i(dialog_padding, 32 + dialog_padding));
	_prev_btn->setPosition(_position + sf::Vector2i(dialog_padding + 32, 32 + dialog_padding));
	_next_btn->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 64, 32 + dialog_padding));
	_last_btn->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 32, 32 + dialog_padding));

	updateText();

	_add_frame->setPosition(_position + sf::Vector2i(dialog_padding, 64 + dialog_padding));
	_remove_frame->setPosition(_position + sf::Vector2i(dialog_padding + 32, 64 + dialog_padding));
	_move_back->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 64, 64 + dialog_padding));
	_move_next->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 32, 64 + dialog_padding));
}

void FramesPanel::updateText() {
	_text->setString(std::to_wstring(getCurrentAnimation()->getCurrentFrameID() + 1) + L"/" + std::to_wstring(getCurrentAnimation()->getFramesCount()));

	int pos_slash = (int)_text->getString().find(L'/');
	_text->setPosition(sf::Vector2f(0, 0));

	float slashLeft = _text->findCharacterPos(pos_slash).x;
	bool bold = (_text->getStyle() & sf::Text::Bold) != 0;
	const sf::Glyph& glyph = basicFont.getGlyph(L'/', 17, bold);
	float slashCenter = slashLeft + glyph.bounds.position.x + glyph.bounds.size.x / 2.f;
	float dialogCenterX = _position.x + getSize().x / 2.f;
	float newX = dialogCenterX - slashCenter;

	_text->setPosition(sf::Vector2f(newX, _position.y + 48 + dialog_padding + (32 - basicFont.getLineSpacing(17)) / 2.f));

}

void FramesPanel::cursorHover() {
	if (!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	if (!static_dialogs.empty() && static_dialogs.front()->_is_moved)
		return;

	if (canvas->_state != CanvasState::Idle)
		return;

	if (resizable_tool != nullptr && resizable_tool->_state != ResizableToolState::None && resizable_tool->_state != ResizableToolState::Selected)
		return;

	Dialog::cursorHover();

	_first_btn->cursorHover();
	_prev_btn->cursorHover();
	_next_btn->cursorHover();
	_last_btn->cursorHover();

	_add_frame->cursorHover();
	_remove_frame->cursorHover();
	_move_back->cursorHover();
	_move_next->cursorHover();
}

void FramesPanel::handleEvent(const sf::Event& event) {

	if (!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	Dialog::handleEvent(event);

	_first_btn->handleEvent(event);
	_prev_btn->handleEvent(event);
	_next_btn->handleEvent(event);
	_last_btn->handleEvent(event);

	if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::B) {
		if (_prev_btn->_state != ButtonState::Pressed)
			_prev_btn->click();

	}

	if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::N) {
		if (_next_btn->_state != ButtonState::Pressed)
			_next_btn->click();
	}

	_add_frame->handleEvent(event);
	_remove_frame->handleEvent(event);
	_move_back->handleEvent(event);
	_move_next->handleEvent(event);
}

void FramesPanel::update() {
	Dialog::update();


	_first_btn->update();
	_prev_btn->update();
	_next_btn->update();
	_last_btn->update();

	_add_frame->update();
	_remove_frame->update();
	_move_back->update();
	_move_next->update();
}

void FramesPanel::draw() {
	Dialog::draw();

	_first_btn->draw();
	_prev_btn->draw();
	_next_btn->draw();
	_last_btn->draw();

	window->draw(*_text);

	_add_frame->draw();
	_remove_frame->draw();
	_move_back->draw();
	_move_next->draw();
}

std::shared_ptr<FramesPanel> frames_panel;
