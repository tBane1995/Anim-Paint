#pragma once
#include "Dialogs/Dialog.hpp"

class PreviewAnimationPanel : public Dialog {
public:

	int _currentFrame;
	sf::Time _timer;

	PreviewAnimationPanel();
	virtual ~PreviewAnimationPanel();

	void setPosition(sf::Vector2i position);

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};

extern std::shared_ptr<PreviewAnimationPanel> preview_animation_panel;