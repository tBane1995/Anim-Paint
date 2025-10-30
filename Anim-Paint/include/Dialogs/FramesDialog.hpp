#pragma once
#include "Dialog.hpp"

class FramesDialog : public Dialog {
public:

	std::shared_ptr<NormalButton> _first_btn;
	std::shared_ptr<NormalButton> _prev_btn;
	std::shared_ptr<NormalButton> _next_btn;
	std::shared_ptr<NormalButton> _last_btn;
	std::unique_ptr<sf::Text> _text;


	FramesDialog(std::wstring title, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(0, 0));
	~FramesDialog();

	void setPosition(sf::Vector2i position);
	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

extern FramesDialog* frames_dialog;