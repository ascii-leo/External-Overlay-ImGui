#include <thread>
#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>

class external_overlay {
private:
	DWORD process_identifier = 0;
	RECT target_rect{};
	const char* process_name_internal = nullptr;
	//DWORD window_extended_styles = {};


	void locate_process() {
		HWND window_handle = FindWindowA(NULL, process_name_internal);
		GetWindowThreadProcessId(window_handle, &this->process_identifier);
	};


	uintptr_t get_module_base_address(_In_ const char* module_name, _In_ DWORD process_id) {

		uintptr_t base_address = 0;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);

		if (snapshot != INVALID_HANDLE_VALUE) {
			MODULEENTRY32 me32;
			me32.dwSize = sizeof(me32);

			if (Module32First(snapshot, &me32)) {
				do {
					if (_stricmp(me32.szModule, module_name) == 0) {
						base_address = (uintptr_t)me32.modBaseAddr;
						break;
					}
				} while (Module32Next(snapshot, &me32));
			}
			CloseHandle(snapshot);
		}
		return base_address;
	}


public:
	bool occluded = true;
	external_overlay(const char* process_name, const char* base_module_name, HWND window) : process_name_internal(process_name)  {
		this->locate_process();
		auto get_external_window = GetForegroundWindow();
		DWORD external_process_id = 0; GetWindowThreadProcessId(get_external_window, &external_process_id);
		

		if (external_process_id == this->process_identifier && this->process_identifier != 0) {
			this->occluded = false;
			GetWindowRect(get_external_window, &this->target_rect);
			int x = this->target_rect.left;
			int y = this->target_rect.top;
			int width = this->target_rect.right - this->target_rect.left;
			int height = this->target_rect.bottom;
			SetWindowPos(window, HWND_TOPMOST, x, y, width, height, SWP_SHOWWINDOW);
		}

		else
			this->occluded = true;
	}
};

