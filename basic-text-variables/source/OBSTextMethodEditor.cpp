
#include "OBSTextMethodEditor.h"
#include <util/util.hpp>

VariableEditorUI::VariableEditorUI(QWidget* parent) : QDialog(parent), ui(new Ui_VariableEditor())
{
	ui->setupUi(this);

	setSizeGripEnabled(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);


}

void VariableEditorUI::ShowHideDialog()
{
	SetupPropertiesView();
	SetupPreviewPropertiesView();

	setVisible(!isVisible());
}


void VariableEditorUI::SetupPropertiesView()
{
	if (properties_view) delete properties_view;

	obs_data_t* settings = obs_data_create();

	OBSData data = LoadSettings();

	if (data) obs_data_apply(settings, data);

	properties_view = new OBSPropertiesView(settings, "text_variables", (PropertiesReloadCallback)obs_get_output_properties, 0);
	ui->dialogLayout->addWidget(properties_view);

	obs_data_release(settings);

	connect(properties_view, SIGNAL(Changed()), this,
			SLOT(PropertiesChanged()));
}

void VariableEditorUI::SetupPreviewPropertiesView()
{

}

void VariableEditorUI::PropertiesChanged()
{
	SaveSettings();
}

OBSData VariableEditorUI::LoadSettings()
{
	BPtr<char> path = obs_module_get_config_path(
		obs_current_module(), "textVariableProps.json");
	BPtr<char> jsonData = os_quick_read_utf8_file(path);
	if (!!jsonData) {
		obs_data_t* data = obs_data_create_from_json(jsonData);
		OBSData dataRet(data);
		obs_data_release(data);
		return dataRet;
	}

	return nullptr;
}

void VariableEditorUI::SaveSettings()
{
	BPtr<char> modulePath =
		obs_module_get_config_path(obs_current_module(), "");

	os_mkdirs(modulePath);

	BPtr<char> path = obs_module_get_config_path(
		obs_current_module(), "textVariableProps.json");

	obs_data_t* settings = properties_view->GetSettings();
	if (settings)
		obs_data_save_json_safe(settings, path, "tmp", "bak");
}

static inline long long color_to_int(const QColor& color)
{
	auto shift = [&](unsigned val, int shift) {
		return ((val & 0xff) << shift);
	};

	return shift(color.red(), 0) | shift(color.green(), 8) |
		shift(color.blue(), 16) | shift(color.alpha(), 24);
}

static inline QColor rgba_to_color(uint32_t rgba)
{
	return QColor::fromRgb(rgba & 0xFF, (rgba >> 8) & 0xFF,
		(rgba >> 16) & 0xFF, (rgba >> 24) & 0xFF);
}

