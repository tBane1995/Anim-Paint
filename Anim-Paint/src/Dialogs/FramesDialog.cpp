#include "Dialogs/FramesDialog.hpp"
#include "Animation/Animation.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Dialogs/LayersDialog.hpp"

FramesDialog::FramesDialog(std::wstring title, sf::Vector2i size, sf::Vector2i position) : Dialog(title, size, position) {

	_first_btn = std::make_shared<NormalButton>(getTexture(L"tex\\frames\\first.png"), getTexture(L"tex\\frames\\first_hover.png"));
	_prev_btn = std::make_shared<NormalButton>(getTexture(L"tex\\frames\\prev.png"), getTexture(L"tex\\frames\\prev_hover.png"));
	_next_btn = std::make_shared<NormalButton>(getTexture(L"tex\\frames\\next.png"), getTexture(L"tex\\frames\\next_hover.png"));
	_last_btn = std::make_shared<NormalButton>(getTexture(L"tex\\frames\\last.png"), getTexture(L"tex\\frames\\last_hover.png"));

	_first_btn->_onclick_func = [this, position]() {
		animation->firstFrame();
		layers_dialog->loadLayersFromCurrentFrame();
		};

	_prev_btn->_onclick_func = [this, position]() {
		animation->prevFrame();
		layers_dialog->loadLayersFromCurrentFrame();
	};

	_next_btn->_onclick_func = [this, position]() {
		animation->nextFrame();
		layers_dialog->loadLayersFromCurrentFrame();
	};

	_last_btn->_onclick_func = [this, position]() {
		animation->lastFrame();
		layers_dialog->loadLayersFromCurrentFrame();
	};

	setPosition(position);
}

FramesDialog::~FramesDialog() {

}

void FramesDialog::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);
}

void FramesDialog::cursorHover() {
	if (!dialogs.empty())
		return;

	Dialog::cursorHover();

	_first_btn->cursorHover();
	_prev_btn->cursorHover();
	_next_btn->cursorHover();
	_last_btn->cursorHover();
}

void FramesDialog::handleEvent(const sf::Event& event) {

	if (!dialogs.empty())
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
}

void FramesDialog::update() {
	Dialog::update();

	_first_btn->update();
	_prev_btn->update();
	_next_btn->update();
	_last_btn->update();
}

void FramesDialog::draw() {
	Dialog::draw();

	_first_btn->setPosition(_position + sf::Vector2i(dialog_padding, 32 + dialog_padding));
	_prev_btn->setPosition(_position + sf::Vector2i(dialog_padding + 32, 32 + dialog_padding));
	_next_btn->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 64, 32 + dialog_padding));
	_last_btn->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 32, 32 + dialog_padding));

	_first_btn->draw();
	_prev_btn->draw();
	_next_btn->draw();
	_last_btn->draw();

	if (_text == nullptr) {
		_text = std::make_unique<sf::Text>(basicFont, std::to_wstring(animation->getCurrentFrameID() + 1) + L" / " + std::to_wstring(animation->getFramesCount()), 17);
	}
	_text->setString(std::to_wstring(animation->getCurrentFrameID() + 1) + L" / " + std::to_wstring(animation->getFramesCount()));
	_text->setPosition(sf::Vector2f(_position) + sf::Vector2f(getSize().x / 2 - _text->getGlobalBounds().size.x / 2.0f, 32 + dialog_padding + (32 - basicFont.getLineSpacing(17)) / 2));
	window->draw(*_text);
}

FramesDialog* frames_dialog = nullptr;
