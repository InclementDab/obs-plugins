
#include "window-user-variables-editor.h"

UserVariablesEditor::UserVariablesEditor(QWidget* parent)
{
	ui->setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	
}

void UserVariablesEditor::ShowAbout()
{
	
}
