#ifndef Dialog_Image_Rotation_hpp
#define Dialog_Image_Rotation_hpp

enum class RotationState { Idle, Edited };

class Dialog_Image_Rotation : public Dialog {
public:

	RotationState state;

	sf::Text rotation_text;
	Slider* rotation_slider;
	NormalButtonWithText* reset;
	NormalButtonWithText* confirm;

	std::vector < Layer* > original_layers;
	std::vector < Layer* > edited_layers;

	Dialog_Image_Rotation(std::vector < Layer* > layers) : Dialog(L"rotation", sf::Vector2f(256,160), sf::Vector2f(8, 120)) {
		
		state = RotationState::Idle;
		rotation_text = sf::Text(L"rotation", basicFont, 13);

		rotation_slider = new Slider(0,359);
		rotation_slider->setValue(0);

		reset = new NormalButtonWithText(L"reset", sf::Vector2f(64, 32));
		reset->onclick_func = [this]() {
			rotation_slider->setValue(0);
			setTheFilter();
			};

		confirm = new NormalButtonWithText(L"confirm", sf::Vector2f(64, 32));
		confirm->onclick_func = [this]() {
			Dialog::state = DialogState::ToClose;
			Dialog_Image_Rotation::state = RotationState::Edited;

			animation->getCurrentFrame()->getLayers().clear();
			animation->getCurrentFrame()->getLayers() = edited_layers;
			layers_dialog->loadLayersFromCurrentFrame();
			};

		original_layers.clear();
		for (auto& l : layers) {
			original_layers.push_back(new Layer(l));
			edited_layers.push_back(new Layer(l));
		}

		setPosition(position);
	}

	~Dialog_Image_Rotation() {
		if (Dialog_Image_Rotation::state == RotationState::Idle) {
			rotation_slider->setValue(0);
			setTheFilter();

			// TO-DO
			animation->getCurrentFrame()->layers.clear();
			animation->getCurrentFrame()->layers = edited_layers;
			//layers_dialog->loadLayersFromCurrentFrame();
		}
		
	}

	void setPosition(sf::Vector2f position) {
		Dialog::setPosition(position);

		sf::Vector2f text_pos;
		text_pos.x = int(position.x) / 8 * 8 + 24;
		text_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
		rotation_text.setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2));

		sf::Vector2f slider_pos;
		slider_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 64 / 2;
		slider_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
		rotation_slider->setPosition(slider_pos);

		sf::Vector2f button_pos;
		button_pos.x = int(position.x) / 8 * 8 +256/2 - 32;
		button_pos.y = int(position.y) / 8 * 8 + 160 - confirm->getSize().y - 16;
		reset->setPosition(button_pos - sf::Vector2f(48,0));
		confirm->setPosition(button_pos + sf::Vector2f(48,0));
	}

	void setTheFilter() {
		for (auto& lr : edited_layers) {
			delete lr;
		}

		edited_layers.clear();

		for (auto& org : original_layers) {
			edited_layers.push_back(new Layer(org));
			set_rotation(edited_layers.back()->image, rotation_slider->getValue(), true);
		}

		animation->getCurrentFrame()->layers.clear();
		animation->getCurrentFrame()->layers = edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}

	void cursorHover() {
		Dialog::cursorHover();

		this->rotation_slider->cursorHover();
		reset->cursorHover();
		confirm->cursorHover();
	}

	void handleEvent(sf::Event& event) {
		Dialog::handleEvent(event);

		this->rotation_slider->handleEvent(event);
		reset->handleEvent(event);
		confirm->handleEvent(event);
	}

	void update() {
		Dialog::update();

		rotation_slider->update();

		if (rotation_slider->state == SliderState::Changed) {
			
			setTheFilter();
		}
		
		reset->update();
		confirm->update();
	}

	void draw() {
		Dialog::draw();

		window->draw(rotation_text);
		rotation_slider->draw();
		reset->draw();
		confirm->draw();
	}

};

#endif