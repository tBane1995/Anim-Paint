#ifndef Frame_hpp
#define Frame_hpp

class Frame {
public:

	std::vector < Layer* > layers;

	Frame() {
		layers.clear();
		std::wstring name = L"Layer " + std::to_wstring(layers.size());
		layers.push_back(new Layer(name, sf::Vector2i(16, 16)));
	}
	~Frame() { }
};

#endif