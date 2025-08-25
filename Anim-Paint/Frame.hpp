#ifndef Frame_hpp
#define Frame_hpp

class Frame {
public:

	std::vector < Layer* > layers;

	Frame() {
		layers.clear();
		int i = 64;
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(i, i)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(i, i)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(i, i)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(i, i)));
	}
	~Frame() { }
};

#endif