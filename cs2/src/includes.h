#pragma once

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "d3d11.lib")

#include <Windows.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <intrin.h>
#include <d3dx9.h>
#include <dxgi.h>
#include <d3d11.h>
#include "sdk/signatures.h"
#include "thirdparty/imgui.h"
#include "thirdparty/imgui_impl_dx11.h"
#include "thirdparty/imgui_impl_win32.h"
#include "thirdparty/imgui_internal.h"
#include "thirdparty/minhook/include/MinHook.h"
#include "menu/menu.h"
#include "hooking/vmt.h"
#include "hooking/hooks.h"

namespace interfaces
{
	void* client = nullptr;
}