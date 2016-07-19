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

#ifndef QCINTER_H
#define QCINTER_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include "scriptexecwin.h"

const char newline[] = "\n";



class QCInter : public scriptExecWin
{
Q_OBJECT
public:
    QCInter(QWidget* parent = 0);

    ~QCInter();

    void exec(const QByteArray &scode);

protected:
    enum {   SYM_ERROR      //
            ,SYM_SPACE      //
            ,SYM_MINUS      // -
            ,SYM_PLUS       // +
            ,SYM_DIGIT     // 1234567890
            ,SYM_LESS       // <
            ,SYM_MORE       // >
            ,SYM_EXCLAM     // !
            ,SYM_OPERATOR   //  = * /
            ,SYM_LETTER       // a-z A-Z
            ,SYM_QUOTE      // "
            ,SYM_DOT        // .
            ,SYM_DOT_NUMBER // 1234567890 after sym_dot
//            ,SYM_       //
    };

    enum {	TYPE_NOTHING,
			TYPE_CHAR
    };

	int getType(QByteArray name);
    bool checkLeksAanalysis(const QByteArray &scode);
	bool checkSyntaxAanalysis(void);
	void execute(int begin, int end);

	int declarationChar(int bgn);
	int execExpression(int bgn,int typ,QByteArray varname);
    QList <QByteArray> m_Words;
	QMap <QByteArray,int> m_CharVars;
};

#endif
