/********************************************************************************
** Form generated from reading UI file 'Radome.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RADOME_H
#define UI_RADOME_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RadomeClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RadomeClass)
    {
        if (RadomeClass->objectName().isEmpty())
            RadomeClass->setObjectName(QString::fromUtf8("RadomeClass"));
        RadomeClass->resize(600, 400);
        menuBar = new QMenuBar(RadomeClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        RadomeClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RadomeClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        RadomeClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(RadomeClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        RadomeClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(RadomeClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        RadomeClass->setStatusBar(statusBar);

        retranslateUi(RadomeClass);

        QMetaObject::connectSlotsByName(RadomeClass);
    } // setupUi

    void retranslateUi(QMainWindow *RadomeClass)
    {
        RadomeClass->setWindowTitle(QCoreApplication::translate("RadomeClass", "Radome", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RadomeClass: public Ui_RadomeClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RADOME_H
