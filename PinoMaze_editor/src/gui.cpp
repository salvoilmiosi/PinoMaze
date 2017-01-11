#include "gui.h"

#ifdef _WIN32

#include <windows.h>
#include <cstring>

const char *getMazeFilename(dialog_type diag, const char *lastName) {
	static char filename[MAX_PATH];
	memset(filename, 0, MAX_PATH);

	if (lastName) {
		snprintf(filename, MAX_PATH, "%s", lastName);
	}

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "Mazes (*.pmz)\0*.pmz\0All files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = nullptr;
	ofn.Flags |= OFN_NOCHANGEDIR;

	switch (diag) {
	case DIALOG_OPEN:
	    if (GetOpenFileName(&ofn)) {
	    	return filename;
	    }
	    break;
	case DIALOG_SAVE:
		if (GetSaveFileName(&ofn)) {
			return filename;
		}
		break;
	}

    return nullptr;
}

void messageBox(message_type diag, const char *message, const char *title) {
	UINT flags = 0;
	switch (diag) {
	case MESSAGE_ERROR:
		flags |= MB_OK | MB_ICONERROR;
		break;
	case MESSAGE_INFORMATION:
		flags |= MB_OK | MB_ICONINFORMATION;
		break;
	case MESSAGE_WARNING:
		flags |= MB_OK | MB_ICONWARNING;
		break;
	}
	MessageBox(nullptr, message, title, flags);
}

#else

#include <SDL2/SDL.h>

const char *getMazeFilename(dialog_type diag, const char *lastName) {
	// TODO
	return nullptr;
}

void messageBox(message_type diag, const char *message, const char *title) {
	Uint32 flags = 0;
	switch (diag) {
	case MESSAGE_ERROR:
		flags |= SDL_MESSAGEBOX_ERROR;
		break;
	case MESSAGE_INFORMATION:
		flags |= SDL_MESSAGEBOX_INFORMATION;
		break;
	case MESSAGE_WARNING:
		flags |= SDL_MESSAGEBOX_WARNING;
		break;
	}
	SDL_ShowSimpleMessageBox(flags, title, message, nullptr);
}

#endif