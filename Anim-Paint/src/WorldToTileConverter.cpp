#include "WorldToTileConverter.hpp"
#include <iostream>

sf::Vector2i worldToTile(sf::Vector2i p, sf::Vector2i canvasPosition, sf::Vector2i size, float zoom, float zoom_delta) {
		float scale = zoom * zoom_delta;
		sf::Vector2f local = sf::Vector2f(p - canvasPosition);
		int tx = std::clamp(int(std::floor(local.x / scale)), 0, size.x-1);
		int ty = std::clamp(int(std::floor(local.y / scale)), 0, size.y-1);

		//std::wcout << L"tx = " << tx << L", ty = " << ty << L"\n";
		return sf::Vector2i(tx, ty);
}

sf::Vector2i worldToTile(sf::Vector2i p, sf::Vector2i canvasPosition, float zoom, float zoom_delta) {
	float scale = zoom * zoom_delta;
	sf::Vector2f local = sf::Vector2f(p - canvasPosition);
	int tx = int(std::floor(local.x / scale));
	int ty = int(std::floor(local.y / scale));

	//std::cout << "tx = " << tx << ", ty = " << ty << "\n";
	return sf::Vector2i(tx, ty);
}

sf::Vector2i tileToWorld(sf::Vector2i p, sf::Vector2i canvas_position, float zoom, float zoom_delta) {
	float scale = zoom * zoom_delta;
	return sf::Vector2i(sf::Vector2f(p) * scale) + canvas_position;
}
