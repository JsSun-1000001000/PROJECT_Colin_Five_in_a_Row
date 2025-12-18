/********************************************************************************
** Form generated from reading UI file 'demodialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEMODIALOG_H
#define UI_DEMODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DemoDialog
{
public:
    QPushButton *pb_commit;
    QLineEdit *le_name;
    QLabel *lb_name;

    void setupUi(QDialog *DemoDialog)
    {
        if (DemoDialog->objectName().isEmpty())
            DemoDialog->setObjectName("DemoDialog");
        DemoDialog->resize(800, 600);
        pb_commit = new QPushButton(DemoDialog);
        pb_commit->setObjectName("pb_commit");
        pb_commit->setGeometry(QRect(280, 100, 101, 51));
        le_name = new QLineEdit(DemoDialog);
        le_name->setObjectName("le_name");
        le_name->setGeometry(QRect(30, 110, 211, 31));
        lb_name = new QLabel(DemoDialog);
        lb_name->setObjectName("lb_name");
        lb_name->setGeometry(QRect(40, 86, 51, 20));

        retranslateUi(DemoDialog);

        QMetaObject::connectSlotsByName(DemoDialog);
    } // setupUi

    void retranslateUi(QDialog *DemoDialog)
    {
        DemoDialog->setWindowTitle(QCoreApplication::translate("DemoDialog", "DemoDialog", nullptr));
        pb_commit->setText(QCoreApplication::translate("DemoDialog", "\346\217\220\344\272\244", nullptr));
        lb_name->setText(QCoreApplication::translate("DemoDialog", "\347\224\250\346\210\267\345\220\215", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DemoDialog: public Ui_DemoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEMODIALOG_H
