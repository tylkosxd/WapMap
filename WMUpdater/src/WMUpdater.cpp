#include <filesystem>
#include <thread>
#include <chrono>
#include <windows.h>

namespace fs = std::filesystem;

#define UPDATE_DIRECTORY ".wm_update"
#define UPDATE_DIRECTORY_LENGTH sizeof(UPDATE_DIRECTORY)

int main()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));

	for (const fs::directory_entry& entry : fs::recursive_directory_iterator(UPDATE_DIRECTORY)) {
		auto* relativePath = entry.path().c_str() + UPDATE_DIRECTORY_LENGTH;

		if (entry.is_directory()) {
			fs::create_directory(relativePath);
			continue;
		}

		if (fs::exists(relativePath)) {
			if (fs::last_write_time(relativePath) >= entry.last_write_time()) continue;
			fs::remove(relativePath);
		}

		fs::rename(entry.path(), relativePath);
	}

	fs::remove_all(UPDATE_DIRECTORY);

	ShellExecuteA(
		NULL,
		"open",
		"WapMap.exe",
		"",
		"",
		SW_SHOWNORMAL
	);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdline, int) {
	return main();
}
