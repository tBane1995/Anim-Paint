#include "History.hpp"
#include "Animation/Frame.hpp"
#include "Animation/Layer.hpp"
#include "DebugLog.hpp"
#include "Components/AnimationsPanel.hpp"
#include "Components/FramesPanel.hpp"
#include "Components/LayersPanel/LayersPanel.hpp"
#include "Components/Canvas.hpp"
#include "Tools/Selection.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Components/Toolbar/Toolbar.hpp"

Step::Step() {
	_animations.clear();
	_canvasResized = false;
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

	step->_currentAnimation = getCurrentAnimationId();
	step->_currentFrame = getCurrentAnimation()->getCurrentFrameID();
	step->_currentLayer = getCurrentAnimation()->getCurrentLayerID();

	if (_currentStep == -1 || canvas->_clickedEdgePoint) {
		step->_canvasResized = true;
		step->_canvasSize = canvas->_size;
	}
	

	_steps.push_back(step);
	_currentStep++;

	main_menu->edit_undo->setActive(canUndo());
	main_menu->edit_redo->setActive(canRedo());
}

bool History::canUndo() {
	return (_currentStep > 0 || (resizable_tool != nullptr && resizable_tool->_state != ResizableToolState::None));
}

bool History::canRedo() {
	return _currentStep < (int)(_steps.size())-1;
}

void History::undo()
{

	if (resizable_tool != nullptr && resizable_tool->_state != ResizableToolState::None) {
		pasteImageWithAlpha(getCurrentAnimation()->getCurrentLayer()->_image, *resizable_tool->_image, resizable_tool->_rect.position.x, resizable_tool->_rect.position.y, sf::Color::Transparent);
		saveStep();
		resizable_tool->reset();
	}
	else if (!canUndo()) return;

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

	currentAnimationId = step->_currentAnimation;
	getCurrentAnimation()->setCurrentFrameID(step->_currentFrame);
	getCurrentAnimation()->setCurrentLayerID(step->_currentLayer);

	animations_panel->updateText();
	frames_panel->updateText();
	layers_panel->loadLayersFromCurrentFrame();

	if (step->_canvasResized) {
		canvas->resize(step->_canvasSize);
		canvas->setCenter();
	}
	
	main_menu->edit_undo->setActive(canUndo());
	main_menu->edit_redo->setActive(canRedo());
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

	currentAnimationId = step->_currentAnimation;
	getCurrentAnimation()->setCurrentFrameID(step->_currentFrame);
	getCurrentAnimation()->setCurrentLayerID(step->_currentLayer);

	animations_panel->updateText();
	frames_panel->updateText();
	layers_panel->loadLayersFromCurrentFrame();

	if (step->_canvasResized) {
		canvas->resize(step->_canvasSize);
		canvas->setCenter();
	}

	main_menu->edit_redo->setActive(canRedo());
	main_menu->edit_undo->setActive(canUndo());

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