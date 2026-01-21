#include "Theme.hpp"
#include "SFML/Graphics.hpp"

sf::Font basicFont;

// dialogs
int dialog_margin;				// distance from edge of screen to dialog
int dialog_padding;
int dialog_title_font_size;
int dialog_content_font_size;
int dialog_title_rect_height;
int dialog_border_width;
sf::Color dialog_border_color;
sf::Color dialog_title_text_color;
sf::Color dialog_title_rect_color;
sf::Color dialog_content_rect_color;
sf::Color dialog_content_rect_color_2;

// text
int basic_text_size;
sf::Color basic_text_color;
int basic_text_rect_height;

// file dialogs
sf::Color file_dialog_file_rect_idle_color;
sf::Color file_dialog_file_rect_hover_color;
sf::Color file_dialog_file_rect_press_color;
int file_dialog_separator_width;
sf::Color file_dialog_separator_idle_color;
sf::Color file_dialog_separator_hover_color;
sf::Color file_dialog_separator_press_color;

// buttons
sf::Color dark_and_red_button_select_color;
sf::Color dark_and_red_button_normal_color;
sf::Color dark_and_red_button_hover_color;
sf::Color dark_and_red_button_press_color;
sf::Color dark_and_red_button_text_color;

sf::Color dark_button_select_color;
sf::Color dark_button_normal_color;
sf::Color dark_button_hover_color;
sf::Color dark_button_press_color;
sf::Color dark_button_text_color;

// main menu
int menu_height;
int menu_horizontal_margin;
int menu_padding;
int menu_font_size;
sf::Color menu_bar_color;
sf::Color menu_text_color;
sf::Color menubox_open_color;
sf::Color menubox_idle_color;
sf::Color menubox_hover_color;
sf::Color menubox_press_color;
int menuoptions_border_width;
sf::Color menuoptions_border_color;
sf::Color optionbox_select_color;
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

int tools_border_width;
sf::Color tools_button_select_color;
sf::Color tools_button_idle_color;
sf::Color tools_button_hover_color;
sf::Color tools_button_press_color;
sf::Color tools_button_select_border_color;
sf::Color tools_button_idle_border_color;
sf::Color tools_button_hover_border_color;
sf::Color tools_button_press_border_color;

// normal text
sf::Color normal_text_color;
int normal_text_font_size;

// Text inputs
sf::Color textinput_idle_color;
sf::Color textinput_hover_color;
sf::Color textinput_textentered_color;
sf::Color textInput_incorrect_data_color;

// canvas
sf::Color canvas_color1;
sf::Color canvas_color2;

// palette with colors
int colors_dialog_margin;

void loadTheme() {

	basicFont.openFromFile("C:/Windows/Fonts/arial.ttf");

	// dialogs
	dialog_margin = 8;
	dialog_padding = 8;
	dialog_title_font_size = 17;
	dialog_content_font_size = 13;
	dialog_title_rect_height = 32;
	dialog_border_width = 1;
	dialog_border_color = sf::Color(63, 63, 63);
	dialog_title_text_color = sf::Color(191, 191, 191);
	dialog_title_rect_color = sf::Color(15, 15, 15);
	dialog_content_rect_color = sf::Color(31, 31, 31);
	dialog_content_rect_color_2 = sf::Color(47, 47, 47);


	// textE
	basic_text_size = 13;
	basic_text_color = sf::Color(191, 191, 191);
	basic_text_rect_height = 20.0f;

	// file dialogs
	file_dialog_file_rect_idle_color = sf::Color::Transparent;
	file_dialog_file_rect_hover_color = sf::Color(95, 47, 47);
	file_dialog_file_rect_press_color = sf::Color(63, 47, 47);
	file_dialog_separator_width = 6;
	file_dialog_separator_idle_color = sf::Color(63, 47, 47);
	file_dialog_separator_hover_color = sf::Color(79, 47, 47);
	file_dialog_separator_press_color = sf::Color(95, 47, 47);

	// buttons
	dark_and_red_button_select_color = sf::Color(79, 31, 31);
	dark_and_red_button_normal_color = sf::Color(47, 47, 47);
	dark_and_red_button_hover_color = sf::Color(31, 31, 31);
	dark_and_red_button_press_color = sf::Color(23, 23, 23);
	dark_and_red_button_text_color = sf::Color(191, 191, 191);

	dark_button_select_color = sf::Color(23, 23, 23);
	dark_button_normal_color = sf::Color(31, 31, 31);
	dark_button_hover_color = sf::Color(7, 7, 7);
	dark_button_press_color = sf::Color(23, 23, 23);
	dark_button_text_color = sf::Color(191, 191, 191);

	//main menu
	menu_height = 28;
	menu_horizontal_margin = 8;
	menu_font_size = 17;
	menu_bar_color = sf::Color(15, 15, 15);
	menu_text_color = sf::Color(191, 191, 191);

	menubox_open_color = sf::Color(31, 31, 31);
	menubox_idle_color = sf::Color(15, 15, 15);
	menubox_hover_color = sf::Color(47, 47, 47);
	menubox_press_color = sf::Color(39, 39, 39);

	menuoptions_border_width = 2.0f;
	menuoptions_border_color = sf::Color(63, 63, 63);

	optionbox_select_color = sf::Color(79, 31, 31);
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

	tools_border_width = 1.0f;
	tools_button_select_color = sf::Color(79, 47, 47);
	tools_button_idle_color = sf::Color::Transparent;
	tools_button_hover_color = sf::Color(95,47,47);
	tools_button_press_color = sf::Color(111, 47, 47);
	tools_button_select_border_color = sf::Color(95, 47, 47);
	tools_button_idle_border_color = sf::Color::Transparent;
	tools_button_hover_border_color = sf::Color(95, 47, 47);
	tools_button_press_border_color = sf::Color(127, 47, 47);

	// normal text
	normal_text_color = sf::Color(191, 191, 191);
	normal_text_font_size = 17;

	// Text inputs
	textinput_idle_color = sf::Color(63, 63, 63);
	textinput_hover_color = sf::Color(95, 95, 95);
	textinput_textentered_color = sf::Color(95, 95, 95);
	textInput_incorrect_data_color = sf::Color(127, 31, 31);

	// canvas
	canvas_color1 = sf::Color(63, 63, 63);
	canvas_color2 = sf::Color(95, 95, 95);

	// palette with colors
	colors_dialog_margin = 2;
}
