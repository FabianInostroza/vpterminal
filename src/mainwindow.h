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

#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "serial.h"


class QSocketNotifier;
class CppHighlighter;
class SendThread;

///////////////////////////////////////////////////////////////////////////////
typedef struct
{
	QString			DeviceName;
	int				BaudRate;
	unsigned char	DataBits;
	unsigned char   Parity;
	unsigned char	StopBits;
	unsigned char	FlowCtrl;

	unsigned char	RtsPolarity;
	int				RtsBeforeTime;
	int				RtsAfterTime;

    char        	CRLF_Symbol;
    bool            BeforeSymbol;
	bool			MacroSendOnEnter;
} SETTINGS;

///////////////////////////////////////////////////////////////////////////////
class MainWindow : public QMainWindow, private Ui_MainWindow
{
		Q_OBJECT

	public:
		MainWindow		( QWidget *parent = 0 );
		~MainWindow		();
                static bool connected	() { return m_Connected; };
		void			initRTS			();
		void			driveRTS		( bool before );
        void			ConvertCommand	( QByteArray &ba );
        unsigned char	DecConvert		( QByteArray ba );
        unsigned char	HexConvert		( QByteArray ba );
        unsigned char	CalcCheckSum	( int XORSUM,QByteArray ba );
		void			calcFontMetrics	();

		static const	QString AppName;
		static const	QString AppVers;
		static const	QString ParityNames  	[3];
		static const	QString FlowCtrlNames	[3];
		static const	QString AsciiChart		[32];

		SETTINGS	    m_Settings;
		SerialPort	    *m_Serial;

	public slots:
        void statusBarMessageChange(const QString &message);
		void on_buttonConnect_clicked();
		void on_buttonDisconnect_clicked();
		void on_buttonSettings_clicked();
		void on_buttonExit_clicked();
		void on_buttonAbout_clicked();
		void on_buttonRecvClear_clicked();
		void on_buttonSendClear_clicked();
		void on_buttonSendFile_clicked();
		void on_buttonAsciiTable_clicked();
		void on_buttonRecvLog_clicked();
        void on_sendButton_clicked();
        void on_removeButton_clicked();
        void on_addButton_clicked();
        void on_loadButton_clicked();
        void on_saveasButton_clicked();
        void on_checkMacroSendOnEnter_clicked();
        void on_repeatCheckBox_clicked();

        void on_buttonScrExecute_clicked();
        void on_buttonScrOpen_clicked();
        void on_buttonScrSave_clicked();
        void on_buttonScrSaveAs_clicked();


		void readData();
		void doOutput();
        void changeLF_Symbol(int index);
        void changeBeforeAfterSym(bool checked);
        void macrosTableItemChange(QTableWidgetItem * item);
        void macrosTableSelChange();
        void macrosDelay();
        void setWhatIsThisMode();
        void aboutQt();

	private:
		void		setupViews		();
		void		setupVars		();
		void		sendFile		( QString fn );
		void		updateRTS		( bool value );
		void		updateDTR		( bool value );
		void		updateInputLines( bool force );
		void		formatOutAscii	( QByteArray &src, QPlainTextEdit *te );
		void		formatOutHex	( QByteArray &src, QPlainTextEdit *te );        
        void        buttonConnect_setToolTip();

        QString		toHex ( QByteArray &array );
        static bool	m_Connected;

		bool		m_NewData;
		QByteArray	m_RecvData;
		QByteArray	m_OutputData;

		bool		m_RTS;
		bool		m_DTR;
		bool		m_CTS;
		bool		m_DSR;
		bool		m_CD;
		bool		m_RI;

		QTimer      *m_OutputTimer;

        QString     m_scriptDirName;
        QString     m_scriptFileName;
        QString     m_macrosDirName;
        QString     m_macrosFileName;
		int			m_FontWidth;
		int			m_RecvWidth;
		int			m_CharPosA;
		int			m_CharPosH;
		QMutex		m_recvMutex;

        CppHighlighter	*m_highLighter;


    protected:
		QTimer      *m_macrosTimer;

		void		startMacrosTimer	();
		bool		eventFilter			( QObject *obj, QEvent *event );
        virtual 	void resizeEvent	( QResizeEvent * event );
        virtual 	void closeEvent		( QCloseEvent * event );		
        int			openMacrosFile		( QString fileName );
        void		openScriptFile		( QString fileName );

		QMessageBox::StandardButton saveMacrosChanges(void);
        virtual void contextMenuEvent ( QContextMenuEvent * event );
        bool connectedToPort();
};

#endif
