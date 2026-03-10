#pragma once
#include "Dialogs/Dialog.hpp"
#include "Controls/ButtonWithSprite.hpp"

class AnimationsPanel : public Dialog {
public:

	std::shared_ptr<ButtonWithSprite> _first_btn;
	std::shared_ptr<ButtonWithSprite> _prev_btn;
	std::shared_ptr<ButtonWithSprite> _next_btn;
	std::shared_ptr<ButtonWithSprite> _last_btn;
	std::unique_ptr<sf::Text> _text;

	std::shared_ptr<ButtonWithSprite> _add_anim;
	std::shared_ptr<ButtonWithSprite> _sub_anim;
	std::shared_ptr<ButtonWithSprite> _move_back;
	std::shared_ptr<ButtonWithSprite> _move_next;

	AnimationsPanel();
	~AnimationsPanel();

	void setPosition(sf::Vector2i position);
	void updateText();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

extern std::shared_ptr<AnimationsPanel> animations_panel;