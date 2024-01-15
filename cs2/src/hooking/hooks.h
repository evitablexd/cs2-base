#pragma once

namespace hooks
{
	inline DWORD_PTR* pSwapChainVtable = NULL;
	inline HWND window = NULL;
	inline ID3D11Device* p_device = NULL;
	inline ID3D11DeviceContext* p_context = NULL;
	inline ID3D11RenderTargetView* main_render_target_view;

	inline vmt_hook client_hooks;

	namespace wndproc
	{
		inline WNDPROC original;
		LRESULT __stdcall  wndproc_hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	}

	namespace present
	{
		typedef HRESULT(__stdcall* presentfn)(IDXGISwapChain*, UINT, UINT);
		inline presentfn original;

		HRESULT __stdcall present_hook(IDXGISwapChain* swap_chain, UINT sync, UINT flags);
	}

	namespace framestagenotify
	{
		typedef void(__thiscall* framestagefn)(void*, int);
		framestagefn original;

		void __fastcall framestage_hk(void* rcx, int stage);
	}

	void initialize_main();
	void initialize_dx11();
	void uninitialize();
}