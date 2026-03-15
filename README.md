![](https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=Modex&desc=SKSE%20Menu%20Framework%20Addon&fontColor=181818&section=header&reversal=false)
# Modex Lite - An SKSE Menu Framework Addon

![GitHub last commit](https://img.shields.io/github/last-commit/patchulidev/modex-skse-menu?style=for-the-badge) ![GitHub License](https://img.shields.io/github/license/patchulidev/modex-skse-menu?style=for-the-badge) ![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/patchulidev/modex-skse-menu?style=for-the-badge) ![GitHub Release](https://img.shields.io/github/v/release/patchulidev/modex-skse-menu?include_prereleases&display_name=release&style=for-the-badge) ![Static Badge](https://img.shields.io/badge/nexus-page-gray?style=for-the-badge&labelColor=orange&link=https%3A%2F%2Fwww.nexusmods.com%2Fskyrimspecialedition%2Fmods%2F174813)

This is a SKSE Menu Framework addon which utilizes the new Modex API released in version 2.1.0+ to expose a Settings module and some quicker interactions

### Requirements
* [XMake](https://xmake.io) [2.8.2+]
* C++23 Compiler (MSVC, Clang-CL)

### Dependencies (Managed)
* [nlohmann-json](https://github.com/nlohmann/json) [v.3.12.0]
* [commonlibsse-ng](https://github.com/alandtse/CommonLibVR/) [Latest]

### Information

This project is natively maintained and built on Windows 11 using Neovim. Mileage may vary.
This project is setup for a *local* install of Commonlib in the project folder. Will require reconfiguration if you have a global instance of it.
> ***Note:*** *You may have include path issues with my xmake configuration - sorry.*

## Getting Started
```bat
git clone --recurse-submodules https://github.com/patchulidev/modex-skse-menu
cd modex-skse-menu
xmake config -m releasedbg
```

### Build
To build the project, run the following command:
```bat
xmake build
```

> ***Note:*** *This will generate a `/build/` directory in the **project's root directory** with the build output.*
> ***Note:*** *Project packages are installed locally in the .xmake directory in your workspace folder. This can be turned off*

### Build Output (Optional)
The project configuration is designed to copy the contents of `/dist/` into your mod manager `/data/`
directory after building the plugin. This requires the below environment variable set. Otherwise,
the binaries will only be distributed to `/dist/`.

If you want to redirect the build output, set one of or both of the following environment variables:

- Path to a Mod Manager mods folder: `MO2_MODS_FOLDER`

### Project Generation (Optional) (Untested)
If you want to generate a Visual Studio project, run the following command:
```bat
xmake project -k vsxmake
```

> ***Note:*** *This will generate a `vsxmakeXXXX/` directory in the **project's root directory** using the latest version of Visual Studio installed on the system.*

### Upgrading Packages (Optional)
If you want to upgrade/modify the project's dependencies, run the following commands:
```bat
xmake repo --update
xmake require --upgrade
```
Alternatively, if you want to clean and re-install project dependencies, run the following commands:
```bat
xrepo remove --all
xrepo clean
xmake f -c
```
Doing so will redownload project dependencies from source. Follow "Getting Started" afterwards.

### Clean and Reconfigure (Optional)
Similarly to CMake, you may need to clean and reconfigure your installation.
```bat
xmake f -c
xmake build
```

## Documentation
Please refer to the [Wiki](https://github.com/libxse/commonlibsse-ng-template/wiki) for more advanced topics and template guidance.
