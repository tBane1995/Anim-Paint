#ifndef Theme_hpp
#define Theme_hpp

sf::Font basicFont;

// dialogs
int dialog_margin;				// distance from edge of screen to dialog
int dialog_padding;
float dialog_title_font_size;
int dialog_title_rect_height;
int dialog_border_width;
sf::Color dialog_border_color;
sf::Color dialog_title_text_color;
sf::Color dialog_title_rect_color;
sf::Color dialog_content_rect_color;


// main menu
int menu_height;
int menu_horizontal_margin;
int menu_padding;
int menu_font_size;
sf::Color menu_bar_color;
sf::Color menu_text_color;
sf::Color menubox_idle_color;
sf::Color menubox_hover_color;
sf::Color menubox_press_color;
int menuoptions_border_width;
sf::Color menuoptions_border_color;
sf::Color optionbox_idle_color;
sf::Color optionbox_hover_color;
sf::Color optionbox_press_color;

// tools
int tools_height;
sf::Color tools_bar_color;
int tools_separator_height;
sf::Color tools_separator_color;
sf::Color tools_text_color;
sf::Color tools_text_hover_color;

// normal text
sf::Color normal_text_color;

// canvas
sf::Color canvas_color1;
sf::Color canvas_color2;

// palette with colors
int colors_dialog_margin;

void loadTheme() {

	basicFont.loadFromFile("C:/Windows/Fonts/arial.ttf");

	// dialogs
	dialog_margin = 8;
	dialog_padding = 8;
	dialog_title_font_size = 17;
	dialog_title_rect_height = 32;
	dialog_border_width = 1;
	dialog_border_color = sf::Color(63, 63, 63);
	dialog_title_text_color = sf::Color(191, 191, 191);
	dialog_title_rect_color = sf::Color(15, 15, 15);
	dialog_content_rect_color = sf::Color(31, 31, 31);

	//main menu
	menu_height = 28;
	menu_horizontal_margin = 8;
	menu_font_size = 17;
	menu_bar_color = sf::Color(15, 15, 15);
	menu_text_color = sf::Color(191, 191, 191);

	menubox_idle_color = sf::Color(15, 15, 15);
	menubox_hover_color = sf::Color(47, 47, 47);
	menubox_press_color = sf::Color(31, 31, 31);

	menuoptions_border_width = 2.0f;
	menuoptions_border_color = sf::Color(63, 63, 63);

	optionbox_idle_color = sf::Color(31, 31, 31);
	optionbox_hover_color = sf::Color(47, 47, 47);
	optionbox_press_color = sf::Color(39, 39, 39);

	// tools
	tools_height = 64 + 24;
	tools_bar_color = sf::Color(31, 31, 31);
	tools_separator_height = 64+8;
	tools_separator_color = sf::Color(127, 127, 127);
	tools_text_color = sf::Color(127, 127, 127);
	tools_text_hover_color = sf::Color(163, 163, 163);

	// normal text
	normal_text_color = sf::Color(191, 191, 191);

	// canvas
	canvas_color1 = sf::Color(63, 63, 63);
	canvas_color2 = sf::Color(95, 95, 95);

	// palette with colors
	colors_dialog_margin = 2;
}
#endif