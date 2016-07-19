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

#ifndef __SERIAL_H
#define __SERIAL_H

#include <QtGui>
#include <QIODevice>

#ifdef WIN32

#include <windows.h>

#else

#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#endif

#define __BUFF_SIZE 4096

class QEvent;
enum {
    NUL,  //  00
    SOH,  //  01
    STX,  //  02
    ETX,  //  03
    EOT,  //  04
    ENQ,  //  05
    ACK,  //  06
    BEL,  //  07
    BS,   //  08
    HT,   //  09
    LF,   //  0A
    VT,   //  0B
    FF,   //  0C
    CR,   //  0D
    SO,   //  0E
    SI,   //  0F
    DLE,  //  10
    DC1,  //  11
    DC2,  //  12
    DC3,  //  13
    DC4,  //  14
    NAK,  //  15
    SYN,  //  16
    ETB,  //  17
    CAN,  //  18
    EM,   //  19
    SUB,  //  1A
    ESC,  //  1B
    FS,   //  1C
    GS,   //  1D
    RS,   //  1E
    US   //  1F
    };

///////////////////////////////////////////////////////////////////////////////
class SerialPort : public QObject
{
		Q_OBJECT

	public:
		SerialPort ( QObject *parent );
		~SerialPort ();

		bool openDevice ( const char *device );
		bool configureDevice ( int BaudRate, char DataBits, char StopBits, char Parity, char FlowCtrl );
		void closeDevice();
		void writeByte ( const char byte );
        void writeBuffer ( const char *buff, int blen);
        void FlushBuffers();

		void drainBytes();

		static QStringList devices();

		QByteArray  m_Buff;
		QMutex		m_Mutex;

#if defined(WIN32)
		HANDLE		m_hDevice;
		char		m_CharToWrite;

		OVERLAPPED	m_SendOverlapped;

		HANDLE		m_hRecvEvent;
		HANDLE		m_hRecvThread;
		OVERLAPPED	m_RecvOverlapped;

		bool		m_QuitThreads;

		DCB			m_oldTermParams;
#else
		QSocketNotifier	*m_Notifier;
		int				m_Device;
		struct termios	m_oldTermParams;
#endif
		void  GetDeviceStatus ( bool *CTS, bool *DSR,bool *DCD ,bool *RI);
		void  SetRTS ( bool RTS );
		void  SetDTR ( bool DTR );

	signals:
		void write_ready();
		void read_ready();
		void error();

	protected slots:
		void readReady ( int );

	protected:
		bool event ( QEvent *e );
};
#endif
