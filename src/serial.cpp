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
#include "serial.h"
#ifdef WIN32
#include <setupapi.h>
#include <winioctl.h>
#include <initguid.h>

const unsigned SERIAL_TIMEOUT   = 1000;
#define SERIAL_RD_COMPLETE  ( (DWORD)(QEvent::User+0) )

///////////////////////////////////////////////////////////////////////////////
static DWORD __stdcall RecvThread ( LPVOID lpParameter )
{
	DWORD	bytesRead;
	DWORD	res;
	DWORD	dwEvtMask;
	DWORD	dwLength;
	DWORD	dwErrors,dwErrorFlags,dwError;
	COMSTAT	ComStat;
	BOOL	ReadStat;
	char	acRSStr[4096];

	SerialPort *p = ( SerialPort* ) lpParameter;

	while ( TRUE )
	{
		if ( p->m_QuitThreads )
		{
			break;
		}

		if ( p->m_hDevice == INVALID_HANDLE_VALUE )
		{
			Sleep(500);
			continue;
		}

		dwEvtMask = 0;
		res = WaitCommEvent ( p->m_hDevice, &dwEvtMask, 0 );

		ClearCommError ( p->m_hDevice, &dwErrors, &ComStat );
		bytesRead = ComStat.cbInQue;

		if ( ( dwEvtMask & EV_RXCHAR ) == EV_RXCHAR )
		{
			memset ( acRSStr, 0, sizeof ( acRSStr ) );
			ReadStat = ReadFile ( p->m_hDevice, acRSStr, bytesRead, &dwLength, &p->m_RecvOverlapped );

			if ( !ReadStat )
			{
				if ( GetLastError() == ERROR_IO_PENDING )
				{
					while ( !GetOverlappedResult ( p->m_hDevice, &p->m_RecvOverlapped, &dwLength, TRUE ) )
					{
						dwError = GetLastError();
						if ( dwError == ERROR_IO_INCOMPLETE )
						{
							continue;
						}
						else
						{
							ClearCommError ( p->m_hDevice, &dwErrorFlags, &ComStat ) ;
							break;
						}
					}
				}
				else
				{
					dwLength = 0 ;
					ClearCommError ( p->m_hDevice, &dwErrorFlags, &ComStat ) ;
				}
				continue;
			}

			if ( dwLength )
			{
				p->m_Mutex.lock();
                for ( int unsigned i = 0; i < dwLength; i++ )
				{
					p->m_Buff.append ( acRSStr[i] );
				}
				p->m_Mutex.unlock();
				QApplication::postEvent ( p, new QEvent ( ( QEvent::Type ) SERIAL_RD_COMPLETE ) );
			}
		}

		if ( ( dwEvtMask & EV_ERR ) == EV_ERR )
		{
			dwErrors = EV_ERR;
			ClearCommError ( p->m_hDevice,&dwErrors, NULL );
		}
/*
        if ( ( dwEvtMask & EV_CTS ) == EV_CTS )
        {
        }

        if ( ( dwEvtMask & EV_DSR ) == EV_DSR )
        {
        }

        if ( ( dwEvtMask & EV_RING ) == EV_RING )
        {
        }
*/
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
SerialPort::SerialPort ( QObject *parent )
{
	m_hDevice = INVALID_HANDLE_VALUE;
	m_Buff.reserve ( __BUFF_SIZE );

	m_QuitThreads = FALSE;

	memset ( &m_SendOverlapped, 0, sizeof ( OVERLAPPED ) ) ;
	m_SendOverlapped.hEvent = CreateEvent ( NULL,   // no security
											TRUE,                   // explicit reset req
											FALSE,                    // initial event reset
											NULL ) ;                  // no name

	memset ( &m_RecvOverlapped, 0, sizeof ( OVERLAPPED ) ) ;
	m_RecvOverlapped.hEvent = CreateEvent ( NULL,   // no security
											TRUE,                   // explicit reset req
											FALSE,                    // initial event reset
											NULL ) ;                  // no name

	m_hRecvEvent = CreateEvent ( NULL, FALSE, FALSE, NULL );

	DWORD RecvThreadId;
	m_hRecvThread = CreateThread ( NULL, 0, RecvThread, this, 0, &RecvThreadId );
}

///////////////////////////////////////////////////////////////////////////////
SerialPort::~SerialPort()
{
    m_QuitThreads = TRUE;

	closeDevice();

	SetEvent ( m_hRecvEvent );

	CloseHandle ( m_hRecvEvent );

	WaitForSingleObject ( m_hRecvThread, INFINITE );

	CloseHandle ( m_RecvOverlapped.hEvent );
	CloseHandle ( m_SendOverlapped.hEvent );
}

///////////////////////////////////////////////////////////////////////////////
void SerialPort::closeDevice()
{
	if ( m_hDevice != INVALID_HANDLE_VALUE )
	{
		SetCommState ( m_hDevice, &m_oldTermParams );
		CloseHandle ( m_hDevice );
		m_hDevice = INVALID_HANDLE_VALUE;
		m_Buff.clear();
	}
}

///////////////////////////////////////////////////////////////////////////////
void SerialPort::readReady ( int )
{
}

///////////////////////////////////////////////////////////////////////////////
bool SerialPort::openDevice ( const char *DeviceName )
{
	closeDevice();
	QString cname(DeviceName);
	int index = cname.indexOf("(COM");
	if(index != -1)
	{
		index++;
		cname = cname.mid(index,5);
		cname.remove(')');
	}


    QString fname = "\\\\.\\";
    fname += cname;

	m_hDevice = CreateFileA ( fname.toAscii().constData(),
								GENERIC_READ | GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
								NULL );

	if ( m_hDevice == INVALID_HANDLE_VALUE )
	{
		closeDevice();
		return FALSE;
	}

	SetCommMask ( m_hDevice, EV_RXCHAR/*|EV_CTS|EV_DSR|EV_RING*/);
	SetupComm ( m_hDevice, 4096, 4096 ) ;

	FlushFileBuffers ( m_hDevice );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
bool SerialPort::configureDevice ( int BaudRate, char DataBits, char StopBits, char Parity, char FlowCtrl )
{
	DCB	dcb;

	GetCommState ( m_hDevice, &dcb );

	memcpy( (char*)&m_oldTermParams, (char*)&dcb, sizeof(DCB) );

	switch ( BaudRate )
	{
		case 300:		dcb.BaudRate = CBR_300;		break;
		case 600:		dcb.BaudRate = CBR_600;		break;
		case 1200:		dcb.BaudRate = CBR_1200;	break;
		case 2400:		dcb.BaudRate = CBR_2400;	break;
		case 4800:		dcb.BaudRate = CBR_4800;	break;
		case 9600:		dcb.BaudRate = CBR_9600;	break;
		case 19200:		dcb.BaudRate = CBR_19200;	break;
		case 38400:		dcb.BaudRate = CBR_38400;	break;
		case 57600:		dcb.BaudRate = CBR_57600;	break;
		case 115200:	dcb.BaudRate = CBR_115200;	break;
		default:		dcb.BaudRate = CBR_9600;	break;
	}

	switch ( DataBits )
	{
		case 7:
			dcb.ByteSize = 7;
			break;
		case 8:
		default:
			dcb.ByteSize = 8;
			break;
	}

	switch ( StopBits )
	{
		case 2:
			dcb.StopBits = TWOSTOPBITS;
			break;
		case 1:
		default:
			dcb.StopBits = ONESTOPBIT;
			break;
	}

	switch ( Parity )
	{
		case 1:
			dcb.fParity   = TRUE;
			dcb.Parity    = ODDPARITY;
			break;
		case 2:
			dcb.fParity   = TRUE;
			dcb.Parity    = EVENPARITY;
			break;
		case 0:
		default:
			dcb.fParity   = FALSE;
			dcb.Parity    = NOPARITY;
			break;
	}

	switch ( FlowCtrl )
	{
		case 0:
			// No Flow Cointrol
			dcb.fOutxCtsFlow	= FALSE;
			dcb.fRtsControl		= RTS_CONTROL_ENABLE;
			dcb.fOutX			= FALSE;
			dcb.fInX			= FALSE;
			break;
		case 1:
			// Hardware Flow Control
			dcb.fOutxCtsFlow	= TRUE;
			dcb.fRtsControl		= RTS_CONTROL_HANDSHAKE;
			dcb.fOutX			= FALSE;
			dcb.fInX			= FALSE;
			break;
		case 2:
			// Software Flow Control
			dcb.fOutxCtsFlow	= FALSE;
			dcb.fRtsControl		= RTS_CONTROL_ENABLE;
			dcb.fOutX			= TRUE;
			dcb.fInX			= TRUE;
			break;
	}

	dcb.fBinary			= TRUE;
	dcb.fOutxDsrFlow	= FALSE;
	dcb.fDtrControl		= DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity		= FALSE;
	dcb.fTXContinueOnXoff	= FALSE;
	dcb.fErrorChar		= FALSE;
	dcb.fNull			= FALSE;
	dcb.fAbortOnError	= FALSE;
	dcb.fDummy2			= FALSE;
	dcb.XonLim			= 100;
	dcb.XoffLim			= 100;
	dcb.XonChar			= 0x17;
	dcb.XoffChar		= 0x19;
	dcb.ErrorChar		= 0;
	dcb.EofChar			= 0;
	dcb.EvtChar			= 0;

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/DataBits;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts ( m_hDevice, &CommTimeOuts );

	SetCommState ( m_hDevice, &dcb );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void SerialPort::writeByte ( const char byte )
{
	if ( m_hDevice == INVALID_HANDLE_VALUE )
	{
		return;
	}
	DWORD dwLength;
	int res = WriteFile ( m_hDevice,&byte,1,&dwLength,&m_SendOverlapped );

	if ( res < 0 )
	{
		emit error();
	}
	else
	{
		emit write_ready();
	}
}
///////////////////////////////////////////////////////////////////////////////
void SerialPort::writeBuffer ( const char *buff, int blen )
{
    if ( m_hDevice == INVALID_HANDLE_VALUE )
    {
        return;
    }
    DWORD dwLength;

#ifdef WRITE_WITH_TIMEOUT
	for(int i=0;i<blen;i++)
	{
		writeByte (buff[i]);
		Sleep(33);
	}
#else
    int res = WriteFile ( m_hDevice,buff,blen,&dwLength,&m_SendOverlapped );

    if ( res < 0 )
    {
        emit error();
    }
    else
    {
        emit write_ready();
    }
#endif
}
///////////////////////////////////////////////////////////////////////////////
void SerialPort::FlushBuffers()
{
	PurgeComm(m_hDevice,PURGE_TXABORT | PURGE_RXABORT |
									PURGE_TXCLEAR | PURGE_RXCLEAR );
}
///////////////////////////////////////////////////////////////////////////////
void SerialPort::drainBytes ()
{
	DWORD	dwErrors;
	COMSTAT	comStat;

	do
	{
		ClearCommError ( m_hDevice, &dwErrors, &comStat );
	}
	while ( comStat.cbOutQue );
}

///////////////////////////////////////////////////////////////////////////////
bool SerialPort::event ( QEvent *e )
{
	DWORD type;

	if ( e->type() < QEvent::User || e->type() > QEvent::MaxUser )
	{
		return QObject::event ( e );
	}

	type = ( DWORD ) e->type();

	switch ( type )
	{
		case SERIAL_RD_COMPLETE:
		emit read_ready();
		break;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT,
            0x86e0d1e0L, 0x8089, 0x11d0, 0x9c,
            0xe4,0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);

#define GUID_CLASS_COMPORT          GUID_DEVINTERFACE_COMPORT
#endif
QStringList SerialPort::devices()
{
	QStringList res;

	GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;


	hDevInfo = SetupDiGetClassDevs( guidDev,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
		);

	if(hDevInfo == INVALID_HANDLE_VALUE)
	{
		res << "COM1" << "COM2" << "COM3" << "COM4"
			<< "COM5" << "COM6" << "COM7" << "COM8";
	}
	else
	{
		bool bOk = TRUE;
		SP_DEVICE_INTERFACE_DATA ifcData;
		DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];

		ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		for (DWORD ii=0; bOk; ii++)
		{
			bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
				NULL, guidDev, ii, &ifcData);
			if (bOk)
			{

				SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
				bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
					&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
				if (bOk)
				{
					unsigned short fname[256];

					if(SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
						(unsigned char*)fname, sizeof(fname), NULL)

						)
					{
#if(0)
					    QString cname;

			            int index = QString::fromUtf16(fname).indexOf("(COM");
			            if(index != -1)
			            {
			                index++;
			                cname = QString::fromUtf16(fname).mid(index,5);
			                cname.remove(')');
			                res << cname;
			            }
#else
						res << QString::fromUtf16(fname);
#endif
					}

				}
			}
		}
	}
	if (pDetData != NULL)
		delete [] (char*)pDetData;

	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);


//	res.sort();
	return res;
}

///////////////////////////////////////////////////////////////////////////////
// return the status of the two control lines we are interested in:
// CTS is true when the modem has raised CTS (letting us know it is ok to send)
// DCD is true when the carrier detect line is active
///////////////////////////////////////////////////////////////////////////////
void SerialPort::GetDeviceStatus ( bool *CTS, bool *DSR,bool *DCD, bool *RI)
{

    DWORD ModemStat;

    *CTS=*DSR=*DCD=*RI=false;
    if(GetCommModemStatus(m_hDevice,&ModemStat))
    {
        if(ModemStat & MS_CTS_ON)
        {
            *CTS=true;
        }
        if(ModemStat & MS_DSR_ON)
        {
            *DSR=true;
        }
        if(ModemStat & MS_RLSD_ON)
        {
            *DCD=true;
        }
        if(ModemStat & MS_RING_ON)
        {
            *RI=true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Set the state of the RTS handshake line
///////////////////////////////////////////////////////////////////////////////
void SerialPort::SetRTS ( bool RTS )
{
	if ( RTS )
	{
		EscapeCommFunction ( m_hDevice, SETRTS );
	}
	else
	{
		EscapeCommFunction ( m_hDevice, CLRRTS );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Set the state of the DTR handshake line
///////////////////////////////////////////////////////////////////////////////
void SerialPort::SetDTR ( bool DTR )
{
	if ( DTR )
	{
		EscapeCommFunction ( m_hDevice, SETDTR );
	}
	else
	{
		EscapeCommFunction ( m_hDevice, CLRDTR );
	}
}

#else
///////////////////////////////////////////////////////////////////////////////
// Linux !!! :)
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
SerialPort::SerialPort ( QObject *parent )
{
	parent = parent;

	m_Device = -1;
	m_Notifier  = NULL;
	m_Buff.reserve ( __BUFF_SIZE );
}

///////////////////////////////////////////////////////////////////////////////
SerialPort::~SerialPort()
{
	closeDevice();
}

///////////////////////////////////////////////////////////////////////////////
void SerialPort::closeDevice()
{
	if ( m_Notifier )
	{
		delete m_Notifier;
		m_Notifier = NULL;
	}

	if ( m_Device == -1 )
	{
		return;
	}

	tcsetattr ( m_Device, TCSANOW, &m_oldTermParams );

	close ( m_Device );
	m_Device = -1;
}

///////////////////////////////////////////////////////////////////////////////
bool SerialPort::openDevice ( const char *DeviceName )
{
	closeDevice();

	QString fname = "/dev/";
	fname += DeviceName;

	// NOTE: the NOCTTY will prevent us from grabbing this terminal as our
	// controlling terminal (when run from init, we have no controlling
	// terminal, and we do not want this device to become one!)
	m_Device = open ( fname.toAscii().constData(), O_RDWR | O_NOCTTY | O_NONBLOCK );

	if ( m_Device == -1 )
	{
		return FALSE;
	}

	// attempt to read configuration, to verify this is a serial device
	// and to save settings
	if ( tcgetattr ( m_Device, &m_oldTermParams ) == -1 )
	{
		closeDevice();
		return FALSE;
	}

	m_Notifier = new QSocketNotifier ( m_Device, QSocketNotifier::Read, this );
	connect ( m_Notifier, SIGNAL ( activated ( int ) ), this, SLOT ( readReady ( int ) ) );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
bool SerialPort::configureDevice ( int BaudRate, char DataBits, char StopBits, char Parity, char FlowCtrl )
{
	struct termios  terminalParams;
	speed_t         theSpeed;

	tcflush ( m_Device, TCIOFLUSH );          // flush any unwritten, unread data

	// read the old value
	if ( tcgetattr ( m_Device, &terminalParams ) != -1 )
	{
		memcpy ( ( char* ) &m_oldTermParams, ( char* ) &terminalParams,sizeof ( termios ) );

		switch ( BaudRate )
		{
			/*
			case 5:			theSpeed = B50;		break;
			case 75:		theSpeed = B75;		break;
			case 110:		theSpeed = B110;	break;
			case 134:		theSpeed = B134;	break;
			case 150:		theSpeed = B150;	break;
			case 200:		theSpeed = B200;	break;
			*/
			case 300:		theSpeed = B300;	break;
			case 600:		theSpeed = B600;	break;
			case 1200:		theSpeed = B1200;	break;
			/*
			case 1800:		theSpeed = B1800;	break;
			*/
			case 2400:		theSpeed = B2400;	break;
			case 4800:		theSpeed = B4800;	break;
			case 9600:		theSpeed = B9600;	break;
			case 19200:		theSpeed = B19200;	break;
			case 38400:		theSpeed = B38400;	break;
			case 57600:		theSpeed = B57600;	break;
			case 115200:	theSpeed = B115200;	break;
			/*
			case 128000:	theSpeed = B128000;	break;
			case 256000:	theSpeed = B256000;	break;
			*/
			default:		theSpeed = B19200;	break;
		}

		cfsetospeed ( &terminalParams, theSpeed );
		cfsetispeed ( &terminalParams, theSpeed );

		terminalParams.c_cflag &= ~CSIZE;     // mask off the data bits

		switch ( DataBits )
		{
			case 7:
				terminalParams.c_cflag |= CS7;
				terminalParams.c_iflag |= ISTRIP;
				break;
			case 8:
			default:
				terminalParams.c_cflag |= CS8;
				terminalParams.c_iflag &= ~ISTRIP;
				break;
		}

		switch ( StopBits )
		{
			case 2:
				terminalParams.c_cflag |= CSTOPB;
				break;
			case 1:
			default:
				terminalParams.c_cflag &= ~CSTOPB;
				break;
		}

		switch ( Parity )
		{
			case 0:
				// No Parity
				terminalParams.c_cflag &= ~ ( PARENB | PARODD );
				terminalParams.c_iflag &= ~INPCK;
				break;
			case 1:
				// Odd Parity
				terminalParams.c_cflag |= ( PARENB | PARODD );
				terminalParams.c_iflag |= INPCK;
				break;
			case 2:
				// Even Parity
				terminalParams.c_cflag |= PARENB;
				terminalParams.c_cflag &= ~PARODD;
				terminalParams.c_iflag |= INPCK;
				break;
			default:
				break;
		}

		switch ( FlowCtrl )
		{
			case 0:
				// No Flow Cointrol
				terminalParams.c_cflag &= ~CRTSCTS;
				terminalParams.c_iflag &= ~( IXON|IXOFF|IXANY );
				break;
			case 1:
				// Hardware Flow Control
				terminalParams.c_cflag |= CRTSCTS;
				terminalParams.c_iflag &= ~( IXON|IXOFF|IXANY );
				break;
			case 2:
				// Software Flow Control
				terminalParams.c_cflag &= ~CRTSCTS;
				terminalParams.c_iflag |= ( IXON|IXOFF|IXANY );
				break;
		}

		// *** Input flag settings. ***
		terminalParams.c_iflag |= ( IGNBRK/*|PARMRK */);
		terminalParams.c_iflag &= ~ ( BRKINT|IGNPAR|INLCR|IGNCR|ICRNL|IUCLC|IXANY|IMAXBEL/*|IUTF8 */);

		// *** Output flag settings. ***
		terminalParams.c_oflag &= ~ ( OPOST|OLCUC|ONLCR|OCRNL|ONOCR|ONLRET|OFILL|OFDEL );

		// *** Control flag settings. ***
		terminalParams.c_cflag |= ( CLOCAL|CREAD );
		terminalParams.c_cflag &= ~ ( HUPCL );

		// *** Local flag settings. ***
		terminalParams.c_lflag &= ~ ( ICANON|ECHO|ECHONL|ISIG|IEXTEN|TOSTOP );

		terminalParams.c_cc[VINTR]	= _POSIX_VDISABLE;
		terminalParams.c_cc[VQUIT]	= _POSIX_VDISABLE;
		terminalParams.c_cc[VSUSP]	= _POSIX_VDISABLE;
		terminalParams.c_cc[VSTART]	= _POSIX_VDISABLE;
		terminalParams.c_cc[VSTOP]	= _POSIX_VDISABLE;

		terminalParams.c_cc[VMIN]	= 0;        // wait until at least one character or until timeout
		terminalParams.c_cc[VTIME]	= 0;      // wait this long, then fail

		if ( tcsetattr ( m_Device, TCSANOW, &terminalParams ) != -1 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
void SerialPort::readReady ( int )
{
    unsigned char c[4096];
    int res = read ( m_Device, c, 4096 );

    if( res > 0)
    {
        m_Mutex.lock();
        for(int i=0;i<res;i++)
        {
            m_Buff.append ( c[i] );
//qDebug()<< QString("%1").arg(c[i]);
        }
        m_Mutex.unlock();
        emit read_ready();
    }
    else if ( res <  0 )
    {
        if ( errno != EAGAIN )
        {
            emit error();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void SerialPort::writeByte ( const char byte )
{
	int res = write ( m_Device, &byte, 1 );

	if ( res < 0 )
	{
		emit error();
	}
	else
	{
		emit write_ready();
	}
}

///////////////////////////////////////////////////////////////////////////////
void SerialPort::writeBuffer ( const char *buff, int blen)
{
#ifdef WRITE_WITH_TIMEOUT
	for(int i=0;i<blen;i++)
	{
		writeByte (buff[i]);
		usleep(50 * 1000);
	}
#else
    int res = write ( m_Device, buff, blen );

    if ( res < 0 )
    {
        emit error();
    }
    else
    {
        emit write_ready();
    }
#endif
}
///////////////////////////////////////////////////////////////////////////////
void SerialPort::FlushBuffers()
{
}
///////////////////////////////////////////////////////////////////////////////
void SerialPort::drainBytes ()
{
	tcdrain( m_Device );
}

///////////////////////////////////////////////////////////////////////////////
bool SerialPort::event ( QEvent *e )
{
	return QObject::event ( e );
}

///////////////////////////////////////////////////////////////////////////////
QStringList SerialPort::devices()
{
	QStringList res;
	QDir dev ( "/dev" );

	QStringList entries = dev.entryList ( QStringList() << "ttyS*", QDir::CaseSensitive|QDir::System );
    entries += dev.entryList ( QStringList() << "ttyUSB*", QDir::CaseSensitive|QDir::System );

	for ( QStringList::Iterator it = entries.begin(); it != entries.end(); ++it )
	{
		res.append ( *it );
	}

	return res;
}

///////////////////////////////////////////////////////////////////////////////
// return the status of the two control lines we are interested in:
// CTS is true when the modem has raised CTS (letting us know it is ok to send)
// DCD is true when the carrier detect line is active
///////////////////////////////////////////////////////////////////////////////
void SerialPort::GetDeviceStatus ( bool *CTS, bool *DSR,bool *DCD ,bool *RI)
{
	int status;

	*CTS = *DSR = *DCD = *RI = FALSE;

	if ( ioctl ( m_Device, TIOCMGET, &status ) != -1 )
	{
		if ( status & TIOCM_CTS )
		{
			*CTS = TRUE;
		}

		if ( status & TIOCM_DSR )
		{
			*DSR = TRUE;
		}

		if ( status & TIOCM_CAR )
		{
			*DCD = TRUE;
		}
		if ( status & TIOCM_RI)
		{
			*RI = TRUE;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Set the state of the RTS handshake line
///////////////////////////////////////////////////////////////////////////////
void SerialPort::SetRTS ( bool RTS )
{
	int control;

	control = TIOCM_RTS;

	if ( RTS )
	{
		ioctl ( m_Device, TIOCMBIS, &control );
	}
	else
	{
		ioctl ( m_Device, TIOCMBIC, &control );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Set the state of the DTR handshake line
///////////////////////////////////////////////////////////////////////////////
void SerialPort::SetDTR ( bool DTR )
{
	int control;

	control = TIOCM_DTR;

	if ( DTR )
	{
		ioctl ( m_Device, TIOCMBIS, &control );
	}
	else
	{
		ioctl ( m_Device, TIOCMBIC, &control );
	}
}

#endif
