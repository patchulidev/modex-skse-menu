#include "translation.h"

#include "config.h"

namespace Translation
{
	using json = nlohmann::json;

	static std::unordered_map<std::string, std::string> keys;

	static std::filesystem::path GetPath(const std::string& a_language)
	{
		return std::filesystem::path("data") / "interface" / "modex" / "language" / (a_language + ".json");
	}

	bool Load()
	{
		const auto path = GetPath(Config::user.language);

		std::ifstream file(path);
		if (!file.is_open()) {
			SKSE::log::warn("Failed to open translation file: {}", path.string());
			return false;
		}

		try {
			json data = json::parse(file);

			keys.clear();
			keys.reserve(data.size());

			for (auto& [key, value] : data.items()) {
				if (value.is_string()) {
					keys.emplace(key, value.get<std::string>());
				}
			}

		} catch (const json::parse_error& e) {
			SKSE::log::error("Translation parse error: {}", e.what());
			return false;
		}

		return true;
	}

	const char* Lookup(const std::string& a_key)
	{
		if (auto it = keys.find(a_key); it != keys.end()) {
			return it->second.c_str();
		}

		return a_key.c_str();
	}
}
