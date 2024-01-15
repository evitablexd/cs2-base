#pragma once

// shadow vmt hooking
// I took the function count from somewhere on uc, can't remember


class vmt_hook
{
private:
	uintptr_t address;
	uintptr_t* original_vtable;
	uintptr_t* allocated_vtable;

	// last hooked address does not have to be a pointer since we only want to STORE the address of it.
	uintptr_t last_hooked_address;
	int last_hooked_size;
	int size;

public:

	vmt_hook() { }

	size_t get_function_count()
	{
		MEMORY_BASIC_INFORMATION mbi{ };
		size_t i{ };

		// Query memory regions until VirtualQuery fails
		while (VirtualQuery(reinterpret_cast<LPCVOID>(this->original_vtable[i]), &mbi, sizeof(mbi)))
		{
#define PAGE_EXECUTABLE ( PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY )

			// Break on invalid pointers
			if ((mbi.State != MEM_COMMIT) || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) || !(mbi.Protect & PAGE_EXECUTABLE))
				break;

			// Increment function count
			++i;
		}

		return i;
	}

	bool initialize(void* addr)
	{
		this->address = reinterpret_cast<uintptr_t>(addr); // set our address to the one we provided
		this->original_vtable = *reinterpret_cast<uintptr_t**>(addr);

		if (this->last_hooked_address && this->last_hooked_size) // not our first time hooking this address
			if (this->last_hooked_address == this->original_vtable[this->last_hooked_size]) // already hooked
				return false;

		this->size = this->get_function_count(); // get vtable size

		// create the fake vmt
		this->allocated_vtable = new uintptr_t[this->size];

		// fill the fake vtable with the function pointers from orig vtable
		for (size_t i = 0; i <= this->size; ++i)
			this->allocated_vtable[i] = this->original_vtable[i];

		return true;
	}

	template <class orig_fn>
	void hook(void* detour, int index, orig_fn* original)
	{
		if (!this->address || !this->allocated_vtable || !this->original_vtable || index > this->size)
			return;

		*original = orig_fn(this->original_vtable[index]); // set our original

		this->allocated_vtable[index] = uintptr_t(detour); // set our function at the desired index into our detour

		this->last_hooked_address = this->address;
		this->last_hooked_size = index;

		*reinterpret_cast<uintptr_t**>(this->address) = this->allocated_vtable; // apply our shadow vmt
	}

	bool unhook()
	{
		if (!this->original_vtable || !this->allocated_vtable)
			return false;

		// restore our vtable
		*reinterpret_cast<uintptr_t**>(this->address) = this->original_vtable;
	}
};
