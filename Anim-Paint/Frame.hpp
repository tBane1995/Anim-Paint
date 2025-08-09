#ifndef Frame_hpp
#define Frame_hpp

class Frame {
public:

	std::vector < Layer* > layers;

	Frame() {
		layers.clear();
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(32, 32)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(32, 32)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(32, 32)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(32, 32)));
	}
	~Frame() { }
};

#endif