/********************************************************************************
** Form generated from reading UI file 'ui_outputVqWHJv.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OUTPUTVQWHJV_H
#define UI_OUTPUTVQWHJV_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
	QVBoxLayout* verticalLayout;
	QDialogButtonBox* buttonBox;

	void setupUi(QDialog* Dialog)
	{
		if (Dialog->objectName().isEmpty())
			Dialog->setObjectName(QString::fromUtf8("Dialog"));
		Dialog->resize(400, 300);
		verticalLayout = new QVBoxLayout(Dialog);
		verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
		buttonBox = new QDialogButtonBox(Dialog);
		buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Close | QDialogButtonBox::Save);
		buttonBox->setCenterButtons(true);

		verticalLayout->addWidget(buttonBox);


		retranslateUi(Dialog);
		QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

		QMetaObject::connectSlotsByName(Dialog);
	} // setupUi

	void retranslateUi(QDialog* Dialog)
	{
		Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
	} // retranslateUi

};

namespace Ui
{
	class Dialog : public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OUTPUTVQWHJV_H
