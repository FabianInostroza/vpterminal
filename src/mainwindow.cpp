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
#include "settingsdialog.h"
//#include "aboutdialog.h"
#include "asciidialog.h"
#include "sleep.h"
#include "cpphighlighter.h"
#include "qcinter.h"
#include "vpplaintextedit.h"
#include <time.h>
#include <unistd.h>


const QString MainWindow::ParityNames[3] =
{
	"None",
	"Odd",
	"Even",
};

const QString MainWindow::FlowCtrlNames[3] =
{
	"None",
	"Hardware",
	"Software",
};

const QString MainWindow::AsciiChart[32] =
{
	"NUL",	//	00
	"SOH",	//	01
	"STX",	//	02
	"ETX",	//	03
	"EOT",	//	04
	"ENQ",	//	05
	"ACK",	//	06
	"BEL",	//	07
	"BS",	//	08
	"HT",	//	09
	"LF",	//	0A
	"VT",	//	0B
	"FF",	//	0C
	"CR",	//	0D
	"SO",	//	0E
	"SI",	//	0F
	"DLE",	//	10
	"DC1",	//	11
	"DC2",	//	12
	"DC3",	//	13
	"DC4",	//	14
	"NAK",	//	15
	"SYN",	//	16
	"ETB",	//	17
	"CAN",	//	18
	"EM",	//	19
	"SUB",	//	1A
	"ESC",	//	1B
	"FS",	//	1C
	"GS",	//	1D
	"RS",	//	1E
	"US",	//	1F
};

////////////////////////////////////////////////////////////////////////////////////
const char *hex_table[256] =
{
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
	"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
	"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
	"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
	"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
	"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
	"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF",
};

#define OUTPUT_BLOCK_SIZE	1024
#define OUTPUT_TIMER_TOUT	50

///////////////////////////////////////////////////////////////////////////////
const QString MainWindow::AppName = "VPTerminal";
const QString MainWindow::AppVers = "0.5";


bool MainWindow::m_Connected = false;
///////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow ( QWidget *parent ) : QMainWindow ( parent )
{

    setupUi ( this );
    setupViews();
    setupVars();

    m_Serial = new SerialPort ( this );
    m_OutputTimer = new QTimer ( this );

    m_OutputTimer->start ( OUTPUT_TIMER_TOUT );

    buttonConnect_setToolTip();

   connect ( m_Serial,              SIGNAL ( read_ready() ),                this, SLOT ( readData() ) );
   connect ( m_OutputTimer,         SIGNAL ( timeout() ),			this, SLOT ( doOutput() ) );
   connect ( combo_LF_Symbol,       SIGNAL (currentIndexChanged (int)),	this, SLOT (changeLF_Symbol(int)) );
   connect ( radioBeforeSym,        SIGNAL (toggled ( bool)),		this, SLOT (changeBeforeAfterSym(bool)) );

   connect (macrosTable,            SIGNAL(itemSelectionChanged ()),	this, SLOT(macrosTableSelChange()) );
   connect (macrosTable,            SIGNAL(itemChanged(QTableWidgetItem *)),this, SLOT(macrosTableItemChange(QTableWidgetItem *)) );


// menu actions
    connect(actionExit,             SIGNAL(triggered()),                    this, SLOT(on_buttonExit_clicked()));
    connect(actionPortConnect,      SIGNAL(triggered()),                    this, SLOT(on_buttonConnect_clicked()));
    connect(actionPortDisconnect,   SIGNAL(triggered()),                    this, SLOT(on_buttonDisconnect_clicked()));
    connect(actionPortSettings,     SIGNAL(triggered()),                    this, SLOT(on_buttonSettings_clicked()));
    connect(actionASCII_Table,      SIGNAL(triggered()),                    this, SLOT(on_buttonAsciiTable_clicked()));
    connect(actionWhatisThis,       SIGNAL(triggered()),                    this, SLOT(setWhatIsThisMode()));
    connect(actionAbout_VPTerminal, SIGNAL(triggered()),                    this, SLOT(on_buttonAbout_clicked()));
    connect(actionAbout_QT,         SIGNAL(triggered()),                    this, SLOT(aboutQt()));

//context menu items

    connect(recvEditA->actionPortConnect,      SIGNAL(triggered()),                    this, SLOT(on_buttonConnect_clicked()));
    connect(recvEditA->actionPortDisconnect,   SIGNAL(triggered()),                    this, SLOT(on_buttonDisconnect_clicked()));
    connect(recvEditA->actionPortSettings,     SIGNAL(triggered()),                    this, SLOT(on_buttonSettings_clicked()));
    connect(recvEditA->actionASCII_Table,      SIGNAL(triggered()),                    this, SLOT(on_buttonAsciiTable_clicked()));
    connect(recvEditA->actionWhatisThis,       SIGNAL(triggered()),                    this, SLOT(setWhatIsThisMode()));
//    connect(recvEditA->actionAbout_VPTerminal, SIGNAL(triggered()),                    this, SLOT(on_buttonAbout_clicked()));

    connect(recvEditH->actionPortConnect,      SIGNAL(triggered()),                    this, SLOT(on_buttonConnect_clicked()));
    connect(recvEditH->actionPortDisconnect,   SIGNAL(triggered()),                    this, SLOT(on_buttonDisconnect_clicked()));
    connect(recvEditH->actionPortSettings,     SIGNAL(triggered()),                    this, SLOT(on_buttonSettings_clicked()));
    connect(recvEditH->actionASCII_Table,      SIGNAL(triggered()),                    this, SLOT(on_buttonAsciiTable_clicked()));
    connect(recvEditH->actionWhatisThis,       SIGNAL(triggered()),                    this, SLOT(setWhatIsThisMode()));
//    connect(recvEditH->actionAbout_VPTerminal, SIGNAL(triggered()),                    this, SLOT(on_buttonAbout_clicked()));

    connect(sendEdit->actionPortConnect,      SIGNAL(triggered()),                    this, SLOT(on_buttonConnect_clicked()));
    connect(sendEdit->actionPortDisconnect,   SIGNAL(triggered()),                    this, SLOT(on_buttonDisconnect_clicked()));
    connect(sendEdit->actionPortSettings,     SIGNAL(triggered()),                    this, SLOT(on_buttonSettings_clicked()));
    connect(sendEdit->actionASCII_Table,      SIGNAL(triggered()),                    this, SLOT(on_buttonAsciiTable_clicked()));
    connect(sendEdit->actionWhatisThis,       SIGNAL(triggered()),                    this, SLOT(setWhatIsThisMode()));
//    connect(sendEdit->actionAbout_VPTerminal, SIGNAL(triggered()),                    this, SLOT(on_buttonAbout_clicked()));

    connect(statusBar(),            SIGNAL(messageChanged(const QString &)),this, SLOT(statusBarMessageChange(const QString &)));

    // m_Serial->SetRTS(0);
    // m_Serial->SetDTR(0);
    updateRTS( m_RTS );
    updateDTR( m_DTR );
}

///////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::setupVars()
{
	QSettings settings;
    int i;
	// Load Settings -->
	#ifdef WIN32
	m_Settings.DeviceName		= settings.value( "Comm/Device",	"COM1"  ).toString();
	#else
	m_Settings.DeviceName		= settings.value( "Comm/Device",	"ttyS0" ).toString();
	#endif
	m_Settings.BaudRate			= settings.value( "Comm/BaudRate",	9600 ).toInt();
	m_Settings.DataBits			= settings.value( "Comm/DataBits",	8 ).toInt();
	m_Settings.StopBits			= settings.value( "Comm/StopBits",	1 ).toInt();
	m_Settings.Parity			= settings.value( "Comm/Parity",	0 ).toInt();
	m_Settings.FlowCtrl			= settings.value( "Comm/FlowCtrl",	0 ).toInt();

	m_Settings.RtsPolarity		= settings.value( "Rts/Polarity",	0 ).toInt();
	m_Settings.RtsBeforeTime	= settings.value( "Rts/BeforeTime",	0 ).toInt();
	m_Settings.RtsAfterTime		= settings.value( "Rts/AfterTime",	0 ).toInt();
    m_Settings.CRLF_Symbol      = settings.value( "Comm/CRLFSym",	10 ).toInt();
    m_Settings.BeforeSymbol     = settings.value( "Comm/BegoreSym", 1 ).toInt();

    m_Settings.MacroSendOnEnter	= settings.value( "Other/MacroSendOnEnter", 0 ).toBool();

    if(m_Settings.MacroSendOnEnter)
        checkMacroSendOnEnter->setCheckState(Qt::Checked);
    else
        checkMacroSendOnEnter->setCheckState(Qt::Unchecked);



	recvEditA->setMaximumBlockCount(OUTPUT_BLOCK_SIZE);
	recvEditA->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	recvEditA->setWordWrapMode(QTextOption::NoWrap);
	recvEditA->setLineWrapMode(QPlainTextEdit::NoWrap);
	recvEditH->setMaximumBlockCount(OUTPUT_BLOCK_SIZE);
	recvEditH->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	recvEditH->setWordWrapMode(QTextOption::NoWrap);
	recvEditH->setLineWrapMode(QPlainTextEdit::NoWrap);

	tabOutput->setCurrentIndex(0);

    QStringList sl;
    for(i=0;i<32;i++)
    {
        sl << AsciiChart[i];
    }
    combo_LF_Symbol->addItems (sl);
    combo_LF_Symbol->setCurrentIndex (m_Settings.CRLF_Symbol);


    radioBeforeSym->setChecked(m_Settings.BeforeSymbol);
    radioAfterSym->setChecked(!m_Settings.BeforeSymbol);

    m_highLighter = new CppHighlighter(scriptEdit);

	// <--
	m_Connected	= FALSE;
	m_NewData	= FALSE;

    m_RTS  = settings.value( "Rts/Value","false").toBool ();
    m_DTR  = settings.value( "Dtr/Value","false").toBool ();
//	m_RTS = FALSE;
//	m_DTR = FALSE;
	m_CTS = FALSE;
	m_DSR = FALSE;
	m_CD  = FALSE;
	m_RI  = FALSE;


    m_macrosTimer = new QTimer ( this );
	connect ( m_macrosTimer, SIGNAL ( timeout() ),  this, SLOT ( macrosDelay() ) );

	m_RecvData.clear();
    m_macrosDirName = settings.value( "File/MacrosDir", "" ).toString();
    m_macrosFileName = settings.value( "File/MacrosFile", "" ).toString();
    m_scriptDirName = settings.value( "File/ScriptDir", "" ).toString();
    m_scriptFileName = settings.value( "File/ScriptFile", "" ).toString();

    if(openMacrosFile(m_macrosFileName))
    {
//        error open
        macrosTable->setRowCount ( 1 );
        macrosTable->setItem ( 0, 0, new QTableWidgetItem ( "Command string" ) );
        macrosTable->setItem ( 0, 1, new QTableWidgetItem ( "Comment string" ) );
        macrosTable->setCurrentCell ( 0,0 );
    }

    openScriptFile(m_scriptFileName);


	restoreGeometry(settings.value("MainWindow/geometry").toByteArray());

    sendTabWidget->setCurrentIndex(settings.value("MainWindow/currentTab",0).toInt());
    bytespaceSpinBox->setValue(settings.value("Macroses/ByteSpace",0).toInt());
    commandspaceSpinBox->setValue(settings.value("Macroses/CommandsSpace",2000).toInt());

	m_CharPosA = 0;
	m_CharPosH = 0;
	m_OutputData.reserve(4096);
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::setupViews()
{
	setWindowTitle ( AppName + " - " + tr ( "Disconnected" ));

	setWindowIcon ( QIcon ( ":/images/mainicon.png" ) );

	recvSplitter->setStretchFactor ( 0,3 );
	recvSplitter->setStretchFactor ( 1,1 );
	mainSplitter->setStretchFactor ( 0,2 );
	mainSplitter->setStretchFactor ( 1,1 );

	sendEdit->installEventFilter ( this );
	macroSendEdit->installEventFilter ( this );
	macrosTable->installEventFilter ( this );
	btnRTS->installEventFilter ( this );
	btnDTR->installEventFilter ( this );

	buttonDisconnect->setEnabled ( FALSE );
	actionPortDisconnect->setEnabled ( FALSE );
	buttonConnect->setFocus();

//	statusBar()->showMessage ( tr ( "Disconnected" ) );

    buttonConnect_setToolTip();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::resizeEvent ( QResizeEvent * event )
{
    macrosTable->setColumnWidth ( 0,event->size().width() /4 );
    macrosTable->setColumnWidth ( 1,event->size().width() /4 );
    QMainWindow::resizeEvent ( event );
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::statusBarMessageChange(const QString &message)
{
    if(message.isEmpty())
    {
        if(connected())
        {
//	                                "Device=%1, "
//	                                "Baud Rate=%2, "
//	                                "Data Bits=%3, "
//	                                "Parity=%4, "
//	                                "Stop Bits=%5, "
//	                                "Flow Control=%6" )

        	statusBar()->showMessage ( tr ( "Connected - "
	                                "%1, "
	                                "%2, "
	                                "%3, "
	                                "%4, "
	                                "%5, "
	                                "%6" )
	                           .arg ( m_Settings.DeviceName )
	                           .arg ( m_Settings.BaudRate )
	                           .arg ( m_Settings.DataBits )
	                           .arg ( ParityNames[m_Settings.Parity] )
	                           .arg ( m_Settings.StopBits )
	                           .arg ( FlowCtrlNames[m_Settings.FlowCtrl] )
	                         );
        }
        else
        {
            statusBar()->showMessage ( tr ( "Disconnected" ) );
        }

    }
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::changeLF_Symbol(int index)
{
    m_Settings.CRLF_Symbol = index;
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonConnect_clicked()
{
	if ( !m_Serial->openDevice ( ( char* ) m_Settings.DeviceName.toAscii().constData() ) )
	{
		QMessageBox::critical ( this,
		                        tr ( "Error" ),
		                        tr ( "Could not open communication device %1." ).arg ( m_Settings.DeviceName ) );
		return;
	}

	if ( !m_Serial->configureDevice ( m_Settings.BaudRate,
	                                  m_Settings.DataBits,
	                                  m_Settings.StopBits,
	                                  m_Settings.Parity,
	                                  m_Settings.FlowCtrl ) )
	{
		QMessageBox::critical ( this,
		                        tr ( "Error" ),
		                        tr ( "Failed to configure communication device %1." ).arg ( m_Settings.DeviceName ) );
		m_Serial->closeDevice();
		return;
	}


	m_Connected = TRUE;
	buttonConnect->setEnabled ( FALSE );
	buttonSettings->setEnabled ( FALSE );
	buttonDisconnect->setEnabled ( TRUE );

	actionPortConnect->setEnabled (FALSE);
	actionPortSettings->setEnabled (FALSE);
	actionPortDisconnect->setEnabled (TRUE);



    buttonDisconnect->setFocus();

	initRTS();
    	updateDTR( m_DTR );
	updateInputLines(TRUE);

//	statusBar()->showMessage ( tr ( "Connected, "
//	                                "Device=%1, "
//	                                "Baud Rate=%2, "
//	                                "Data Bits=%3, "
//	                                "Parity=%4, "
//	                                "Stop Bits=%5, "
//	                                "Flow Control=%6" )
//	                           .arg ( m_Settings.DeviceName )
//	                           .arg ( m_Settings.BaudRate )
//	                           .arg ( m_Settings.DataBits )
//	                           .arg ( ParityNames[m_Settings.Parity] )
//	                           .arg ( m_Settings.StopBits )
//	                           .arg ( FlowCtrlNames[m_Settings.FlowCtrl] )
//	                         );

    setWindowTitle ( AppName + " - " +  tr ( "Connected - ")+
                                            QString("%1, "
	                                        "%2, "
	                                        "%3, "
	                                        "%4, "
	                                        "%5, "
                                            "%6" )
    	                                   .arg ( m_Settings.DeviceName )
	                                       .arg ( m_Settings.BaudRate )
	                                       .arg ( m_Settings.DataBits )
	                                       .arg ( ParityNames[m_Settings.Parity] )
	                                       .arg ( m_Settings.StopBits )
	                                       .arg ( FlowCtrlNames[m_Settings.FlowCtrl] ));

    buttonConnect_setToolTip();

	calcFontMetrics();
	m_CharPosA = 0;
	m_CharPosH = 0;
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonDisconnect_clicked()
{

	m_Serial->closeDevice();

	m_Connected = FALSE;
	buttonConnect->setEnabled ( TRUE );
	buttonSettings->setEnabled ( TRUE );

	buttonDisconnect->setEnabled ( FALSE );

	actionPortConnect->setEnabled (TRUE);
	actionPortSettings->setEnabled ( TRUE );
	actionPortDisconnect->setEnabled (FALSE);

	buttonConnect->setFocus();

    btnRTS->setPixmap ( QPixmap ( ":images/check_disconnect.png" ) );
    btnDTR->setPixmap ( QPixmap ( ":images/check_disconnect.png" ) );
    btnCTS->setPixmap ( QPixmap ( ":images/check_disconnect.png" ) );
    btnDSR->setPixmap ( QPixmap ( ":images/check_disconnect.png" ) );
    btnCD->setPixmap  ( QPixmap ( ":images/check_disconnect.png" ) );
    btnRI->setPixmap  ( QPixmap ( ":images/check_disconnect.png" ) );


//	statusBar()->showMessage ( tr ( "Disconnected" ) );
    setWindowTitle ( AppName + " - " + tr ( "Disconnected" ));
    buttonConnect_setToolTip();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonSettings_clicked()
{
    SettingsDialog dialog(this);
    if(dialog.exec() == QDialog::Accepted)
    {
        buttonConnect_setToolTip();
    }
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::changeBeforeAfterSym(bool checked)
{
     m_Settings.BeforeSymbol = checked;
//    qDebug("checked = %d",checked);

}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonRecvLog_clicked()
{
    QSettings settings;

    QString dirName = settings.value( "File/SaveDir", "" ).toString();

    QFileDialog fileDialog( this, tr("Save File"), dirName,"*.*");
//    fileDialog.setFileMode( QFileDialog::ExistingFile );
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);

    if ( fileDialog.exec() == QDialog::Accepted )
    {
        QStringList fileNames = fileDialog.selectedFiles();
        QString fileName = fileNames[0];
        QDir dir = fileDialog.directory();

        settings.setValue( "File/SaveDir", dir.absolutePath() );

        if ( !fileName.isEmpty() )
        {
            QFile file(fileName);

            if(file.open(QIODevice::WriteOnly|QIODevice::Text))
            {
                QByteArray b = recvEditA->toPlainText().toUtf8 ();
                file.write(b.data(),b.size());
                file.close();
            }
            else
            {
                QMessageBox::critical ( this,tr ( "Error" ),
                                        tr ( "Could not create file %1." ).arg ( fileName ) );
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////
bool MainWindow::eventFilter ( QObject *obj, QEvent *event )
{
	const char *buff;

	if ( obj == sendEdit )
	{
		if ( event->type() == QEvent::KeyPress )
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*> ( event );

            if ( connected() )
			{
				QString keystr = keyEvent->text();

				if( !keystr.isEmpty() )
				{
					buff = keystr.toLocal8Bit().constData();

					m_Serial->writeByte ( ( const char ) buff[0] );

					if ( buff[0] == '\r' )
					{
						m_Serial->writeByte ( '\n' );
					}
				}
			}
		}
		return false;
	}

    if( checkMacroSendOnEnter->checkState() == Qt::Checked )
	{
		if (( obj ==  macroSendEdit)||( obj ==  macrosTable))
		{
			if ( event->type() == QEvent::KeyPress )
			{
				QKeyEvent *keyEvent = static_cast<QKeyEvent*> ( event );
				int key = keyEvent->key();
				if((key == Qt::Key_Return)||(key == Qt::Key_Enter))
				{
					on_sendButton_clicked();
				}
			}
			return false;
		}
	}

    if ( obj == btnRTS && connected() )
	{
		if ( event->type() == QEvent::MouseButtonPress )
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*> ( event );

			if ( mouseEvent->button() == Qt::LeftButton )
			{
				m_RTS = !m_RTS;
				updateRTS ( m_RTS );
			}
		}
		return false;
	}

    if ( obj == btnDTR && connected() )
	{
		if ( event->type() == QEvent::MouseButtonPress )
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*> ( event );

			if ( mouseEvent->button() == Qt::LeftButton )
			{
				m_DTR = !m_DTR;
				updateDTR ( m_DTR );
			}
		}
		return false;
	}

	return QMainWindow::eventFilter ( obj, event );
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonRecvClear_clicked()
{
	recvEditA->clear();
	recvEditH->clear();
	m_RecvData.clear();

	m_CharPosA = 0;
	m_CharPosH = 0;
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonSendClear_clicked()
{
	sendEdit->clear();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::readData()
{
	m_Serial->m_Mutex.lock();

	m_recvMutex.lock();
	m_RecvData.append ( m_Serial->m_Buff );
	m_recvMutex.unlock();
	m_Serial->m_Buff.clear();

	m_Serial->m_Mutex.unlock();

	m_NewData = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::doOutput()
{
	if ( m_NewData )
	{
		m_NewData = FALSE;

		m_recvMutex.lock();
		m_OutputData = m_RecvData;
		m_RecvData.clear();
		m_recvMutex.unlock();

		formatOutAscii	( m_OutputData, recvEditA );
		formatOutHex	( m_OutputData, recvEditH );
	}

	updateInputLines(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MainWindow::formatOutAscii(QByteArray &src, QPlainTextEdit *te)
{
	QByteArray		dst;
	unsigned char	ch;
	int				i, n, len;

	calcFontMetrics();

	len = src.size();
	dst.clear();

	for(i=0;i<len;i++)
	{
		ch = (unsigned char)src[i];
		if( ch < 0x20 )
		{
            if((ch == m_Settings.CRLF_Symbol) && ( m_Settings.BeforeSymbol) )
            {
                dst += '\n';
				m_CharPosA = 0;
            }

			n = AsciiChart[(int)ch].length() + 2;
			m_CharPosA += n;

			if( m_CharPosA >= m_RecvWidth )
			{
				dst += '\n';
				m_CharPosA = n;
			}

			dst += '<';
			dst += AsciiChart[(int)ch];
			dst += '>';

            if(/*(ch == '\n')||*/( (ch == m_Settings.CRLF_Symbol) && (!m_Settings.BeforeSymbol)) )
            {
                dst += '\n';
				m_CharPosA = 0;
            }
		}

		if( ch >= 0x20 )
		{
			dst += ch;
			m_CharPosA++;

			if( m_CharPosA >= m_RecvWidth )
			{
				dst += '\n';
				m_CharPosA = 0;
			}
		}
	}
	te->moveCursor ( QTextCursor::End );
	te->insertPlainText ( dst );
	te->moveCursor ( QTextCursor::End );
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::formatOutHex(QByteArray &src, QPlainTextEdit *te)
{
	QByteArray	dst;
	int			i, n, len;
    unsigned char idx;

	calcFontMetrics();

	len = src.size();
	dst.clear();

	for(i=0;i<len;i++)
	{
        idx = (unsigned char)src[i];
        if( src[i] == m_Settings.CRLF_Symbol )
        {
            if( m_Settings.BeforeSymbol )
            {
                dst += '\n';
                dst += hex_table[idx];
       		    dst += ' ';
       		    m_CharPosH = 3;
			}
            else
            {
                dst += hex_table[idx];
                dst += '\n';
                m_CharPosH = 0;
			}
        }
        else
        {
		    n = m_CharPosH + 3;

			if( n >= m_RecvWidth )
			{
				dst += '\n';
				m_CharPosH = 0;
			}

            dst += hex_table[idx];
			dst += ' ';
			m_CharPosH += 3;
/*			if( src[i] == '\n' )
			{
				dst += '\n';
				m_CharPosH = 0;
            }*/
        }
	}
	te->moveCursor ( QTextCursor::End );
	te->insertPlainText ( dst );	
	te->moveCursor ( QTextCursor::End );
}
///////////////////////////////////////////////////////////////////////////////
bool MainWindow::connectedToPort()
{
    if(!connected())
    {
        if(QMessageBox::warning ( this, tr("Warning.."), tr("Not connected to device.\nOpen Port and Send?"),
                               QMessageBox::Yes|QMessageBox::No,
                               QMessageBox::Yes) == QMessageBox::Yes)
        {
            on_buttonConnect_clicked();
        }
        else
        {
            return false;
        }
    }
    return true;
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonSendFile_clicked()
{
    if( !connectedToPort())
	{
		return;
	}

	QSettings settings;

    QString fileFilters = tr ( "Text files (*.txt);;All files (*.*)" );
	QString dirName = settings.value( "File/SendDir", "" ).toString();

    QFileDialog fileDialog( this, tr("Send File"), dirName, fileFilters );
	fileDialog.setFileMode( QFileDialog::ExistingFile );

	if ( fileDialog.exec() == QDialog::Accepted )
	{
		QStringList fileNames = fileDialog.selectedFiles();
		QString fileName = fileNames[0];
		QDir dir = fileDialog.directory();

		settings.setValue( "File/SendDir", dir.absolutePath() );

		if ( !fileName.isEmpty() )
		{
            if ( connected() )
			{
				sendFile ( fileName );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::sendFile ( QString fn )
{
	QFile file ( fn );

	if ( !file.open ( QIODevice::ReadOnly ) )
	{
		QMessageBox::critical ( this,
		                        tr ( "Error" ),
		                        tr ( "Could not open file %1." ).arg ( fn ) );
		return;
	}

	if( file.size() > 200 * 1024 )
	{
		QMessageBox::critical ( this,
		                        tr ( "Error" ),
		                        tr ( "File %1 is too long." ).arg ( fn ) );
		file.close();
		return;
	}

	QByteArray ba = file.readAll();

	file.close();

	QProgressDialog progress("Sending file...", "Abort", 0, ba.size(), this);
	progress.setWindowModality( Qt::WindowModal );
	progress.setMinimumDuration( 0 );

	driveRTS( TRUE );

	progress.open();

	for(int i = 0; i < ba.size(); i++)
	{
		m_Serial->writeByte( ba.at(i) );
		m_Serial->drainBytes();

		progress.setValue(i);

		if( progress.wasCanceled() )
		{
			break;
		}

		qApp->processEvents();
	}

	driveRTS( FALSE );
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent ( QCloseEvent * event )
{
    if(saveMacrosChanges() == QMessageBox::Cancel)
    {
        event->ignore();
        return;
    }

	if ( m_Serial != NULL )
	{
        if ( connected() )
		{
			m_Serial->closeDevice();
		}

		delete m_Serial;
	}

    delete m_highLighter;



	QSettings settings;
	settings.setValue("Rts/Value",	m_RTS );
	settings.setValue("Dtr/Value",	m_DTR );
	settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/currentTab",sendTabWidget->currentIndex());

    settings.setValue("Comm/CRLFSym",m_Settings.CRLF_Symbol);
    settings.setValue( "Comm/BegoreSym",   m_Settings.BeforeSymbol );

    settings.setValue("Macroses/ByteSpace",bytespaceSpinBox->value());
    settings.setValue("Macroses/CommandsSpace",commandspaceSpinBox->value());

    settings.setValue( "File/MacrosDir", m_macrosDirName );
    settings.setValue( "File/MacrosFile", m_macrosFileName );

    settings.setValue( "File/ScriptDir", m_scriptDirName );
    settings.setValue( "File/ScriptFile", m_scriptFileName );

    settings.setValue( "Other/MacroSendOnEnter", m_Settings.MacroSendOnEnter);

    QMainWindow::closeEvent (event);
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonExit_clicked()
{
	close();
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::setWhatIsThisMode(void)
{
    if(QWhatsThis::inWhatsThisMode())
        QWhatsThis::leaveWhatsThisMode();
    else
        QWhatsThis::enterWhatsThisMode();
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonAbout_clicked()
{
//	AboutDialog *dialog = new AboutDialog ( this );
//	dialog->show();
    QMessageBox::about (this,tr("About") +"VPTerminal",tr("Version : ") + AppVers +\
    		"\n"+tr("Build at : ")+__DATE__+"\t"+__TIME__);


}

void MainWindow::aboutQt()
{
  QMessageBox::aboutQt(this/*,tr("About") +"QT"*/);
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonAsciiTable_clicked()
{
	AsciiDialog *dialog = new AsciiDialog ( this );
	dialog->show();
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::updateRTS ( bool value )
{
    if(connected())
    {
	    if ( value )
	    {
            btnRTS->setPixmap ( QPixmap ( ":images/check_on.png" ) );
		    m_Serial->SetRTS ( 1 );
	    }
	    else
	    {
            btnRTS->setPixmap ( QPixmap ( ":images/check_off.png" ) );
		    m_Serial->SetRTS ( 0 );
	    }
    }
    else
    {
        btnRTS->setPixmap ( QPixmap ( ":images/check_disconnect.png" ) );
    }

	qApp->processEvents();
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::updateDTR ( bool value )
{
    if(connected())
    {
	    if ( value )
	    {
            btnDTR->setPixmap ( QPixmap ( ":images/check_on.png" ) );
		    m_Serial->SetDTR ( 1 );
	    }
	    else
	    {
            btnDTR->setPixmap ( QPixmap ( ":images/check_off.png" ) );
		    m_Serial->SetDTR ( 0 );
	    }
    }
    else
    {
        btnDTR->setPixmap ( QPixmap ( ":images/check_disconnect.png" ) );
    }
//    qApp->processEvents();
}
///////////////////////////////////////////////////////////////////////////////
void MainWindow::updateInputLines (bool force )
{
	bool getCTS,getDSR,getCD,getRI;
    if(connected())
	{
		m_Serial->GetDeviceStatus(&getCTS,&getDSR,&getCD,&getRI);

		if((m_CTS != getCTS)||(force))
		{
			m_CTS = getCTS;
			if(m_CTS)
                btnCTS->setPixmap ( QPixmap ( ":images/check_on.png" ) );
			else
                btnCTS->setPixmap ( QPixmap ( ":images/check_off.png" ) );
		}

		if((m_DSR != getDSR)||(force))
		{
			m_DSR = getDSR;
			if(m_DSR)
                btnDSR->setPixmap ( QPixmap ( ":images/check_on.png" ) );
			else
                btnDSR->setPixmap ( QPixmap ( ":images/check_off.png" ) );
		}

		if((m_CD != getCD)||(force))
		{
			m_CD = getCD;
			if(m_CD)
                btnCD->setPixmap ( QPixmap ( ":images/check_on.png" ) );
			else
                btnCD->setPixmap ( QPixmap ( ":images/check_off.png" ) );
		}

		if((m_RI != getRI)||(force))
		{
			m_RI = getRI;
			if(m_RI)
                btnRI->setPixmap ( QPixmap ( ":images/check_on.png" ) );
			else
                btnRI->setPixmap ( QPixmap ( ":images/check_off.png" ) );
		}

	}
}
///////////////////////////////////////////////////////////////////////////////
QString MainWindow::toHex ( QByteArray &array )
{
	QString hexstr,tmpstr;
	int i;

	for ( i=0; i<array.size(); i++ )
	{
        if(array[i] == m_Settings.CRLF_Symbol)
        {
            if(m_Settings.BeforeSymbol)
                tmpstr.sprintf ( "\n%02X ", (unsigned char)array[i] );
            else
                tmpstr.sprintf ( "%02X\n", (unsigned char)array[i] );
        }
        else
        {
		    tmpstr.sprintf ( "%02X ", (unsigned char)array[i] );
        }

		hexstr += tmpstr;
	}

	return hexstr;
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::initRTS()
{
	switch( m_Settings.RtsPolarity )
	{
//		case 0:	m_RTS = 0;	break;	// None
		case 1:	m_RTS = 0;	break;	// High
		case 2:	m_RTS = 1;	break;	// Low
	}

	updateRTS( m_RTS );
}

///////////////////////////////////////////////////////////////////////////////
void MainWindow::driveRTS ( bool before )
{
	if ( m_Settings.FlowCtrl == 1 ) // Hardware
	{
		return;
	}

	if ( m_Settings.RtsPolarity == 0 )		// None
	{
		return;
	}

	if ( before )
	{
		if ( m_Settings.RtsPolarity == 1 )	// Hight
		{
			m_RTS = 1;
		}
		else
		if ( m_Settings.RtsPolarity == 2 )	// Low
		{
			m_RTS = 0;
		}

		m_Serial->SetRTS( m_RTS );

        SleepThread::msleep( m_Settings.RtsBeforeTime );
	}
	else
	{
        SleepThread::msleep( m_Settings.RtsAfterTime );

		if ( m_Settings.RtsPolarity == 1 )	// Hight
		{
			m_RTS = 0;
		}
		else
		if ( m_Settings.RtsPolarity == 2 )	// Low
		{
			m_RTS = 1;
		}

		m_Serial->SetRTS( m_RTS );
	}
}
////////////////////////////////////////////////////////////////////////////////////
unsigned char MainWindow::HexConvert ( QByteArray ba )
{
    unsigned char hexVal=0;
    for ( int i=0;i<2;i++ )
    {
        if ( '0' <= ba.at ( i ) && ba.at ( i ) <= '9' )
        {
            hexVal<<=4;
            hexVal += ( unsigned char ) 0x00 + ( ( ba.at ( i )-'0' ) );
        }
        else if ( 'A' <= ba.at ( i ) && ba.at ( i ) <= 'F' )
        {
            hexVal<<=4;
            hexVal += ( unsigned char ) 0x0A + ( ( ba.at ( i )-'A' ) );
        }
        else if ( 'a' <= ba.at ( i ) && ba.at ( i ) <= 'f' )
        {
            hexVal<<=4;
            hexVal += ( unsigned char ) 0x0A + ( ( ba.at ( i )-'a' ) );
        }
        else
        {
            hexVal = 0xFF;
            break;
        }
    }
    return hexVal;
}
////////////////////////////////////////////////////////////////////////////////////
unsigned char MainWindow::DecConvert ( QByteArray ba )
{
    unsigned char DecVal = 0;
    if ( ( ba.at ( 0 ) > '2' ) && ( ba.at ( 0 ) <= '9' ) )
    {
        DecVal = 255;
    }
    else
    {
        DecVal = 0;
        for ( int i=0;i<3;i++ )
        {
            if ( '0' <= ba.at ( i ) && ba.at ( i ) <= '9' )
            {
                DecVal *=10;
                DecVal += ( ba.at ( i )-'0' );
            }
        }
    }

    return DecVal;
}
////////////////////////////////////////////////////////////////////////////////////
unsigned char MainWindow::CalcCheckSum ( int XORSUM,QByteArray ba )
{

    int i;
    unsigned char CheckSum = 0;

    for ( i=0;i<ba.size();i++ )
    {
        if ( XORSUM == 0 )                        //byte sum
        {
            CheckSum+=ba.at ( i );
        }
        else if ( XORSUM == 1 )                   //xor bitwise
        {
            CheckSum^=ba.at ( i );
        }
        else if ( XORSUM == 2 )                   //en7755
        {
            if ( /*(ba.at(i) == STX)||*/ ( ba.at ( i ) == 0x01 ) ) //SOH
                CheckSum = 0;
            else
                CheckSum^=ba.at ( i );
        }
    }

    return CheckSum;
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::ConvertCommand ( QByteArray &ba )
{
    QByteArray      tmp_ba;
    unsigned char   cs;
    unsigned int    i,l,len;

    len = ba.size();

    tmp_ba.clear();

    if ( len )
    {
        l = 0;
        i = 0;

        while ( i<len )
        {
            if ( ba.at ( i ) == '$' )
            {
                if ( ba.at ( i+1 ) == '$' )
                {
                    i++;
                }
                else
                {
                    if ( ( i+2 ) < len )
                    {
                        tmp_ba += HexConvert ( ba.mid ( i+1 ) );
                        i += 3;
                        l++;
                        continue;
                    }
                }

            }
            else
            if ( ba.at ( i ) == '#' )
            {
                if ( ba.at ( i+1 ) == '#' )
                {
                    i++;
                }
                else
                {
                    if ( ( i+3 ) <len )
                    {
                        if ( ( ba.at ( i+1 ) == 'S' ) && ( ba.at ( i+2 ) == 'U' ) && ( ba.at ( i+3 ) == 'M' ) )
                        {
                            cs = CalcCheckSum ( 0,tmp_ba );
                            tmp_ba += cs;
                        }
                        else
                        if((ba.at(i+1) == 'S')&&(ba.at(i+2) == 'U')&&(ba.at(i+3) == '2'))
                        {
                            cs = CalcCheckSum(0,tmp_ba);
                            unsigned char byte1;
                            unsigned char byte2;
                            byte1=(cs & 0xF0)>>4;
                            byte2=(cs & 0x0F);
                            if(byte1>9)
                                byte1+='7';
                            else
                                byte1+='0';
                            if(byte2>9)
                                byte2+='7';
                            else
                                byte2+='0';
                            tmp_ba += byte1;
                            tmp_ba += byte2;
                            l++;
                        }
                        else
                        if ( ( ba.at ( i+1 ) == 'X' ) && ( ba.at ( i+2 ) == 'O' ) && ( ba.at ( i+3 ) == 'R' ) )
                        {
                            cs = CalcCheckSum ( 1,tmp_ba );
                            tmp_ba += cs;
                        }
                        else
                        if ( ( ba.at ( i+1 ) == 'I' ) && ( ba.at ( i+2 ) == 'S' ) && ( ba.at ( i+3 ) == 'O' ) )
                        {
                            cs = CalcCheckSum ( 2,tmp_ba );
                            tmp_ba += cs;
                        }
                        else
                        {
                            tmp_ba += DecConvert ( ba.mid ( i+1 ) );
                        }
                        i += 4;
                        l++;
                        continue;
                    }
                }
            }

            if ( ba.at ( i ) >= 0x20 )
            {
                tmp_ba += ba.at ( i );
                l++;
            }
            i++;
        }

        ba = tmp_ba;
    }
}
////////////////////////////////////////////////////////////////////////////////////
QMessageBox::StandardButton MainWindow::saveMacrosChanges(void)
{
    QMessageBox::StandardButton sb = QMessageBox::NoButton;
    if(macrosTable->isWindowModified ())
    {
        sb = QMessageBox::question (this,
		                            tr ( "Question" ),
		                            tr ( "Save As Macros Changes"),
                                    QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
                                    QMessageBox::Yes);

		if(sb == QMessageBox::Yes)
        {
            on_saveasButton_clicked();

            //if not saved operation is cancel
            if(macrosTable->isWindowModified ())
            {
                sb = QMessageBox::Cancel;
            }

        }
    }
    return sb;
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::macrosTableItemChange(QTableWidgetItem *item)
{
   	int row = macrosTable->currentRow();
	int col = macrosTable->currentColumn();
	if(item)
    {
        macrosTable->setWindowModified(true);
		if( row >= 0 && col == 0 )
		{
    	    macroSendEdit->setText(item->text().toAscii());
    	}
	}
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::macrosTableSelChange(void)
{
    int row = macrosTable->currentRow();
    if( row >= 0 )
    {
        QTableWidgetItem *item = macrosTable->item(row,0);
        if(item)
        {
            macroSendEdit->setText(item->text().toAscii());
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_sendButton_clicked()
{
    if(!connectedToPort())
    {
        return;
    }
//    int row = macrosTable->currentRow();

//    if ( row >= 0 )
    if(!macroSendEdit->text().isEmpty())
    {
        driveRTS( TRUE );
/*
        QTableWidgetItem * item = macrosTable->item ( row,0 );
        QByteArray ba = item->text().toAscii();
*/
        QByteArray ba = macroSendEdit->text().toAscii();
        ConvertCommand ( ba );
        int i;
        for ( i = 0; i < ba.size(); i++ )
        {
            m_Serial->writeByte ( ba.at ( i ) );


            if ( bytespaceSpinBox->value() )
            {
#ifdef WIN32
                Sleep(bytespaceSpinBox->value());
#else
                usleep(bytespaceSpinBox->value()*1000);
#endif

                //thread()->usleep(bytespaceSpinBox->value()*1000);
                //thread()->wait(bytespaceSpinBox->value());
            }

        }

        m_Serial->drainBytes();

        driveRTS( FALSE );

#if 1
//change baud rate if need
        int row = macrosTable->currentRow();
        if ( row >= 0 )
        {
		    if(macrosTable->item(row,1))
		    {
			    QByteArray d = macrosTable->item(row,1)->text().toAscii();
			    if(d.startsWith("BAUDRATE("))
			    {
				    d.remove(0,9/*sizeof("BAUDRATE(");*/);
				    int m = d.indexOf(')');
				    if(m != -1)
				    {
					    d.resize(m);
					    bool ok;
					    int v = d.toInt(&ok, 10);
					    if(ok)
					    {

//                            if( repeatCheckBox->checkState() == Qt::Unchecked)
                            {
#ifdef WIN32
                                Sleep(500);
#else
                                usleep(500*1000);
#endif
                            }


						    m_Settings.BaudRate = v;
	                        if ( !m_Serial->configureDevice ( m_Settings.BaudRate,
	                                                          m_Settings.DataBits,
	                                                          m_Settings.StopBits,
	                                                          m_Settings.Parity,
	                                                          m_Settings.FlowCtrl ) )
	                        {
		                        QMessageBox::critical ( this,
		                                                tr ( "Error" ),
		                                                tr ( "Failed to configure communication device %1." ).arg ( m_Settings.DeviceName ) );
		                        m_Serial->closeDevice();
                            }else
                            {
                                setWindowTitle ( AppName + " - " +  tr ( "Connected - "
	                                                                    "%1, "
	                                                                    "%2, "
	                                                                    "%3, "
	                                                                    "%4, "
	                                                                    "%5, "
	                                                                    "%6" )
    	                                                               .arg ( m_Settings.DeviceName )
	                                                                   .arg ( m_Settings.BaudRate )
	                                                                   .arg ( m_Settings.DataBits )
	                                                                   .arg ( ParityNames[m_Settings.Parity] )
	                                                                   .arg ( m_Settings.StopBits )
	                                                                   .arg ( FlowCtrlNames[m_Settings.FlowCtrl] ));

                            }
					    }
				    }
			    }
		    }
	    }
#endif

    }
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_removeButton_clicked()
{
    int row = macrosTable->currentRow();
    if ( row>=0 )
    {
        macrosTable->removeRow ( row );
        macrosTable->setWindowModified(true);
        if ( macrosTable->rowCount() ==0 )
        {
//            sendButton->setEnabled ( FALSE );
            removeButton->setEnabled ( FALSE );
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_addButton_clicked()
{
    int row = macrosTable->rowCount();
    if ( row != 0 )
    {
        row = macrosTable->currentRow() + 1;
    }

    macrosTable->insertRow ( row );
    macrosTable->setWindowModified(true);

    macrosTable->setCurrentCell ( row,0 );

//    sendButton->setEnabled ( TRUE );
    removeButton->setEnabled ( TRUE );
}
////////////////////////////////////////////////////////////////////////////////////
int MainWindow::openMacrosFile(QString fileName)
{
    if ( fileName.isEmpty() )
    {
        return 1;
    }

    QFile file ( fileName );

    if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        QMessageBox::critical ( this,
                                tr ( "Error" ),
                                tr ( "Could not open file %1." ).arg ( fileName ) );
        return 2;
    }

    labelMacrosFile->setText(tr("Macros file : ") + fileName);

    //remove all rows
    while ( macrosTable->rowCount() != 0 )
    {
        macrosTable->removeRow ( 0 );
    }

    QTextStream in(&file);
    QString     line;
    QString     comment_str;
    int         comment_index;
    int         row = 0;

    while ( !in.atEnd() )
    {
        line = in.readLine();

        if ( line != QString::null )
        {
            /*
            //remove 0x0A (LF) of end line if exist
            if ( ( line.at ( line.size()-1 ) ) == 0x0A )
            {
                line.chop ( 1 );            //delete 1 char of end of string
            }
            */
            comment_index = line.indexOf("####");
            if ( comment_index != -1 )
            {
                                            //sizeof("####"));
                comment_str = line.mid(comment_index+4);
                line.truncate(comment_index);
            }

            macrosTable->insertRow ( row );
            macrosTable->setItem ( row, 0, new QTableWidgetItem ( line ) );

            if(comment_index != -1)
            {
                macrosTable->setItem ( row, 1, new QTableWidgetItem ( comment_str ) );
            }

            row++;
        }
    }

    if ( macrosTable->rowCount() != 0 )
    {
        macrosTable->setCurrentCell ( 0,0 );
        macrosTableItemChange(macrosTable->item(0,0));
//        sendButton->setEnabled ( TRUE );
        removeButton->setEnabled ( TRUE );
    }
    file.close();
    macrosTable->setWindowModified(false);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_loadButton_clicked()
{
    if(saveMacrosChanges() == QMessageBox::Cancel)
    {
        return;
    }

    QString fileFilters = "Macros files (*.txt);;All files (*.*)";

    QFileDialog fileDialog( this, tr("Load Macros File"), m_macrosDirName, fileFilters );
    fileDialog.setFileMode( QFileDialog::ExistingFile );

    if ( fileDialog.exec() == QDialog::Accepted )
    {
        QStringList fileNames = fileDialog.selectedFiles();
        m_macrosFileName = fileNames[0];
        QDir dir = fileDialog.directory();

        openMacrosFile(m_macrosFileName);
    }
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_saveasButton_clicked()
{
    QString fileFilters = tr("Macros files")+" (*.txt);;"+tr("All files")+" (*.*)";

    QFileDialog fileDialog( this, tr("Save Macros File"), m_macrosDirName, fileFilters );
    fileDialog.setFileMode  ( QFileDialog::AnyFile );
    fileDialog.setAcceptMode( QFileDialog::AcceptSave );

    if ( fileDialog.exec() != QDialog::Accepted )
    {
        return;
    }

    QStringList fileNames = fileDialog.selectedFiles();
    m_macrosFileName = fileNames[0];
    QDir dir = fileDialog.directory();

    m_macrosDirName = dir.absolutePath();

    if ( m_macrosFileName.isEmpty() )
    {
        return;
    }

    QFile file ( m_macrosFileName );

    if ( !file.open ( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        QMessageBox::critical ( this,
                                tr ( "Error" ),
                                tr ( "Could not open file %1." ).arg ( m_macrosFileName ) );
        return;
    }

    QString line;
    QTableWidgetItem * item_command;
    QTableWidgetItem * item_comment;
    QByteArray ba;
    int row;

    for ( row=0;row<macrosTable->rowCount(); row++ )
    {
        item_command = macrosTable->item ( row,0 );
        item_comment = macrosTable->item ( row,1 );

		if(item_command)
			ba = item_command->text().toAscii();

        if(item_comment)
        {
            if(item_comment->text().size())
            {
                ba += "####";
                ba += item_comment->text().toAscii();
            }
        }
        ba += "\n";
        file.write ( ba );
    }

    labelMacrosFile->setText(tr("Macros file : ") + m_macrosFileName);
    macrosTable->setWindowModified(false);
    file.close();
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_checkMacroSendOnEnter_clicked()
{
    if( checkMacroSendOnEnter->checkState() == Qt::Checked )
    {
        m_Settings.MacroSendOnEnter = true;
    }
    else
    {
        m_Settings.MacroSendOnEnter = false;
    }
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::startMacrosTimer()
{
	int val = commandspaceSpinBox->value();
    int row = macrosTable->currentRow();
    if ( row >= 0 )
    {
		if(macrosTable->item(row,1))
		{
			QByteArray d = macrosTable->item(row,1)->text().toAscii();
			if(d.startsWith("DELAY("))
			{
				d.remove(0,6/*sizeof("DELAY(");*/);
				int m = d.indexOf(')');
				if(m != -1)
				{
					d.resize(m);
					bool ok;
					int v = d.toInt(&ok, 10);
					if(ok)
					{
						val = v;
					}
				}
			}
		}
	}
	m_macrosTimer->start(val);
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_repeatCheckBox_clicked()
{

    if ( repeatCheckBox->checkState() == Qt::Checked )
    {
        if(connectedToPort())
        {
            if (macrosTable->rowCount())
            {
                on_sendButton_clicked();
                startMacrosTimer();
                //          qDebug() << "1 Timer ID:" << tId;
            }
        }
        else
        {
            repeatCheckBox->setCheckState ( Qt::Unchecked );
        }
    }
    else
    {
        m_macrosTimer->stop();
    }
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::macrosDelay()
{
    //    qDebug() << "2 Timer ID:" << event->timerId();
    int row;

	row = macrosTable->currentRow();
	row++;
	if(row < macrosTable->rowCount())
	{
		macrosTable->setCurrentCell(row,0);
		startMacrosTimer();
		on_sendButton_clicked();
	}
	else
	{
		if( againCheckBox->checkState() == Qt::Checked)
		{
			macrosTable->setCurrentCell ( 0,0 );
			startMacrosTimer();
			on_sendButton_clicked();
		}
		else
		{
			m_macrosTimer->stop();
			repeatCheckBox->setCheckState ( Qt::Unchecked );
		}
	}

}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonScrExecute_clicked()
{

//    QCInter *inter = new QCInter(this);
//    inter->exec(scriptEdit->toPlainText().toUtf8());
    QMessageBox::information ( this,
                        tr ( "Information" ),
                        tr ( "Under Construction"));
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::openScriptFile(QString fileName)
{
    if ( fileName.isEmpty() )
    {
        return;
    }

    QFile file ( fileName );

    if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        QMessageBox::critical ( this,
                            tr ( "Error" ),
                            tr ( "Could not open file %1." ).arg ( fileName ) );
        return;
    }

    labelScriptFile->setText(tr("Script file : ") + fileName);
    scriptEdit->clear();

    QTextStream in(&file);
    scriptEdit->insertPlainText(in.readAll());
}

////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonScrOpen_clicked()
{
    QString fileFilters = tr ( "Script files (*.txt);;All files (*.*)" );

    QFileDialog fileDialog( this, tr("Load Script File"), m_scriptDirName, fileFilters );
    fileDialog.setFileMode( QFileDialog::ExistingFile );

    if ( fileDialog.exec() == QDialog::Accepted )
    {
        QStringList fileNames = fileDialog.selectedFiles();
        m_scriptFileName = fileNames[0];
        QDir dir = fileDialog.directory();

        openScriptFile(m_scriptFileName);
    }
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonScrSave_clicked()
{
    if ( m_scriptFileName.isEmpty() )
    {
        return;
    }
//    m_scriptDirName
    QFile file ( m_scriptFileName );

    if ( !file.open ( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        QMessageBox::critical ( this,
                                tr ( "Error" ),
                                tr ( "Could not open file %1." ).arg ( m_scriptFileName ) );
        return;
    }

    QTextStream out(&file);

    out << scriptEdit->toPlainText();

    labelScriptFile->setText(tr("Script file : ") + m_scriptFileName);

    file.close();
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_buttonScrSaveAs_clicked()
{
    QString fileFilters = tr("Script files")+" (*.txt);;"+tr("All files")+" (*.*)";

    QFileDialog fileDialog( this, tr("Save Script File"), m_scriptDirName, fileFilters );
    fileDialog.setFileMode  ( QFileDialog::AnyFile );
    fileDialog.setAcceptMode( QFileDialog::AcceptSave );

    if ( fileDialog.exec() != QDialog::Accepted )
    {
        return;
    }

    QStringList fileNames = fileDialog.selectedFiles();
    m_scriptFileName = fileNames[0];
    QDir dir = fileDialog.directory();

    m_scriptDirName = dir.absolutePath();

    on_buttonScrSave_clicked();
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::calcFontMetrics()
{
	int width;
	int scroll;

	QFontMetrics fm( font() );

	m_FontWidth  = fm.width			(QChar('W'));
	m_FontWidth += fm.leftBearing	(QChar('W'));
	m_FontWidth += fm.rightBearing	(QChar('W'));

	width  = recvEditA->width();
	scroll = recvEditA->verticalScrollBar()->width();

	m_RecvWidth = (width - scroll) / m_FontWidth - 2;
}

void MainWindow::contextMenuEvent ( QContextMenuEvent * event )
{
//    qDebug() << "contextMenuEvent";

    QList<QAction *> actions;
    QAction sep1(NULL);
    QAction sep2(NULL);

    sep1.setSeparator(true);
    sep2.setSeparator(true);

    if(connected())
    {
        actions.append(actionPortDisconnect);
    }
    else
    {
        actions.append(actionPortConnect);
        actions.append(actionPortSettings);
    }


    actions.append(&sep1);
    actions.append(actionASCII_Table);
    actions.append(&sep2);
    actions.append(actionWhatisThis);
//    actions.append(actionAbout_VPTerminal);

    QMenu::exec ( actions,event->globalPos(),actionPortConnect,this);

    QMainWindow::contextMenuEvent(event);
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::buttonConnect_setToolTip()
{
    if(connected())
    {
        buttonConnect->setToolTip("");
//        actionPortConnect->setToolTip("");
    }else
    {
        /*
        QString ttps = tr("Connect to:\n") +
                                    QString("%1\n"
                                            "%2,"
                                            "%3,"
                                            "%4,"
                                            "%5,"
                                            "%6" )
                                           .arg ( m_Settings.DeviceName )
                                           .arg ( m_Settings.BaudRate )
                                           .arg ( m_Settings.DataBits )
                                           .arg ( ParityNames[m_Settings.Parity] )
                                           .arg ( m_Settings.StopBits )
                                           .arg ( FlowCtrlNames[m_Settings.FlowCtrl] );

         buttonConnect->setToolTip(ttps);
*/
//         actionPortConnect->setToolTip(ttps);
    }
}
