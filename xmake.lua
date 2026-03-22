-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("Modex-Lite")
set_version("1.0.2")
set_license("GPL-3.0")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate")

add_requires("nlohmann_json v3.12.0")
--
-- targets
target("Modex-Lite")
    -- add dependencies to target
    add_deps("commonlibsse-ng")

	-- additional packages
	add_packages("nlohmann_json")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "Modex-Lite",
        author = "patchuli",
        description = "SKSE Menu interface for Modex (Mod Explorer)"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")

	after_build(function (target)
		local project_name = target:name()
		local mods_path = os.getenv("MO2_MODS_FOLDER")
		local dist_path = os.projectdir() .. "/dist/"

		-- create plugin directory structure
		os.mkdir(path.join(dist_path, "SKSE", "Plugins"))

		-- copy plugin to project distributable folder
		os.cp(target:targetfile(), path.join(dist_path, "SKSE", "Plugins"))

		-- copy pdb to project distributable folder
		os.cp(target:targetfile():gsub("%.dll$", ".pdb"), path.join(dist_path, "SKSE", "Plugins"))

		-- copy folders and files from dist to MO2 mods folder if it exists
		os.cp(dist_path .. "/*", path.join(mods_path, project_name))
	end)
