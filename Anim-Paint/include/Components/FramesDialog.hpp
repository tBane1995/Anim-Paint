#pragma once
#include "../Dialogs/Dialog.hpp"

class FramesDialog : public Dialog {
public:

	std::shared_ptr<NormalButton> _first_btn;
	std::shared_ptr<NormalButton> _prev_btn;
	std::shared_ptr<NormalButton> _next_btn;
	std::shared_ptr<NormalButton> _last_btn;
	std::unique_ptr<sf::Text> _text;

	std::shared_ptr<NormalButton> _add_frame;
	std::shared_ptr<NormalButton> _sub_frame;
	std::shared_ptr<NormalButton> _move_back;
	std::shared_ptr<NormalButton> _move_next;

	FramesDialog(std::wstring title, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(0, 0));
	~FramesDialog();

	void setPosition(sf::Vector2i position);
	void updateText();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

extern std::shared_ptr<FramesDialog> frames_dialog;