#ifndef WorldToTileConverter_hpp
#define WorldToTileConverter_hpp

sf::Vector2i worldToTile(sf::Vector2f p, sf::Vector2f canvasPosition, sf::Vector2i size, float zoom, float zoom_delta) {
		float scale = zoom * zoom_delta;
		sf::Vector2f local = p - canvasPosition;
		int tx = std::clamp(int(std::floor(local.x / scale)), 0, size.x-1);
		int ty = std::clamp(int(std::floor(local.y / scale)), 0, size.y-1);

		//std::cout << "tx = " << tx << ", ty = " << ty << "\n";
		return sf::Vector2i(tx, ty);
}

sf::Vector2i selectionToTile(sf::Vector2f p, sf::Vector2f canvasPosition, sf::Vector2i canvas_size, sf::Vector2i selection_size, sf::Vector2i selection_offset, float zoom, float zoom_delta) {
	float scale = zoom * zoom_delta;
	sf::Vector2f local = p - canvasPosition;
	int tx = std::clamp(int(std::floor(local.x / scale)), -selection_size.x + selection_offset.x, canvas_size.x + selection_offset.x);
	int ty = std::clamp(int(std::floor(local.y / scale)), -selection_size.y + selection_offset.y, canvas_size.y + selection_offset.y);

	//std::cout << "tx = " << tx << ", ty = " << ty << "\n";
	return sf::Vector2i(tx, ty);
}

sf::Vector2i worldToTile(sf::Vector2f p, sf::Vector2f canvasPosition, float zoom, float zoom_delta) {
	float scale = zoom * zoom_delta;
	sf::Vector2f local = p - canvasPosition;
	int tx = int(std::floor(local.x / scale));
	int ty = int(std::floor(local.y / scale));

	//std::cout << "tx = " << tx << ", ty = " << ty << "\n";
	return sf::Vector2i(tx, ty);
}

// odwróc funckje
sf::Vector2f tileToWorld(sf::Vector2i p, sf::Vector2f canvas_position, float zoom, float zoom_delta) {
	float scale = zoom * zoom_delta;
	return sf::Vector2f(p) * scale  + canvas_position;
}

#endif