#ifndef Layer_hpp
#define Layer_hpp

class Layer {
public:
	std::wstring name;
	sf::Image image;

	Layer(std::wstring name, sf::Vector2i size) {
		this->name = name;

		image = sf::Image();
		image.create(size.x, size.y, sf::Color::Transparent);
	}

	~Layer() { }
};

#endif