#include "Dialogs/Dialog_Load_SpriteSheet.hpp"
#include "MainMenu.hpp"
#include <iostream>
#include "Animation/Animation.hpp"
#include "Theme.hpp"
#include "Mouse.hpp"
#include "Window.hpp"
#include "Time.hpp"
#include "Canvas.hpp"

Dialog_Load_SpriteSheet::Dialog_Load_SpriteSheet(std::filesystem::path path) : Dialog(L"Import Animation", sf::Vector2i(256+16, 256+16+80)) {
	
	std::wcout << L"Import Animation: " << path << L"\n";

	if (!spriteSheet.loadFromFile(path)) {
		std::wcout << L"cant open SpriteSheet\n";
		return;
	}

	sf::Vector2f size(64, basic_text_rect_height);
	_widthOfFrame = std::make_shared<TextInput>(size, 3, basic_text_size);
	_heightOfFrame = std::make_shared<TextInput>(size, 3, basic_text_size);
	_animationsCount = std::make_shared<TextInput>(size, 3, basic_text_size);
	_frameCount = std::make_shared<TextInput>(size, 3, basic_text_size);

	_widthOfFrameText =std::make_unique<sf::Text>(basicFont, "Width", basic_text_size);
	_heightOfFrameText = std::make_unique<sf::Text>(basicFont, "Height", basic_text_size);
	_animationsCountText = std::make_unique<sf::Text>(basicFont, "Animations", basic_text_size);
	_frameCountText = std::make_unique<sf::Text>(basicFont, "Frames", basic_text_size);

	_widthOfFrame->setText(std::to_wstring(34/*canvas->_minSize.x*/));
	_heightOfFrame->setText(std::to_wstring(53/*canvas->_minSize.y */));
	int animationsCount = std::ceil(float(spriteSheet.getSize().y) / std::stoi(_heightOfFrame->getText()));
	int framesCount = std::ceil(float(spriteSheet.getSize().x) / std::stoi(_widthOfFrame->getText()));
	_animationsCount->setText(std::to_wstring(animationsCount));
	_frameCount->setText(std::to_wstring(framesCount));

	_widthOfFrame->_onEditedFunction = [this]() {

		if (datasIsCorrect()) {
			sf::Vector2i size;
			size.x = std::stoi(_widthOfFrame->getText());
			size.y = std::stoi(_heightOfFrame->getText());

			int animationsCount = std::ceil(float(spriteSheet.getSize().y) / float(size.y));
			int framesCount = std::ceil(float(spriteSheet.getSize().x) / float(size.x));
			_animationsCount->setText(std::to_wstring(animationsCount));
			_frameCount->setText(std::to_wstring(framesCount));

			loadAnimationsByFrameSize(size);
		}
		};

	_heightOfFrame->_onEditedFunction = [this]() {

		if (datasIsCorrect()) {
			sf::Vector2i size;
			size.x = std::stoi(_widthOfFrame->getText());
			size.y = std::stoi(_heightOfFrame->getText());

			int aniamtionsCount = std::ceil(float(spriteSheet.getSize().y) / float(size.y));
			int framesCount = std::ceil(float(spriteSheet.getSize().x) / float(size.x));
			_animationsCount->setText(std::to_wstring(aniamtionsCount));
			_frameCount->setText(std::to_wstring(framesCount));

			loadAnimationsByFrameSize(size);
		}
		};

	_animationsCount->_onEditedFunction = [this]() {

		if (datasIsCorrect()) {
			sf::Vector2i size;
			size.x = std::ceil(float(spriteSheet.getSize().x) / std::stof(_frameCount->getText()));
			size.y = std::ceil(float(spriteSheet.getSize().y) / std::stof(_animationsCount->getText()));
			_widthOfFrame->setText(std::to_wstring(size.x));
			_heightOfFrame->setText(std::to_wstring(size.y));
			loadAnimationsByFrameSize(size);
		}
		};

	_frameCount->_onEditedFunction = [this]() {

		if (datasIsCorrect()) {
			sf::Vector2i size;
			size.x = std::ceil(float(spriteSheet.getSize().x) / std::stof(_frameCount->getText()));
			size.y = std::ceil(float(spriteSheet.getSize().y) / std::stof(_animationsCount->getText()));
			_widthOfFrame->setText(std::to_wstring(size.x));
			_heightOfFrame->setText(std::to_wstring(size.y));
			loadAnimationsByFrameSize(size);
		}
		};

	_widthOfFrame->_onClickedFunction = [this]() { activateOnTabElement(0); };
	_heightOfFrame->_onClickedFunction = [this]() { activateOnTabElement(1); };
	_animationsCount->_onClickedFunction = [this]() { activateOnTabElement(2); };
	_frameCount->_onClickedFunction = [this]() { activateOnTabElement(3); };

	_widthOfFrame->_onEnteredFunction = [this]() { activateOnTabElement(4); };
	_heightOfFrame->_onEnteredFunction = [this]() { activateOnTabElement(4); };
	_animationsCount->_onEnteredFunction = [this]() { activateOnTabElement(4); };
	_frameCount->_onEnteredFunction = [this]() { activateOnTabElement(4); };

	sf::Vector2i btnSize(64, basic_text_rect_height + dialog_padding);
	_rect = sf::RectangleShape(sf::Vector2f(getContentSize().x, btnSize.y + 2 * dialog_padding));
	_rect.setFillColor(dialog_content_rect_color_2);

	_confirmBtn = std::make_shared<ColoredButtonWithText>(L"Confirm", btnSize);
	_confirmBtn->setColors(dark_button_select_color, dark_button_normal_color, dark_button_hover_color, dark_button_press_color);
	_confirmBtn->_onclick_func = [this]() {
		sf::Vector2i size;
		size.x = std::stoi(_widthOfFrame->getText());
		size.y = std::stoi(_heightOfFrame->getText());
		loadAnimationsByFrameSize(size);
		main_menu->importAnimation(_animation);
		_state = DialogState::ToClose;
		};

	_cancelBtn = std::make_shared<ColoredButtonWithText>(L"Cancel", btnSize);
	_cancelBtn->setColors(dark_button_select_color, dark_button_normal_color, dark_button_hover_color, dark_button_press_color);
	_cancelBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};
	// ... 

	_animation = nullptr;
	loadAnimationsByFrameSize(sf::Vector2i(std::stoi(_widthOfFrame->getText()), std::stoi(_heightOfFrame->getText())));
	_lastFrameTime = currentTime;

	////////////////////////////////
	loadFrame();
	////////////////////////////////

	setPosition(_position);

	_onTabElements.push_back(_widthOfFrame);
	_onTabElements.push_back(_heightOfFrame);
	_onTabElements.push_back(_animationsCount);
	_onTabElements.push_back(_frameCount);
	_onTabElements.push_back(_confirmBtn);
	_onTabElements.push_back(_cancelBtn);
	_currentOnTabElement = -1;;
}

Dialog_Load_SpriteSheet::~Dialog_Load_SpriteSheet() {

}

void Dialog_Load_SpriteSheet::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);
}

void Dialog_Load_SpriteSheet::loadAnimationsByFrameSize(sf::Vector2i frameSize) {


	_animation = std::make_shared<Animation>();
	sf::Vector2u spriteSheetSize = spriteSheet.getSize();

	for (int x = 0, id = 1; x < spriteSheetSize.x; x += frameSize.x, id += 1) {
		std::shared_ptr<Frame> frame = std::make_shared<Frame>();
		sf::Image img;
		img.resize(sf::Vector2u(frameSize), sf::Color::White);
		if (img.copy(spriteSheet, sf::Vector2u(0, 0), sf::IntRect(sf::Vector2i(x, 0), frameSize), true)) {
			//std::wcout << L"load the frame: " << id << "\n";
			frame->_layers.push_back(std::make_shared<Layer>(L"sprite", img));
			_animation->_frames.push_back(frame);
		}
	}
}

bool Dialog_Load_SpriteSheet::datasIsCorrect() {
	if (_widthOfFrame->getText().empty())
		return false;
	if (_heightOfFrame->getText().empty())
		return false;
	if (_animationsCount->getText().empty())
		return false;
	if (_frameCount->getText().empty())
		return false;
	if (std::stoi(_widthOfFrame->getText()) < canvas->_minSize.x)
		return false;
	if (std::stoi(_heightOfFrame->getText()) < canvas->_minSize.y)
		return false;
	if (std::stoi(_widthOfFrame->getText()) > canvas->_maxSize.x)
		return false;
	if (std::stoi(_heightOfFrame->getText()) > canvas->_maxSize.y)
		return false;
	if (std::stoi(_animationsCount->getText()) <= 0)
		return false;
	if (std::stoi(_frameCount->getText()) <= 0)
		return false;
	return true;
}

void Dialog_Load_SpriteSheet::loadFrame() {

	std::shared_ptr<Frame> currentFrame = _animation->getCurrentFrame();
	if (currentFrame != nullptr) {
		std::shared_ptr<Layer> currentLayer = currentFrame->getLayer(0);

		if (currentLayer != nullptr) {
			sf::Image& img = currentLayer->_image;
			_frameTexture.loadFromImage(img);
		}
	}
}

void Dialog_Load_SpriteSheet::deactivateOnTabElement() {
	if(_onTabElements.size()==0)
		return;

	if (_currentOnTabElement < 0)
		return;

	std::shared_ptr<TextInput> ti = std::dynamic_pointer_cast<TextInput>(_onTabElements[_currentOnTabElement]);
	if (ti != nullptr) {
		ti->_state = TextInputState::Idle;
		return;
	}

	std::shared_ptr<Button> btn = std::dynamic_pointer_cast<Button>(_onTabElements[_currentOnTabElement]);
	if (btn != nullptr) {
		btn->unselect();
		return;
	}

	sf::Image img;
	const uint8_t* pixels = img.getPixelsPtr();
	
}


void Dialog_Load_SpriteSheet::activateOnTabElement(int id) {
	
	deactivateOnTabElement();
	_currentOnTabElement = id;

	if (_currentOnTabElement >= _onTabElements.size())
		_currentOnTabElement = 0;

	if (_currentOnTabElement >= 0) {
		std::shared_ptr<TextInput> ti = std::dynamic_pointer_cast<TextInput>(_onTabElements[_currentOnTabElement]);
		if (ti != nullptr) {
			ti->_state = TextInputState::TextEntered;
		}

		std::shared_ptr<Button> btn = std::dynamic_pointer_cast<Button>(_onTabElements[_currentOnTabElement]);
		if (btn != nullptr) {
			btn->select();
		}
	}
}

void Dialog_Load_SpriteSheet::cursorHover() {
	Dialog::cursorHover();

	_widthOfFrame->cursorHover();
	_heightOfFrame->cursorHover();
	_animationsCount->cursorHover();
	_frameCount->cursorHover();

	_confirmBtn->cursorHover();
	_cancelBtn->cursorHover();
}

void Dialog_Load_SpriteSheet::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_widthOfFrame->handleEvent(event);
	_heightOfFrame->handleEvent(event);
	_animationsCount->handleEvent(event);
	_frameCount->handleEvent(event);

	_confirmBtn->handleEvent(event);
	_cancelBtn->handleEvent(event);

	if (_onTabElements.size() > 0) {
		if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Tab) {

			activateOnTabElement(_currentOnTabElement+1);
		}
	}
}

void Dialog_Load_SpriteSheet::update() {
	Dialog::update();

	_widthOfFrame->update();
	_heightOfFrame->update();
	_animationsCount->update();
	_frameCount->update();

	_confirmBtn->update();
	_cancelBtn->update();

	if ((currentTime - _lastFrameTime).asSeconds() > 0.5f) {
		_animation->nextFrame();
		//std::wcout << L"frame: " << _animation->getCurrentFrameID() << "\n";
		_lastFrameTime = currentTime;
		loadFrame();
	}

}

void Dialog_Load_SpriteSheet::draw() {

	Dialog::draw();

	sf::Sprite frameSprite(_frameTexture);
	frameSprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(_frameTexture.getSize())));
	float scaleFactor = 128.0f / float(std::max(_frameTexture.getSize().x, _frameTexture.getSize().y));
	frameSprite.setScale(sf::Vector2f(scaleFactor, scaleFactor));
	frameSprite.setPosition(sf::Vector2f(getContentPosition()) + sf::Vector2f((getContentSize().x - frameSprite.getGlobalBounds().size.x) / 2, (128 - frameSprite.getGlobalBounds().size.y) / 2 + dialog_padding));
	window->draw(frameSprite);

	// draw texts inputs
	sf::Vector2f size(64, basic_text_rect_height);
	sf::Vector2f center;
	center.x = getContentPosition().x + getContentSize().x / 2;
	center.y = getContentPosition().y + 128 + dialog_padding + basic_text_rect_height + dialog_padding + 8;
	float dy = 60;
	float dx = 48;

	_widthOfFrame->setPosition(center + sf::Vector2f(-dx - size.x / 2, -size.y / 2));
	_heightOfFrame->setPosition(center + sf::Vector2f(dx - size.x / 2, -size.y / 2));
	_animationsCount->setPosition(center + sf::Vector2f(-dx - size.x / 2, dy - size.y / 2));
	_frameCount->setPosition(center + sf::Vector2f(dx - size.x / 2, dy - size.y / 2));

	_widthOfFrame->draw();
	_heightOfFrame->draw();
	_animationsCount->draw();
	_frameCount->draw();

	// draw texts labels
	float dy2 = basic_text_rect_height;
	_widthOfFrameText->setPosition(_widthOfFrame->getPosition() + sf::Vector2f(size.x / 2 - _widthOfFrameText->getGlobalBounds().size.x / 2, -dy2));
	_heightOfFrameText->setPosition(_heightOfFrame->getPosition() + sf::Vector2f(size.x / 2 - _heightOfFrameText->getGlobalBounds().size.x / 2, -dy2));
	_animationsCountText->setPosition(_animationsCount->getPosition() + sf::Vector2f(size.x / 2 - _animationsCountText->getGlobalBounds().size.x / 2, -dy2));
	_frameCountText->setPosition(_frameCount->getPosition() + sf::Vector2f(size.x / 2 - _frameCountText->getGlobalBounds().size.x / 2, -dy2));

	window->draw(*_widthOfFrameText);
	window->draw(*_heightOfFrameText);
	window->draw(*_animationsCountText);
	window->draw(*_frameCountText);

	// draw bottom rect and buttons
	_rect.setPosition(sf::Vector2f(getContentPosition()) + sf::Vector2f(0, getContentSize().y - _rect.getSize().y));
	float xx = 48;
	_confirmBtn->setPosition(getContentPosition() + sf::Vector2i(getContentSize().x / 2 - xx - _confirmBtn->getSize().x / 2, getContentSize().y - _confirmBtn->getSize().y - dialog_padding));
	_cancelBtn->setPosition(getContentPosition() + sf::Vector2i(getContentSize().x / 2 + xx - _cancelBtn->getSize().x / 2, getContentSize().y - _cancelBtn->getSize().y - dialog_padding));

	window->draw(_rect);
	_confirmBtn->draw();
	_cancelBtn->draw();
}