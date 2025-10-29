#pragma once
#include "Dialog.hpp"
#include "Animation/Animation.hpp"
#include "ElementGUI/TextInput.hpp"

class Dialog_Load_SpriteSheet : public Dialog {
public:

	sf::Image spriteSheet;
	std::shared_ptr<Animation> _animation;
	sf::Texture _frameTexture;
	sf::Time _lastFrameTime;

	TextInput* _widthOfFrame;
	TextInput* _heightOfFrame;
	TextInput* _animationsCount;
	TextInput* _frameCount;

	sf::Text* _widthOfFrameText;
	sf::Text* _heightOfFrameText;
	sf::Text* _animationsCountText;
	sf::Text* _frameCountText;

	sf::RectangleShape _rect;
	ColoredButtonWithText* _confirmBtn;
	ColoredButtonWithText* _cancelBtn;

	std::vector < ElementGUI* > _onTabElements;
	int _currentOnTabElement;

	Dialog_Load_SpriteSheet(std::filesystem::path path);
	~Dialog_Load_SpriteSheet();

	void setPosition(sf::Vector2i position);
	void loadAnimationsByFrameSize(sf::Vector2i frameSize);
	bool datasIsCorrect();
	void loadFrame();
	void deactivateOnTabElement();
	void activateOnTabElement(int id);

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};