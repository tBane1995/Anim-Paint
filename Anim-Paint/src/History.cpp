#include "History.hpp"
#include "Animation/Frame.hpp"
#include "Animation/Layer.hpp"
#include "DebugLog.hpp"

Step::Step() {
	_animations.clear();
}

Step::~Step() {

}

History::History() {
	_currentStep = -1;
	_steps.clear();
}

History::~History() {

}

void History::clear() {
	_currentStep = -1;
	_steps.clear();
}

void History::saveStep() {

	// if we are not at the end of the history, we need to erase the future steps
	if (_currentStep + 1 < (int)(_steps.size())) {
		_steps.erase(_steps.begin() + (_currentStep + 1), _steps.end());
	}
	
	DebugLog(L"History::saveStep: currentStep=" + std::to_wstring(_currentStep) + L", stepsCount=" + std::to_wstring(_steps.size()));	

	// save the current state of the animations
	std::shared_ptr<Step> step = std::make_shared<Step>();

	for (auto& animation : animations) {
		
		std::shared_ptr<Animation> copied_animation = std::make_shared<Animation>();

		for(auto& frame : animation->_frames) {
			
			std::shared_ptr<Frame> copied_frame = std::make_shared<Frame>();
			
			for (auto& layer : frame->_layers) {
				std::shared_ptr<Layer> copied_layer = std::make_shared<Layer>(layer);
				copied_frame->addLayer(copied_layer);
			}
			copied_animation->addFrame(copied_frame);
		}
		step->_animations.push_back(copied_animation);
	}

	_steps.push_back(step);
	_currentStep++;
}

bool History::canUndo() {
	return _currentStep > 0;
}

bool History::canRedo() {
	return _currentStep < (int)(_steps.size())-1;
}

void History::undo()
{
	if (!canUndo()) return;

	_currentStep-=1;

	std::shared_ptr<Step> step = _steps[_currentStep];

	animations.clear();

	for (auto& srcAnim : step->_animations) {
		std::shared_ptr<Animation> dstAnim = std::make_shared<Animation>();
		for (auto& srcFrame : srcAnim->_frames) {
			std::shared_ptr<Frame> dstFrame = std::make_shared<Frame>();
			for (auto& srcLayer : srcFrame->_layers) {
				std::shared_ptr<Layer> dstLayer = std::make_shared<Layer>(srcLayer);
				dstFrame->addLayer(dstLayer);
			}
			dstAnim->addFrame(dstFrame);
		}
		animations.push_back(dstAnim);
	}
}

void History::redo()
{
	if (!canRedo()) return;

	_currentStep+=1;

	std::shared_ptr<Step> step = _steps[_currentStep];

	animations.clear();

	for (auto& srcAnim : step->_animations) {
		std::shared_ptr<Animation> dstAnim = std::make_shared<Animation>();
		for (auto& srcFrame : srcAnim->_frames) {
			std::shared_ptr<Frame> dstFrame = std::make_shared<Frame>();
			for (auto& srcLayer : srcFrame->_layers) {
				std::shared_ptr<Layer> dstLayer = std::make_shared<Layer>(srcLayer);
				dstFrame->addLayer(dstLayer);
			}
			dstAnim->addFrame(dstFrame);
		}
		animations.push_back(dstAnim);
	}
}

void History::handleEvent(const sf::Event& event) {
	if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Z && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
		undo();
	}

	if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Y && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
		redo();
	}
}

std::shared_ptr<History> history;