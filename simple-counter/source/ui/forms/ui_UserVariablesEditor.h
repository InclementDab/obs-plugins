/********************************************************************************
** Form generated from reading UI file 'UserVariablesEditorjsrtQj.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef USERVARIABLESEDITORJSRTQJ_H
#define USERVARIABLESEDITORJSRTQJ_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UserVariablesEditor
{
public:
    QVBoxLayout *verticalLayout;
    QTableView *tableView;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *UserVariablesEditor)
    {
        if (UserVariablesEditor->objectName().isEmpty())
            UserVariablesEditor->setObjectName(QString::fromUtf8("UserVariablesEditor"));
        UserVariablesEditor->resize(400, 300);
        verticalLayout = new QVBoxLayout(UserVariablesEditor);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tableView = new QTableView(UserVariablesEditor);
        tableView->setObjectName(QString::fromUtf8("tableView"));

        verticalLayout->addWidget(tableView);

        buttonBox = new QDialogButtonBox(UserVariablesEditor);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(UserVariablesEditor);
        QObject::connect(buttonBox, SIGNAL(accepted()), UserVariablesEditor, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UserVariablesEditor, SLOT(reject()));

        QMetaObject::connectSlotsByName(UserVariablesEditor);
    } // setupUi

    void retranslateUi(QDialog *UserVariablesEditor)
    {
        UserVariablesEditor->setWindowTitle(QCoreApplication::translate("UserVariablesEditor", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UserVariablesEditor: public Ui_UserVariablesEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // USERVARIABLESEDITORJSRTQJ_H
