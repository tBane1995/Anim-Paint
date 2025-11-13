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

sf::Vector2i selectionToTile(sf::Vector2i p, sf::Vector2i canvasPosition, sf::Vector2i canvas_size, sf::Vector2i selection_size, sf::Vector2i selection_offset, float zoom, float zoom_delta) {
	float scale = zoom * zoom_delta;
	sf::Vector2f local = sf::Vector2f(p - canvasPosition);
	int tx = std::clamp(int(std::floor(local.x / scale)), -selection_size.x + selection_offset.x, canvas_size.x + selection_offset.x);
	int ty = std::clamp(int(std::floor(local.y / scale)), -selection_size.y + selection_offset.y, canvas_size.y + selection_offset.y);

	//std::cout << "tx = " << tx << ", ty = " << ty << "\n";
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
