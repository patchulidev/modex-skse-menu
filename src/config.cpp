#include "config.h"

namespace Config
{
	using json = nlohmann::json;

	ModexAPI::IModexInterface001* Modex = nullptr;
	UserSettings user;

	static json settings;

	std::vector<std::string> s_languages;
	std::vector<std::string> s_fonts;

	const std::vector<std::string> s_sorts = {
		"Alphabetical",
		"Load Order Ascending",
		"Load Order Descending"
	};

	const std::vector<std::string> s_logic = {
		"OR",
		"AND"
	};

	const std::vector<std::string> s_levels = {
		"trace",
		"debug",
		"info",
		"warn",
		"error",
		"critical"
	};

	// clang-format off
	template <typename T>
	struct Setting
	{
		const char* key;
		T UserSettings::* member;
		T defaultValue;
	};

	static const std::vector<Setting<bool>> boolSettings = {
		{ "Developer Mode",              &UserSettings::developerMode,  false },
		{ "Disable Alt Key Shortcut",    &UserSettings::altKeyShortcut, false },
		{ "Fullscreen",                  &UserSettings::fullscreen,     false },
		{ "Lock Menu Position",          &UserSettings::lockPosition,   true  },
		{ "Pause Game While Open",       &UserSettings::pauseGame,      true  },
		{ "Show Missing Plugins",        &UserSettings::missingPlugins, true  },
		{ "Show Splash Logo",            &UserSettings::splashLogo,     false },
		{ "Smooth Scroll",               &UserSettings::smoothScroll,   true  },
		{ "Use Base Plugin",             &UserSettings::useBasePlugin,  false },
	};

	static const std::vector<Setting<int>> intSettings = {
		{ "Filter Logic",                &UserSettings::filterLogic,    0    },
		{ "Global Font Size",            &UserSettings::fontSize,       16   },
		{ "Log Level",                   &UserSettings::logLevel,       2    },
		{ "Logo Scale",                  &UserSettings::logoScale,      100  },
		{ "Max Action Query",            &UserSettings::maxActions,     3000 },
		{ "Open Menu Keybind",           &UserSettings::menuKey,        211  },
		{ "Open Menu Modifier",          &UserSettings::menuModifier,   0    },
		{ "Plugin List Sorting",         &UserSettings::pluginSort,     0    },
		{ "UI Scale Horizontal",         &UserSettings::uiScaleX,      100  },
		{ "UI Scale Vertical",           &UserSettings::uiScaleY,      100  },
	};

	static const std::vector<Setting<std::string>> stringSettings = {
		{ "Global Font",                 &UserSettings::font,           "Ubuntu-Regular" },
		{ "Language",                    &UserSettings::language,       "English"        },
		{ "Modex Theme",                 &UserSettings::theme,          "default"        },
	};
	// clang-format on

	template <typename T>
	static T GetSetting(const std::string& a_key, const T& a_default)
	{
		if (settings.contains(a_key)) {
			try {
				return settings[a_key].get<T>();
			} catch (const json::type_error&) {
				return a_default;
			}
		}

		return a_default;
	}

	template <typename T>
	static void SetSetting(const std::string& a_key, const T& a_value)
	{
		settings[a_key] = a_value;
	}

	std::filesystem::path GetPath()
	{
		return std::filesystem::path("data") / "interface" / "modex" / "user" / "settings.json";
	}

	std::vector<std::string> GetAvailableFonts()
	{
		const std::filesystem::path IMGUI_FONT_DIR = std::filesystem::path("data") / "interface" / "imguiicons" / "fonts";
		const std::filesystem::path MODEX_FONT_DIR = std::filesystem::path("data") / "interface" / "modex" / "fonts";
		const std::filesystem::path MODEX_ICON_FILE = MODEX_FONT_DIR / "lucide.ttf";

		std::vector<std::string> list;

		if (std::filesystem::exists(MODEX_FONT_DIR)) {
			for (const auto& entry : std::filesystem::directory_iterator(MODEX_FONT_DIR)) {
				if (entry.path().filename().extension() != ".ttf" && entry.path().filename().extension() != ".otf") {
					continue;
				}

				if (entry == MODEX_ICON_FILE) {
					continue;
				}

				list.push_back(entry.path().stem().string());
			}
		}

		if (std::filesystem::exists(IMGUI_FONT_DIR)) {
			for (const auto& entry : std::filesystem::directory_iterator(IMGUI_FONT_DIR)) {
				if (entry.path().filename().extension() != ".ttf" && entry.path().filename().extension() != ".otf") {
					continue;
				}

				list.push_back(entry.path().stem().string());
			}
		}

		return list;
	}

	std::vector<std::string> GetLanguageFiles()
	{
		const std::filesystem::path LOCALE_JSON_DIR = 
		std::filesystem::path("data") / "interface" / "modex" / "language";

		std::vector<std::string> list;

		if (!std::filesystem::exists(LOCALE_JSON_DIR)) {
			return list;
		}

		for (const auto& entry : std::filesystem::directory_iterator(LOCALE_JSON_DIR)) {
			if (entry.is_regular_file() && entry.path().extension() == ".json") {
				list.push_back(entry.path().filename().stem().string());
			}
		}

		return list;
	}

	bool Load()
	{
		const auto path = GetPath();

		std::ifstream file(path);
		if (!file.is_open()) {
			settings = json::object();
			return false;
		}

		try {
			settings = json::parse(file);
		} catch (const json::parse_error&) {
			settings = json::object();
			return false;
		}

		for (const auto& [key, member, def] : boolSettings)
			user.*member = GetSetting<bool>(key, def);

		for (const auto& [key, member, def] : intSettings)
			user.*member = GetSetting<int>(key, def);

		for (const auto& [key, member, def] : stringSettings)
			user.*member = GetSetting<std::string>(key, def);

		s_languages = GetLanguageFiles();
		s_fonts = GetAvailableFonts();

		return true;
	}

	void Unload()
	{
		s_languages.clear();
		s_fonts.clear();
	}

	bool Save()
	{
		const auto path = GetPath();

		std::filesystem::create_directories(std::filesystem::path(path).parent_path());

		std::ofstream file(path);
		if (!file.is_open()) {
			return false;
		}

		for (const auto& [key, member, def] : boolSettings)
			SetSetting<bool>(key, user.*member);

		for (const auto& [key, member, def] : intSettings)
			SetSetting<int>(key, user.*member);

		for (const auto& [key, member, def] : stringSettings)
			SetSetting<std::string>(key, user.*member);

		file << settings.dump(4);

		const bool success = file.good();
		file.close();

		if (success && Modex) {
			Modex->UpdateSettings();
		}

		return success;
	}
}
