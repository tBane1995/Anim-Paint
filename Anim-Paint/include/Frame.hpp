#ifndef Frame_hpp
#define Frame_hpp

class Frame {

public:
	std::vector < Layer* > layers;

	Frame() {
		layers.clear();
		
	}

	~Frame() { }

	std::vector < Layer* >& getLayers() {
		return layers;
	}
};

#endif