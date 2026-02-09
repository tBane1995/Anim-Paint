#pragma once
#include "SFML/Graphics.hpp"
#include "Animation/Animation.hpp"

class Step {
public:
	std::vector<std::shared_ptr<Animation>> _animations;

	Step();
	~Step();
};

class History {
public:
	int _currentStep;
	std::vector<std::shared_ptr<Step>> _steps;

	History();
	~History();

	void clear();
	void saveStep();

	bool canUndo();
	bool canRedo();

	void undo();
	void redo();

	void handleEvent(const sf::Event& event);

	
};

extern std::shared_ptr<History> history;