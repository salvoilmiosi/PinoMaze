#ifndef __GUI_H__
#define __GUI_H__

enum dialog_type {
	DIALOG_OPEN,
	DIALOG_SAVE,
};

enum message_type {
	MESSAGE_INFORMATION,
	MESSAGE_ERROR,
	MESSAGE_WARNING,
};

const char *getMazeFilename(dialog_type diag, const char *lastName = nullptr);

void messageBox(message_type diag, const char *message, const char *title = "Error");

#endif