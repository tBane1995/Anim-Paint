#include "Dialogs/Dialog_Load_SpriteSheet.hpp"
#include "MainMenu.hpp"
#include <iostream>
#include "Animation/Animation.hpp"
#include "Theme.hpp"
#include "Cursor.hpp"
#include "Window.hpp"
#include "Time.hpp"
#include "Canvas.hpp"
#include "Dialogs/FramesDialog.hpp"
#include "Dialogs/AnimationsDialog.hpp"
#include "DebugLog.hpp"

Dialog_Load_SpriteSheet::Dialog_Load_SpriteSheet(std::filesystem::path path) : Dialog(L"Import Animation", sf::Vector2i(256+16, 256+16+80)) {
	
	std::wcout << L"Import Animation: " << path << L"\n";

	spriteSheet = sf::Image();

	if (!spriteSheet.loadFromFile(path)) {
		std::wcout << L"cant open SpriteSheet\n";
	}

	if (spriteSheet.getSize().x == 0 || spriteSheet.getSize().y == 0) {
		std::wcout << L"SpriteSheet has invalid size\n";
		spriteSheet = sf::Image();
	}

	if ((int)spriteSheet.getSize().x > canvas->_maxSize.x*maxFramesCount || (int)spriteSheet.getSize().y > canvas->_maxSize.y * maxAnimationsCount) {
		std::wcout << L"SpriteSheet is too big (max " + std::to_wstring(canvas->_maxSize.x * maxFramesCount) + L"x" + std::to_wstring(canvas->_maxSize.y * maxAnimationsCount) + L")\n";
		spriteSheet = sf::Image();
	}

	
	//////////////////////////////////////
	sf::Vector2i size(64, basic_text_rect_height);
	int baseWidth = findBaseWidth(spriteSheet);
	int baseHeight = findBaseHeight(spriteSheet);
	int tileWidth = findTileWidth(spriteSheet, baseWidth);
	int tileHeight = findTileHeight(spriteSheet, baseHeight);

	_widthOfFrame = std::make_shared<NumberInput>(size, 3, basic_text_size, tileWidth, canvas->_minSize.x, canvas->_maxSize.x);
	_heightOfFrame = std::make_shared<NumberInput>(size, 3, basic_text_size, tileHeight, canvas->_minSize.y, canvas->_maxSize.y);
	_animationsCount = std::make_shared<NumberInput>(size, 3, basic_text_size, 0, 0, maxAnimationsCount);
	_frameCount = std::make_shared<NumberInput>(size, 3, basic_text_size, 0, 0, maxFramesCount);

	_widthOfFrameText = std::make_unique<sf::Text>(basicFont, "Width", basic_text_size);
	_heightOfFrameText = std::make_unique<sf::Text>(basicFont, "Height", basic_text_size);
	_animationsCountText = std::make_unique<sf::Text>(basicFont, "Animations", basic_text_size);
	_frameCountText = std::make_unique<sf::Text>(basicFont, "Frames", basic_text_size);

	int animationsCount = (int)(std::ceil(float(spriteSheet.getSize().y) / std::stoi(_heightOfFrame->getText())));
	int framesCount = (int)(std::ceil(float(spriteSheet.getSize().x) / std::stoi(_widthOfFrame->getText())));
	_animationsCount->setText(std::to_wstring(animationsCount));
	_frameCount->setText(std::to_wstring(framesCount));

	//////////////////////////////////////

	_widthOfFrame->_onEditedFunction = [this]() {

		sf::Vector2i size;
		size.x = std::stoi(_widthOfFrame->getText());
		size.y = std::stoi(_heightOfFrame->getText());

		int animationsCount = (int)(std::ceil(float(spriteSheet.getSize().y) / float(size.y)));
		int framesCount = (int)(std::ceil(float(spriteSheet.getSize().x) / float(size.x)));
		_animationsCount->setText(std::to_wstring(animationsCount));
		_frameCount->setText(std::to_wstring(framesCount));

		if (datasIsCorrect())
			loadAnimationsByFrameSize(size);
		};

	_heightOfFrame->_onEditedFunction = [this]() {

		sf::Vector2i size;
		size.x = std::stoi(_widthOfFrame->getText());
		size.y = std::stoi(_heightOfFrame->getText());

		int animationsCount = (int)(std::ceil(float(spriteSheet.getSize().y) / float(size.y)));
		int framesCount = (int)(std::ceil(float(spriteSheet.getSize().x) / float(size.x)));
		_animationsCount->setText(std::to_wstring(animationsCount));
		_frameCount->setText(std::to_wstring(framesCount));

		if (datasIsCorrect())
			loadAnimationsByFrameSize(size);
		
	};

	_animationsCount->_onEditedFunction = [this]() {

		sf::Vector2i size;
		size.x = (int)(std::ceil(float(spriteSheet.getSize().x) / std::stof(_frameCount->getText())));
		size.y = (int)(std::ceil(float(spriteSheet.getSize().y) / std::stof(_animationsCount->getText())));
		_widthOfFrame->setText(std::to_wstring(size.x));
		_heightOfFrame->setText(std::to_wstring(size.y));

		if (datasIsCorrect())
			loadAnimationsByFrameSize(size);
	};

	_frameCount->_onEditedFunction = [this]() {

		sf::Vector2i size;
		size.x = (int)(std::ceil(float(spriteSheet.getSize().x) / std::stof(_frameCount->getText())));
		size.y = (int)(std::ceil(float(spriteSheet.getSize().y) / std::stof(_animationsCount->getText())));
		_widthOfFrame->setText(std::to_wstring(size.x));
		_heightOfFrame->setText(std::to_wstring(size.y));
		
		if (datasIsCorrect())
			loadAnimationsByFrameSize(size);
	};

	// clicked
	_widthOfFrame->_onClickedFunction = [this]() { activateOnTabElement(0); };
	_heightOfFrame->_onClickedFunction = [this]() { activateOnTabElement(1); };
	_animationsCount->_onClickedFunction = [this]() { activateOnTabElement(2); };
	_frameCount->_onClickedFunction = [this]() { activateOnTabElement(3); };

	// pressed enter
	_widthOfFrame->_onEnteredFunction = [this]() { activateOnTabElement(4); };
	_heightOfFrame->_onEnteredFunction = [this]() { activateOnTabElement(4); };
	_animationsCount->_onEnteredFunction = [this]() { activateOnTabElement(4); };
	_frameCount->_onEnteredFunction = [this]() { activateOnTabElement(4); };

	
	sf::Vector2i btnSize(64, basic_text_rect_height + dialog_padding);
	_bottomRect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(getContentSize().x, btnSize.y + 2 * dialog_padding));

	_confirmBtn = std::make_shared<ColoredButtonWithText>(L"Confirm", btnSize);
	_confirmBtn->setRectColors(dark_button_select_color, dark_button_normal_color, dark_button_hover_color, dark_button_press_color);
	_confirmBtn->activateByEnter(true);
	_confirmBtn->_onclick_func = [this]() {
		if (datasIsCorrect()) {
			sf::Vector2i size;
			size.x = std::stoi(_widthOfFrame->getText());
			size.y = std::stoi(_heightOfFrame->getText());
			loadAnimationsByFrameSize(size);
			main_menu->importAnimation(_animations);
			animations_dialog->updateText();
			frames_dialog->updateText();
			_state = DialogState::ToClose;
		}
		};

	_cancelBtn = std::make_shared<ColoredButtonWithText>(L"Cancel", btnSize);
	_cancelBtn->setRectColors(dark_button_select_color, dark_button_normal_color, dark_button_hover_color, dark_button_press_color);
	_cancelBtn->activateByEnter(true);
	_cancelBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};
	// ... 

	_animations.clear();
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
	_currentOnTabElement = -1;
}

Dialog_Load_SpriteSheet::~Dialog_Load_SpriteSheet() {

}

void Dialog_Load_SpriteSheet::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	// draw texts inputs
	sf::Vector2i size(64, basic_text_rect_height);
	sf::Vector2i center;
	center.x = getContentPosition().x + getContentSize().x / 2;
	center.y = getContentPosition().y + 128 + dialog_padding + basic_text_rect_height + dialog_padding + 8;
	int dy = 60;
	int dx = 48;

	sf::Vector2i widthOfFramePosition = center + sf::Vector2i(-dx - size.x / 2, -size.y / 2);
	sf::Vector2i heightOfFramePosition = center + sf::Vector2i(dx - size.x / 2, -size.y / 2);
	sf::Vector2i animationsCountPosition = center + sf::Vector2i(-dx - size.x / 2, dy - size.y / 2);
	sf::Vector2i frameCountPosition = center + sf::Vector2i(dx - size.x / 2, dy - size.y / 2);

	_widthOfFrame->setPosition(widthOfFramePosition);
	_heightOfFrame->setPosition(heightOfFramePosition);
	_animationsCount->setPosition(animationsCountPosition);
	_frameCount->setPosition(frameCountPosition);

	// draw texts labels
	float dy2 = (float)basic_text_rect_height;
	sf::Vector2f widthOfFrameTextPosition = sf::Vector2f(widthOfFramePosition) + sf::Vector2f(float(size.x / 2) - _widthOfFrameText->getGlobalBounds().size.x / 2.0f, -dy2);
	sf::Vector2f heightOfFrameTextPosition = sf::Vector2f(heightOfFramePosition) + sf::Vector2f(float(size.x / 2) - _heightOfFrameText->getGlobalBounds().size.x / 2.0f, -dy2);
	sf::Vector2f animationsCountTextPosition = sf::Vector2f(animationsCountPosition) + sf::Vector2f(float(size.x / 2) - _animationsCountText->getGlobalBounds().size.x / 2.0f, -dy2);
	sf::Vector2f frameCountTextPosition = sf::Vector2f(frameCountPosition) + sf::Vector2f(float(size.x / 2) - _frameCountText->getGlobalBounds().size.x / 2.0f, -dy2);

	_widthOfFrameText->setPosition(widthOfFrameTextPosition);
	_heightOfFrameText->setPosition(heightOfFrameTextPosition);
	_animationsCountText->setPosition(animationsCountTextPosition);
	_frameCountText->setPosition(frameCountTextPosition);


	// bottom buttons
	_bottomRect.position = sf::Vector2i(getContentPosition()) + sf::Vector2i(0, getContentSize().y - _bottomRect.size.y);
	int xx = 48;
	_cancelBtn->setPosition(getContentPosition() + sf::Vector2i(getContentSize().x / 2 - xx - _cancelBtn->getSize().x / 2, getContentSize().y - _cancelBtn->getSize().y - dialog_padding));
	_confirmBtn->setPosition(getContentPosition() + sf::Vector2i(getContentSize().x / 2 + xx - _confirmBtn->getSize().x / 2, getContentSize().y - _confirmBtn->getSize().y - dialog_padding));

}

int Dialog_Load_SpriteSheet::findBaseWidth(sf::Image& image) {

	int width = image.getSize().x;
	int height = image.getSize().y;
	sf::Color backgroundColor = image.getPixel(sf::Vector2u(0, 0));

	const uint8_t* pixels = image.getPixelsPtr();

	for (int x = 0; x < width; x++) {
		bool foundContent = false;
		for (int y = 0; y < height; y++) {

			uint8_t r = pixels[(y * width + x) * 4 + 0];
			uint8_t g = pixels[(y * width + x) * 4 + 1];
			uint8_t b = pixels[(y * width + x) * 4 + 2];
			uint8_t a = pixels[(y * width + x) * 4 + 3];

			if (r != backgroundColor.r || g != backgroundColor.g || b != backgroundColor.b || a != backgroundColor.a) {
				foundContent = true;
				break;
			}
		}
		if (foundContent) {
			for (int endX = x + 1; endX < width; endX++) {
				bool allBackground = true;
				for (int y = 0; y < height; y++) {

					uint8_t r = pixels[(y * width + endX) * 4 + 0];
					uint8_t g = pixels[(y * width + endX) * 4 + 1];
					uint8_t b = pixels[(y * width + endX) * 4 + 2];
					uint8_t a = pixels[(y * width + endX) * 4 + 3];

					if (r != backgroundColor.r || g != backgroundColor.g || b != backgroundColor.b || a != backgroundColor.a) {
						allBackground = false;
						break;
					}
				}
				if (allBackground) {
					return endX - x;
				}
			}
			return width - x;
		}
	}
	return width;
}

int Dialog_Load_SpriteSheet::findBaseHeight(sf::Image& image) {

	int width = image.getSize().x;
	int height = image.getSize().y;
	sf::Color backgroundColor = image.getPixel(sf::Vector2u(0, 0));

	const uint8_t* pixels = image.getPixelsPtr();

	for (int y = 0; y < height; y++) {
		bool foundContent = false;
		for (int x = 0; x < width; x++) {

			uint8_t r = pixels[(y * width + x) * 4 + 0];
			uint8_t g = pixels[(y * width + x) * 4 + 1];
			uint8_t b = pixels[(y * width + x) * 4 + 2];
			uint8_t a = pixels[(y * width + x) * 4 + 3];

			if (r != backgroundColor.r || g != backgroundColor.g || b != backgroundColor.b || a != backgroundColor.a) {
				foundContent = true;
				break;
			}

		}
		if (foundContent) {
			for (int endY = y + 1; endY < height; endY++) {
				bool allBackground = true;
				for (int x = 0; x < width; x++) {

					uint8_t r = pixels[(endY * width + x) * 4 + 0];
					uint8_t g = pixels[(endY * width + x) * 4 + 1];
					uint8_t b = pixels[(endY * width + x) * 4 + 2];
					uint8_t a = pixels[(endY * width + x) * 4 + 3];

					if (r != backgroundColor.r || g != backgroundColor.g || b != backgroundColor.b || a != backgroundColor.a) {
						allBackground = false;
						break;
					}
				}
				if (allBackground) {
					return endY - y;
				}
			}
			return height - y;
		}
	}
	return height;
}

int Dialog_Load_SpriteSheet::findTileWidth(sf::Image& image, int baseWidth) {
	// Sprawdź wszystkie możliwe szerokości klatki zaczynając od baseWidth

	int width = image.getSize().x;
	int height = image.getSize().y;
	sf::Color backgroundColor = image.getPixel(sf::Vector2u(0, 0));

	const uint8_t* pixels = image.getPixelsPtr();

	for (int candidate = baseWidth; candidate <= width; candidate++) {
		if (width % candidate != 0) continue;

		bool allLinesClean = true;
		int tileCount = width / candidate;

		// Sprawdź wszystkie pionowe linie podziału
		for (int i = 1; i < tileCount; i++) {
			int lineX = i * candidate;
			bool lineClean = true;

			for (int y = 0; y < height; y++) {

				uint8_t r = pixels[(y * width + lineX) * 4 + 0];
				uint8_t g = pixels[(y * width + lineX) * 4 + 1];
				uint8_t b = pixels[(y * width + lineX) * 4 + 2];
				uint8_t a = pixels[(y * width + lineX) * 4 + 3];

				if (r != backgroundColor.r || g != backgroundColor.g || b != backgroundColor.b || a != backgroundColor.a) {
					lineClean = false;
					break;
				}

				if (!lineClean) {
					allLinesClean = false;
					break;
				}
			}
		}

		if (allLinesClean) {
			return candidate;
		}
	}

	return width;
}

int Dialog_Load_SpriteSheet::findTileHeight(sf::Image& image, int baseHeight) {

	int width = image.getSize().x;
	int height = image.getSize().y;
	sf::Color backgroundColor = image.getPixel(sf::Vector2u(0, 0));

	const uint8_t* pixels = image.getPixelsPtr();

	// Sprawdź wszystkie możliwe wysokości klatki zaczynając od baseHeight
	for (int candidate = baseHeight; candidate <= height; candidate++) {
		if (height % candidate != 0) continue;

		bool allLinesClean = true;
		int tileCount = height / candidate;

		// Sprawdź wszystkie poziome linie podziału
		for (int i = 1; i < tileCount; i++) {
			int lineY = i * candidate;
			bool lineClean = true;

			for (int x = 0; x < width; x++) {
				uint8_t r = pixels[(lineY * width + x) * 4 + 0];
				uint8_t g = pixels[(lineY * width + x) * 4 + 1];
				uint8_t b = pixels[(lineY * width + x) * 4 + 2];
				uint8_t a = pixels[(lineY * width + x) * 4 + 3];

				if (r != backgroundColor.r || g != backgroundColor.g || b != backgroundColor.b || a != backgroundColor.a) {
					lineClean = false;
					break;
				}
			}

			if (!lineClean) {
				allLinesClean = false;
				break;
			}
		}

		if (allLinesClean) {
			return candidate;
		}
	}

	return height;
}

void Dialog_Load_SpriteSheet::loadAnimationsByFrameSize(sf::Vector2i frameSize) {

	_animations.clear();

	sf::Vector2u spriteSheetSize = spriteSheet.getSize();

	for (int y = 0, id=1; y < (int)spriteSheetSize.y; y+= frameSize.y, id+=1) {
		//std::wcout << L"load the anim: " << id << "\n";
		std::shared_ptr animation = std::make_shared<Animation>();

		for (int x = 0; x < (int)spriteSheetSize.x; x += frameSize.x) {

			std::shared_ptr<Frame> frame = std::make_shared<Frame>();
			sf::Image img;
			img.resize(sf::Vector2u(frameSize), sf::Color::White);
			
			if (!img.copy(spriteSheet, sf::Vector2u(0, 0), sf::IntRect(sf::Vector2i(x, y), frameSize), true)) {
				DebugError(L"Dialog_Load_SpriteSheet::loadAnimationsByFrameSize: cant copy image from spritesheet");
				exit(0);
			}

			frame->_layers.push_back(std::make_shared<Layer>(L"sprite", img));
			animation->_frames.push_back(frame);
		}

		_animations.push_back(animation);
	}
}

bool Dialog_Load_SpriteSheet::datasIsCorrect() {
	
	return (this->_widthOfFrame->dataIsCorrect() &&
		this->_heightOfFrame->dataIsCorrect() &&
		this->_animationsCount->dataIsCorrect() &&
		this->_frameCount->dataIsCorrect());
}

void Dialog_Load_SpriteSheet::loadFrame() {

	std::shared_ptr<Frame> currentFrame = _animations[0]->getCurrentFrame();
	if (currentFrame == nullptr) {
		DebugError(L"Dialog_Load_SpriteSheet::loadFrame: currentFrame is nullptr");
		exit(0);
	}
	
	std::shared_ptr<Layer> currentLayer = currentFrame->getLayer(0);

	if (currentLayer == nullptr) {
		DebugError(L"Dialog_Load_SpriteSheet::loadFrame: currentLayer is nullptr");
		exit(0);
	}

	sf::Image& img = currentLayer->_image;
	if (!_frameTexture.loadFromImage(img)) {
		DebugError(L"Dialog_Load_SpriteSheet::loadFrame: cant load texture from image");
		exit(0);
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

	// Handle Enter key for NumberInput elements
	if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Enter) {
		if (_currentOnTabElement >= 0 && _currentOnTabElement < (int)_onTabElements.size()) {
			if (auto ni = std::dynamic_pointer_cast<NumberInput>(_onTabElements[_currentOnTabElement])) {
				ni->handleEvent(event);
				return;
			}
		}
		
	}

	// Handle TextEntered event for Enter key (in case of IME input)
	if (const auto* te = event.getIf<sf::Event::TextEntered>(); te && (te->unicode == 13 || te->unicode == 10)) {
		if (_currentOnTabElement >= 0 && _currentOnTabElement < (int)_onTabElements.size()) {
			if (auto ni = std::dynamic_pointer_cast<NumberInput>(_onTabElements[_currentOnTabElement])) {
				ni->handleEvent(event);
				return;
			}
		}
		
	}

	_widthOfFrame->handleEvent(event);
	_heightOfFrame->handleEvent(event);
	_animationsCount->handleEvent(event);
	_frameCount->handleEvent(event);

	_confirmBtn->handleEvent(event);
	_cancelBtn->handleEvent(event);

	
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
		_animations[0]->nextFrame();
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
	_widthOfFrame->draw();
	_heightOfFrame->draw();
	_animationsCount->draw();
	_frameCount->draw();

	// draw texts labels
	window->draw(*_widthOfFrameText);
	window->draw(*_heightOfFrameText);
	window->draw(*_animationsCountText);
	window->draw(*_frameCountText);

	// draw bottom rect and buttons
	sf::RectangleShape bottomRect(sf::Vector2f(_bottomRect.size));
	bottomRect.setFillColor(dialog_content_rect_color_2);
	bottomRect.setPosition(sf::Vector2f(_bottomRect.position));
	window->draw(bottomRect);

	_confirmBtn->draw();
	_cancelBtn->draw();
}