#pragma once
#include "SFML/Graphics.hpp"

extern sf::Font basicFont;

// dialogs
extern int dialog_margin;				// distance from edge of screen to dialog
extern int dialog_padding;
extern int dialog_title_font_size;
extern int dialog_content_font_size;
extern int dialog_title_rect_height;
extern int dialog_border_width;
extern sf::Color dialog_border_color;
extern sf::Color dialog_title_text_color;
extern sf::Color dialog_title_rect_color;
extern sf::Color dialog_content_rect_color;
extern sf::Color dialog_content_rect_color_2;

// text
extern int basic_text_size;
extern sf::Color basic_text_color;
extern int basic_text_rect_height;

// file dialogs
extern sf::Color file_dialog_file_rect_idle_color;
extern sf::Color file_dialog_file_rect_hover_color;
extern sf::Color file_dialog_file_rect_press_color;
extern int file_dialog_separator_width;
extern sf::Color file_dialog_separator_idle_color;
extern sf::Color file_dialog_separator_hover_color;
extern sf::Color file_dialog_separator_press_color;

// buttons
extern sf::Color dark_and_red_button_select_color;
extern sf::Color dark_and_red_button_idle_color;
extern sf::Color dark_and_red_button_hover_color;
extern sf::Color dark_and_red_button_press_color;
extern sf::Color dark_and_red_button_text_color;

extern sf::Color dark_button_select_color;
extern sf::Color dark_button_idle_color;
extern sf::Color dark_button_hover_color;
extern sf::Color dark_button_press_color;
extern sf::Color dark_button_text_color;

// main menu
extern int menu_height;
extern int menu_horizontal_margin;
extern int menu_padding;
extern int menu_font_size;
extern sf::Color menu_bar_color;
extern sf::Color menu_text_color;
extern sf::Color menubox_open_color;
extern sf::Color menubox_idle_color;
extern sf::Color menubox_hover_color;
extern sf::Color menubox_press_color;
extern int menuoptions_border_width;
extern sf::Color menuoptions_border_color;
extern sf::Color optionbox_select_color;
extern sf::Color optionbox_idle_color;
extern sf::Color optionbox_hover_color;
extern sf::Color optionbox_press_color;

// tools
extern int tools_height;
extern sf::Color tools_bar_color;
extern int tools_separator_height;
extern sf::Color tools_separator_color;
extern sf::Color tools_text_color;
extern sf::Color tools_text_hover_color;

extern int tools_border_width;
extern sf::Color tools_button_select_color;
extern sf::Color tools_button_idle_color;
extern sf::Color tools_button_hover_color;
extern sf::Color tools_button_press_color;
extern sf::Color tools_button_select_border_color;
extern sf::Color tools_button_idle_border_color;
extern sf::Color tools_button_hover_border_color;
extern sf::Color tools_button_press_border_color;

// Tooltip
extern sf::Color tooltip_rect_color;
extern int tooltip_rect_border_width;
extern sf::Color tooltip_rect_border_color;
extern int tooltip_padding;
extern sf::Color tooltip_text_color;
extern int tooltip_text_font_size;

// normal text
extern sf::Color normal_text_color;
extern int normal_text_font_size;

// Text inputs
extern sf::Color textinput_idle_color;
extern sf::Color textinput_hover_color;
extern sf::Color textinput_textentered_color;
extern sf::Color textInput_incorrect_data_color;
extern sf::Color textInput_border_color;
extern int textInput_border_width;

// Sliders
extern sf::Color slider_bar_idle_color;
extern sf::Color slider_bar_hover_color;
extern sf::Color slider_bar_press_color;
extern sf::Color slider_thumb_idle_color;
extern sf::Color slider_thumb_hover_color;
extern sf::Color slider_thumb_press_color;
extern sf::Color slider_text_color;
extern int slider_font_size;

// canvas
extern sf::Color canvas_color1;
extern sf::Color canvas_color2;

// palette with colors
extern int colors_dialog_margin;

// selection
extern sf::Color selection_color;
extern int selection_border_width;
extern sf::Color selection_border_color;

void loadTheme();