#pragma once

#include <Main.h>
#include <WheelElement.h>
#include <ConfigurationOption.h>
#include <SettingsMenu.h>

#include <Input.h>

namespace GW2Radial
{

class Wheel : public SettingsMenu::Implementer
{
public:
	enum class CenterBehavior : int
	{
		NOTHING = 0,
		PREVIOUS = 1,
		FAVORITE = 2
	};
	enum class BehaviorBeforeDelay : int
	{
		NOTHING = 0,
		PREVIOUS = 1,
		FAVORITE = 2,
		DIRECTION = 3
	};

	Wheel(uint bgResourceId, uint inkResourceId, std::string nickname, std::string displayName, IDirect3DDevice9* dev);
	virtual ~Wheel();

	template<typename T>
	static std::unique_ptr<Wheel> Create(uint bgResourceId, uint inkResourceId, std::string nickname, std::string displayName, IDirect3DDevice9* dev)
	{
		// TODO: Would be nice to somehow let wheel element .cpps determine these parameters as well
		auto wheel = std::make_unique<Wheel>(bgResourceId, inkResourceId, std::move(nickname), std::move(displayName), dev);
		wheel->Setup<T>(dev);
		return std::move(wheel);
	}

	template<typename T>
	void Setup(IDirect3DDevice9* dev); // Requires implementation for each wheel element type

	void UpdateHover();
	void AddElement(std::unique_ptr<WheelElement>&& we) { wheelElements_.push_back(std::move(we)); Sort(); }
	void Draw(IDirect3DDevice9* dev, Effect* fx, class UnitQuad* quad);
	void OnFocusLost();

	bool drawOverUI() const { return showOverGameUIOption_.value(); }

	void SetAlphaBlended(bool enabled) { alphaBlended_ = enabled; }
	void SetResetCursorPositionBeforeKeyPress(bool enabled) { resetCursorPositionBeforeKeyPress_ = enabled; }

protected:
	void Sort();
	WheelElement* GetCenterHoveredElement();
	WheelElement* GetFavorite(int favoriteId);
	std::vector<WheelElement*> GetActiveElements();
	bool OnMouseMove();
	InputResponse OnInputChange(bool changed, const std::set<uint>& keys, const std::list<EventKey>& changedKeys);
	void ActivateWheel(bool isMountOverlayLocked);
	void DeactivateWheel();

	std::string nickname_, displayName_;
	bool alphaBlended_ = false;
	bool resetCursorPositionBeforeKeyPress_ = false;
	bool resetCursorPositionToCenter_ = false;

	std::vector<std::unique_ptr<WheelElement>> wheelElements_;
	bool isVisible_ = false;
	uint minElementSortingPriority_ = 0;
	Keybind keybind_, centralKeybind_;

	ConfigurationOption<int> centerBehaviorOption_;
	ConfigurationOption<int> centerFavoriteOption_;
	ConfigurationOption<int> delayFavoriteOption_;
	
	ConfigurationOption<float> scaleOption_;
	ConfigurationOption<float> centerScaleOption_;
	
	ConfigurationOption<int> displayDelayOption_;
	ConfigurationOption<int> animationTimeOption_;
	
	ConfigurationOption<bool> resetCursorOnLockedKeybindOption_;
	ConfigurationOption<bool> lockCameraWhenOverlayedOption_;
	ConfigurationOption<bool> showOverGameUIOption_;
	ConfigurationOption<bool> noHoldOption_;
	ConfigurationOption<int> behaviorOnReleaseBeforeDelay_;

	std::optional<Point> cursorResetPosition_;
	fVector2 currentPosition_;
	mstime currentTriggerTime_ = 0;

	WheelElement* currentHovered_ = nullptr;
	WheelElement* previousUsed_ = nullptr;
	
	IDirect3DTexture9* backgroundTexture_ = nullptr;
	IDirect3DTexture9* inkTexture_ = nullptr;
	
	Input::MouseMoveCallback mouseMoveCallback_;
	Input::InputChangeCallback inputChangeCallback_;

	fVector3 inkSpot_;

	const char* GetTabName() const override { return displayName_.c_str(); }
	void DrawMenu() override;

	friend class WheelElement;
};

}
