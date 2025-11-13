#include "Dialogs/AnimationsDialog.hpp"
#include "SFML/Graphics.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Window.hpp"
#include "Dialogs/FramesDialog.hpp"

AnimationsDialog::AnimationsDialog(std::wstring title, sf::Vector2i size, sf::Vector2i position) : Dialog(title, size, position) {


	_text = std::make_unique<sf::Text>(basicFont, std::to_wstring(getCurrentAnimationId()+1) + L" / " + std::to_wstring(getAnimationsCount()), 17);

	_first_btn = std::make_shared<NormalButton>(getTexture(L"tex\\frames\\first.png"), getTexture(L"tex\\frames\\first_hover.png"));
	_prev_btn = std::make_shared<NormalButton>(getTexture(L"tex\\frames\\prev.png"), getTexture(L"tex\\frames\\prev_hover.png"));
	_next_btn = std::make_shared<NormalButton>(getTexture(L"tex\\frames\\next.png"), getTexture(L"tex\\frames\\next_hover.png"));
	_last_btn = std::make_shared<NormalButton>(getTexture(L"tex\\frames\\last.png"), getTexture(L"tex\\frames\\last_hover.png"));

	_first_btn->_onclick_func = [this, position]() {
		firstAnimation();
		updateText();
		frames_dialog->_first_btn->_onclick_func();
		};

	_prev_btn->_onclick_func = [this, position]() {
		prevAnimation();
		updateText();
		frames_dialog->_first_btn->_onclick_func();
		};

	_next_btn->_onclick_func = [this, position]() {
		nextAnimation();
		updateText();
		frames_dialog->_first_btn->_onclick_func();
		};

	_last_btn->_onclick_func = [this, position]() {
		lastAnimation();
		updateText();
		frames_dialog->_first_btn->_onclick_func();
	};

	setPosition(position);
}

AnimationsDialog::~AnimationsDialog() {

}

void AnimationsDialog::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	_first_btn->setPosition(_position + sf::Vector2i(dialog_padding, 32 + dialog_padding));
	_prev_btn->setPosition(_position + sf::Vector2i(dialog_padding + 32, 32 + dialog_padding));
	_next_btn->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 64, 32 + dialog_padding));
	_last_btn->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 32, 32 + dialog_padding));

	_text->setPosition(sf::Vector2f(_position) + sf::Vector2f(getSize().x / 2 - _text->getGlobalBounds().size.x / 2.0f, 32 + dialog_padding + (32 - basicFont.getLineSpacing(17)) / 2));
}

void AnimationsDialog::updateText() {
	_text->setString(std::to_wstring(getCurrentAnimationId() + 1) + L" / " + std::to_wstring(getAnimationsCount()));
	_text->setPosition(sf::Vector2f(_position) + sf::Vector2f(getSize().x / 2 - _text->getGlobalBounds().size.x / 2.0f, 32 + dialog_padding + (32 - basicFont.getLineSpacing(17)) / 2));

}

void AnimationsDialog::cursorHover() {
	if (!dialogs.empty())
		return;

	Dialog::cursorHover();

	_first_btn->cursorHover();
	_prev_btn->cursorHover();
	_next_btn->cursorHover();
	_last_btn->cursorHover();
}

void AnimationsDialog::handleEvent(const sf::Event& event) {

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

void AnimationsDialog::update() {
	Dialog::update();


	_first_btn->update();
	_prev_btn->update();
	_next_btn->update();
	_last_btn->update();
}

void AnimationsDialog::draw() {
	Dialog::draw();

	_first_btn->draw();
	_prev_btn->draw();
	_next_btn->draw();
	_last_btn->draw();

	window->draw(*_text);
}

std::shared_ptr<AnimationsDialog> animations_dialog;
