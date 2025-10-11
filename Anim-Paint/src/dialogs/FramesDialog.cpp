#include "Dialogs/FramesDialog.hpp"
#include "Animation/Animation.hpp"
#include "Theme.hpp"
#include "Window.hpp"

FramesDialog::FramesDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position) : Dialog(title, size, position) {

	_first_btn = new NormalButton(getTexture(L"tex\\frames\\first.png"), getTexture(L"tex\\frames\\first_hover.png"));
	_prev_btn = new NormalButton(getTexture(L"tex\\frames\\prev.png"), getTexture(L"tex\\frames\\prev_hover.png"));
	_next_btn = new NormalButton(getTexture(L"tex\\frames\\next.png"), getTexture(L"tex\\frames\\next_hover.png"));
	_last_btn = new NormalButton(getTexture(L"tex\\frames\\last.png"), getTexture(L"tex\\frames\\last_hover.png"));

	_first_btn->_onclick_func = [this, position]() {
		animation->firstFrame();
		generateText();
		setPosition(position);
		};

	_prev_btn->_onclick_func = [this, position]() {
		animation->prevFrame();
		generateText();
		setPosition(position);
		};

	_next_btn->_onclick_func = [this, position]() {
		animation->nextFrame();
		generateText();
		setPosition(position);
		};

	_last_btn->_onclick_func = [this, position]() {
		animation->lastFrame();
		generateText();
		setPosition(position);
		};

	generateText();

	setPosition(position);
}

FramesDialog::~FramesDialog() {}

void FramesDialog::generateText() {
	_text = sf::Text(std::to_wstring(animation->getCurrentFrameID() + 1) + L" / " + std::to_wstring(animation->getFramesCount()), basicFont, 17);
}

void FramesDialog::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);

	_first_btn->setPosition(_position + sf::Vector2f(dialog_padding, 32 + dialog_padding));
	_prev_btn->setPosition(_position + sf::Vector2f(dialog_padding + 32, 32 + dialog_padding));
	_next_btn->setPosition(_position + sf::Vector2f(getSize().x - dialog_padding - 64, 32 + dialog_padding));
	_last_btn->setPosition(_position + sf::Vector2f(getSize().x - dialog_padding - 32, 32 + dialog_padding));

	_text.setPosition(_position + sf::Vector2f(getSize().x / 2 - _text.getGlobalBounds().width / 2.0f, 32 + dialog_padding + (32 - basicFont.getLineSpacing(17)) / 2));
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

void FramesDialog::handleEvent(sf::Event& event) {

	if (!dialogs.empty())
		return;

	Dialog::handleEvent(event);

	_first_btn->handleEvent(event);
	_prev_btn->handleEvent(event);
	_next_btn->handleEvent(event);
	_last_btn->handleEvent(event);

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
		if (_prev_btn->_state != ButtonState::Pressed)
			_prev_btn->click();
	}

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
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

	_first_btn->draw();
	_prev_btn->draw();
	_next_btn->draw();
	_last_btn->draw();
	window->draw(_text);
}

FramesDialog* frames_dialog = nullptr;
