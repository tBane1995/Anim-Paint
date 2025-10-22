#include "Dialogs/Dialog_Import_Animation.hpp"
#include "MainMenu.hpp"
#include <iostream>
#include "Animation/Animation.hpp"
#include "Theme.hpp"
#include "Mouse.hpp"
#include "Window.hpp"
#include "Time.hpp"

Dialog_Import_Animation::Dialog_Import_Animation(std::filesystem::path path) : 
	Dialog(L"Import Animation", sf::Vector2f(256+16, 256+16+80)),
	_widthOfFrameText(basicFont, "Width", basic_text_size),
	_heightOfFrameText(basicFont, "Height", basic_text_size),
	_animationsCountText(basicFont, "Animations", basic_text_size),
	_frameCountText(basicFont, "Frames", basic_text_size),
	_frameSprite(emptyTexture_128x128)
{
	
	std::wcout << L"Import Animation: " << path << L"\n";

	if (!spriteSheet.loadFromFile(path)) {
		std::wcout << L"cant open SpriteSheet\n";
		return;
	}

	sf::Vector2f size(64, basic_text_rect_height);
	_widthOfFrame = new TextInput(size, 3, basic_text_size);
	_heightOfFrame = new TextInput(size, 3, basic_text_size);
	_animationsCount = new TextInput(size, 3, basic_text_size);
	_frameCount = new TextInput(size, 3, basic_text_size);

	_widthOfFrame->_function = [this]() {
		sf::Vector2i size;
		size.x = std::stoi(_widthOfFrame->getText());
		size.y = std::stoi(_heightOfFrame->getText());

		int aniamtionsCount = std::ceil(float(spriteSheet.getSize().y) / float(size.y));
		int framesCount = std::ceil(float(spriteSheet.getSize().x) / float(size.x));
		_animationsCount->setText(std::to_wstring(aniamtionsCount));
		_frameCount->setText(std::to_wstring(framesCount));

		loadAnimationsByFrameSize(size);
		};

	_heightOfFrame->_function = [this]() {
		sf::Vector2i size;
		size.x = std::stoi(_widthOfFrame->getText());
		size.y = std::stoi(_heightOfFrame->getText());

		int aniamtionsCount = std::ceil(float(spriteSheet.getSize().y) / float(size.y));
		int framesCount = std::ceil(float(spriteSheet.getSize().x) / float(size.x));
		_animationsCount->setText(std::to_wstring(aniamtionsCount));
		_frameCount->setText(std::to_wstring(framesCount));

		loadAnimationsByFrameSize(size);
		};

	_animationsCount->_function = [this]() {
		sf::Vector2i size;
		size.x = std::ceil(float(spriteSheet.getSize().x) / std::stof(_frameCount->getText()));
		size.y = std::ceil(float(spriteSheet.getSize().y) / std::stof(_animationsCount->getText()));
		_widthOfFrame->setText(std::to_wstring(size.x));
		_heightOfFrame->setText(std::to_wstring(size.y));
		loadAnimationsByFrameSize(size);
		};

	_frameCount->_function = [this]() {
		sf::Vector2i size;
		size.x = std::ceil(float(spriteSheet.getSize().x) / std::stof(_frameCount->getText()));
		size.y = std::ceil(float(spriteSheet.getSize().y) / std::stof(_animationsCount->getText()));
		_widthOfFrame->setText(std::to_wstring(size.x));
		_heightOfFrame->setText(std::to_wstring(size.y));
		loadAnimationsByFrameSize(size);
		};


	sf::Vector2f btnSize(64, basic_text_rect_height + dialog_padding);
	_rect = sf::RectangleShape(sf::Vector2f(getContentSize().x, btnSize.y + 2 * dialog_padding));
	_rect.setFillColor(dialog_content_rect_color_2);

	_confirmBtn = new ColoredButtonWithText(L"Confirm", btnSize);
	_confirmBtn->setColors(sf::Color(31, 31, 31), sf::Color(31, 31, 31), sf::Color(7, 7, 7), sf::Color(23, 23, 23));
	_confirmBtn->_onclick_func = [this]() {
		sf::Vector2i size;
		size.x = std::stoi(_widthOfFrame->getText());
		size.y = std::stoi(_heightOfFrame->getText());
		loadAnimationsByFrameSize(size);
		main_menu->importAnimation(_animation);
		_state = DialogState::ToClose;
		};

	_cancelBtn = new ColoredButtonWithText(L"Cancel", btnSize);
	_cancelBtn->setColors(sf::Color(31, 31, 31), sf::Color(31, 31, 31), sf::Color(7, 7, 7), sf::Color(23, 23, 23));
	_cancelBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};
	// ... 

	_animation = nullptr;
	loadAnimationsByFrameSize(sf::Vector2i(64, 64));
	_lastFrameTime = currentTime;

	setPosition(_position);
	
}

Dialog_Import_Animation::~Dialog_Import_Animation() {

}

void Dialog_Import_Animation::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);

	

	_frameSprite.setPosition(getContentPosition() + sf::Vector2f((getContentSize().x - _frameSprite.getGlobalBounds().size.x) / 2, dialog_padding));

	sf::Vector2f size(64, basic_text_rect_height);
	
	sf::Vector2f center;
	center.x = _frameSprite.getPosition().x + _frameSprite.getGlobalBounds().size.x / 2;
	center.y = _frameSprite.getPosition().y + _frameSprite.getGlobalBounds().size.y + dialog_padding + basic_text_rect_height + dialog_padding + 8;
	float dy = 60;
	float dx = 48;

	_widthOfFrame->setPosition(center + sf::Vector2f(-dx - size.x / 2, -size.y / 2));
	_heightOfFrame->setPosition(center + sf::Vector2f(dx - size.x / 2, -size.y / 2));
	_animationsCount->setPosition(center + sf::Vector2f(-dx - size.x / 2, dy -size.y / 2));
	_frameCount->setPosition(center + sf::Vector2f(dx - size.x / 2, dy -size.y / 2));

	float dy2 = basic_text_rect_height;
	_widthOfFrameText.setPosition(_widthOfFrame->getPosition() + sf::Vector2f(size.x/2-_widthOfFrameText.getGlobalBounds().size.x/2, -dy2));
	_heightOfFrameText.setPosition(_heightOfFrame->getPosition() + sf::Vector2f(size.x/2-_heightOfFrameText.getGlobalBounds().size.x/2, -dy2));
	_animationsCountText.setPosition(_animationsCount->getPosition() + sf::Vector2f(size.x/2-_animationsCountText.getGlobalBounds().size.x/2, -dy2));
	_frameCountText.setPosition(_frameCount->getPosition() + sf::Vector2f(size.x/2-_frameCountText.getGlobalBounds().size.x/2, -dy2));

	_rect.setPosition(getContentPosition() + sf::Vector2f(0, getContentSize().y- _rect.getSize().y));

	float xx = 48;
	_confirmBtn->setPosition(getContentPosition() + sf::Vector2f(getContentSize().x/2 - xx - _confirmBtn->getSize().x/2, getContentSize().y - _confirmBtn->getSize().y - dialog_padding));
	_cancelBtn->setPosition(getContentPosition() + sf::Vector2f(getContentSize().x/2 + xx - _cancelBtn->getSize().x/2, getContentSize().y - _cancelBtn->getSize().y - dialog_padding));
}

void Dialog_Import_Animation::loadAnimationsByFrameSize(sf::Vector2i frameSize) {

	if (_animation != nullptr)
		delete _animation;

	_animation = new Animation();
	sf::Vector2u spriteSheetSize = spriteSheet.getSize();

	for (int x = 0, id = 1; x < spriteSheetSize.x; x += frameSize.x, id += 1) {
		Frame* frame = new Frame();

		sf::Image img;
		img.resize(sf::Vector2u(frameSize), sf::Color::White);
		if (img.copy(spriteSheet, sf::Vector2u(0, 0), sf::IntRect(sf::Vector2i(x, 0), frameSize), true)) {
			std::wcout << L"load the frame: " << id << "\n";
			frame->_layers.push_back(new Layer(L"sprite", img));
			_animation->_frames.push_back(frame);
		}
	}
}

void Dialog_Import_Animation::cursorHover() {
	Dialog::cursorHover();

	_widthOfFrame->cursorHover();
	_heightOfFrame->cursorHover();
	_animationsCount->cursorHover();
	_frameCount->cursorHover();

	_confirmBtn->cursorHover();
	_cancelBtn->cursorHover();
}

void Dialog_Import_Animation::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_widthOfFrame->handleEvent(event);
	_heightOfFrame->handleEvent(event);
	_animationsCount->handleEvent(event);
	_frameCount->handleEvent(event);

	_confirmBtn->handleEvent(event);
	_cancelBtn->handleEvent(event);
}

void Dialog_Import_Animation::update() {
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
	}

}

void Dialog_Import_Animation::draw() {

	Dialog::draw();

	sf::Image& img = _animation->getCurrentFrame()->_layers[0]->_image;
	sf::Texture tex(img);
	_frameSprite.setTextureRect(sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(tex.getSize())));
	float scaleFactor = 128.0f / float(std::max(tex.getSize().x, tex.getSize().y));
	_frameSprite.setScale(sf::Vector2f(scaleFactor, scaleFactor));
	_frameSprite.setTexture(tex);
	_frameSprite.setPosition(getContentPosition() + sf::Vector2f((getContentSize().x-_frameSprite.getGlobalBounds().size.x)/2, dialog_padding));

	window->draw(_frameSprite);

	_widthOfFrame->draw();
	_heightOfFrame->draw();
	_animationsCount->draw();
	_frameCount->draw();

	window->draw(_widthOfFrameText);
	window->draw(_heightOfFrameText);
	window->draw(_animationsCountText);
	window->draw(_frameCountText);

	window->draw(_rect);
	_confirmBtn->draw();
	_cancelBtn->draw();
}