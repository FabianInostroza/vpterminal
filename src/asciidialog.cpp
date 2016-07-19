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

#include <QtGui>
#include "mainwindow.h"
#include "asciidialog.h"

///////////////////////////////////////////////////////////////////////////////
AsciiDialog::AsciiDialog ( QWidget *parent ) : QDialog ( parent )
{
	setupUi ( this );
	

	#ifdef WIN32
	QFont font ( "Courier New",	11 );
	#else
	QFont font ( "Monospace",	11 );
	#endif

	AsciiTable->verticalHeader()->setEnabled( FALSE );
	AsciiTable->verticalHeader()->hide();
	AsciiTable->setShowGrid( FALSE );

	AsciiTable->setColumnWidth(0, 80);
	AsciiTable->setColumnWidth(1, 80);
	AsciiTable->setColumnWidth(2, 80);

	for ( int i = 0x00; i < 0x80; i++ )
	{
		AsciiTable->setItem(i,0,new QTableWidgetItem(""));
		AsciiTable->setItem(i,1,new QTableWidgetItem(""));
		AsciiTable->setItem(i,2,new QTableWidgetItem(""));
		//													 R	 G	 B
		//AsciiTable->item(i,0)->setBackground( QBrush( QColor(230,230,255) ) );
		//AsciiTable->item(i,1)->setBackground( QBrush( QColor(230,255,230) ) );
		//AsciiTable->item(i,2)->setBackground( QBrush( QColor(255,255,230) ) );

		AsciiTable->item(i,0)->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
		AsciiTable->item(i,1)->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
		AsciiTable->item(i,2)->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

		AsciiTable->item(i,0)->setTextAlignment( Qt::AlignHCenter|Qt::AlignVCenter );
		AsciiTable->item(i,1)->setTextAlignment( Qt::AlignHCenter|Qt::AlignVCenter );
		AsciiTable->item(i,2)->setTextAlignment( Qt::AlignHCenter|Qt::AlignVCenter );

		AsciiTable->item(i,0)->setFont( font );
		AsciiTable->item(i,1)->setFont( font );
		AsciiTable->item(i,2)->setFont( font );
	}

	QString temp;
	for ( int i = 0x00; i < 0x20; i++ )
	{		
		MainWindow *p = ( MainWindow* ) parentWidget();
		AsciiTable->item(i,0)->setText( temp.sprintf( "%3d",	i) );
		AsciiTable->item(i,1)->setText( temp.sprintf( "0x%02X",	i) );
		AsciiTable->item(i,2)->setText( p->AsciiChart[i] );
	}

	for ( int i = 0x20; i < 0x80; i++ )
	{
		AsciiTable->item(i,0)->setText( temp.sprintf("%3d", 	i) );
		AsciiTable->item(i,1)->setText( temp.sprintf("0x%02X",	i) );
		AsciiTable->item(i,2)->setText( temp.sprintf("%c",		i) );
	}
}

///////////////////////////////////////////////////////////////////////////////
void AsciiDialog::resizeEvent ( QResizeEvent * event )
{
	QWidget::resizeEvent ( event );
}
