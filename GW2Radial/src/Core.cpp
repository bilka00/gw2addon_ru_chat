#include <Core.h>
#include <Direct3D9Hooks.h>
#include <imgui.h>
#include <examples/imgui_impl_dx9.h>
#include <examples/imgui_impl_win32.h>
#include <Input.h>
#include <ConfigurationFile.h>
#include <UnitQuad.h>
#include <Wheel.h>
#include <Mount.h>
#include <SettingsMenu.h>
#include <Utility.h>
#include <imgui/imgui_internal.h>
#include <Novelty.h>
#include <shellapi.h>
#include <UpdateCheck.h>
#include <ImGuiPopup.h>
#include <Marker.h>
#include <MiscTab.h>
#include <MumbleLink.h>
#include <Effect_dx12.h>
#include <iostream>
#include <string>
#include <regex>

std::wstring LastString;

namespace GW2Radial
{
DEFINE_SINGLETON(Core);

void Core::Init(HMODULE dll)
{	
	i()->dllModule_ = dll;
	i()->InternalInit();
}

void Core::Shutdown()
{
	i_.reset();

	// We'll just leak a bunch of things and let the driver/OS take care of it, since we have no clean exit point
	// and calling FreeLibrary in DllMain causes deadlocks
}

Core::~Core()
{
	ImGui::DestroyContext();

	if(auto i = Direct3D9Hooks::iNoInit(); i != nullptr)
	{
		i->preCreateDeviceCallback(nullptr);
		i->postCreateDeviceCallback(nullptr);
		
		i->preResetCallback(nullptr);
		i->postResetCallback(nullptr);
		
		i->drawOverCallback(nullptr);
		i->drawUnderCallback(nullptr);
	}
}

std::string WstrToUtf8Str(const std::wstring& wstr)
{
	std::string retStr = "err";
	if (!wstr.empty())
	{
		int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

		if (sizeRequired > 0)
		{
			std::vector<char> utf8String(sizeRequired);
			int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
				-1, &utf8String[0], utf8String.size(), NULL,
				NULL);
			if (bytesConverted != 0)
			{
				retStr = &utf8String[0];
			}
			else
			{
				
			}
		}
	}
	return retStr;
}

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = 0; j < i; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

std::wstring FilterShit(const std::wstring base)
{
	//const std::wstring base(L"<quote>[&CGFMngACJRHqgbaVEDwNZJJNAGkAeQB1AGsAaQBuAHkAYQAAAA==]: ≈бучее гв 2");
	//OutputDebugString(TEXT("FilterShit touch"));
	std::wregex rgx(L".*quote>\\[&(.*)\\]: (.*)$");
	std::wsmatch match;
	std::wstring text;
	std::wstring b64name;
	if (std::regex_search(base.begin(), base.end(), match, rgx)) {
		b64name = match[1];
		text = match[2];
	}
	std::string decode = base64_decode(std::string(b64name.begin(), b64name.end()));
	wchar_t username[255];
	memset(username, 0, 255);
	if (decode.length()>17)
		memcpy(username, decode.c_str() + 17, decode.length() - 17);

	if ((std::wstring(username).length()) == 0) {
		return std::wstring(L"NONE");
	}
	if ((std::wstring(text).length()) == 0) {
		return std::wstring(L"NONE");
	}
	return std::wstring(username) + L": " + text;
}

void Core::DrawTextDatas()
{
	int cnt = 0;

	EnterCriticalSection(&cs);

	char buf[2048];

	ImGui::Begin("ru chat");

	{
		auto tmp = textDataMap;

		while (!tmp.empty())
		{
			wchar_t* txt = tmp.front();
			if (LastString != std::wstring(txt)) {
				std::wstring txt2 = FilterShit(std::wstring(txt));
				
				if (txt2 != std::wstring(L"NONE"))
				{
					std::string str = WstrToUtf8Str(txt2);	

					ImGui::Text("%s", str.c_str());

					LastString = std::wstring(txt);
				}
			}
				
			tmp.pop();
			++cnt;

		}
	}

	ImGui::End();

	if (cnt > 10)
	{
		if ((GetTickCount() - lastRemTime) > 1000)
		{
			lastRemTime = GetTickCount();
			free(textDataMap.front());
			textDataMap.pop();
		}
	}

	LeaveCriticalSection(&cs);
}

void Core::InsertTextData(wchar_t * val)
{
	EnterCriticalSection(&cs);

	textDataMap.push(_wcsdup(val));

	LeaveCriticalSection(&cs);
}

void Core::InternalInit()
{
	// Add an extra reference count to the library so it persists through GW2's load-unload routine
	// without which problems start arising with ReShade
	{
		TCHAR selfpath[MAX_PATH];
		GetModuleFileName(dllModule_, selfpath, MAX_PATH);
		LoadLibrary(selfpath);
	}
		
	InitializeCriticalSection(&cs);

	Direct3D9Hooks::i()->preCreateDeviceCallback([this](HWND hWnd){ PreCreateDevice(hWnd); });
	Direct3D9Hooks::i()->postCreateDeviceCallback([this](IDirect3DDevice9* d, D3DPRESENT_PARAMETERS* pp){ PostCreateDevice(d, pp); });
	
	Direct3D9Hooks::i()->preResetCallback([this](){ PreReset(); });
	Direct3D9Hooks::i()->postResetCallback([this](IDirect3DDevice9* d, D3DPRESENT_PARAMETERS* pp){ PostReset(d, pp); });
	
	Direct3D9Hooks::i()->drawOverCallback([this](IDirect3DDevice9* d, bool frameDrawn, bool sceneEnded){ DrawOver(d, frameDrawn, sceneEnded); });
	Direct3D9Hooks::i()->drawUnderCallback([this](IDirect3DDevice9* d, bool frameDrawn, bool sceneEnded){ DrawUnder(d, frameDrawn, sceneEnded); });
	
	imguiContext_ = ImGui::CreateContext();
}

void Core::OnFocusLost()
{
	Input::i()->OnFocusLost();
}

LRESULT Core::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KILLFOCUS)
		i()->OnFocusLost();
	else if(Input::i()->OnInput(msg, wParam, lParam))
		return 0;

	// Whatever's left should be sent to the game
	return CallWindowProc(i()->baseWndProc_, hWnd, msg, wParam, lParam);
}

void Core::PreCreateDevice(HWND hFocusWindow)
{
	gameWindow_ = hFocusWindow;

	// Hook WndProc
	if (!baseWndProc_)
	{
		baseWndProc_ = WNDPROC(GetWindowLongPtr(hFocusWindow, GWLP_WNDPROC));
		SetWindowLongPtr(hFocusWindow, GWLP_WNDPROC, LONG_PTR(&WndProc));
	}
}

void Core::PostCreateDevice(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentationParameters)
{
	// Init ImGui
	auto &imio = ImGui::GetIO();
	imio.IniFilename = nullptr;
	imio.IniSavingRate = 1.0f;
	auto fontCfg = ImFontConfig();
	fontCfg.FontDataOwnedByAtlas = false;
	
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x044F, // Cyrillic
		0,
	};
	   
	void *fontPtr, *fontBlackPtr, *fontItalicPtr;
	size_t fontSize, fontBlackSize, fontItalicSize;	
	if(LoadFontResource(IDR_FONT, fontPtr, fontSize))
		font_ = imio.Fonts->AddFontFromMemoryTTF(fontPtr, int(fontSize), 25.f, &fontCfg, ranges);
	if(LoadFontResource(IDR_FONT_BLACK, fontBlackPtr, fontBlackSize))
		fontBlack_ = imio.Fonts->AddFontFromMemoryTTF(fontBlackPtr, int(fontBlackSize), 35.f, &fontCfg, ranges);
	if(LoadFontResource(IDR_FONT_ITALIC, fontItalicPtr, fontItalicSize))
		fontItalic_ = imio.Fonts->AddFontFromMemoryTTF(fontItalicPtr, int(fontItalicSize), 25.f, &fontCfg, ranges);

	if(font_)
		imio.FontDefault = font_;

	ImGui_ImplWin32_Init(gameWindow_);

	OnDeviceSet(device, presentationParameters);
}

void Core::OnDeviceSet(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *presentationParameters)
{
	// Initialize graphics
	ImGui_ImplDX9_Init(device);
}

void Core::OnDeviceUnset()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Core::PreReset()
{
	OnDeviceUnset();
}

void Core::PostReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS *presentationParameters)
{
	OnDeviceSet(device, presentationParameters);
}

void Core::DrawUnder(IDirect3DDevice9* device, bool frameDrawn, bool sceneEnded)
{

}

void Core::DrawOver(IDirect3DDevice9* device, bool frameDrawn, bool sceneEnded)
{
	// This is the closest we have to a reliable "update" function, so use it as one
	Input::i()->OnUpdate();

	{
		// We have to use Present rather than hooking EndScene because the game seems to do final UI compositing after EndScene
		// This unfortunately means that we have to call Begin/EndScene before Present so we can render things, but thankfully for modern GPUs that doesn't cause bugs
		if (sceneEnded)
			device->BeginScene();

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		////
		DrawTextDatas();

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());	

		if (sceneEnded)
			device->EndScene();
	}
}

}
