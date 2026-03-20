#pragma once
#include "Dialogs/Dialog.hpp"

class FramesPanel : public Dialog {
public:

	std::shared_ptr<ButtonWithSprite> _first_btn;
	std::shared_ptr<ButtonWithSprite> _prev_btn;
	std::shared_ptr<ButtonWithSprite> _next_btn;
	std::shared_ptr<ButtonWithSprite> _last_btn;
	std::unique_ptr<sf::Text> _text;

	std::shared_ptr<ButtonWithSprite> _add_frame;
	std::shared_ptr<ButtonWithSprite> _remove_frame;
	std::shared_ptr<ButtonWithSprite> _move_back;
	std::shared_ptr<ButtonWithSprite> _move_next;

	FramesPanel();
	~FramesPanel();

	void setPosition(sf::Vector2i position);
	void updateText();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

extern std::shared_ptr<FramesPanel> frames_panel;