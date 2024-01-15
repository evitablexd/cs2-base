#include "../includes.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall hooks::wndproc::wndproc_hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		g_menu.is_opened() = !g_menu.is_opened();

    if (g_menu.is_opened())
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

	// call original
    return ::CallWindowProcA(original, hwnd, msg, wparam, lparam);
}

HRESULT __stdcall hooks::present::present_hook(IDXGISwapChain* swap_chain, UINT sync, UINT flags)
{
    static bool initialized{ false };
    if (!initialized)
    {
        if (SUCCEEDED(swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&p_device)))) {
            p_device->GetImmediateContext(&p_context);

            auto swapchain_desc = DXGI_SWAP_CHAIN_DESC{};
            swap_chain->GetDesc(&swapchain_desc);
            window = swapchain_desc.OutputWindow;

            ID3D11Texture2D* back_buffer{};
            swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
            p_device->CreateRenderTargetView(back_buffer, nullptr, &main_render_target_view);
            back_buffer->Release();

            wndproc::original = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)wndproc::wndproc_hook);

            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
            ImGui_ImplWin32_Init(window);
            ImGui_ImplDX11_Init(p_device, p_context);
            ImGui::GetStyle().WindowMinSize = ImVec2(500, 350);

            initialized = true;
        }
        else {
            return original(swap_chain, sync, flags);
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (g_menu.is_opened()) {
        ImGui::Begin("cheat", 0, ImGuiWindowFlags_NoCollapse);

        g_menu.render();

        ImGui::End();
    }

    ImGui::Render();

    p_context->OMSetRenderTargets(1, &main_render_target_view, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return original(swap_chain, sync, flags);
}

void __fastcall hooks::framestagenotify::framestage_hk(void* rcx, int stage)
{
    printf("fsn called, stage: %d\n", stage);

    // call original
    return original(rcx, stage);
}

void hooks::initialize_main()
{
    if (client_hooks.initialize(interfaces::client)) {
        client_hooks.hook(reinterpret_cast<void*>(hooks::framestagenotify::framestage_hk), 33, &hooks::framestagenotify::original);
	client_hooks.swap_context();
    }
}

void hooks::uninitialize()
{
    client_hooks.unhook();
}

// From UC
void hooks::initialize_dx11()
{
    HWND hWnd = GetForegroundWindow();
    IDXGISwapChain* pSwapChain;
    ID3D11Device* pDevice;
    ID3D11DeviceContext* pContext;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1
        , D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext);

    DWORD_PTR* pSwapChainVtable = reinterpret_cast<DWORD_PTR*>(pSwapChain);
    pSwapChainVtable = reinterpret_cast<DWORD_PTR*>(pSwapChainVtable[0]);

    MH_CreateHook(reinterpret_cast<DWORD_PTR*>((BYTE*)pSwapChainVtable[8]), present::present_hook, reinterpret_cast<void**>(&present::original));

    pDevice->Release();
    pContext->Release();
    pSwapChain->Release();
}
