#ifndef Theme_hpp
#define Theme_hpp

sf::Font basicFont;

int dialog_border_width;
int dialog_margin;						// distance from edge of screen to dialog
int dialog_padding;
float dialog_title_font_size;
sf::Color dialog_title_text_color;
sf::Color dialog_rect_color;
sf::Color dialog_title_rect_color;
sf::Color dialog_content_rect_color;

sf::Color canvas_color;

void loadTheme() {

	basicFont.loadFromFile("C:/Windows/Fonts/arial.ttf");

	dialog_margin = 16;
	dialog_padding = 8;
	dialog_title_font_size = 17;
	dialog_border_width = 1;
	dialog_title_text_color = sf::Color(192, 192, 192);
	dialog_rect_color = sf::Color(64, 64, 64);
	dialog_title_rect_color = sf::Color(16, 16, 16);
	dialog_content_rect_color = sf::Color(32, 32, 32);
	
	canvas_color = sf::Color(128, 128, 128);
}
#endif