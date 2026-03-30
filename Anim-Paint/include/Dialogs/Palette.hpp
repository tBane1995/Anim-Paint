#pragma once
#include "Dialogs/Dialog.hpp"
#include "Controls/NumberInput.hpp"

struct HSV { float h, s, v, a; }; // zakresy 0..1
HSV rgbToHsv(sf::Color c);
sf::Vector2i cursorOnHues(sf::Vector2i rectSize, sf::Color rgba);
sf::Vector2i cursorOnValues(sf::Vector2i rectSize, sf::Color rgba);
sf::Vector2i cursorOnAlpha(sf::Vector2i rectSize, sf::Color rgba);

enum class PaletteState { None, Selecting };

class PaletteValues : public Element {
public:

	sf::Shader _shader;
	sf::IntRect _rect;
	sf::RenderTexture _renderTexture;
	PaletteState _state;
	std::function<void()> _function;

	PaletteValues(sf::Vector2i position, sf::Vector2i size, std::string shader, sf::Color color);
	~PaletteValues();

	void loadTexture(sf::Color color);
	void setPosition(sf::Vector2i position);
	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};


class Palette : public Dialog {
public:
	std::shared_ptr<PaletteValues> _hues;
	std::shared_ptr<PaletteValues> _values;
	std::shared_ptr<PaletteValues> _alphaValues;
	sf::Vector2i _huesCursor;
	sf::Vector2i _valuesCursor;
	sf::Vector2i _alphaCursor;

	std::unique_ptr<sf::Text> _r;
	std::unique_ptr<sf::Text> _g;
	std::unique_ptr<sf::Text> _b;
	std::unique_ptr<sf::Text> _a;

	std::shared_ptr<NumberInput> _red;
	std::shared_ptr<NumberInput> _green;
	std::shared_ptr<NumberInput> _blue;
	std::shared_ptr<NumberInput> _alpha;

	Palette();
	~Palette();

	void setColorInRGBInputs(sf::Color color);
	void loadColorFromRGBInputs();
	void setTheColor();
	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

extern std::shared_ptr<Palette> palette;