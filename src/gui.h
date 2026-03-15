#pragma once

#include "translation.h"
#include "config.h"

namespace ImGuiMCPComponents
{
	inline void InlineDropdown(const char* a_key, int* a_value, const std::vector<std::string>& a_items)
	{
		if (a_items.empty()) return;

		ImGuiMCP::Text("%s", Translation::Lookup(a_key));

		ImGuiMCP::ImVec2 space;
		ImGuiMCP::GetContentRegionAvail(&space);
		const float width = space.x / 3.0f;

		ImGuiMCP::SameLine(space.x - width);

		ImGuiMCP::SetNextItemWidth(width);
		if (ImGuiMCP::BeginCombo(("##Combo_" + std::string(a_key)).c_str(), a_items[*a_value].c_str())) {
			for (size_t i = 0; i < a_items.size(); i++) {
				if (ImGuiMCP::Selectable(a_items[i].c_str(), (int)i == *a_value)) {
					*a_value = static_cast<int>(i);
					Config::Save();
				}
			}

			ImGuiMCP::EndCombo();
		}
	}

	inline void InlineDropdown(const char* a_key, std::string* a_value, const std::vector<std::string>& a_items)
	{
		if (a_items.empty()) return;

		ImGuiMCP::Text("%s", Translation::Lookup(a_key));

		ImGuiMCP::ImVec2 space;
		ImGuiMCP::GetContentRegionAvail(&space);
		const float width = space.x / 3.0f;

		ImGuiMCP::SameLine(space.x - width);

		ImGuiMCP::SetNextItemWidth(width);
		if (ImGuiMCP::BeginCombo(("##Combo_" + std::string(a_key)).c_str(), a_value->c_str())) {
			for (const auto& string : a_items) {
				if (ImGuiMCP::Selectable(string.c_str(), string == *a_value)) {
					*a_value = string;
					Config::Save();
				}
			}

			ImGuiMCP::EndCombo();
		}
	}

	inline void InlineToggle(const char* a_key, bool& a_value)
	{
		ImGuiMCP::Text("%s", Translation::Lookup(a_key));

		ImGuiMCP::ImVec2 space;
		ImGuiMCP::GetContentRegionAvail(&space);
		const float width = space.x / 3.0f;

		ImGuiMCP::SameLine(space.x - width);

		const std::string label = (a_value ? Translation::Lookup("TRUE") : Translation::Lookup("FALSE")) + std::string("##Toggle_") + a_key;

		ImGuiMCP::SetNextItemWidth(width);
		if (ImGuiMCP::Button(label.c_str(), (ImGuiMCP::ImVec2(width, 0.f)))) {
			a_value = !a_value;
			Config::Save();
		}
	}

	inline void InlineInt(const char* a_key, int* a_value, int a_min, int a_max)
	{
		ImGuiMCP::Text("%s", Translation::Lookup(a_key));

		ImGuiMCP::ImVec2 space;
		ImGuiMCP::GetContentRegionAvail(&space);
		const float width = space.x / 3.0f;

		ImGuiMCP::SameLine(space.x - width);

		ImGuiMCP::SetNextItemWidth(width);
		ImGuiMCP::SliderInt(("##Slider_" + std::string(a_key)).c_str(), a_value, a_min, a_max, "%d");

		if (ImGuiMCP::IsItemDeactivatedAfterEdit()) {
			Config::Save();
		}
	}

	inline void InlineInputInt(const char* a_key, int* a_value)
	{
		ImGuiMCP::Text("%s", Translation::Lookup(a_key));

		ImGuiMCP::ImVec2 space;
		ImGuiMCP::GetContentRegionAvail(&space);
		const float width = space.x / 3.0f;

		ImGuiMCP::SameLine(space.x - width);

		ImGuiMCP::SetNextItemWidth(width);
		ImGuiMCP::InputInt(("##InputInt_" + std::string(a_key)).c_str(), a_value, 1, 1);

		if (ImGuiMCP::IsItemDeactivatedAfterEdit()) {
			Config::Save();
		}
	}
}
