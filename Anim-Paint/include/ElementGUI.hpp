#ifndef ElementGUI_hpp
#define ElementGUI_hpp

class ElementGUI {
public:

	ElementGUI() { }
	~ElementGUI() { }

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

ElementGUI* ElementGUI_hovered = nullptr;
ElementGUI* ElementGUI_pressed = nullptr;

#endif