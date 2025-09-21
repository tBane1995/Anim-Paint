#include "FramesDialog.hpp"
#include "Animation.hpp"
#include "Theme.hpp"
#include "Window.hpp"

FramesDialog::FramesDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position) : Dialog(title, size, position) {

	first_btn = new NormalButton(getTexture(L"tex\\frames\\first.png"), getTexture(L"tex\\frames\\first_hover.png"));
	prev_btn = new NormalButton(getTexture(L"tex\\frames\\prev.png"), getTexture(L"tex\\frames\\prev_hover.png"));
	next_btn = new NormalButton(getTexture(L"tex\\frames\\next.png"), getTexture(L"tex\\frames\\next_hover.png"));
	last_btn = new NormalButton(getTexture(L"tex\\frames\\last.png"), getTexture(L"tex\\frames\\last_hover.png"));

	first_btn->onclick_func = [this, position]() {
		animation->firstFrame();
		generateText();
		setPosition(position);
		};

	prev_btn->onclick_func = [this, position]() {
		animation->prevFrame();
		generateText();
		setPosition(position);
		};

	next_btn->onclick_func = [this, position]() {
		animation->nextFrame();
		generateText();
		setPosition(position);
		};

	last_btn->onclick_func = [this, position]() {
		animation->lastFrame();
		generateText();
		setPosition(position);
		};

	generateText();

	setPosition(position);
}

FramesDialog::~FramesDialog() {}

void FramesDialog::generateText() {
	text = sf::Text(std::to_wstring(animation->getCurrentFrameID() + 1) + L" / " + std::to_wstring(animation->getFramesSize()), basicFont, 17);
}

void FramesDialog::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);

	first_btn->setPosition(this->position + sf::Vector2f(dialog_padding, 32 + dialog_padding));
	prev_btn->setPosition(this->position + sf::Vector2f(dialog_padding + 32, 32 + dialog_padding));
	next_btn->setPosition(this->position + sf::Vector2f(getSize().x - dialog_padding - 64, 32 + dialog_padding));
	last_btn->setPosition(this->position + sf::Vector2f(getSize().x - dialog_padding - 32, 32 + dialog_padding));

	text.setPosition(this->position + sf::Vector2f(getSize().x / 2 - text.getGlobalBounds().width / 2.0f, 32 + dialog_padding + (32 - basicFont.getLineSpacing(17)) / 2));
}

void FramesDialog::cursorHover() {
	Dialog::cursorHover();

	first_btn->cursorHover();
	prev_btn->cursorHover();
	next_btn->cursorHover();
	last_btn->cursorHover();
}

void FramesDialog::handleEvent(sf::Event& event) {
	Dialog::handleEvent(event);

	first_btn->handleEvent(event);
	prev_btn->handleEvent(event);
	next_btn->handleEvent(event);
	last_btn->handleEvent(event);

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
		if (prev_btn->state != ButtonState::Pressed)
			prev_btn->click();
	}

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
		if (next_btn->state != ButtonState::Pressed)
			next_btn->click();
	}
}

void FramesDialog::update() {
	Dialog::update();

	first_btn->update();
	prev_btn->update();
	next_btn->update();
	last_btn->update();
}

void FramesDialog::draw() {
	Dialog::draw();

	first_btn->draw();
	prev_btn->draw();
	next_btn->draw();
	last_btn->draw();
	window->draw(text);
}

FramesDialog* frames_dialog = nullptr;
