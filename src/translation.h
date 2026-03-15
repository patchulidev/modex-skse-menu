#pragma once

#include <string>

namespace Translation
{
	bool Load();
	const char* Lookup(const std::string& a_key);
}

#define T(text) Translation::Lookup(text)
