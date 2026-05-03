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
public:
	bool occluded = true;
	external_overlay(const char* process_name, HWND window) : process_name_internal(process_name)  {
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

