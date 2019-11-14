#include <Main.h>
#include <Utility.h>
#include <d3d9types.h>
#include <Core.h>
#include <winuser.h>
#include "DDSTextureLoader.h"

namespace GW2Radial
{

std::string utf8_encode(const std::wstring &wstr)
{
    if( wstr.empty() ) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring utf8_decode(const std::string &str)
{
    if( str.empty() ) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::wstring GetKeyName(unsigned int virtualKey)
{
	unsigned int scanCode = MapVirtualKeyW(virtualKey, MAPVK_VK_TO_VSC);

	switch (virtualKey)
	{
	case VK_LBUTTON:
		return L"M1";
	case VK_RBUTTON:
		return L"M2";
	case VK_MBUTTON:
		return L"M3";
	case VK_XBUTTON1:
		return L"M4";
	case VK_XBUTTON2:
		return L"M5";
	case VK_F13:
		return L"F13";
	case VK_F14:
		return L"F14";
	case VK_F15:
		return L"F15";
	case VK_F16:
		return L"F16";
	case VK_F17:
		return L"F17";
	case VK_F18:
		return L"F18";
	case VK_F19:
		return L"F19";
	case VK_F20:
		return L"F20";
	case VK_F21:
		return L"F21";
	case VK_F22:
		return L"F22";
	case VK_F23:
		return L"F23";
	case VK_F24:
		return L"F24";
	// because MapVirtualKey strips the extended bit for some keys
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
	case VK_PRIOR: case VK_NEXT: // page up and page down
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE: // numpad slash
	case VK_NUMLOCK:
		scanCode |= 0x100; // set extended bit
		break;
	case VK_LCONTROL:
		return L"LCTRL";
	case VK_RCONTROL:
		return L"RCTRL";
	case VK_LSHIFT:
		return L"LSHIFT";
	case VK_RSHIFT:
		return L"RSHIFT";
	case VK_LMENU:
		return L"LALT";
	case VK_RMENU:
		return L"RALT";
	default:
		break;
	}

	wchar_t keyName[50];
	if (GetKeyNameTextW(scanCode << 16, keyName, sizeof(keyName)) != 0)
		return keyName;
	
	return L"[Error]";
}

void SplitFilename(const tstring& str, tstring* folder, tstring* file)
{
	const auto found = str.find_last_of(TEXT("/\\"));
	if (folder) *folder = str.substr(0, found);
	if (file) *file = str.substr(found + 1);
}

mstime TimeInMilliseconds()
{
	mstime iCount;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&iCount));
	mstime iFreq;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&iFreq));
	return 1000 * iCount / iFreq;
}

bool FileExists(const TCHAR* path)
{
	const auto dwAttrib = GetFileAttributes(path);

	return dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool ShaderIsEnd(DWORD token)
{
	return (token & D3DSI_OPCODE_MASK) == D3DSIO_END;
}

int GetShaderFuncLength(const DWORD *pFunction)
{
	int op = 0, l = 1;
	while (!ShaderIsEnd(pFunction[op++]))  l++;
	return l;
}

bool LoadFontResource(UINT resId, void*& dataPtr, size_t& dataSize)
{
	auto res = FindResource(Core::i()->dllModule(), MAKEINTRESOURCE(resId), RT_RCDATA);
	if(res)
	{
		auto handle = LoadResource(Core::i()->dllModule(), res);
		if(handle)
		{
			dataSize = SizeofResource(Core::i()->dllModule(), res);
			dataPtr = LockResource(handle);

			return true;
		}
	}

	return false;
}

IDirect3DTexture9 *
CreateTextureFromResource(
	IDirect3DDevice9 * pDev,
	HMODULE hModule,
	unsigned uResource)
{
	void* dataPtr;
	size_t dataSz;
	   
	
	if (!LoadFontResource(uResource, dataPtr, dataSz))
		return 0;

	IDirect3DBaseTexture9* ret = NULL;

	CreateDDSTextureFromMemory(pDev, dataPtr, dataSz, &ret);

	return (IDirect3DTexture9*)ret;
}

}
