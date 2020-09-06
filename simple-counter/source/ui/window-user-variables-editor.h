#pragma once

#include <QDialog>
#include <memory>
#include "ui_UserVariablesEditor.h"

class UserVariablesEditor : public QDialog
{
	Q_OBJECT

public:
	explicit UserVariablesEditor(QWidget* parent = nullptr);

	std::unique_ptr<Ui::UserVariablesEditor> ui;

private slots:
	void ShowAbout();
};