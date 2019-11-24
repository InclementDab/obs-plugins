/********************************************************************************
** Form generated from reading UI file 'variable-editorv17756.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef VARIABLE_2D_EDITORV17756_H
#define VARIABLE_2D_EDITORV17756_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_VariableEditor
{
public:
	QVBoxLayout* dialogLayout;
	QListWidget* listWidget;
	QDialogButtonBox* buttonBox;

	void setupUi(QDialog* VariableEditor)
	{
		if (VariableEditor->objectName().isEmpty())
			VariableEditor->setObjectName(QStringLiteral("VariableEditor"));
		VariableEditor->resize(400, 300);
		dialogLayout = new QVBoxLayout(VariableEditor);
		dialogLayout->setObjectName(QStringLiteral("dialogLayout"));
		listWidget = new QListWidget(VariableEditor);
		listWidget->setObjectName(QStringLiteral("listWidget"));

		dialogLayout->addWidget(listWidget);

		buttonBox = new QDialogButtonBox(VariableEditor);
		buttonBox->setObjectName(QStringLiteral("buttonBox"));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Close);
		buttonBox->setCenterButtons(true);

		dialogLayout->addWidget(buttonBox);


		retranslateUi(VariableEditor);
		QObject::connect(buttonBox, SIGNAL(accepted()), VariableEditor, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), VariableEditor, SLOT(reject()));

		QMetaObject::connectSlotsByName(VariableEditor);
	} // setupUi

	void retranslateUi(QDialog* VariableEditor)
	{
		VariableEditor->setWindowTitle(QApplication::translate("VariableEditor", "Dialog", Q_NULLPTR));
	} // retranslateUi

};

namespace Ui
{
	class VariableEditor : public Ui_VariableEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // VARIABLE_2D_EDITORV17756_H
