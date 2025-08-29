#ifndef dialog_Image_Brightness_Contrast_hpp
#define dialog_Image_Brightness_Contrast_hpp

enum class BrightnessContrastState { Idle, Edited };

class Dialog_Image_Brightness_Contrast : public Dialog {
public:
	BrightnessContrastState state;

	sf::Text brightness_text;
	sf::Text contrast_text;
	
	Slider* brightness_slider;
	Slider* contrast_slider;

	NormalButtonWithText* reset;
	NormalButtonWithText* confirm;

	std::vector < Layer* > original_layers;
	std::vector < Layer* > edited_layers;

	

	Dialog_Image_Brightness_Contrast(std::vector < Layer* > layers) : Dialog(L"brightness-contrast", sf::Vector2f(256,160), sf::Vector2f(8, 120)) {
		
		state = BrightnessContrastState::Idle;

		brightness_text = sf::Text(L"brightness", basicFont, 13);
		contrast_text = sf::Text(L"contrast", basicFont, 13);

		brightness_slider = new Slider(-50,50);
		brightness_slider->setValue(0);

		contrast_slider = new Slider(-50,50);
		contrast_slider->setValue(0);

		reset = new NormalButtonWithText(L"reset", sf::Vector2f(64, 32));
		reset->onclick_func = [this]() {
			brightness_slider->setValue(0);
			contrast_slider->setValue(0);
			setTheFilter();
			};

		confirm = new NormalButtonWithText(L"confirm", sf::Vector2f(64, 32));
		confirm->onclick_func = [this]() {
			Dialog_Image_Brightness_Contrast::state = BrightnessContrastState::Edited;
			Dialog::state = DialogState::ToClose;

			frames_dialog->getCurrentFrame()->layers.clear();
			frames_dialog->getCurrentFrame()->layers = edited_layers;
			layers_dialog->loadLayersFromCurrentFrame();
			};


		original_layers.clear();
		edited_layers.clear();
		for (auto& l : layers) {
			original_layers.push_back(new Layer(l));
			edited_layers.push_back(new Layer(l));
		}

		setPosition(position);
	}

	~Dialog_Image_Brightness_Contrast() {

		if (Dialog_Image_Brightness_Contrast::state == BrightnessContrastState::Idle) {
			brightness_slider->setValue(0);
			contrast_slider->setValue(0);
			setTheFilter();

			frames_dialog->getCurrentFrame()->layers.clear();
			frames_dialog->getCurrentFrame()->layers = edited_layers;
			layers_dialog->loadLayersFromCurrentFrame();
		}
	}

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);

		sf::Vector2f text_pos;
		text_pos.x = int(position.x) / 8 * 8 + 24;
		text_pos.y = int(position.y) / 8 * 8 + 160 / 2 - 28;

		brightness_text.setPosition(text_pos + sf::Vector2f(0,2-basicFont.getLineSpacing(13)/2));
		contrast_text.setPosition(text_pos + sf::Vector2f(0, 2-basicFont.getLineSpacing(13)/2) + sf::Vector2f(0,32));

		sf::Vector2f slider_pos;
		slider_pos.x = int(position.x)/8*8 + 256 / 2 - 64 / 2;
		slider_pos.y = int(position.y)/8*8 + 160 / 2 - 28;
		brightness_slider->setPosition(slider_pos);
		contrast_slider->setPosition(slider_pos + sf::Vector2f(0,32));

		sf::Vector2f button_pos;
		button_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 32;
		button_pos.y = int(position.y) / 8 * 8 + 160 - confirm->getSize().y - 16;
		reset->setPosition(button_pos - sf::Vector2f(48, 0));
		confirm->setPosition(button_pos + sf::Vector2f(48, 0));
	}

	void setTheFilter() {
		for (auto& lr : edited_layers) {
			delete lr;
		}

		edited_layers.clear();

		for (auto& org : original_layers) {
			edited_layers.push_back(new Layer(org));
			set_brightness(edited_layers.back()->image, float(brightness_slider->getValue()) / 100.0f);
			set_contrast(edited_layers.back()->image, float(contrast_slider->getValue()) / 100.0f);
		}

		frames_dialog->getCurrentFrame()->layers.clear();
		frames_dialog->getCurrentFrame()->layers = edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}

	void cursorHover() {
		Dialog::cursorHover();

		brightness_slider->cursorHover();
		contrast_slider->cursorHover();

		reset->cursorHover();
		confirm->cursorHover();
	}

	void handleEvent(sf::Event& event) {
		Dialog::handleEvent(event);

		brightness_slider->handleEvent(event);
		contrast_slider->handleEvent(event);

		reset->handleEvent(event);
		confirm->handleEvent(event);
	}

	void update() {
		Dialog::update();

		brightness_slider->update();
		contrast_slider->update();

		if (brightness_slider->state == SliderState::Changed || contrast_slider->state == SliderState::Changed) {
			setTheFilter();
		}

		reset->update();
		confirm->update();
		
	}

	void draw() {
		Dialog::draw();

		window->draw(brightness_text);
		window->draw(contrast_text);

		brightness_slider->draw();
		contrast_slider->draw();

		reset->draw();
		confirm->draw();
	}

};

#endif