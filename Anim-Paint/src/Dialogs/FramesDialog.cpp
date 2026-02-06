#include "Dialogs/FramesDialog.hpp"
#include "Animation/Animation.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Tools/Selection.hpp"

FramesDialog::FramesDialog(std::wstring title, sf::Vector2i size, sf::Vector2i position) : Dialog(title, size, position) {


	_text = std::make_unique<sf::Text>(basicFont, std::to_wstring(getCurrentAnimation()->getCurrentFrameID() + 1) + L"/" + std::to_wstring(getCurrentAnimation()->getFramesCount()), 17);

	_first_btn = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\first.png"), getTexture(L"tex\\btn32\\first_hover.png"));
	_prev_btn = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\prev.png"), getTexture(L"tex\\btn32\\prev_hover.png"));
	_next_btn = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\next.png"), getTexture(L"tex\\btn32\\next_hover.png"));
	_last_btn = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\last.png"), getTexture(L"tex\\btn32\\last_hover.png"));

	_first_btn->_onclick_func = [this, position]() {
		getCurrentAnimation()->firstFrame();
		layers_dialog->loadLayersFromCurrentFrame();
		updateText();
		};

	_prev_btn->_onclick_func = [this, position]() {
		getCurrentAnimation()->prevFrame();
		layers_dialog->loadLayersFromCurrentFrame();
		updateText();
		};

	_next_btn->_onclick_func = [this, position]() {
		getCurrentAnimation()->nextFrame();
		layers_dialog->loadLayersFromCurrentFrame();
		updateText();
		};

	_last_btn->_onclick_func = [this, position]() {
		getCurrentAnimation()->lastFrame();
		layers_dialog->loadLayersFromCurrentFrame();
		updateText();
	};

	_add_frame = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\add_frame.png"), getTexture(L"tex\\btn32\\add_frame_hover.png"));
	_sub_frame = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\sub_frame.png"), getTexture(L"tex\\btn32\\sub_frame_hover.png"));
	_move_back = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\move_back.png"), getTexture(L"tex\\btn32\\move_back_hover.png"));
	_move_next = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\move_next.png"), getTexture(L"tex\\btn32\\move_next_hover.png"));

	_add_frame->_onclick_func = [this]() {
		if (getCurrentAnimation()->getFramesCount() < maxFramesCount) {
			getCurrentAnimation()->addFrame();
			getCurrentAnimation()->nextFrame();
			updateText();
			layers_dialog->loadLayersFromCurrentFrame();
		}
		};

	_sub_frame->_onclick_func = [this]() {
		if (getCurrentAnimation()->getFramesCount() > 0) {
			getCurrentAnimation()->subFrame();
			getCurrentAnimation()->prevFrame();
			updateText();
			layers_dialog->loadLayersFromCurrentFrame();
		}
		};

	_move_back->_onclick_func = [this]() {
		getCurrentAnimation()->moveBackFrame();
		getCurrentAnimation()->prevFrame();
		updateText();
		layers_dialog->loadLayersFromCurrentFrame();		
		};

	_move_next->_onclick_func = [this]() {
		getCurrentAnimation()->moveNextFrame();
		getCurrentAnimation()->nextFrame();
		updateText();
		layers_dialog->loadLayersFromCurrentFrame();
		};

	setPosition(position);
}

FramesDialog::~FramesDialog() {

}

void FramesDialog::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	_first_btn->setPosition(_position + sf::Vector2i(dialog_padding, 32 + dialog_padding));
	_prev_btn->setPosition(_position + sf::Vector2i(dialog_padding + 32, 32 + dialog_padding));
	_next_btn->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 64, 32 + dialog_padding));
	_last_btn->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 32, 32 + dialog_padding));

	updateText();

	_add_frame->setPosition(_position + sf::Vector2i(dialog_padding, 64 + dialog_padding));
	_sub_frame->setPosition(_position + sf::Vector2i(dialog_padding + 32, 64 + dialog_padding));
	_move_back->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 64, 64 + dialog_padding));
	_move_next->setPosition(_position + sf::Vector2i(getSize().x - dialog_padding - 32, 64 + dialog_padding));
}

void FramesDialog::updateText() {
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

void FramesDialog::cursorHover() {
	if (!dialogs.empty())
		return;


	if (selection->_state == SelectionState::Selecting)
		return;

	Dialog::cursorHover();

	_first_btn->cursorHover();
	_prev_btn->cursorHover();
	_next_btn->cursorHover();
	_last_btn->cursorHover();

	_add_frame->cursorHover();
	_sub_frame->cursorHover();
	_move_back->cursorHover();
	_move_next->cursorHover();
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

	_add_frame->handleEvent(event);
	_sub_frame->handleEvent(event);
	_move_back->handleEvent(event);
	_move_next->handleEvent(event);
}

void FramesDialog::update() {
	Dialog::update();


	_first_btn->update();
	_prev_btn->update();
	_next_btn->update();
	_last_btn->update();

	_add_frame->update();
	_sub_frame->update();
	_move_back->update();
	_move_next->update();
}

void FramesDialog::draw() {
	Dialog::draw();

	_first_btn->draw();
	_prev_btn->draw();
	_next_btn->draw();
	_last_btn->draw();

	window->draw(*_text);

	_add_frame->draw();
	_sub_frame->draw();
	_move_back->draw();
	_move_next->draw();
}

std::shared_ptr<FramesDialog> frames_dialog;
