#pragma once
#include "Dialog.hpp"
#include "Animation/Animation.hpp"
#include "ElementGUI/TextInput.hpp"

class Dialog_Import_Animation : public Dialog {
public:

	sf::Image spriteSheet;
	Animation* _animation;
	sf::Sprite _frameSprite;
	sf::Time _lastFrameTime;

	TextInput* _widthOfFrame;
	TextInput* _heightOfFrame;
	TextInput* _animationsCount;
	TextInput* _frameCount;

	sf::Text _widthOfFrameText;
	sf::Text _heightOfFrameText;
	sf::Text _animationsCountText;
	sf::Text _frameCountText;

	sf::RectangleShape _rect;
	ColoredButtonWithText* _confirmBtn;
	ColoredButtonWithText* _cancelBtn;

	Dialog_Import_Animation(std::filesystem::path path);
	~Dialog_Import_Animation();

	void setPosition(sf::Vector2f position);
	void loadAnimationsByFrameSize(sf::Vector2i frameSize);
	void loadFrame();

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};