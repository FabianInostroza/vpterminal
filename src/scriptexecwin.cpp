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


#include "scriptexecwin.h"

scriptExecWin::scriptExecWin(QWidget* parent, Qt::WFlags fl)
: QDialog( parent, fl ), Ui::scriptExecute()
{
	setupUi(this);
    console->setReadOnly(true);
}

scriptExecWin::~scriptExecWin()
{

}

/*$SPECIALIZATION$*/
/*!
    \fn scriptExecWin::settext(const QString &txt)
 */
void scriptExecWin::message(const QString &txt)
{
    /// @todo implement me
    console->insertHtml(txt);
    console->ensureCursorVisible();
}
