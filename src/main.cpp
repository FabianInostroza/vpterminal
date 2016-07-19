/***************************************************************************
 *   Copyright (C) 2007 by Vladimir Krushkin and  Pavel Rangelov           *
 *   krushkin@gmail.com  	                                               *
 *   nifelheim@darkstar                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QApplication>
#include <QTextCodec>
#include <QFont>

#include "mainwindow.h"

int main ( int argc, char *argv[] )
{

//QCDEStyle
//QPlastiqueStyle
//QMotifStyle
#ifdef WIN32
    QApplication::setStyle(new QPlastiqueStyle);
#endif

    Q_INIT_RESOURCE ( main );


    QApplication app ( argc, argv );

    app.setOrganizationName	( "VPSoftware" );
    app.setApplicationName	( "VPTerminal" );

#ifdef WIN32
    QFont mainFont ( "Courier New",	9 );
    app.setFont ( mainFont );
#else
    QFont mainFont ( "Monospace",	9 );
    app.setFont ( mainFont );
#endif

    QTextCodec::setCodecForTr ( QTextCodec::codecForName ( "CP1251" ) );
    QTextCodec::setCodecForLocale ( QTextCodec::codecForName ( "CP1251" ) );
    QTextCodec::setCodecForCStrings ( QTextCodec::codecForName ( "CP1251" ) );


    MainWindow *mw = new MainWindow;
    mw->show();
//	mw->showMaximized();
    return app.exec();
}
