#include <iostream>
#include <string>
#include <functional>
#define NOMINMAX
#include <windows.h>
#include <fcntl.h>
#include <io.h>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"



void loadFrame(sf::Image& image, int x, int y, std::vector<sf::IntRect>& framesRectsList, sf::IntRect& frameRect, sf::Color backgroundColor) {

	if (x < 0 || y < 0 || x >= image.getSize().x || y >= image.getSize().y)
		return;

	if (image.getPixel(sf::Vector2u(x, y)) != backgroundColor) {

		if (x < frameRect.position.x)
			frameRect.position.x = x;

		if (y < frameRect.position.y)
			frameRect.position.y = y;

		if (x >= frameRect.position.x + frameRect.size.x)
			frameRect.size.x = x - frameRect.position.x + 1;

		if (y >= frameRect.position.y + frameRect.size.y)
			frameRect.size.y = y - frameRect.position.y + 1;

		image.setPixel(sf::Vector2u(x, y), backgroundColor);

		loadFrame(image, x + 1, y, framesRectsList, frameRect, backgroundColor);
		loadFrame(image, x - 1, y, framesRectsList, frameRect, backgroundColor);
		loadFrame(image, x, y + 1, framesRectsList, frameRect, backgroundColor);
		loadFrame(image, x, y - 1, framesRectsList, frameRect, backgroundColor);
		loadFrame(image, x + 1, y + 1, framesRectsList, frameRect, backgroundColor);
		loadFrame(image, x - 1, y - 1, framesRectsList, frameRect, backgroundColor);
		loadFrame(image, x + 1, y - 1, framesRectsList, frameRect, backgroundColor);
		loadFrame(image, x - 1, y + 1, framesRectsList, frameRect, backgroundColor);


	}

}

void mergeFrames(std::vector<sf::IntRect>& framesRectsList) {

	int i = 0;
	while (i < framesRectsList.size()) {

		for (int j = 0; j < framesRectsList.size(); j++) {
			if (i == j)
				continue;

			sf::IntRect& rectA = framesRectsList[i];
			sf::IntRect& rectB = framesRectsList[j];
			if (rectA.findIntersection(rectB)) {
				// merge rectB into rectA
				int left = std::min(rectA.position.x, rectB.position.x);
				int top = std::min(rectA.position.y, rectB.position.y);
				int right = std::max(rectA.position.x + rectA.size.x, rectB.position.x + rectB.size.x);
				int bottom = std::max(rectA.position.y + rectA.size.y, rectB.position.y + rectB.size.y);
				rectA.position.x = left;
				rectA.position.y = top;
				rectA.size.x = right - left + 1;
				rectA.size.y = bottom - top + 1;
				// remove rectB
				framesRectsList.erase(framesRectsList.begin() + j);
				if (j < i)
					i--;
				j--;
			}
		}

		i += 1;

	}

}

void deleteSmallFrames(std::vector<sf::IntRect>& framesRectsList) {

	for (int i = 0; i < framesRectsList.size(); i++) {
		sf::IntRect& rect = framesRectsList[i];
		if (rect.size.x < 16 || rect.size.y < 16) {
			framesRectsList.erase(framesRectsList.begin() + i);
			i--;
		}
	}
}

sf::Vector2i getFrameSize(std::vector<sf::IntRect>& framesRectsList) {

	sf::Vector2i frameSize(0, 0);

	for (int i = 0; i < framesRectsList.size(); i++) {

		if (framesRectsList[i].size.x > frameSize.x)
			frameSize.x = framesRectsList[i].size.x;
		if (framesRectsList[i].size.y > frameSize.y)
			frameSize.y = framesRectsList[i].size.y;
	}

	if (frameSize.x < 16)
		frameSize.x = 16;

	if (frameSize.y < 16)
		frameSize.y = 16;

	return frameSize;
}

sf::Vector2i getOffset(sf::Image& image, std::vector<sf::IntRect>& framesRectsList) {
	sf::Vector2i offset(image.getSize().x, image.getSize().y);

	for (int i = 0; i < framesRectsList.size(); i++) {
		if (framesRectsList[i].position.x < offset.x)
			offset.x = framesRectsList[i].position.x;
		if (framesRectsList[i].position.y < offset.y)
			offset.y = framesRectsList[i].position.y;
	}

	return offset;
}

void loadAnimations(sf::Image image) {

	sf::Color backgroundColor = image.getPixel(sf::Vector2u(0, 0));

	std::vector<std::vector<sf::IntRect>> animations;
	std::vector<sf::IntRect> framesRectsList;

	for (int y = 0; y < image.getSize().y; y++) {
		for (int x = 0; x < image.getSize().x; x++) {
			if (image.getPixel(sf::Vector2u(x, y)) != backgroundColor) {
				sf::IntRect frameRect;
				frameRect.position.x = x;
				frameRect.position.y = y;
				frameRect.size.x = 1;
				frameRect.size.y = 1;
				loadFrame(image, x, y, framesRectsList, frameRect, backgroundColor);
				framesRectsList.push_back(frameRect);
			}
		}
	}

	mergeFrames(framesRectsList);
	deleteSmallFrames(framesRectsList);

	sf::Vector2i frameSize = getFrameSize(framesRectsList);
	std::wcout << L"Detected frame size: " + std::to_wstring(frameSize.x) + L"x" + std::to_wstring(frameSize.y) + L"\n";

	sf::Vector2i offset = getOffset(image, framesRectsList);
	std::wcout << L"Detected offset: " + std::to_wstring(offset.x) + L"," + std::to_wstring(offset.y) + L"\n";

}

int main() {
	_setmode(_fileno(stdout), _O_U16TEXT); // wide char UTF-16 output
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({ 800, 600 }), "Anim Loader", sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
	window->setFramerateLimit(60);


	// FPS counter
	sf::Clock fpsClock;
	float fps = 0.0f;
	int frameCount = 0;

	sf::Image spriteSheet;
	spriteSheet.loadFromFile("sprite sheet — kopia.png");
	std::vector<std::vector<sf::IntRect>> animations;
	loadAnimations(spriteSheet);

	while (window->isOpen()) {

		// calculate the FPS
		frameCount++;

		if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
			fps = frameCount / fpsClock.restart().asSeconds();
			frameCount = 0;
			window->setTitle(L"Anim Paint - FPS: " + std::to_wstring(int(fps)));
		}

		// handle events
		while (const std::optional event = window->pollEvent()) {

			if (event->is<sf::Event::Closed>())
				window->close();
		}

		// update

		// render
		window->clear(sf::Color(56, 56, 56));

		window->display();

	}


}