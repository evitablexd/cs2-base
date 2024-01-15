#include "includes.h"

typedef void* (*create_interface_fn)(const char* name, int* code);
void* create_interface(std::string module_name, std::string interface_name)
{
	const auto module = GetModuleHandleA(module_name.c_str());
	if (!module)
		return nullptr;

	const auto create_interface_export = GetProcAddress(module, "CreateInterface");
	if (!create_interface_export)
		return nullptr;

	printf("create_interface_export -> %p\n", create_interface_export);

	// note: vac monitores the calls to this function, I recommend another way of doing this.

	const auto create_interface_func = reinterpret_cast<create_interface_fn>(create_interface_export);
	return create_interface_func(interface_name.c_str(), 0);
}

void thread(void* module)
{
	printf("allocated thread at %p\n", module);

	interfaces::client = create_interface("client.dll", "Source2Client002");
	printf("Source2Client002 -> %p\n", interfaces::client);

	hooks::initialize_main();
	hooks::initialize_dx11();

	if (MH_EnableHook(MH_ALL_HOOKS) == MH_STATUS::MH_OK)
		printf("enabled all hooks\n");

	while (!GetAsyncKeyState(VK_END))
		Sleep(100);

	hooks::uninitialize();

	// reset wndproc
	SetWindowLongPtrA(hooks::window, GWLP_WNDPROC, LONG_PTR(hooks::wndproc::original));

	if (MH_Uninitialize() == MH_STATUS::MH_OK)
		printf("uninitialized hooks\n");

	FreeConsole();
	FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(module), EXIT_SUCCESS);
}

BOOL APIENTRY DllMain(void* module, unsigned long reason, void* reserved)
{
	if (reason != DLL_PROCESS_ATTACH)
		return FALSE;

	DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(module));

	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	if (MH_Initialize() == MH_STATUS::MH_OK)
		printf("initialized minhook\n");

	void* handle = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(thread), module, NULL, NULL);
	if (handle) CloseHandle(handle);

	return TRUE;
}

