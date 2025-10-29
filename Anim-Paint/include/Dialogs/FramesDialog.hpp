#pragma once
#include "Dialog.hpp"

class FramesDialog : public Dialog {
public:

	NormalButton* _first_btn;
	NormalButton* _prev_btn;
	NormalButton* _next_btn;
	NormalButton* _last_btn;
	sf::Text* _text;


	FramesDialog(std::wstring title, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(0, 0));
	~FramesDialog();

	void generateText();
	void setPosition(sf::Vector2i position);
	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

extern FramesDialog* frames_dialog;