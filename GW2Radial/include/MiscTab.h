#pragma once
#include <SettingsMenu.h>
#include <Singleton.h>

namespace GW2Radial
{

class MiscTab : public SettingsMenu::Implementer, public Singleton<MiscTab>
{
public:
	MiscTab();
	~MiscTab();

	const char * GetTabName() const override { return "Misc"; }
	void DrawMenu() override;

	uint vk_ = 0;
};

}