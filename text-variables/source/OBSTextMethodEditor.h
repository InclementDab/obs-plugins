#pragma once


#include <memory>
#include <QDialog>

#include <ui_output.h>
#include <UI/properties-view.hpp>


struct VariableEditorUI : QDialog
{
	Q_OBJECT

private:
	OBSPropertiesView* properties_view;
	
public slots:
	void PropertiesChanged();

public:

	std::unique_ptr<Ui_Dialog> ui;
	VariableEditorUI(QWidget* parent = nullptr);
	~VariableEditorUI() = default;

	void ShowHideDialog();
	OBSData LoadSettings();
	void SaveSettings();
	void SetupPropertiesView();

};

