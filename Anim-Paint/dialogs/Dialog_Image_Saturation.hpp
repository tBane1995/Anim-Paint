#ifndef Dialog_Image_Saturation_hpp
#define Dialog_Image_Saturation_hpp

class Dialog_Image_Saturation : public Dialog {
public:
	Slider* saturation_slider;

	std::vector < Layer* > original_layers;
	std::vector < Layer* > edited_layers;

	Dialog_Image_Saturation(std::vector < Layer* > layers) : Dialog(L"saturation", sf::Vector2f(256,160), sf::Vector2f(8, 120)) {
		saturation_slider = new Slider(0,200);
		saturation_slider->setValue(100);

		original_layers.clear();
		for (auto& l : layers) {
			original_layers.push_back(new Layer(l));
			edited_layers.push_back(new Layer(l));
		}

		setPosition(position);
	}

	~Dialog_Image_Saturation() {
		for (auto& o : original_layers) {
			delete o;
		}

		frames_dialog->getCurrentFrame()->layers.clear();
		frames_dialog->getCurrentFrame()->layers = edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);

		sf::Vector2f slider_pos;
		slider_pos.x = int(position.x)/8*8 + 256 / 2 - 128 / 2;
		slider_pos.y = int(position.y)/8*8 + dialog_title_rect_height/2 + (160)/2 - 4;
		saturation_slider->setPosition(slider_pos);
	}

	void cursorHover() {
		Dialog::cursorHover();

		this->saturation_slider->cursorHover();
	}

	void handleEvent(sf::Event& event) {
		Dialog::handleEvent(event);

		this->saturation_slider->handleEvent(event);
	}

	void update() {
		Dialog::update();

		this->saturation_slider->update();

		if (saturation_slider->state == SliderState::Changed) {
			
			for (auto& lr : edited_layers) {
				delete lr;
			}

			edited_layers.clear();

			for (auto& org : original_layers) {
				edited_layers.push_back(new Layer(org));
				set_saturation(edited_layers.back()->image, float(saturation_slider->getValue()) / 100.0f);
			}

			frames_dialog->getCurrentFrame()->layers.clear();
			frames_dialog->getCurrentFrame()->layers = edited_layers;
			layers_dialog->loadLayersFromCurrentFrame();
		}
		
	}

	void draw() {
		Dialog::draw();

		this->saturation_slider->draw();
	}

};

#endif