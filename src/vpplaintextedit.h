#ifndef VPPLAINTEXTEDIT_H
#define VPPLAINTEXTEDIT_H

#include <QtGui>

class VPPlainTextEdit : public QPlainTextEdit
{
public:
    VPPlainTextEdit ( QWidget *parent = 0 );
    ~VPPlainTextEdit ();
    
    QAction *actionPortDisconnect;
    QAction *actionPortConnect;
    QAction *actionPortSettings;
    QAction *actionASCII_Table;
    QAction *actionWhatisThis;
//    QAction *actionAbout_VPTerminal;

      
protected:
    
    virtual void contextMenuEvent(QContextMenuEvent *event);

};
#endif // VPPLAINTEXTEDIT_H
