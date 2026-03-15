#pragma once

#include "api/ModexAPI.h"

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

namespace ModexSKSE
{
	class FormEntryTable
	{
	public:
		using RowCallback = std::function<void(const ModexAPI::FormEntry&)>;

		FormEntryTable(const char* a_id, ModexAPI::CacheType a_cacheType);

		void SetRowCallback(RowCallback a_callback);
		void SetFormTypeFilters(std::vector<RE::FormType> a_filters);
		void Draw();
		void Reset();

	private:
		void LoadItems();
		void ApplySearchFilter();
		void ApplySorting();

		static bool ContainsCaseInsensitive(const char* a_haystack, const char* a_needle);

		std::string m_id;
		ModexAPI::CacheType m_cacheType;
		RowCallback m_rowCallback;

		std::vector<ModexAPI::FormEntry> m_list;
		std::vector<int> m_filteredIndices;
		bool m_dataLoaded{ false };

		std::unordered_set<std::string> m_pluginList;
		std::string m_currentPlugin;

		std::vector<RE::FormType> m_formTypeFilters;
		RE::FormType m_currentFilter;

		char m_searchBuf[256]{};
		char m_prevSearchBuf[256]{};
		int m_sortColumnIndex{ -1 };
		int m_sortDirection{ 0 };
	};
}
