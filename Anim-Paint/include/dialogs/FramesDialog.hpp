#pragma once
#include "Dialog.hpp"

class FramesDialog : public Dialog {
public:

	NormalButton* _first_btn;
	NormalButton* _prev_btn;
	NormalButton* _next_btn;
	NormalButton* _last_btn;
	sf::Text _text;


	FramesDialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0));
	~FramesDialog();

	void generateText();
	void setPosition(sf::Vector2f position);
	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};

extern FramesDialog* frames_dialog;