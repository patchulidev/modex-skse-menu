#include "table.h"

#include "api/SKSEMenuFramework.h"
#include "config.h"
#include "translation.h"

#include <algorithm>
#include <cstring>

namespace ModexSKSE
{
	FormEntryTable::FormEntryTable(const char* a_id, ModexAPI::CacheType a_cacheType)
		: m_id(a_id), m_cacheType(a_cacheType)
	{}

	void FormEntryTable::SetRowCallback(RowCallback a_callback)
	{
		m_rowCallback = std::move(a_callback);
	}

	void FormEntryTable::SetFormTypeFilters(std::vector<RE::FormType> a_filters)
	{
		m_formTypeFilters = std::move(a_filters);
		m_currentFilter = m_formTypeFilters.front();
	}

	void FormEntryTable::Reset()
	{
		m_list.clear();
		m_filteredIndices.clear();
		m_dataLoaded = false;
		m_searchBuf[0] = '\0';
		m_prevSearchBuf[0] = '\0';
		m_sortColumnIndex = -1;
		m_sortDirection = 0;
	}

	void FormEntryTable::LoadItems()
	{
		int32_t count = Config::Modex->GetCachedFormCount(m_cacheType);
		if (count <= 0) {
			m_list.clear();
			m_filteredIndices.clear();
			return;
		}

		std::vector<ModexAPI::FormEntry> temp(count);
		Config::Modex->GetCachedForms(m_cacheType, temp.data(), count);

		m_list = std::move(temp);

		m_pluginList.clear();
		m_pluginList.insert(T("SHOWALL"));

		m_filteredIndices.clear();
		m_filteredIndices.reserve(m_list.size());
		for (int i = 0; i < static_cast<int>(m_list.size()); ++i) {
			m_filteredIndices.push_back(i);

			auto entry = m_list[i];
			if (entry.plugin && !m_pluginList.contains(entry.plugin)) {
				m_pluginList.insert(entry.plugin);
			}
		}

		m_currentPlugin = T("SHOWALL");
	}

	bool FormEntryTable::ContainsCaseInsensitive(const char* a_haystack, const char* a_needle)
	{
		if (!a_haystack || !a_needle || a_needle[0] == '\0') return true;
		if (a_haystack[0] == '\0') return false;

		for (const char* h = a_haystack; *h; ++h) {
			const char* hi = h;
			const char* ni = a_needle;
			while (*hi && *ni && (std::tolower(static_cast<unsigned char>(*hi)) ==
			                      std::tolower(static_cast<unsigned char>(*ni)))) {
				++hi;
				++ni;
			}
			if (*ni == '\0') return true;
		}
		return false;
	}

	void FormEntryTable::ApplySearchFilter()
	{
		m_filteredIndices.clear();
		m_filteredIndices.reserve(m_list.size());

		bool hasSearch = m_searchBuf[0] != '\0';

		for (int i = 0; i < static_cast<int>(m_list.size()); ++i) {
			const auto& entry = m_list[i];

			if (m_currentFilter != RE::FormType::None && entry.formType != m_currentFilter) {
				continue;
			}

			if (!m_currentPlugin.empty() && m_currentPlugin != T("SHOWALL")) {
				const auto copy = std::string(entry.plugin);
				if (m_currentPlugin.compare(copy) != 0) {
					continue;
				}
			}

			if (!hasSearch) {
				m_filteredIndices.push_back(i);
				continue;
			}

			if (ContainsCaseInsensitive(entry.name, m_searchBuf) ||
			    ContainsCaseInsensitive(entry.editorID, m_searchBuf) ||
			    ContainsCaseInsensitive(entry.plugin, m_searchBuf)) {
				m_filteredIndices.push_back(i);
			}
		}
	}

	void FormEntryTable::ApplySorting()
	{
		if (m_sortColumnIndex < 0 || m_sortDirection == 0) {
			return;
		}

		const int col = m_sortColumnIndex;
		const bool descending = (m_sortDirection == ImGuiMCP::ImGuiSortDirection_Descending);

		std::sort(m_filteredIndices.begin(), m_filteredIndices.end(),
			[this, col, descending](int a, int b) {
				const auto& entryA = m_list[a];
				const auto& entryB = m_list[b];

				int result = 0;
				switch (col) {
				case 0:  // Plugin
					result = _stricmp(entryA.plugin ? entryA.plugin : "", entryB.plugin ? entryB.plugin : "");
					break;
				case 1:  // FormID
					result = (entryA.formID < entryB.formID) ? -1 : (entryA.formID > entryB.formID) ? 1 : 0;
					break;
				case 2:  // Name
					result = _stricmp(entryA.name ? entryA.name : "", entryB.name ? entryB.name : "");
					break;
				case 3:  // EditorID
					result = _stricmp(entryA.editorID ? entryA.editorID : "", entryB.editorID ? entryB.editorID : "");
					break;
				default:
					return false;
				}

				return descending ? (result > 0) : (result < 0);
			});
	}

	void FormEntryTable::Draw()
	{
		if (!m_dataLoaded) {
			if (!Config::Modex->IsDataReady()) {
				ImGuiMCP::TextDisabled(T("INVALID_MODEX_DATA"));
				return;
			}
			m_dataLoaded = true;
			LoadItems();
		}

		const bool has_filters = !m_formTypeFilters.empty();

		// Search bar
		ImGuiMCP::AlignTextToFramePadding();
		ImGuiMCP::Text(T("HEADER_SEARCH"));
		ImGuiMCP::SameLine(0.0f);

		ImGuiMCP::ImVec2 avail_width;
		ImGuiMCP::GetContentRegionAvail(&avail_width);
		const float input_width = has_filters ? avail_width.x / 3.0f : avail_width.x / 2.0f;

		ImGuiMCP::SetNextItemWidth(input_width);
		ImGuiMCP::InputText(("##Search_" + m_id).c_str(), m_searchBuf, sizeof(m_searchBuf));

		// Formtype Filter Dropdown
		if (has_filters) {
			ImGuiMCP::SameLine(0.0f);
			ImGuiMCP::GetContentRegionAvail(&avail_width);

			ImGuiMCP::SetNextItemWidth(avail_width.x / 2.0f);
			const auto current_filter = RE::FormTypeToString(m_currentFilter);
			if (ImGuiMCP::BeginCombo(("##Filter_" + m_id).c_str(), current_filter.data())) {
				for (auto entry : m_formTypeFilters) {
					const auto string = RE::FormTypeToString(entry);
					if (ImGuiMCP::Selectable(string.data(), m_currentFilter == entry)) {
						m_currentFilter = entry;
						ApplySearchFilter();
						ApplySorting();
					}
				}

				ImGuiMCP::EndCombo();
			}
		}

		// Plugin Filter Dropdown
		ImGuiMCP::SameLine();
		ImGuiMCP::GetContentRegionAvail(&avail_width);
		ImGuiMCP::SetNextItemWidth(avail_width.x);
		if (ImGuiMCP::BeginCombo(("##Plugin_" + m_id).c_str(), m_currentPlugin.c_str())) {
			static char buffer[256];

			ImGuiMCP::ImVec2 another_var;
			ImGuiMCP::GetContentRegionAvail(&another_var);
			ImGuiMCP::SetNextItemWidth(another_var.x);
			ImGuiMCP::InputText("##PluginSearch", buffer, sizeof(buffer));

			ImGuiMCP::Spacing();
			ImGuiMCP::Spacing();

			for (const auto& plugin : m_pluginList) {
				if (buffer[0] != '\0') {
					auto compare = std::string(buffer);
					std::transform(compare.begin(), compare.end(), compare.data(), ::tolower);
					auto copy = plugin;
					std::transform(copy.begin(), copy.end(), copy.data(), ::tolower);

					if (copy.find(compare) == std::string::npos) {
						continue;
					}
				}

				if (ImGuiMCP::Selectable(plugin.c_str(), m_currentPlugin == plugin)) {
					m_currentPlugin = plugin;
					ApplySearchFilter();
					ApplySorting();
				}
			}

			ImGuiMCP::EndCombo();
		}

		if (std::strcmp(m_searchBuf, m_prevSearchBuf) != 0) {
			strcpy_s(m_prevSearchBuf, m_searchBuf);
			ApplySearchFilter();
			ApplySorting();
		}

		// Result count
		ImGuiMCP::AlignTextToFramePadding();
		ImGuiMCP::Text("%s: %d / %d", T("RESULTS"), static_cast<int>(m_filteredIndices.size()), static_cast<int>(m_list.size()));
		ImGuiMCP::Separator();

		// Table
		constexpr ImGuiMCP::ImGuiTableFlags tableFlags =
			ImGuiMCP::ImGuiTableFlags_Resizable |
			ImGuiMCP::ImGuiTableFlags_Sortable |
			ImGuiMCP::ImGuiTableFlags_RowBg |
			ImGuiMCP::ImGuiTableFlags_Borders |
			ImGuiMCP::ImGuiTableFlags_ScrollY |
			ImGuiMCP::ImGuiTableFlags_Hideable |
			ImGuiMCP::ImGuiTableFlags_Reorderable |
			ImGuiMCP::ImGuiTableFlags_SizingStretchProp;

		ImGuiMCP::ImVec2 avail;
		ImGuiMCP::GetContentRegionAvail(&avail);

		if (ImGuiMCP::BeginTable(("##Table_" + m_id).c_str(), 4, tableFlags, ImGuiMCP::ImVec2(0.0f, avail.y))) {
			ImGuiMCP::TableSetupScrollFreeze(0, 1);
			ImGuiMCP::TableSetupColumn(T("kPlugin"),   ImGuiMCP::ImGuiTableColumnFlags_PreferSortAscending, 0.0f, 0);
			ImGuiMCP::TableSetupColumn(T("kFormID"),   ImGuiMCP::ImGuiTableColumnFlags_PreferSortAscending | ImGuiMCP::ImGuiTableColumnFlags_DefaultHide, 0.0f, 1);
			ImGuiMCP::TableSetupColumn(T("kName"),     ImGuiMCP::ImGuiTableColumnFlags_PreferSortAscending | ImGuiMCP::ImGuiTableColumnFlags_DefaultSort, 0.0f, 2);
			ImGuiMCP::TableSetupColumn(T("kEditorID"), ImGuiMCP::ImGuiTableColumnFlags_PreferSortAscending, 0.0f, 3);
			ImGuiMCP::TableHeadersRow();

			ImGuiMCP::ImGuiTableSortSpecs* sortSpecs = ImGuiMCP::TableGetSortSpecs();
			if (sortSpecs && sortSpecs->SpecsDirty) {
				sortSpecs->SpecsDirty = false;
				if (sortSpecs->SpecsCount > 0) {
					m_sortColumnIndex = sortSpecs->Specs[0].ColumnIndex;
					m_sortDirection = sortSpecs->Specs[0].SortDirection;
				} else {
					m_sortColumnIndex = -1;
					m_sortDirection = 0;
				}
				ApplySorting();
			}

			int itemCount = static_cast<int>(m_filteredIndices.size());
			ImGuiMCP::ImGuiListClipper* clipper = ImGuiMCP::ImGuiListClipperManager::Create();
			ImGuiMCP::ImGuiListClipperManager::Begin(clipper, itemCount, -1.0f);

			while (ImGuiMCP::ImGuiListClipperManager::Step(clipper)) {
				for (int row = clipper->DisplayStart; row < clipper->DisplayEnd; ++row) {
					const auto& entry = m_list[m_filteredIndices[row]];

					ImGuiMCP::TableNextRow();

					char formIDBuf[16];
					std::snprintf(formIDBuf, sizeof(formIDBuf), "%08X", entry.formID);

					ImGuiMCP::PushID(entry.formID);
					ImGuiMCP::TableSetColumnIndex(0);
					ImGuiMCP::Text("%s", entry.plugin ? entry.plugin : "");

					ImGuiMCP::SameLine();
					if (ImGuiMCP::Selectable("##Button", false, ImGuiMCP::ImGuiSelectableFlags_SpanAllColumns)) {
						if (m_rowCallback) {
							m_rowCallback(entry);
						}
					}

					ImGuiMCP::TableSetColumnIndex(1);
					ImGuiMCP::Text("%s", formIDBuf);

					ImGuiMCP::TableSetColumnIndex(2);
					ImGuiMCP::Text("%s", entry.name ? entry.name : "");

					ImGuiMCP::TableSetColumnIndex(3);
					ImGuiMCP::Text("%s", entry.editorID ? entry.editorID : "");
					ImGuiMCP::PopID();
				}
			}

			ImGuiMCP::ImGuiListClipperManager::End(clipper);
			ImGuiMCP::ImGuiListClipperManager::Destroy(clipper);

			ImGuiMCP::EndTable();
		}
	}
}
