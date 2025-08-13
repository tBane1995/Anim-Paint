#ifndef WorldToTileConverter_hpp
#define WorldToTileConverter_hpp

sf::Vector2i worldToTile(sf::Vector2f p, sf::Vector2f position, sf::Vector2i size, float zoom, float zoom_delta) {
		float scale = zoom * zoom_delta;
		sf::Vector2f local = p - position;
		int tx = std::clamp(int(std::floor(local.x / scale)), 0, size.x);
		int ty = std::clamp(int(std::floor(local.y / scale)), 0, size.y);
		return sf::Vector2i(tx, ty);
}

// odwróc funckje
sf::Vector2f tileToWorld(sf::Vector2i p, sf::Vector2f canvas_position, float zoom, float zoom_delta) {
	return sf::Vector2f(p) * zoom * zoom_delta + canvas_position;
}

#endif