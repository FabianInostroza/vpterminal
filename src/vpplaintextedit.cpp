#include <QtGui>
#include "vpplaintextedit.h"
#include "mainwindow.h"

VPPlainTextEdit::VPPlainTextEdit ( QWidget *parent )
{
    Q_UNUSED(parent);

    actionPortConnect           = new QAction(QIcon(":/images/connect.png")    ,tr("Connect"), this);
    actionPortDisconnect        = new QAction(QIcon(":/images/disconnect.png") ,tr("Disconnect"), this);
    actionPortSettings          = new QAction(QIcon(":/images/settings.png")   ,tr("Settings"), this);
    actionASCII_Table           = new QAction(QIcon(":/images/ascii.png")      ,tr("ASCII Table"), this);
    actionWhatisThis            = new QAction(QIcon(":/images/whatisthis.png") ,tr("What is this"), this);
//    actionAbout_VPTerminal      = new QAction(QIcon(":/images/mainicon.png")   ,tr("About"), this);
}

VPPlainTextEdit::~VPPlainTextEdit ()
{
    delete actionPortConnect;
    delete actionPortDisconnect;
    delete actionPortSettings;
    delete actionASCII_Table;
    delete actionWhatisThis;
//    delete actionAbout_VPTerminal;
}


void VPPlainTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    QAction sep1(NULL);
    QAction sep2(NULL);
    QAction sep3(NULL);

    sep1.setSeparator(true);
    sep2.setSeparator(true);
    sep3.setSeparator(true);

    menu->addAction(&sep1);

    if(MainWindow::connected())
    {
        menu->addAction(actionPortDisconnect);
    }
    else
    {
        menu->addAction(actionPortConnect);
        menu->addAction(actionPortSettings);
    }


    menu->addAction(&sep2);
    menu->addAction(actionASCII_Table);
    menu->addAction(&sep3);
    menu->addAction(actionWhatisThis);
//    menu->addAction(actionAbout_VPTerminal);


    menu->exec(event->globalPos());
    delete menu;
}
