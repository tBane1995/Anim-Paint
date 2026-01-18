#pragma once
#include "Dialog.hpp"
#include "Animation/Animation.hpp"
#include "ElementGUI/NumberInput.hpp"

class Dialog_Load_SpriteSheet : public Dialog {
public:

	sf::Image spriteSheet;
	std::vector<std::shared_ptr<Animation>> _animations;
	sf::Texture _frameTexture;
	sf::Time _lastFrameTime;

	std::shared_ptr<NumberInput> _widthOfFrame;
	std::shared_ptr<NumberInput> _heightOfFrame;
	std::shared_ptr<NumberInput> _animationsCount;
	std::shared_ptr<NumberInput> _frameCount;

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
	int findBaseWidth(sf::Image& image);
	int findBaseHeight(sf::Image& image);
	int findTileWidth(sf::Image& image, int baseWidth);
	int findTileHeight(sf::Image& image, int baseHeight);

	void loadAnimationsByFrameSize(sf::Vector2i frameSize);
	bool datasIsCorrect();
	void loadFrame();
	

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};