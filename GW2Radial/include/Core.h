#pragma once

#include <Main.h>
#include <Singleton.h>
#include <Wheel.h>
#include <UnitQuad.h>
#include <queue>

#define _CRT_SECURE_NO_WARNINGS

namespace GW2Radial
{

class Core : public Singleton<Core>
{
public:
	static void Init(HMODULE dll);
	static void Shutdown();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Core() = default;
	~Core();

	HWND gameWindow() const { return gameWindow_; }
	HMODULE dllModule() const { return dllModule_; }
	WNDPROC baseWndProc() const { return baseWndProc_; }
	uint screenWidth() const { return screenWidth_; }
	uint screenHeight() const { return screenHeight_; }
	ImFont* font() const { return font_; }
	ImFont* fontBlack() const { return fontBlack_; }
	ImFont* fontItalic() const { return fontItalic_; }

	void DrawTextDatas();
	void InsertTextData(wchar_t* val);

protected:
	std::queue<wchar_t*> textDataMap;
	DWORD lastRemTime = 0;
	CRITICAL_SECTION cs;

	void InternalInit();
	void OnFocusLost();

	void OnDeviceSet(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentationParameters);
	void OnDeviceUnset();

	void PreCreateDevice(HWND hFocusWindow);
	void PostCreateDevice(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentationParameters);

	void PreReset();
	void PostReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS *presentationParameters);
	
	void DrawUnder(IDirect3DDevice9* device, bool frameDrawn, bool sceneEnded);
	void DrawOver(IDirect3DDevice9* device, bool frameDrawn, bool sceneEnded);

	HWND gameWindow_ = nullptr;
	HMODULE dllModule_ = nullptr;
	WNDPROC baseWndProc_ = nullptr;

	uint screenWidth_ = 0, screenHeight_ = 0;

	ImFont *font_ = nullptr, *fontBlack_ = nullptr, *fontItalic_ = nullptr;

	ImGuiContext* imguiContext_ = nullptr;
};
}
