
#include "common.h"
#include "TextMethods.h"
#include "ui_variable-editor.h"
#include "UI/properties-view.hpp"

#include <QWidget>
#include <QWindow>
#include <QScreen>
#include <QResizeEvent>
#include <QShowEvent>

#define GREY_COLOR_BACKGROUND 0xFF4C4C4C

struct VariableEditorUI : QDialog
{
	Q_OBJECT
private:
	OBSPropertiesView* properties_view;
	OBSPropertiesView* preview_properties_view;

public slots:
	void PropertiesChanged();
	
public:

	std::unique_ptr<Ui_VariableEditor> ui;
	VariableEditorUI(QWidget* parent = nullptr);

	void ShowHideDialog();

	OBSData LoadSettings();
	void SaveSettings();

	void SetupPropertiesView();
	void SetupPreviewPropertiesView();

};