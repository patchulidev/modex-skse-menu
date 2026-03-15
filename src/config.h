#pragma once

#include "api/ModexAPI.h"

namespace Config
{
	extern ModexAPI::IModexInterface001* Modex;

	struct UserSettings
	{
		bool developerMode = false;
		bool altKeyShortcut = false;
		bool fullscreen = false;
		bool splashLogo = false;
		bool useBasePlugin = false;
		bool missingPlugins = true;
		bool smoothScroll = true;
		bool pauseGame = true;
		bool lockPosition = true;

		int logLevel = 2;
		int filterLogic = 0;
		int fontSize = 16;
		int maxActions = 3000;
		int menuKey = 211;
		int menuModifier = 0;
		int pluginSort = 0;
		int uiScaleX = 100;
		int uiScaleY = 100;
		int logoScale = 100;

		std::string theme = "default";
		std::string language = "English";
		std::string font = "Ubuntu-Regular";
	};

	extern UserSettings user;

	extern const std::vector<std::string> s_sorts;
	extern const std::vector<std::string> s_logic;
	extern const std::vector<std::string> s_levels;
	extern std::vector<std::string> s_languages;
	extern std::vector<std::string> s_fonts;

	std::filesystem::path GetPath();
	bool Load();
	void Unload();
	bool Save();
	std::vector<std::string> GetAvailableFonts();
	std::vector<std::string> GetLanguageFiles();
}
