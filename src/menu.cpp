#include "menu.h"

#include "api/SKSEMenuFramework.h"
#include "table.h"
#include "gui.h"

#ifdef PlaySound
#	undef PlaySound
#endif

namespace ModexSKSE
{
	static FormEntryTable addItemTable("AddItem", ModexAPI::CacheType::kItem);
	static FormEntryTable actorTable("Actor", ModexAPI::CacheType::kNPC);
	static FormEntryTable objectTable("Object", ModexAPI::CacheType::kObject);
	static FormEntryTable outfitTable("Outfit", ModexAPI::CacheType::kOutfit);
	static FormEntryTable cellTable("Teleport", ModexAPI::CacheType::kCell);

	void Register()
	{
		if (!SKSEMenuFramework::IsInstalled()) {
			SKSE::log::warn("SKSEMenuFramework is not installed, skipping registration");
			return;
		}

		Config::Modex = ModexAPI::GetModexInterface001();
		if (!Config::Modex) {
			SKSE::log::error("Failed to acquire Modex API interface");
			return;
		}

		addItemTable.SetRowCallback([](const ModexAPI::FormEntry& entry) {
			Config::Modex->AddItemToPlayer(entry.formID);
			RE::PlaySound("UIMenuOK");
		});

		addItemTable.SetFormTypeFilters({
				RE::FormType::None,
				RE::FormType::Armor,
				RE::FormType::Weapon,
				RE::FormType::AlchemyItem,
				RE::FormType::Ingredient,
				RE::FormType::Book,
				RE::FormType::KeyMaster,
				RE::FormType::SoulGem,
				RE::FormType::Scroll,
				RE::FormType::Ammo,
				RE::FormType::Misc
		});

		actorTable.SetRowCallback([](const ModexAPI::FormEntry& entry) {
			Config::Modex->PlaceAtMe(entry.formID);
			RE::PlaySound("UIMenuOK");
		});

		objectTable.SetRowCallback([](const ModexAPI::FormEntry& entry) {
			Config::Modex->PlaceAtMe(entry.formID);
			RE::PlaySound("UIMenuOK");
		});

		// 0x14 = player reference
		outfitTable.SetRowCallback([](const ModexAPI::FormEntry& entry) {
			Config::Modex->AddOutfitToInventory(entry.formID, 0x14);
			RE::PlaySound("UIMenuOK");
		});

		cellTable.SetRowCallback([](const ModexAPI::FormEntry& entry) {
			Config::Modex->CenterOnCell(entry.editorID);
			RE::PlaySound("UIMenuOK");
		});

		SKSEMenuFramework::SetSection("Modex");

		// Not sure how problematic it is to do this on event, instead of on register, but register
		// is called too early to load Modex data for configuration and translation files.
		SKSEMenuFramework::AddEvent([](SKSEMenuFramework::Model::EventType eventType) {
			if (eventType == SKSEMenuFramework::Model::EventType::kOpenMenu) {
				Config::Load();
				Translation::Load();

				SKSEMenuFramework::AddSectionItem(T("SETTINGS"), RenderSettings);
				SKSEMenuFramework::AddSectionItem(T("MODULE_ADDITEM"), RenderAddItem);
				SKSEMenuFramework::AddSectionItem(T("MODULE_ACTOR"), RenderActor);
				SKSEMenuFramework::AddSectionItem(T("MODULE_OBJECT"), RenderObject);
				SKSEMenuFramework::AddSectionItem(T("MODULE_OUTFIT"), RenderOutfits);
				SKSEMenuFramework::AddSectionItem(T("MODULE_TELEPORT"), RenderTeleport);

			} else if (eventType == SKSEMenuFramework::Model::EventType::kCloseMenu) {
				Config::Unload();
			}
		}, 0.0f);
	}

	void RenderSettings()
	{
		if (!Config::Modex) {
			ImGuiMCP::TextDisabled(T("ERROR_MODEX_API"));
			return;
		}

		ImGuiMCPComponents::InlineInputInt("Show Menu Key", &Config::user.menuKey);
		ImGuiMCPComponents::InlineInputInt("Show Menu Modifier", &Config::user.menuModifier);
		ImGuiMCPComponents::InlineToggle("Show Splash Logo", Config::user.splashLogo);

		if (Config::user.splashLogo) {
			ImGuiMCPComponents::InlineInt("Logo Scale", &Config::user.logoScale, 25, 200);
		}

		ImGuiMCPComponents::InlineToggle("Fullscreen", Config::user.fullscreen);

		if (!Config::user.fullscreen) {
			ImGuiMCPComponents::InlineInt("UI Scale Horizontal", &Config::user.uiScaleX, 25, 200);
			ImGuiMCPComponents::InlineInt("UI Scale Vertical", &Config::user.uiScaleY, 25, 200);
		}

		ImGuiMCPComponents::InlineToggle("Lock Menu Position", Config::user.lockPosition);
		ImGuiMCPComponents::InlineToggle("Smooth Scroll", Config::user.smoothScroll);
		ImGuiMCPComponents::InlineDropdown("Plugin List Sorting", &Config::user.pluginSort, Config::s_sorts);
		ImGuiMCPComponents::InlineDropdown("Filter Logic", &Config::user.filterLogic, Config::s_logic);
		ImGuiMCPComponents::InlineToggle("Use Base Plugin", Config::user.useBasePlugin);
		ImGuiMCPComponents::InlineToggle("Show Missing Plugins", Config::user.missingPlugins);
		ImGuiMCPComponents::InlineInt("Max Action Query", &Config::user.maxActions, 10, 2500);
		ImGuiMCPComponents::InlineToggle("Pause Game While Open", Config::user.pauseGame);
		ImGuiMCPComponents::InlineDropdown("Log Level", &Config::user.logLevel, Config::s_levels);
		ImGuiMCPComponents::InlineDropdown("Global Font", &Config::user.font, Config::s_fonts);
		ImGuiMCPComponents::InlineInt("Global Font Size", &Config::user.fontSize, 8, 48);
		ImGuiMCPComponents::InlineDropdown("Language", &Config::user.language, Config::s_languages);
		ImGuiMCPComponents::InlineToggle("Developer Mode", Config::user.developerMode);

		ImGuiMCP::ImVec2 avail;
		ImGuiMCP::GetContentRegionAvail(&avail);
		if (ImGuiMCP::Button(T("Open"), ImGuiMCP::ImVec2(avail.x, 0.f))) {
			Config::Modex->OpenMenu();
		}
	}

	void RenderAddItem()
	{
		if (!Config::Modex) {
			ImGuiMCP::TextDisabled(T("ERROR_MODEX_API"));
			return;
		}

		addItemTable.Draw();
	}

	void RenderTeleport()
	{
		if (!Config::Modex) {
			ImGuiMCP::TextDisabled(T("ERROR_MODEX_API"));
			return;
		}

		cellTable.Draw();
	}

	void RenderActor()
	{
		if (!Config::Modex) {
			ImGuiMCP::TextDisabled(T("ERROR_MODEX_API"));
			return;
		}

		actorTable.Draw();
	}

	void RenderObject()
	{
		if (!Config::Modex) {
			ImGuiMCP::TextDisabled(T("ERROR_MODEX_API"));
			return;
		}

		objectTable.Draw();
	}

	void RenderOutfits()
	{
		if (!Config::Modex) {
			ImGuiMCP::TextDisabled(T("ERROR_MODEX_API"));
			return;
		}

		outfitTable.Draw();
	}
}
