#pragma once
#include "Dialog.hpp"
#include "Animation/Animation.hpp"
#include "ElementGUI/TextInput.hpp"

class Dialog_Load_SpriteSheet : public Dialog {
public:

	sf::Image spriteSheet;
	std::vector<std::shared_ptr<Animation>> _animations;
	sf::Texture _frameTexture;
	sf::Time _lastFrameTime;

	std::shared_ptr<TextInput> _widthOfFrame;
	std::shared_ptr<TextInput> _heightOfFrame;
	std::shared_ptr<TextInput> _animationsCount;
	std::shared_ptr<TextInput> _frameCount;

	std::unique_ptr<sf::Text> _widthOfFrameText;
	std::unique_ptr<sf::Text> _heightOfFrameText;
	std::unique_ptr<sf::Text> _animationsCountText;
	std::unique_ptr<sf::Text> _frameCountText;

	sf::IntRect _bottomRect;
	std::shared_ptr<ColoredButtonWithText> _confirmBtn;
	std::shared_ptr<ColoredButtonWithText> _cancelBtn;

	

	Dialog_Load_SpriteSheet(std::filesystem::path path);
	~Dialog_Load_SpriteSheet();

	void setPosition(sf::Vector2i position);
	void loadAnimationsByFrameSize(sf::Vector2i frameSize);
	bool datasIsCorrect();
	void loadFrame();
	

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};