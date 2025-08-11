#ifndef Theme_hpp
#define Theme_hpp

sf::Font basicFont;

// dialogs
int dialog_border_width;
int dialog_margin;						// distance from edge of screen to dialog
int dialog_padding;
float dialog_title_font_size;
int dialog_title_rect_height;
sf::Color dialog_title_text_color;
sf::Color dialog_rect_color;
sf::Color dialog_title_rect_color;
sf::Color dialog_content_rect_color;

// main menu
int menu_height;
int menu_padding;
int menu_font_size;
sf::Color menu_rect_color;
sf::Color menu_text_color;
sf::Color menubox_idle_color;
sf::Color menubox_hover_color;
sf::Color menubox_press_color;

sf::Color normal_text_color;

sf::Color canvas_color1;
sf::Color canvas_color2;

int colors_dialog_margin;

void loadTheme() {

	basicFont.loadFromFile("C:/Windows/Fonts/arial.ttf");

	// dialogs
	dialog_margin = 8;
	dialog_padding = 8;
	dialog_title_font_size = 17;
	dialog_title_rect_height = 32;
	dialog_border_width = 1;
	dialog_title_text_color = sf::Color(191, 191, 191);
	dialog_rect_color = sf::Color(63, 63, 63);
	dialog_title_rect_color = sf::Color(15, 15, 15);
	dialog_content_rect_color = sf::Color(31, 31, 31);

	//main menu
	menu_height = 28;
	menu_padding = 2;
	menu_font_size = 17;
	menu_rect_color = sf::Color(15, 15, 15);
	menu_text_color = sf::Color(191, 191, 191);
	menubox_idle_color = sf::Color(15, 15, 15);
	menubox_hover_color = sf::Color(47, 47, 47);
	menubox_press_color = sf::Color(31, 31, 31);

	normal_text_color = sf::Color(191, 191, 191);

	canvas_color1 = sf::Color(63, 63, 63);
	canvas_color2 = sf::Color(95, 95, 95);

	colors_dialog_margin = 2;
}
#endif