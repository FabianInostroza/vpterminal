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

#ifndef __SETTINGSDIALOG_H
#define __SETTINGSDIALOG_H

#include <QDialog>
#include "ui_settingsdialog.h"

class SettingsDialog : public QDialog, public Ui_SettingsDialog
{
		Q_OBJECT

	public:
		SettingsDialog ( QWidget *parent = 0 );

	public slots:
		void on_comboRtsPolarity_currentIndexChanged ( int index );
		void on_buttonOK_clicked();
		void on_buttonCancel_clicked();
};
#endif
