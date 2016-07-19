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
#include "serial.h"
#include "settingsdialog.h"

///////////////////////////////////////////////////////////////////////////////
SettingsDialog::SettingsDialog ( QWidget *parent ) : QDialog ( parent )
{
	setupUi ( this );

	MainWindow *p = ( MainWindow* ) parentWidget();

	// Device Settings

	comboDevice->addItems ( p->m_Serial->devices() );

	comboBaudRate->addItems ( QStringList()	<< "300"
	                          				<< "600"
	                          				<< "1200"
	                          				<< "2400"
	                          				<< "4800"
	                          				<< "9600"
	                          				<< "19200"
	                          				<< "38400"
	                          				<< "57600"
	                          				<< "115200" );

	comboDataBits->addItems ( QStringList()	<< "7" << "8" );
	comboParity->addItems	( QStringList()	<< "None" << "Odd" << "Even" );
	comboStopBits->addItems ( QStringList()	<< "1" << "2" );
	comboFlowCtrl->addItems ( QStringList()	<< "None" << "Hardware" << "Software" );

	// Drive RTS

	comboRtsPolarity->addItems	( QStringList() << "None" << "High" << "Low" );
	editRtsPolarity->setText	( "None" );
	editRtsPolarity->setEnabled	( FALSE );

	QValidator *validator = new QIntValidator(0, 999, this);
	editRtsBeforeTime->setValidator( validator );
	editRtsAfterTime->setValidator( validator );

	// Set Values

	comboDevice->setEditText( p->m_Settings.DeviceName );

	int ix;

	ix = comboBaudRate->findText( tr("%1").arg( p->m_Settings.BaudRate ) );
	if( ix == -1 )
	{
		ix = 5;
	}
	comboBaudRate->setCurrentIndex( ix );

	ix = comboDataBits->findText( tr("%1").arg( p->m_Settings.DataBits ) );
	if( ix == -1 )
	{
		ix = 1;
	}
	comboDataBits->setCurrentIndex( ix );

	ix = comboStopBits->findText( tr("%1").arg( p->m_Settings.StopBits ) );
	if( ix == -1 )
	{
		ix = 0;
	}
	comboStopBits->setCurrentIndex( ix );

	comboParity->setCurrentIndex	( p->m_Settings.Parity );
	comboFlowCtrl->setCurrentIndex	( p->m_Settings.FlowCtrl );


	comboRtsPolarity->setCurrentIndex( p->m_Settings.RtsPolarity );
	editRtsBeforeTime->setText( tr("%1").arg( p->m_Settings.RtsBeforeTime ) );
	editRtsAfterTime->setText( tr("%1").arg( p->m_Settings.RtsAfterTime ) );
}

///////////////////////////////////////////////////////////////////////////////
void SettingsDialog::on_comboRtsPolarity_currentIndexChanged ( int index )
{
	switch ( index )
	{
		case 0:	editRtsPolarity->setText ( "None" );	break;
		case 1:	editRtsPolarity->setText ( "Low"  );	break;
		case 2:	editRtsPolarity->setText ( "High" );	break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void SettingsDialog::on_buttonOK_clicked()
{
	QSettings settings;

	MainWindow  *parent = ( MainWindow* ) parentWidget();

	parent->m_Settings.DeviceName		= comboDevice->currentText();
	parent->m_Settings.BaudRate			= comboBaudRate->currentText().toInt();
	parent->m_Settings.DataBits			= comboDataBits->currentText().toInt();
	parent->m_Settings.StopBits			= comboStopBits->currentText().toInt();
	parent->m_Settings.Parity			= comboParity->currentIndex();
	parent->m_Settings.FlowCtrl			= comboFlowCtrl->currentIndex();
	//
	parent->m_Settings.RtsPolarity		= comboRtsPolarity->currentIndex();
	parent->m_Settings.RtsBeforeTime	= editRtsBeforeTime->text().toInt();
	parent->m_Settings.RtsAfterTime		= editRtsAfterTime->text().toInt();

	//

	settings.setValue( "Comm/Device",		parent->m_Settings.DeviceName );
	settings.setValue( "Comm/BaudRate",		parent->m_Settings.BaudRate );
	settings.setValue( "Comm/DataBits",		parent->m_Settings.DataBits );
	settings.setValue( "Comm/StopBits",		parent->m_Settings.StopBits );
	settings.setValue( "Comm/Parity",		parent->m_Settings.Parity );
	settings.setValue( "Comm/FlowCtrl",		parent->m_Settings.FlowCtrl );
	//
	settings.setValue( "Rts/Polarity",		parent->m_Settings.RtsPolarity );
	settings.setValue( "Rts/BeforeTime",	parent->m_Settings.RtsBeforeTime );
	settings.setValue( "Rts/AfterTime",		parent->m_Settings.RtsAfterTime );

	accept();
}

///////////////////////////////////////////////////////////////////////////////
void SettingsDialog::on_buttonCancel_clicked()
{
	reject();
}
