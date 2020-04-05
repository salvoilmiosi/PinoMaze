#include "gui.h"

#include "tinyfiledialogs.h"

static const char *TITLE = "PinoMaze";

const char *getMazeFilename(dialog_type diag, const char *lastName) {
	const char *FILTERS[] = {"*.pmz"};
	const char *DESCRIPTION = "PinoMaze mazes";

	switch (diag) {
	case DIALOG_OPEN:
		return tinyfd_openFileDialog(TITLE, "", 1, FILTERS, DESCRIPTION, 0);
	case DIALOG_SAVE:
		return tinyfd_saveFileDialog(TITLE, lastName, 1, FILTERS, DESCRIPTION);
	default:
		return nullptr;
	}
}

void messageBox(message_type diag, const char *message) {
	const char *TYPES[] = {"info", "error", "warning"};

	tinyfd_messageBox(TITLE, message, "ok", TYPES[static_cast<int>(diag)], 1);
}