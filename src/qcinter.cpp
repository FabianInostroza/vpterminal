/**************************************************************************
*   Copyright (C) 2008 by Vladimir Krushkin and Pavel                              *
*   krushkin@gmail.com                                                    *
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
#include "scriptexecwin.h"
#include "qcinter.h"

QCInter::QCInter(QWidget* parent):scriptExecWin(parent)
{
}


QCInter::~QCInter()
{
}

int QCInter::getType(QByteArray name)
{
    if (m_CharVars.contains(name))
    {
        return TYPE_CHAR;
    }
    return TYPE_NOTHING;
}

void QCInter::exec(const QByteArray &scode)
{
//	QString str;
    show();
    if(checkLeksAanalysis(scode))
    {
        checkSyntaxAanalysis();
        qDebug() << m_CharVars;
        execute(8,m_Words.size());
        qDebug() << m_CharVars;
//        str << m_CharVars;
//        message("<b><hr><font color=blue>"+str+"</font><\b>");
    }
    else
    {

    }
    message("<b><hr><font color=green>"+tr("Script finished.Press Esc to exit")+"</font><\b>");
}

bool QCInter::checkLeksAanalysis(const QByteArray &scode)
{
    QByteArray word;
    char symbol;
    int i,linenum,colnum;
    int prevsymbol = SYM_SPACE;
    bool ok=true;

    m_Words.clear();
    linenum=0;
    colnum=0;
    for(i=0;i<scode.size();i++)
    {
        symbol = scode[i];
        //        qDebug("%c",symbol);
        if(symbol == 0x0D)
        {
            continue;
        }
        colnum++;

        if(prevsymbol==SYM_SPACE)
        {
            switch (symbol)
            {
            case ' ' :
                break;

            case 0x0A:
                linenum++;
                colnum=0;
                m_Words.append(newline);
                word.clear();
                break;

            case '-' :
                word += symbol;
                prevsymbol = SYM_MINUS;
                break;

            case '+' :
                word += symbol;
                prevsymbol = SYM_PLUS;
                break;

            case '1':  case '2':  case '3':  case '4':  case '5':
            case '6':  case '7':  case '8':  case '9':  case '0':
                word +=symbol;
                prevsymbol = SYM_DIGIT;
                break;

            case '<' :
                word += symbol;
                prevsymbol = SYM_LESS;
                break;

            case '>' :
                word += symbol;
                prevsymbol = SYM_MORE;
                break;

            case '!' :
                word+=symbol;
                prevsymbol = SYM_EXCLAM;
                break;

            case '=' : case '*' : case '/' :
                word += symbol;
                prevsymbol = SYM_OPERATOR;
                break;

            case '{' : case '}' :
            case '(' : case ')' :
            case ',' : case ';' :
                word += symbol;
                m_Words.append(word);
                word.clear();
                prevsymbol = SYM_SPACE;
                break;

            case 'q': case 'w' : case 'e' : case 'r' : case 't' :
            case 'y': case 'u' : case 'i' : case 'o' : case 'p' :
            case 'a': case 's' : case 'd' : case 'f' : case 'g' :
            case 'h': case 'j' : case 'k' : case 'l' : case 'z' :
            case 'x': case 'c' : case 'v' : case 'b' : case 'n' :
            case 'm': case 'Q':
            case 'W': case 'E' : case 'R' : case 'T' : case 'Y' :
            case 'U': case 'I' : case 'O' : case 'P' : case 'A' :
            case 'S': case 'D' : case 'F' : case 'G' : case 'H' :
            case 'J': case 'K' : case 'L' : case 'Z' : case 'X' :
            case 'C': case 'V' : case 'B' : case 'N' : case 'M' :
            case '#':
                word += symbol;
                prevsymbol = SYM_LETTER;
                break;

            case '"' :
                word += symbol;
                prevsymbol = SYM_QUOTE;
                break;

            default :
                prevsymbol=SYM_ERROR;
                break;
            }
        }else
        {
            if (prevsymbol==SYM_DIGIT)
            {
                switch (symbol)
                {
                case '.' :
                    word += symbol;
                    prevsymbol = SYM_DOT;
                    break;

                case ' ':
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 0x0A:
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;

                    linenum++;
                    colnum=0;
                    m_Words.append(newline);
                    word.clear();
                    break;


                case '!' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_EXCLAM;
                    break;

                case '<' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LESS;
                    break;

                case '>' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MORE;
                    break;

                case '-' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MINUS;
                    break;

                case '+' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_PLUS;
                    break;

                case '=' : case '*' : case '/' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_OPERATOR;
                    break;

                case '{' : case '}' : case ';' : case '(' : case ')' : case ',' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case '1' : case '2' : case '3' : case '4' : case '5':
                case '6' : case '7' : case '8' : case '9' : case '0':
                    word += symbol;
                    prevsymbol = SYM_DIGIT;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }

            }else if (prevsymbol == SYM_DOT)
            {
                switch (symbol)
                {
                case '1':  case '2':  case '3':  case '4':  case '5':
                case '6':  case '7':  case '8':  case '9':  case '0':
                    word += symbol;
                    prevsymbol = SYM_DOT_NUMBER;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if (prevsymbol == SYM_EXCLAM)
            {
                switch (symbol)
                {
                case '=':
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if (prevsymbol == SYM_MINUS)
            {
                switch (symbol)
                {
                case '1':  case '2':  case '3':   case '4':    case '5':
                case '6':  case '7':   case '8':    case '9':   case '0':
                    word += symbol;
                    prevsymbol = SYM_DIGIT;
                    break;

                case ' ' :
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 0x0A:
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;

                    linenum++;
                    colnum=0;
                    m_Words.append(newline);
                    word.clear();
                    break;

                case '-' :
                case '=' :
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case '!' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_EXCLAM;
                    break;

                case '<' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LESS;
                    break;

                case '>' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MORE;
                    break;

                case '+' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_PLUS;
                    break;

                case '*' : case '/' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_OPERATOR;
                    break;

                case '{' : case '}' : case ';' : case '(' : case ')' : case ',' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 'q': case 'w' : case 'e' : case 'r' : case 't' :
                case 'y': case 'u' : case 'i' : case 'o' : case 'p' :
                case 'a': case 's' : case 'd' : case 'f' : case 'g' :
                case 'h': case 'j' : case 'k' : case 'l' : case 'z' :
                case 'x': case 'c' : case 'v' : case 'b' : case 'n' :
                case 'm': case 'Q' :
                case 'W': case 'E' : case 'R' : case 'T' : case 'Y' :
                case 'U': case 'I' : case 'O' : case 'P' : case 'A' :
                case 'S': case 'D' : case 'F' : case 'G' : case 'H' :
                case 'J': case 'K' : case 'L' : case 'Z' : case 'X' :
                case 'C': case 'V' : case 'B' : case 'N' : case 'M' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LETTER;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if(prevsymbol == SYM_PLUS)
            {
                switch (symbol)
                {
                case '1':  case '2':  case '3':   case '4':    case '5':
                case '6':  case '7':  case '8':    case '9':   case '0':
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_DIGIT;
                    break;

                case ' ' :
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 0x0A:
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;

                    linenum++;
                    colnum=0;
                    m_Words.append(newline);
                    word.clear();
                    break;

                case '!' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_EXCLAM;
                    break;

                case '+' :
                case '=' :
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case '<' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LESS;
                    break;

                case '>' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MORE;
                    break;

                case '-' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MINUS;
                    break;

                case '*' : case '/' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_OPERATOR;
                    break;

                case '{' : case '}' : case ';' : case '(' : case ')' : case ',' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 'q': case 'w' : case 'e' : case 'r' : case 't' :
                case 'y': case 'u' : case 'i' : case 'o' : case 'p' :
                case 'a': case 's' : case 'd' : case 'f' : case 'g' :
                case 'h': case 'j' : case 'k' : case 'l' : case 'z' :
                case 'x': case 'c' : case 'v' : case 'b' : case 'n' :
                case 'm': case 'Q' :
                case 'W': case 'E' : case 'R' : case 'T' : case 'Y' :
                case 'U': case 'I' : case 'O' : case 'P' : case 'A' :
                case 'S': case 'D' : case 'F' : case 'G' : case 'H' :
                case 'J': case 'K' : case 'L' : case 'Z' : case 'X' :
                case 'C': case 'V' : case 'B' : case 'N' : case 'M' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LETTER;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if (prevsymbol == SYM_DOT_NUMBER)
            {
                switch (symbol){

                case ' ' :
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 0x0A:
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;

                    linenum++;
                    colnum=0;
                    m_Words.append(newline);
                    word.clear();
                    break;

                case '<' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol=SYM_LESS;
                    break;

                case '>' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol=SYM_MORE;
                    break;

                case '!' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol=SYM_EXCLAM;
                    break;

                case '-' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol=SYM_MINUS;
                    break;

                case '+' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol=SYM_PLUS;
                    break;

                case '=' : case '*' : case '/' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol=SYM_OPERATOR;
                    break;

                case '{' : case '}' : case ';' : case '(' : case ')' : case ',' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol=SYM_SPACE;
                    break;

                case '1':
                case '2':  case '3':   case '4':    case '5':
                case '6':  case '7':   case '8':    case '9':   case '0':
                    word += symbol;
                    prevsymbol = SYM_DOT_NUMBER;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if (prevsymbol == SYM_LESS)
            {
                switch (symbol)
                {
                case '1':  case '2':  case '3':  case '4':  case '5':
                case '6':  case '7':  case '8':  case '9':  case '0':
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_DIGIT;
                    break;

                case '-' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MINUS;
                    break;

                case '!' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_EXCLAM;
                    break;

                case ' ' :
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 0x0A:
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;

                    linenum++;
                    colnum=0;
                    m_Words.append(newline);
                    word.clear();
                    break;

                case '<' :
                case '=' :
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case '>' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MORE;
                    break;

                case '+' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_PLUS;
                    break;

                case '*' : case '/' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_OPERATOR;
                    break;

                case '{' : case '}' : case ';' : case '(' : case ')' : case ',' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol=SYM_SPACE;
                    break;

                case 'q': case 'w' : case 'e' : case 'r' : case 't' :
                case 'y': case 'u' : case 'i' : case 'o' : case 'p' :
                case 'a': case 's' : case 'd' : case 'f' : case 'g' :
                case 'h': case 'j' : case 'k' : case 'l' : case 'z' :
                case 'x': case 'c' : case 'v' : case 'b' : case 'n' :
                case 'm': case '_' : case 'Q':
                case 'W': case 'E' : case 'R' : case 'T' : case 'Y' :
                case 'U': case 'I' : case 'O' : case 'P' : case 'A' :
                case 'S': case 'D' : case 'F' : case 'G' : case 'H' :
                case 'J': case 'K' : case 'L' : case 'Z' : case 'X' :
                case 'C': case 'V' : case 'B' : case 'N' : case 'M' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LETTER;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if(prevsymbol == SYM_MORE)
            {
                switch (symbol)
                {
                case '1':  case '2':  case '3':  case '4':  case '5':
                case '6':  case '7':  case '8':  case '9':  case '0':
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_DIGIT;
                    break;

                case '-' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MINUS;
                    break;

                case '!' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_EXCLAM;
                    break;

                case ' ' :
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 0x0A:
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;

                    linenum++;
                    colnum=0;
                    m_Words.append(newline);
                    word.clear();
                    break;

                case '>' :
                case '=' :
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case '<' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LESS;
                    break;

                case '+' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_PLUS;
                    break;

                case '*' : case '/' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_OPERATOR;
                    break;

                case '{' : case '}' : case ';' : case '(' : case ')' : case ',' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 'q': case 'w' : case 'e' : case 'r' : case 't' :
                case 'y': case 'u' : case 'i' : case 'o' : case 'p' :
                case 'a': case 's' : case 'd' : case 'f' : case 'g' :
                case 'h': case 'j' : case 'k' : case 'l' : case 'z' :
                case 'x': case 'c' : case 'v' : case 'b' : case 'n' :
                case 'm': case '_' : case 'Q':
                case 'W': case 'E' : case 'R' : case 'T' : case 'Y' :
                case 'U': case 'I' : case 'O' : case 'P' : case 'A' :
                case 'S': case 'D' : case 'F' : case 'G' : case 'H' :
                case 'J': case 'K' : case 'L' : case 'Z' : case 'X' :
                case 'C': case 'V' : case 'B' : case 'N' : case 'M' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LETTER;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if(prevsymbol == SYM_OPERATOR)
            {
                switch (symbol)
                {
                case '1':
                case '2':  case '3':   case '4':    case '5':
                case '6':  case '7':   case '8':    case '9':   case '0':
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_DIGIT;
                    break;

                case ' ' :
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 0x0A:
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;

                    linenum++;
                    colnum=0;
                    m_Words.append(newline);
                    word.clear();
                    break;

                case '=' :
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case '<' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LESS;
                    break;

                case '>' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MORE;
                    break;

                case '!' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_EXCLAM;
                    break;

                case '-' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MINUS;
                    break;

                case '+' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_PLUS;
                    break;

                case '*' : case '/' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_OPERATOR;
                    break;

                case '{' : case '}' : case ';' : case '(' : case ')' : case ',' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 'q': case 'w' : case 'e' : case 'r' : case 't' :
                case 'y': case 'u' : case 'i' : case 'o' : case 'p' :
                case 'a': case 's' : case 'd' : case 'f' : case 'g' :
                case 'h': case 'j' : case 'k' : case 'l' : case 'z' :
                case 'x': case 'c' : case 'v' : case 'b' : case 'n' :
                case 'm': case '_' : case 'Q':
                case 'W': case 'E' : case 'R' : case 'T' : case 'Y' :
                case 'U': case 'I' : case 'O' : case 'P' : case 'A' :
                case 'S': case 'D' : case 'F' : case 'G' : case 'H' :
                case 'J': case 'K' : case 'L' : case 'Z' : case 'X' :
                case 'C': case 'V' : case 'B' : case 'N' : case 'M' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LETTER;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if(prevsymbol == SYM_LETTER)
            {
                switch (symbol)
                {
                case '1':  case '2':  case '3':  case '4':  case '5':
                case '6':  case '7':  case '8':  case '9':  case '0':
                case '.':
                    word += symbol;
                    prevsymbol = SYM_LETTER;
                    break;

                case ' ' :
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 0x0A:
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;

                    linenum++;
                    colnum=0;
                    m_Words.append(newline);
                    word.clear();
                    break;

                case '-' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MINUS;
                    break;

                case '!' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_EXCLAM;
                    break;

                case '+' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_PLUS;
                    break;

                case '<' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_LESS;
                    break;

                case '>' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_MORE;
                    break;

                case '=' : case '*' : case '/' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    prevsymbol = SYM_OPERATOR;
                    break;

                case '(' : case ')' :
                case '{' : case '}' :
                case ';' : case ',' :
                    m_Words.append(word);
                    word.clear();
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                case 'q': case 'w' : case 'e' : case 'r' : case 't' :
                case 'y': case 'u' : case 'i' : case 'o' : case 'p' :
                case 'a': case 's' : case 'd' : case 'f' : case 'g' :
                case 'h': case 'j' : case 'k' : case 'l' : case 'z' :
                case 'x': case 'c' : case 'v' : case 'b' : case 'n' :
                case 'm': case '_' : case 'Q':
                case 'W': case 'E' : case 'R' : case 'T' : case 'Y' :
                case 'U': case 'I' : case 'O' : case 'P' : case 'A' :
                case 'S': case 'D' : case 'F' : case 'G' : case 'H' :
                case 'J': case 'K' : case 'L' : case 'Z' : case 'X' :
                case 'C': case 'V' : case 'B' : case 'N' : case 'M' :
                    word += symbol;
                    prevsymbol = SYM_LETTER;
                    break;
                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }else if(prevsymbol == SYM_QUOTE)
            {
                switch (symbol)
                {
                case ',' : case '.' : case '(' : case ')' : case '1' :
                case '2' : case '3' : case '4' : case '5' : case ' ' :
                case '6' : case '7' : case '8' : case '9' : case '0' :
                case 'q' : case 'w' : case 'e' : case 'r' : case 't' :
                case 'y' : case 'u' : case 'i' : case 'o' : case 'p' :
                case 'a' : case 's' : case 'd' : case 'f' : case 'g' :
                case 'h' : case 'j' : case 'k' : case 'l' : case 'z' :
                case 'x' : case 'c' : case 'v' : case 'b' : case 'n' :
                case 'm' : case '_' : case ':' : case 'Q':  case '+' :
                case 'W' : case 'E' : case 'R' : case 'T' : case 'Y' :
                case 'U' : case 'I' : case 'O' : case 'P' : case 'A' :
                case 'S' : case 'D' : case 'F' : case 'G' : case 'H' :
                case 'J' : case 'K' : case 'L' : case 'Z' : case 'X' :
                case 'C' : case 'V' : case 'B' : case 'N' : case 'M' :
                case '-' : case '=' : case '<' : case '>' : case '\\' :
                case '/' : case '?' : case '%':
                    word += symbol;
                    prevsymbol = SYM_QUOTE;
                    break;

                case '"' :
                    word += symbol;
                    m_Words.append(word);
                    word.clear();
                    prevsymbol = SYM_SPACE;
                    break;

                default :
                    prevsymbol = SYM_ERROR;
                    break;
                }
            }
         }

         if(prevsymbol == SYM_ERROR)
         {
//             QMessageBox::critical(0,tr("Error"),tr("Illegal symbol of the line: %1 column: %2").arg(linenum+1).arg(colnum));
             ok=false;
             break;
         }
    }

    if(ok)
    {
for (i = 0; i < m_Words.size(); ++i)
{
//qDebug() << m_Words.at(i);
    message("<font color=blue>"+m_Words.at(i)+"</font><br>");
}
    }
    else
    {
          message("<font color=red>"+tr("Error : Illegal symbol of the line: %1 column: %2").arg(linenum+1).arg(colnum)+"</font><br>");
    }

    return ok;
}


///////////////////////////////////////////////////////////////////////////////
bool QCInter::checkSyntaxAanalysis(void)
{
    bool ok = true;
    int i,linenum;
    QByteArray word;

    if (!m_Words.empty())
    {
/*
 for (int i=0;i<100;i++){
  tab_int[i]="";
  tab_float[i]="";
  tab_bool[i]="";
 }
 ile_int=0;
 ile_float=0;
 ile_bool=0;
*/
 word.clear();
 linenum = 1;

 int vartype;

 int k;
 float kk;
 int cos=0;
 int co=0,step=1;
 bool bgn_co=false;
 int brackets = 0;

 for (int i=0;i<m_Words.size();i++)
 {
     word=m_Words.at(i);

     if(word[0] == '\n')
     {
        linenum++;
     }

     if(i == 0)
     {
         if(word != "void")
         {
             message("<font color=red>Script must be begin with \"void main(void)\" </font><br>");
             ok=false;
             break;
         }
     }
     if (i == 1)
     {
         if (word != "main")
         {
             message("<font color=red>Script must be begin with \"void main(void)\" </font><br>");
             ok=false;
             break;
         }
     }
     if (i == 2)
     {
         if (word != "(")
         {
             message("<font color=red>Script must be begin with \"void main(void)\" </font><br>");
             ok=false;
             break;
         }
     }
     if(i == 3)
     {
         if(word != "void")
         {
             message("<font color=red>Script must be begin with \"void main(void)\" </font><br>");
             ok=false;
             break;
         }
     }

     if (i == 4)
     {
         if(word != ")")
         {
             message("<font color=red>Script must be begin with \"void main(void)\" </font><br>");
             ok=false;
             break;
         }
         else
         {
             bgn_co=true;
             continue;
         }
     }
     if(bgn_co)
     {

        switch (co)
        {

        case 0 : //означава, че предишната операция е бил тълкувана правилно и да започне новa
                if(word=="char") {co=1;step=1;vartype=TYPE_NOTHING;}
                else if (word=="if") {co=4;step=1;vartype=TYPE_NOTHING;}
                else if (word=="for") {co=6;step=1;vartype=TYPE_NOTHING;}
                else if (word=="{") {brackets++;vartype=TYPE_NOTHING;co=0;step=1;}//pom="nawias_otw";}
                else if (word=="}") {brackets--;vartype=TYPE_NOTHING;co=0;step=1;}//pom="nawias_zam";}
                else if (((word[0]>='a')&&(word[0]<='z'))||((word[0]>='A')&&(word[0]<='Z')))
                {
                    vartype = getType(word);
                    if(vartype != TYPE_NOTHING)
                    {
                        step=1;
                        co=5;
                    }
                    else
                    {
                        message("<font color=red>"+tr("Error : Don`t declared variable %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                        ok=false;
                    }
                }
                break;

        case 1 : // new  char variable
            if(step == 1)
            {
                if(((word[0]>='a')&&(word[0]<='z'))||((word[0]>='A')&&(word[0]<='Z')))
                {
                    if(getType(word) == TYPE_NOTHING)
                    {
                        m_CharVars.insert(word,0);
                        step=2;
                    }
                    else
                    {
                        message("<font color=red>"+tr("Error : Do not declared variable %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                        ok=false;
                    }
                }
                else
                {
                    message("<font color=red>"+tr("Error : Expected variable name of the line %1").arg(linenum)+"</font><br>");
                    ok=false;
                }
            }
            else if(step==2)
            {
                if(word==";"){co=0;step=1;}
                else if (word==","){step=1;}
                else if (word=="="){step=3;}
                else
                {
                    message("<font color=red>"+tr("Error : Expected ';' lub ',' lub '=' of the line %1").arg(linenum)+"</font><br>");
                    ok=false;
                }
            }
            else if (step==3)
            {
                //the type of values that the user wants to assign to a variable.
                bool ok;
                int val = word.toInt(&ok, 10);
                Q_UNUSED(val);
                if(!ok)
                {
                    message("<font color=red>"+tr("Error : Expected number of the line %1").arg(linenum)+"</font><br>");
                    ok=false;
                }
                step=4;
            }
            else if(step==4)
            {
                if (word==";"){co=0;step=1;}        // ; - end declaration
                else if (word==","){step=1;}        // , - next declaration
                else
                {
                    message("<font color=red>"+tr("Error : Expected ';' or ',' and received: %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                    ok=false;
                }
            }
            else
            {
                message("<font color=red>"+tr("Error : Not expected %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                ok=false;
            }
            break;

        case 5 : //encountered variable -------------------------------------------------------------------------------------------
            if (step==1)
            {
                if (((word=="++")||(word=="--"))/*&&((vartype==TYPE_CHAR)||(vartype=="float"))*/)
                {
                    step=3;
                }else if(((word=="+=")||(word=="-=")||(word=="*=")||(word=="/=")))
                {
                    step=2;
                    cos=0;
                }else if (word=="=")
                {
                    step=2;
                    cos=1;
                }
                else
                {
                    message("<font color=red>"+tr("Error : Expected -=,+=,*=,/=,=,++,-- and received: %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                    ok=false;
                }
            }else if(step==2)
            {
                if(((word[0]>='a')&&(word[0]<='z'))||((word[0]>='A')&&(word[0]<='Z')))
                {
                    if(getType(word) != TYPE_NOTHING)
                    {
                        if (cos==0)
                            step=3;
                        else
                            step=4;
                    }
                    else
                    {
                        message("<font color=red>"+tr("Error : Do not declared variable: : %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                        ok=false;
                    }
                }
                else
                {

                    bool ok;
                    char ch = (char)word.toInt(&ok, 10);
                    Q_UNUSED(ch);
                    if(ok)
                    {
                        if (cos==0)
                            step=3;
                        else
                            step=4;
                    }
                    else
                    {
                        message("<font color=red>"+tr("Error : Type mismatch or is not a number : %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                        ok=false;
                    }
                }

            }else if(step==3)
            {
                if (word==";")
                {
                    co=0;step=1; vartype=TYPE_NOTHING;
                }
                else
                {
                    message("<font color=red>"+tr("Error : Expected ; and received: %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                    ok=false;
                }
            }
            else if(step==4)
            {
                if((word=="/")||(word=="*")||(word=="-")||(word=="+"))
                {
                    step=2;
                }
                else if(word==";")
                {
                    step=1;co=0;
                }
                else
                {
                    bool ok;
                    char ch = (char)word.toInt(&ok, 10);
                    Q_UNUSED(ch);
                    if(!ok)
                    {
                        message("<font color=red>"+tr("Error : Expected +,-,*,/ or types of non-compliance were received: %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                        ok=false;
                    }
                }
            }
            else
            {
                message("<font color=red>"+tr("Error : Unexpected : %1 of the line %2").arg(QString(word)).arg(linenum)+"</font><br>");
                ok=false;
            }
            break;
#ifdef UNDER_DEVELOP
        case 4 : // napotkano if ------------------------------------------------------------------------------------------
                if (krok==1) {    //pierwszy musi być nawaist otwierający warunek
                   if (wyraz=="(") {krok=2;}
                        else {okno->Lines->Add("Oczekiwano '(' gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                                ok=false;}
                 }
                 else
                 if (krok==2){   // tera może wystapić albo zmienna albo liczba albo wartość true lub false
                   if ((wyraz=="true")||(wyraz=="false")){krok=3;}
                   else if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                             bool jest=false;
                             for (int t=0;t<ile_int;t++){
                                        if (tab_int[t]==wyraz){jest=true;
                                                                pom="int";
                                                                break;}
                                        }
                             if (!jest){
                                 for (int t=0;t<ile_float;t++){
                                        if (tab_float[t]==wyraz){jest=true;
                                                                pom="float";
                                                                break;}
                                        }
                                }
                             if (!jest){
                                 for (int t=0;t<ile_bool;t++){
                                        if (tab_bool[t]==wyraz){jest=true;
                                                                pom="bool";
                                                                break;}
                                        }
                                }
                             if (jest) {krok=4;}
                                else {okno->Lines->Add("Nie zadeklarowana zmienna : '"+wyraz+"' w linni : "+linia);
                                        ok=false;}
                            }
                   else {
                         try {
                           k=StrToFloat(wyraz);
                           krok=4;
                           }catch (Exception &e){
                                       okno->Lines->Add("Oczekiwano wartości lub zmiennej gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}

                        }
                 }
                 else if (krok==3){  // ten krok jest wykonywano gdy wystapila wartość true lub false
                    if (wyraz==")"){krok=7;}
                       else {okno->Lines->Add("Oczekiwano ')' gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                                ok=false;}
                 }
                 else if (krok==4){ // ten krok gdy wystaąpila zmienna    tzn ze oczekuje teraz
                    if ((wyraz==")")&&(pom=="bool")){krok=7;}        // albo nawias zamykający warunek
                    else if (((wyraz=="==")||(wyraz==">=")||              // albo operatora warunkowego
                             (wyraz=="<=")||(wyraz==">")||
                             (wyraz=="<")||(wyraz=="!="))&&(pom!="bool")){krok=5;}
                    else if (((wyraz=="==")||(wyraz=="!="))&&(pom=="bool")){krok=5;}  //jeżeli była zmienna typu bool
                       else {okno->Lines->Add("Oczekiwano operatora warunku gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                                ok=false;}
                 }
                 else if (krok==5){
                   if ((pom=="bool")&&((wyraz=="true")||(wyraz=="false"))){krok=6;}
                   else if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                             int tmp;
                             tmp=typ(wyraz);
                             if ((tmp==3)&&(pom=="bool")) {krok=6;}
                             else if (((tmp==1)||(tmp==2))&&(pom!="bool")) {krok=6;}
                             else {okno->Lines->Add("Nie zadeklarowana zmienna lub niezgodność typów : '"+wyraz+"' w linni : "+linia);
                                        ok=false;}
                            }
                   else {
                         try {
                           k=StrToFloat(wyraz);
                           krok=6;
                           }catch (Exception &e){
                                       okno->Lines->Add("Oczekiwano wartości lub zmiennej gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                        }
                 }
                 else if (krok==6){
                   if ((pom=="bool")&&(wyraz==")")){krok=7;}
                   else if (wyraz==")"){krok=7;}
                   else if ((wyraz=="+")||(wyraz=="-")||(wyraz=="*")||(wyraz=="/")){krok=5;}
                   else {
                         try {
                           k=StrToFloat(wyraz);
                           if (k<0) krok=6;
                                else {
                                okno->Lines->Add("Oczekiwano +,-,*,/,) gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                                ok=false; }
                           }catch (Exception &e){
                                okno->Lines->Add("Oczekiwano +,-,*,/,) gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                                ok=false;}
                        }
                 }
                 else
                if(krok==7){
                   if (wyraz=="{"){krok=1;co=0;brackets++;}
                   else if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                      int temp;
                      temp=typ(wyraz);
                      if ((temp>=1)&&(temp<=3)) {krok=1;co=5;
                                                if (temp==1) pom="int";
                                                else if(temp==2)pom="float";
                                                else if(temp==3)pom="bool";}
                      else {okno->Lines->Add("Oczekiwano { lub operacji przypisania a otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}

                      }
                      else {okno->Lines->Add("Oczekiwano { lub operacji przypisania a otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                }
                 else {okno->Lines->Add("Nie oczekiwano: '"+wyraz+"' w linni : "+linia);
                        ok=false;}

                break;

        case 6 : // napotkano for --------------------------------------------------------------------------------------------
                if (krok==1) {  //pierwszy musi być nawaist otwierający
                   if (wyraz=="(") {krok=2;}
                        else {okno->Lines->Add("Oczekiwano '(' gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                                ok=false;}
                }
                else
                if (krok==2){
                   if (wyraz=="int"){krok=3;pom="int";}
                   else if (wyraz=="float"){krok=3;pom="float";}
                   else if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                             bool jest=false;
                             for (int t=0;t<ile_int;t++){
                                        if (tab_int[t]==wyraz){ jest=true;
                                                                pom="int";
                                                                break;}
                                        }
                             if (!jest){
                                 for (int t=0;t<ile_float;t++){
                                        if (tab_float[t]==wyraz){ jest=true;
                                                                  pom="float";
                                                                  break;}
                                        }
                                }

                             if (jest) {krok=4;}
                                else { okno->Lines->Add("Nie zadeklarowana zmienna : '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                            }
                }
                else
                if (krok==3){
                     if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                      int temp;
                      temp=typ(wyraz);
                      if (temp==4){
                        if (pom=="int"){
                           tab_int[ile_int]=wyraz;
                           ile_int++;
                        }
                        if (pom=="float"){
                           tab_float[ile_float]=wyraz;
                           ile_float++;
                        }
                        krok=4;
                      }
                      else { okno->Lines->Add("Zmienna o nazwie : '"+wyraz+"' już istnieje w linni : "+linia);
                                       ok=false;}
                     }
                     else { okno->Lines->Add("Spodziewano się nazwy zmiennej a otrzymano : '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                }
                else
                if (krok==4){
                    if (wyraz=='='){
                        krok=5;
                        }
                     else { okno->Lines->Add("Spodziewano się operacji przypisania w linni : "+linia);
                                       ok=false;}
                }
                else
                if (krok==5){
                     if ((wyraz[1]>='0')&&(wyraz[1]<='9')) {
                        if (pom=="int"){
                           try {
                           k=StrToInt(wyraz);
                           }catch (Exception &e){
                                okno->Lines->Add("Spodziewano się liczby całkowitej a otrzymano : '"+wyraz+"' w linni : "+linia);
                                       ok=false;
                                }
                        }
                        if (pom=="float"){
                           try {
                           kk=StrToFloat(wyraz);
                           }catch (Exception &e){
                                okno->Lines->Add("Spodziewano się liczby rzeczywistej a otrzymano : '"+wyraz+"' w linni : "+linia);
                                       ok=false;
                                }
                        }
                        krok=6;
                     }
                     else { okno->Lines->Add("Spodziewano się liczby a otrzymano : '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                }
                else
                if (krok==6){
                    if (wyraz==';'){
                        krok=7;
                        }
                     else { okno->Lines->Add("Spodziewano się średnika a otrzymano : '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                }
                else
                if (krok==7){
                    if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                             bool jest=false;
                             for (int t=0;t<ile_int;t++){
                                        if (tab_int[t]==wyraz){ jest=true;
                                                                pom="int";
                                                                break;}
                                        }
                             if (!jest){
                                 for (int t=0;t<ile_float;t++){
                                        if (tab_float[t]==wyraz){ jest=true;
                                                                  pom="float";
                                                                  break;}
                                        }
                                }

                             if (jest) {krok=8;}
                                else { okno->Lines->Add("Nie zadeklarowana zmienna : '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                            }
                   else {okno->Lines->Add("Oczekiwano nazwy zmiennej gdy otrzymano: '"+wyraz+"' w linni : "+linia);
                        ok=false;}
                }
                else
                if (krok==8){
                   if ((wyraz==">")||(wyraz=="<")||(wyraz=="<=")||(wyraz==">=")||(wyraz=="!=")||(wyraz=="==")) {
                            krok=9;
                            }
                   else {okno->Lines->Add("Oczekiwano >,<,<=,>=,!=,== a otrzymano: '"+wyraz+"' w linni : "+linia);
                        ok=false;}
                }
                else
                if (krok==9){
                    if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                             bool jest=false;
                             for (int t=0;t<ile_int;t++){
                                        if (tab_int[t]==wyraz){ jest=true;
                                                                break;}
                                        }
                             if (!jest){
                                 for (int t=0;t<ile_float;t++){
                                        if (tab_float[t]==wyraz){ jest=true;
                                                                  break;}
                                        }
                                }

                             if (jest) {krok=10;}
                                else { okno->Lines->Add("Nie zadeklarowana zmienna : '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                            }
                   else {
                        try {
                           kk=StrToFloat(wyraz);
                           krok=10;
                           }catch (Exception &e){
                                       okno->Lines->Add("Oczekiwano nazwy zmiennej lub liczby a otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                        }
                }
                else
                if (krok==10){
                    if (wyraz>=";"){
                            krok=11;
                            }
                   else {
                        okno->Lines->Add("Oczekiwano nazwy zmiennej lub liczby a otrzymano: '"+wyraz+"' w linni : "+linia);
                        ok=false;
                        }
                }
                else
                if (krok==11){
                    if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                             bool jest=false;
                             for (int t=0;t<ile_int;t++){
                                        if (tab_int[t]==wyraz){ jest=true;
                                                                pom="int";
                                                                break;}
                                        }
                             if (!jest){
                                 for (int t=0;t<ile_float;t++){
                                        if (tab_float[t]==wyraz){ jest=true;
                                                                  pom="float";
                                                                  break;}
                                        }
                                }

                             if (jest) {krok=12;}
                                else { okno->Lines->Add("Nie zadeklarowana zmienna : '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                            }
                   else {
                        okno->Lines->Add("Oczekiwano nazwy zmiennej a otrzymano: '"+wyraz+"' w linni : "+linia);
                        ok=false;}
                }
                else
                if (krok==12){
                    if (((wyraz=="++")||(wyraz=="--"))&&((pom=="int")||(pom=="float"))){krok=16;}
                    else if((wyraz=="+=")||(wyraz=="-=")||
                            (wyraz=="*=")||(wyraz=="/=")) {
                                krok=13;
                            }
                   else if (wyraz=="="){
                                krok=13;
                   }
                   else {okno->Lines->Add("Oczekiwano -=,+=,*=,/=,=,++,-- a otrzymano: '"+wyraz+"' w linni : "+linia);
                        ok=false;}
                }
                else
                if(krok==13){
                   if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                             int tmp;
                             tmp=typ(wyraz);

                             if ((tmp==1)&&(pom=="int")) {krok=16;}
                             else if (tmp==2) {krok=16;}
                                else { okno->Lines->Add("Nie zadeklarowana zmienna : '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                            }
                   else {
                        if (pom=="int"){
                        try {
                           k=StrToFloat(wyraz);
                           krok=16;
                           }catch (Exception &e){
                                       okno->Lines->Add("Oczekiwano nazwy zmiennej lub liczby lub niezgodność typów, otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                        }
                        else if (pom=="float"){
                        try {
                           kk=StrToFloat(wyraz);
                           krok=16;
                           }catch (Exception &e){
                                       okno->Lines->Add("Oczekiwano nazwy zmiennej lub liczby lub niezgodność typów, otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                       } }

                }
                else
                if(krok==16){
                   if (wyraz==")"){krok=17;}
                      else {okno->Lines->Add("Oczekiwano ) a otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                }
                else
                if(krok==17){
                   if (wyraz=="{"){krok=1;co=0;brackets++;}
                   else if (((wyraz[1]>='a')&&(wyraz[1]<='z'))||((wyraz[1]>='A')&&(wyraz[1]<='Z'))) {
                      int temp;
                      temp=typ(wyraz);
                      if ((temp>=1)&&(temp<=3)) {krok=1;co=5;
                                                 if (temp==1) pom="int";
                                                 else if(temp==2)pom="float";
                                                 else if(temp==3)pom="bool";}
                      else {okno->Lines->Add("Oczekiwano { lub operacji przypisania a otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}

                      }
                      else {okno->Lines->Add("Oczekiwano { lub operacji przypisania a otrzymano: '"+wyraz+"' w linni : "+linia);
                                       ok=false;}
                }
                else {okno->Lines->Add("Nie oczekiwano: '"+wyraz+"' w linni : "+linia);
                        ok=false;}


                break;
#endif
        }//end switch co

       }//if(bgn_co)
#ifdef UNDER_DEVELOP
    if ((i==Memo1->Lines->Count-1)&&(wyraz!="}"))
                        {okno->Lines->Add("Oczekiwano } w linni : "+linia);
                         ok=false;}
#endif

    if (!ok) {break;}
 }//for m_Words


#ifdef UNDER_DEVELOP
 if (ok)
 {

    if (brackets>0){okno->Lines->Add("Oczekiwano '}' w linii "+IntToStr(SynEdit1->Lines->Count+1));}

    if (brackets==0) {okno->Lines->Add("Sprawdzanie składni zakończone powodzeniem.");sprawdzone=true; }

    if (brackets<0) {okno->Lines->Add("Nie oczekiwano '}' w linii "+IntToStr(SynEdit1->Lines->Count));}
 }
#endif
}//koniec if ok

    return ok;
}


void QCInter::execute(int pos, int end)
{
    int i=pos;
    QByteArray word;
//  int linenum;
//  exec=true;

    word.clear();

    while (i<end)
    {
        word=m_Words.at(i);
qDebug() << word;
        if(word=="char")
        {
            i = declarationChar(i+1);
        }else if(((word[0]>='a')&&(word[0]<='z'))||((word[0]>='A')&&(word[0]<='Z')))
        {
            int typ;
            typ=getType(word);
            i = execExpression(i+1,typ,word);
        }
        i++;
    }
}


int QCInter::declarationChar(int bgn)
{
    QByteArray word,varname;
    int vartype=TYPE_NOTHING;
    int step=1;
    int i;

    for (i = bgn; i < m_Words.size(); ++i)
    {
        word=m_Words.at(i);
        if (step==1)
        {
            varname = word;
            vartype=getType(varname);
            step=2;
        }
        else if(step==2)
        {
            if (word==",") step=1;
            else if (word=="=") step=3;
            else if (word==";") break;
        }
        else if (step==3)
        {
            bool ok;
            int val = (char)word.toInt(&ok, 10);
            if(ok)
            {
                m_CharVars[varname]=val;
            }
            step=2;
        }
    }
    return i;
}



int QCInter::execExpression(int bgn,int typ,QByteArray varname)
{

    bool errexec = false;
    int i=bgn;
    QByteArray word;
    QList <QByteArray> words;
    char vartotalvalue = 0;

    while(true)
    {
        words.append(m_Words.at(i));

        word=m_Words.at(i);

        if (word == ";")
            break;

        i++;
    }


qDebug() << words;

    if(words.at(0) == "=")
    {
        int q=1,step=1;

        char oper;
        QByteArray w;
        if(typ == TYPE_CHAR)
        {
            int var1,var2;
            while (q < words.count())
            {
                w=words.at(q);
qDebug() << w;
                if (step==1)
                {
                    if (((w[0]>='a')&&(w[0]<='z'))||((w[0]>='A')&&(w[0]<='Z')))
                    {
                        var1=m_CharVars[w];
                        step=2;
                    }
                    else
                    {
                        bool ok;
                        var1 = w.toInt(&ok, 10);
                        if(ok)
                        {
                            step=2;
                        }
                        else
                        {
                            message("<font color=red>"+tr("Error : Unexpected symbol : %1 of the line X").arg(QString(word))/*.arg(linenum)*/+"</font><br>");
                            errexec = true;
                        }
                    }
                }else if(step==2)
                {
                    oper=w[0];

//                    if (oper=='-')
//                    	q--;
                    if (oper==';')
                    	vartotalvalue=var1;

                    step=3;
                }
                else if (step==3)
                {
                    switch (oper)
                    {
                    case '+' : case '-' :
                    case '*' : case '/' :
                    	vartotalvalue=var1;
                    	if (((w[0]>='a')&&(w[0]<='z'))||((w[0]>='A')&&(w[0]<='Z')))
                    	{
                    		var2=m_CharVars[w];
                            step=2;
                        }
                        else
                        {
                            bool ok;
                            var2 = w.toInt(&ok, 10);
                            if(ok)
                            {
                                step=2;
                            }
                            else
                            {
                                message("<font color=red>"+tr("Error : Unexpected symbol : %1 of the line X").arg(QString(word))/*.arg(linenum)*/+"</font><br>");
                                errexec = true;
                            }
                        }
                    	 if(!errexec)
                    	 {
							 if (oper=='+')
							 {
								 var1=var1+var2;
							 }else if (oper=='-')
							 {
								 var1=var1-var2;
							 }else if (oper=='*')
							 {
								 var1=var1*var2;
							 }else if (oper=='/')
							 {
								 if(var2 != 0)
								 {
									 var1=var1/var2;
								 }else
								 {
									 message("<font color=red>"+tr("Error : Divide by zero of the line X")/*.arg(linenum)*/+"</font><br>");
									 errexec = true;
								 }
							 }
                    	 }
						 break;
                    }
                }//koniec if co==3
                if(errexec)
                	break;
                q++;

            }//while

            if(!errexec)
              m_CharVars[varname]=vartotalvalue;
        }//if typ==1
    }


#if 0

else if (temp->Lines->operator [](0)=="++"){//pocz№tek if ++
      switch (typ){
      case 1:
             wart_int[daj_poz(nazwa,typ)]++;
             break;
      case 2:
             wart_float[daj_poz(nazwa,typ)]++;
             break;
      }
} //koniec if ++

else if (temp->Lines->operator [](0)=="--"){//pocz№tek if ++
      switch (typ){
      case 1:
             wart_int[daj_poz(nazwa,typ)]--;
             break;
      case 2:
             wart_float[daj_poz(nazwa,typ)]--;
             break;
      }
} //koniec if ++

else if (temp->Lines->operator [](0)=="+="){//pocz№tek if +=
      pom=temp->Lines->operator [](1);
      switch (typ){
      case 1:
             if (((pom[1]>='a')&&(pom[1]<='z'))||((pom[1]>='A')&&(pom[1]<='Z'))) {
                wart_int[daj_poz(nazwa,typ)]+=daj_int(pom);
             }
             else {
                try{
                    wart_int[daj_poz(nazwa,typ)]+=StrToInt(pom);
                }catch(Exception &e){}
             }
             break;
      case 2:
             int pom_typ;
             if (((pom[1]>='a')&&(pom[1]<='z'))||((pom[1]>='A')&&(pom[1]<='Z'))) {
                pom_typ=this->typ(pom);
                if (pom_typ==1) wart_float[daj_poz(nazwa,typ)]+=daj_int(pom);
                else if (pom_typ==2) wart_float[daj_poz(nazwa,typ)]+=daj_float(pom);
             }
             else {
                try{
                    wart_float[daj_poz(nazwa,typ)]+=StrToFloat(pom);
                }catch(Exception &e){}
             }
             break;
      }

} //koniec if +=

else if (temp->Lines->operator [](0)=="-="){//pocz№tek if -=
      pom=temp->Lines->operator [](1);
      switch (typ){
      case 1:
             if (((pom[1]>='a')&&(pom[1]<='z'))||((pom[1]>='A')&&(pom[1]<='Z'))) {
                wart_int[daj_poz(nazwa,typ)]-=daj_int(pom);
             }
             else {
                try{
                    wart_int[daj_poz(nazwa,typ)]-=StrToInt(pom);
                }catch(Exception &e){}
             }
             break;
      case 2:
             int pom_typ;
             if (((pom[1]>='a')&&(pom[1]<='z'))||((pom[1]>='A')&&(pom[1]<='Z'))) {
                pom_typ=this->typ(pom);
                if (pom_typ==1) wart_float[daj_poz(nazwa,typ)]-=daj_int(pom);
                else if (pom_typ==2) wart_float[daj_poz(nazwa,typ)]-=daj_float(pom);
             }
             else {
                try{
                    wart_float[daj_poz(nazwa,typ)]-=StrToFloat(pom);
                }catch(Exception &e){}
             }
             break;
      }

} //koniec if -=

else if (temp->Lines->operator [](0)=="*="){//pocz№tek if *=
      pom=temp->Lines->operator [](1);
      switch (typ){
      case 1:
             if (((pom[1]>='a')&&(pom[1]<='z'))||((pom[1]>='A')&&(pom[1]<='Z'))) {
                wart_int[daj_poz(nazwa,typ)]*=daj_int(pom);
             }
             else {
                try{
                    wart_int[daj_poz(nazwa,typ)]*=StrToInt(pom);
                }catch(Exception &e){}
             }
             break;
      case 2:
             int pom_typ;
             if (((pom[1]>='a')&&(pom[1]<='z'))||((pom[1]>='A')&&(pom[1]<='Z'))) {
                pom_typ=this->typ(pom);
                if (pom_typ==1) wart_float[daj_poz(nazwa,typ)]*=daj_int(pom);
                else if (pom_typ==2) wart_float[daj_poz(nazwa,typ)]*=daj_float(pom);
             }
             else {
                try{
                    wart_float[daj_poz(nazwa,typ)]*=StrToFloat(pom);
                }catch(Exception &e){}
             }
             break;
      }

} //koniec if *=

else if (temp->Lines->operator [](0)=="/="){//pocz№tek if /=
      pom=temp->Lines->operator [](1);
      switch (typ){
      case 1:
             if (((pom[1]>='a')&&(pom[1]<='z'))||((pom[1]>='A')&&(pom[1]<='Z'))) {
                wart_int[daj_poz(nazwa,typ)]/=daj_int(pom);
             }
             else {
                try{
                    wart_int[daj_poz(nazwa,typ)]/=StrToInt(pom);
                }catch(Exception &e){}
             }
             break;
      case 2:
             int pom_typ;
             if (((pom[1]>='a')&&(pom[1]<='z'))||((pom[1]>='A')&&(pom[1]<='Z'))) {
                pom_typ=this->typ(pom);
                if (pom_typ==1) wart_float[daj_poz(nazwa,typ)]/=daj_int(pom);
                else if (pom_typ==2) wart_float[daj_poz(nazwa,typ)]/=daj_float(pom);
             }
             else {
                try{
                    wart_float[daj_poz(nazwa,typ)]/=StrToFloat(pom);
                }catch(Exception &e){}
             }
             break;
      }
} //koniec if +=
#endif



    return i;

}
