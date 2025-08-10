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

sf::Color normal_text_color;

sf::Color canvas_color1;
sf::Color canvas_color2;

int colors_dialog_margin;

void loadTheme() {

	basicFont.loadFromFile("C:/Windows/Fonts/arial.ttf");

	dialog_margin = 16;
	dialog_padding = 8;
	dialog_title_font_size = 17;
	dialog_border_width = 1;
	dialog_title_text_color = sf::Color(191, 191, 191);
	dialog_rect_color = sf::Color(63, 63, 63);
	dialog_title_rect_color = sf::Color(15, 15, 15);
	dialog_content_rect_color = sf::Color(31, 31, 31);

	normal_text_color = sf::Color(191, 191, 191);

	canvas_color1 = sf::Color(63, 63, 63);
	canvas_color2 = sf::Color(95, 95, 95);

	colors_dialog_margin = 2;
}
#endif