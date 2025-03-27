#pragma execution_character_set("utf-8")

#include "Keyboard.h"
#include "ui_Keyboard.h"

#define PROPERTY_PINYIN		"btnPinyin"
#define PROPERTY_NUM		"btnNum"	 
#define PROPERTY_FUN		"btnFun"   
#define PROPERTY_OTHER		"btnOther"

#define CTRL_LINEEDIT		"QLineEdit"
#define CTRL_TEXTEDIT		"QTextEdit"
#define CTRL_WIDGET			"QWidget"

#include "../MathApi.h"

Keyboard *Keyboard::m_pSelf = nullptr;
Keyboard *Keyboard::Instance(QWidget *parent)
{
    if (!m_pSelf) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_pSelf) {
            m_pSelf = new Keyboard(parent);
        }
    }
    return m_pSelf;
}

Stt_KeyboardBtnDescs g_oKeyBoardBtnDescs;
Stt_KeyboardBtnDescs::stt_keyboardbtndescs()
{
	Init();
}
Stt_KeyboardBtnDescs::~stt_keyboardbtndescs()
{

}

void Stt_KeyboardBtnDescs::Init()
{
	m_strVolFunc1 = "100V";
	m_strVolFunc2 = "57.735";
	m_strVolFunc3 = "10V";
	m_strVolMinus = "-";
	m_strCurFunc1 = "10A";
	m_strCurFunc2 = "5A";
	m_strCurFunc3 = "1A";
	m_strCurMinus = "-";
	m_strFreqFunc1 = "60Hz";
	m_strFreqFunc2 = "50Hz";

	m_strPercentageBtn0 = "-";
	m_strPercentageBtn1 = "20%";
	m_strPercentageBtn2 = "30%";
	m_strPercentageBtn3 = "40%";
	m_strPercentageBtn4 = "50%";
	m_strPercentageBtn5 = "60%";
	m_strPercentageBtn6 = "70%";
	m_strPercentageBtn7 = "80%";
	m_strPercentageBtn8 = "90%";
	m_strPercentageBtn9 = "100%";
	m_strPercentageBtnDot = "120%";
//	m_strPercentageBtnMinus = "25%";  
}

Keyboard::Keyboard(QWidget *parent,tagMode oMode) : QWidget(parent), ui(new Ui::Keyboard)
{
	m_pFatherObject = parent;
    ui->setupUi(this);
	m_nCurrVolKeyBoardType = Stt_CurrVolKeyboardType_Vol;
 
	//Start Keyboard Initialization
	InitProperty();
	InitCNList();
	InitUI(oMode);
	InitAttr();
}

Keyboard::~Keyboard()
{
    delete ui;
}

void Keyboard::InitUI(tagMode oMode)
{
	QWidget* pMainWindow = m_pFatherObject;////Get the main window property and bind the main window pointer
	while(!pMainWindow->inherits("QMainWindow"))
	{
		pMainWindow = pMainWindow->parentWidget();
	}
	m_pAnchorWidget = pMainWindow;
	m_nDeskWidth = pMainWindow->geometry().width();
	m_nDeskHeight = pMainWindow->geometry().height();

	//Initialize mouse state
	m_bMousePressed = false;
	m_ptMousePoint = QPoint();
	
	//Initialize case status
	m_bIsUpper = false;

	//Initialization function key
	SetUserFunSetup(false);

	//Initialize WEB component judgment
	m_bIsWebCtrl = false;
	m_bSelectAll = false;

	//Default keyboard auto placement
	m_bAuto = true;

	//Long press the start button to detect
	m_bIsLongPress = false;
	m_tmDeletePress = new QTimer(this);
	connect(m_tmDeletePress, SIGNAL(timeout()), this, SLOT(reClicked()));

	//Initialize focus window
    //m_pCurrentCtrl = nullptr;
	m_bIsDialog = false;
    m_pCurrentWidget = nullptr;
    m_pCurrentLineEdit = nullptr;
    m_pCurrentTextEdit = nullptr;
    m_pCurrentPlain = nullptr;
    m_pCurrentBrowser = nullptr;
	m_strCurrentEditType = "";

	//Initialize Keyboard Panel Properties
	SetFont(font().family(), 18, 16);
	SetFramSize(480, 200);
	SetIconSize(20, 20);
	SetSpacing(6);
	SetTopHeight(40);
	ui->widgetMain->setVisible(true);
	ui->widgetTop->layout()->setContentsMargins(0, 4, 0, 0);
	SetDefaultStyle();
   QPixmap pixmap;
   pixmap.load(":/ctrls/images/keyboard_upper.png");
   m_icoUpper = QIcon(pixmap);
   pixmap.load(":/ctrls/images/keyboard_chinese.png");
   m_icoChinese = QIcon(pixmap);
   pixmap.load(":/ctrls/images/keyboard_english.png");
   m_icoEnglish = QIcon(pixmap);
   pixmap.load(":/ctrls/images/keyboard_number.png");
   m_icoNumber = QIcon(pixmap);
   pixmap.load(":/ctrls/images/keyboard_symbol.png");
   m_icoSymbol = QIcon(pixmap);

	m_bPreSmooth = false;
    m_pPreSmoothObj = nullptr;

	SetInputType(oMode);

	//Load Google Chinese Character Library
    QString strBinPath = "test";
#ifndef _NOT_USE_GOOGLE_PINYIN_
	bool b = m_oPinYinGoogle.open(strBinPath);
#endif

	//Initialize focus style
	m_strFoucsInStyle = "border:3px rgb(0,0,255);border-style:groove;";

	//Bind button to event
	QList<QPushButton *> btns = ui->widgetMain->findChildren<QPushButton *>();
	foreach (QPushButton *btn, btns) 
	{
		connect(btn, SIGNAL(clicked()), this, SLOT(btnClicked()));
	}
	connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(btnClicked()));
	connect(ui->btnNext, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
	connect(ui->btnPre, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));

	//Bind global change focus signal slot
	connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(focusChanged(QWidget *, QWidget *)));

	//Bind key event filter
	qApp->installEventFilter(this);
}

void Keyboard::InitCNList()
{
	m_listCNChar.append(ui->labCh1);
	m_listCNChar.append(ui->labCh2);
	m_listCNChar.append(ui->labCh3);
	m_listCNChar.append(ui->labCh4);
	m_listCNChar.append(ui->labCh5);
}

void Keyboard::InitProperty()
{
	//The buttons on the interface are divided into several categories

	//1. btnPinyin is the button corresponding to the letters a~z, in which 9 keys are repeated with 1~9

	//2. btnNum is 0~9 of the numeric keypad

	//3. btnFun is the four key positions on the left, and the function changes dynamically

	//4. btnOther rollback, cancellation, etc
	ui->btn0->setProperty(PROPERTY_NUM, true);
	ui->btn1->setProperty(PROPERTY_NUM, true);
	ui->btn2->setProperty(PROPERTY_NUM, true);
	ui->btn3->setProperty(PROPERTY_NUM, true);
	ui->btn4->setProperty(PROPERTY_NUM, true);
	ui->btn5->setProperty(PROPERTY_NUM, true);
	ui->btn6->setProperty(PROPERTY_NUM, true);
	ui->btn7->setProperty(PROPERTY_NUM, true);
	ui->btn8->setProperty(PROPERTY_NUM, true);
	ui->btn9->setProperty(PROPERTY_NUM, true);

	ui->btnR->setProperty(PROPERTY_PINYIN, true);
	ui->btnT->setProperty(PROPERTY_PINYIN, true);
	ui->btnY->setProperty(PROPERTY_PINYIN, true);
	ui->btnU->setProperty(PROPERTY_PINYIN, true);
	ui->btnI->setProperty(PROPERTY_PINYIN, true);
	ui->btnO->setProperty(PROPERTY_PINYIN, true);
	ui->btnP->setProperty(PROPERTY_PINYIN, true);
	ui->btnF->setProperty(PROPERTY_PINYIN, true);
	ui->btnG->setProperty(PROPERTY_PINYIN, true);
	ui->btnH->setProperty(PROPERTY_PINYIN, true);
	ui->btnJ->setProperty(PROPERTY_PINYIN, true);
	ui->btnK->setProperty(PROPERTY_PINYIN, true);
	ui->btnL->setProperty(PROPERTY_PINYIN, true);
	ui->btnV->setProperty(PROPERTY_PINYIN, true);
	ui->btnB->setProperty(PROPERTY_PINYIN, true);
	ui->btnN->setProperty(PROPERTY_PINYIN, true);
	ui->btnM->setProperty(PROPERTY_PINYIN, true);

	ui->btnFun1->setProperty(PROPERTY_FUN, true);
	ui->btnFun2->setProperty(PROPERTY_FUN, true);
	ui->btnFun3->setProperty(PROPERTY_FUN, true);
	ui->btnFun4->setProperty(PROPERTY_FUN, true);

	ui->btnDot->setProperty(PROPERTY_OTHER, true);
	ui->btnMinus->setProperty(PROPERTY_OTHER, true);
	ui->btnSpace->setProperty(PROPERTY_OTHER, true);
	ui->btnEnter->setProperty(PROPERTY_OTHER, true);
	ui->btnDelete->setProperty(PROPERTY_OTHER, true);
	ui->btnCancel->setProperty(PROPERTY_OTHER, true);
}

void Keyboard::InitAttr()
{
	m_unEnAttri.m_bMax = false;
	m_unEnAttri.m_bMin = false;
	m_unEnAttri.m_bSize = false;
	m_unAttri.m_fMax = 0;
	m_unAttri.m_fMin = 0;
	m_unAttri.m_nSize = 0;
}

void Keyboard::SetFont(const QString &strFontName, int nBtnFontSize, int nLabFontSize)
{
	QFont btnFont(strFontName, nBtnFontSize);
	QFont labFont(strFontName, nLabFontSize);

	QList<QPushButton *> btns = this->findChildren<QPushButton *>();
	foreach (QPushButton *btn, btns) {
		btn->setFont(btnFont);
	}

	QList<QLabel *> labs = ui->widgetTop->findChildren<QLabel *>();
	foreach (QLabel *lab, labs) {
		lab->setFont(labFont);
	}

	ui->btnPre->setFont(btnFont);
	ui->btnNext->setFont(btnFont);
}

void Keyboard::SetIconSize(int nWidth, int nHeight)
{
	this->m_nIconWidth = nWidth;
	this->m_nIconHeight = nHeight;
	QSize icoSize(nWidth, nHeight);
	QSize icoSize2(nWidth - 5, nHeight - 5);
	QSize icoSize3(nWidth + 3, nHeight + 3);

	ui->btnSpace->setIconSize(icoSize);
	ui->btnCancel->setIconSize(icoSize);
	ui->btnFun4->setIconSize(icoSize);
	ui->btnEnter->setIconSize(icoSize2);
	ui->btnFun3->setIconSize(icoSize2);
	ui->btnDelete->setIconSize(icoSize3);
}

void Keyboard::SetFramSize(int nWidth, int nHeight)
{
	m_nFramWidth = nWidth;
	m_nFramHeight = nHeight;
	setMinimumSize(nWidth, nHeight);
}

void Keyboard::SetSpacing(int nSpacing)
{
	m_nSpacing = nSpacing;
	ui->widgetMain->layout()->setSpacing(nSpacing);
}

void Keyboard::SetTopHeight(int nTopHeight)
{
	m_nTopHeight = nTopHeight;
	ui->widgetTop->setMaximumHeight(ui->btn1->geometry().height());//Set the maximum height of input method of Chinese selection bar
	ui->widgetTop->setMinimumHeight(ui->btn1->geometry().height());//Set the minimum height of input method of Chinese selection bar
}

void Keyboard::SetDefaultStyle()
{
	SetColor("#191919", "#F3F3F3", "#313131", "#4189D3", "#F3F3F3", "#F95717", "#F3F3F3");
}

void Keyboard::UpdateVisible(tagMode nMode)
{
	switch(nMode)
	{
	case CHINESE: 
		ui->btnFun1->hide();
		ui->btnFun2->hide();
		ui->btnFun3->hide();
		ui->btnFun4->show();
		ui->btnFun3->setIcon(QIcon());
		ui->btnFun4->setIcon(m_icoNumber);
		ui->widgetTop->setVisible(true);
		ui->btnSpace->setVisible(true);
		break;
	case ENGLISH: 
		ui->btnFun1->hide();
		ui->btnFun2->hide();
		ui->btnFun3->show();
		ui->btnFun4->show();
		ui->btnFun3->setIcon(m_icoUpper);
		ui->btnFun4->setIcon(m_icoSymbol);
		ui->widgetTop->setVisible(false);
		ui->btnSpace->setVisible(true);
		break;	
	case NUMBER: 
		ui->btnFun1->hide();
		ui->btnFun2->hide();
		ui->btnFun3->hide();
		ui->btnFun4->show();
		ui->btnFun3->setIcon(QIcon());
		ui->btnFun4->setIcon(m_icoEnglish);
		ui->widgetTop->setVisible(false);
		ui->btnSpace->setVisible(false);
		break;
	case ONLYNUMBER:
		ui->btnFun1->hide();
		ui->btnFun2->hide();
		ui->btnFun3->hide();
		ui->btnFun4->hide();
		ui->widgetTop->setVisible(false);
		ui->btnSpace->setVisible(false);
		break;
	case PERCENTAGE:
		ui->btnFun1->show();
		ui->btnFun2->show();
		ui->btnFun3->show();
		ui->btnFun4->show();
		ui->widgetTop->setVisible(false);
		ui->btnSpace->setVisible(false);
		break;
	case SYMBOL: 
		ui->btnFun1->hide();
		ui->btnFun2->hide();
		ui->btnFun3->hide();
		ui->btnFun4->show();
		ui->btnFun3->setIcon(QIcon());
		ui->btnFun4->setIcon(m_icoChinese);
		ui->widgetTop->setVisible(false);
		ui->btnSpace->setVisible(true);
		break;
	default:
		break;
	}

	QList<QPushButton *> btns = this->findChildren<QPushButton *>();
	foreach (QPushButton *btn, btns)
	{
		if(btn->property(PROPERTY_PINYIN).toBool())
		{
			if(nMode == NUMBER || nMode == ONLYNUMBER || nMode == PERCENTAGE)
			{
				btn->setVisible(false);
			}
			else
			{
				btn->setVisible(true);
			}
		}
	}
}

void Keyboard::SetInputType(tagMode nMode)
{
	m_nInputMode = nMode;
	switch(nMode)
	{
	case CHINESE: 
	case ENGLISH: 
		ui->btn0->setText("?");
		ui->btn1->setText("q");
		ui->btn2->setText("w");
		ui->btn3->setText("e");
		ui->btn4->setText("a");
		ui->btn5->setText("s");
		ui->btn6->setText("d");
		ui->btn7->setText("z");
		ui->btn8->setText("x");
		ui->btn9->setText("c");
		ui->btnR->setText("r");
		ui->btnT->setText("t");
		ui->btnY->setText("y");
		ui->btnU->setText("u");
		ui->btnI->setText("i");
		ui->btnO->setText("o");
		ui->btnP->setText("p");
		ui->btnF->setText("f");
		ui->btnG->setText("g");
		ui->btnH->setText("h");
		ui->btnJ->setText("j");
		ui->btnK->setText("k");
		ui->btnL->setText("l");
		ui->btnV->setText("v");
		ui->btnB->setText("b");
		ui->btnN->setText("n");
		ui->btnM->setText("m");
		break;
	case SYMBOL: 
		ui->btn0->setText("?");
		ui->btn1->setText("~");
		ui->btn2->setText("!");
		ui->btn3->setText("@");
		ui->btn4->setText("#");
		ui->btn5->setText("$");
		ui->btn6->setText("%");
		ui->btn7->setText("^");
		ui->btn8->setText("&&");
		ui->btn9->setText("*");
		ui->btnR->setText("(");
		ui->btnT->setText(")");
		ui->btnY->setText("=");
		ui->btnU->setText("+");
		ui->btnI->setText("/");
		ui->btnO->setText("\"");
		ui->btnP->setText("'");
		ui->btnF->setText("\\");
		ui->btnG->setText("[");
		ui->btnH->setText("]");
		ui->btnJ->setText("<");
		ui->btnK->setText(">");
		ui->btnL->setText(",");
		ui->btnV->setText("_");
		ui->btnB->setText(";");
		ui->btnN->setText(":");
		ui->btnM->setText("|");
		break;
	case NUMBER: 
	case ONLYNUMBER:
		ui->btn0->setText("0");
		ui->btn1->setText("1");
		ui->btn2->setText("2");
		ui->btn3->setText("3");
		ui->btn4->setText("4");
		ui->btn5->setText("5");
		ui->btn6->setText("6");
		ui->btn7->setText("7");
		ui->btn8->setText("8");
		ui->btn9->setText("9");
		break;
	case PERCENTAGE:
		ui->btn0->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn0);
		ui->btn1->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn1);
		ui->btn2->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn2);
		ui->btn3->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn3);
		ui->btn4->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn4);
		ui->btn5->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn5);
		ui->btn6->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn6);
		ui->btn7->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn7);
		ui->btn8->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn8);
		ui->btn9->setText(g_oKeyBoardBtnDescs.m_strPercentageBtn9);
		ui->btnDot->setText(g_oKeyBoardBtnDescs.m_strPercentageBtnDot);
//		ui->btnMinus->setText(g_oKeyBoardBtnDescs.m_strPercentageBtnMinus);
		break;
	default:
		break;
	}

	UpdateVisible(nMode);
	UpdateTopWidget();
	UpdateShowRect();
	UpdateBtnsRect();
	
	//Every time you switch to the mode, you need to clear the information in the previous Chinese mode
	ClearChinese();
	ui->labPY->setText("");
}

void Keyboard::UpdateBtnsRect()
{
	long nBtnWidth = 60;
	long nBtnHeight = 60;
	long nSpace = 10;

	long nWidth = geometry().width();
	long nHeight = geometry().height();
	long nPYHeight = ui->labPY->geometry().height();

	switch(m_nInputMode)
	{
	case CHINESE: 
		nBtnWidth = (nWidth - 11 * nSpace) / 12;
		nBtnHeight = (nHeight - nPYHeight - 3 * nSpace) / 4;
		break;
	case ENGLISH: 
	case SYMBOL: 
		nBtnWidth = (nWidth - 11 * nSpace) / 12;
		nBtnHeight = (nHeight - 3 * nSpace) / 4;
		break;
	case NUMBER: 
		nBtnWidth = (nWidth - 4 * nSpace) / 5;
		nBtnHeight = (nHeight - 3 * nSpace) / 4;
		break;
	case ONLYNUMBER:
		nBtnWidth = (nWidth - 3 * nSpace) / 4;
		nBtnHeight = (nHeight - 3 * nSpace) / 4;
		if(ui->btnFun1->isVisible())
		{
			nBtnWidth = (nWidth - 4 * nSpace) / 5;
			nBtnHeight = (nHeight - 3 * nSpace) / 4;
		}
		break;
	case PERCENTAGE:
		nBtnWidth = (nWidth - 4 * nSpace) / 5;
		nBtnHeight = (nHeight - 3 * nSpace) / 4;
		//if(ui->btnFun1->isVisible())
		//{
		//	nBtnWidth = (nWidth - 4 * nSpace) / 5;
		//	nBtnHeight = (nHeight - 3 * nSpace) / 4;
		//}
		break;
	default:
		break;
	}

	QList<QPushButton *> btns = ui->widgetMain->findChildren<QPushButton *>();
	foreach (QPushButton *btn, btns) 
	{
		if(btn->objectName() == "btnDelete"
			|| btn->objectName() == "btnEnter")
		{
			btn->setMinimumSize(nBtnWidth, nBtnHeight * 2 + nSpace / 2);
		}
		else
		{
			btn->setMinimumSize(nBtnWidth, nBtnHeight);
		}
	}
}

void Keyboard::SetColor(const QString &strMainBackgroundColor, const QString &strMainTextColor, const QString &strBtnBackgroundColor,
						 const QString &strBtnHoveColor, const QString &strBtnHoveTextColor, const QString &strLabHoveColor, const QString &strLabHoveTextColor)
{
	QStringList listQss;
	listQss.append(QString("QWidget#widgetPreNext{background:none;}"));
	listQss.append(QString("QWidget#widgetMain,QWidget#widgetTop,QWidget#widgetNumber{background-color:%1;}").arg(strMainBackgroundColor));
	listQss.append(QString("QPushButton{border-width:0px;border-radius:3px;color:%1;}").arg(strMainTextColor));
	listQss.append(QString("QPushButton{padding:0px;background-color:%1;}").arg(strBtnBackgroundColor));
	//listQss.append(QString("QPushButton:pressed,QPushButton:hover{background-color:%1;color:%2;}").arg(strBtnHoveColor).arg(strBtnHoveTextColor));
	listQss.append(QString("QPushButton:pressed{background-color:%1;color:%2;}").arg(strBtnHoveColor).arg(strBtnHoveTextColor));
	listQss.append(QString("QLabel{border-width:0px;border-radius:3px;color:%1;background:none;}").arg(strMainTextColor));
	//listQss.append(QString("QLabel:pressed,QLabel:hover{background-color:%1;color:%2;}").arg(strLabHoveColor).arg(strLabHoveTextColor));
	listQss.append(QString("QLabel:pressed{background-color:%1;color:%2;}").arg(strLabHoveColor).arg(strLabHoveTextColor));
	//Set the main style of input method
	this->setStyleSheet(listQss.join("QWidget{}"));
	//this->setStyleSheet(listQss.join("QWidget{font-size:12px;}"));
}

void Keyboard::focusChanged(QWidget *oldWidget, QWidget *nowWidget)
{
	if (nowWidget != 0 && !isAncestorOf(nowWidget)) 
	{
		//In Qt5 and Linux systems (except for embedded linux), when the input method panel is closed, the focus will become none, and then the focus will move to the focus control again
//As a result, the close button of the input method panel will not work. After closing, a control will get the focus and display again
//To this end, add judgment, and do not display when the focus is changed from an object to a non object and then to an object
//Here is another judgment. If the first focus of the first form falls on the inputtable object, it should be filtered out
// 		static bool bFirst = true;
// 
// 		if (oldWidget == 0x0 && !bFirst) 
// 		{
// 			qDebug() << "bFirst";
// 			HidePanel();
// 			g_theTestCntrFrame->m_pResultWidget->LogString(nullptr, "focusChanged: bFirst");
// 			return;
// 		}
// 
// 		//If the corresponding attribute noinput is true, it will not be displayed
// 		if (nowWidget->property("noinput").toBool()) 
// 		{
// 			QTimer::singleShot(0, this, SLOT(hidePanel()));
// 			return;
// 		}
// 		bFirst = false;

		m_strCurrentFlag = nowWidget->property("flag").toString();
		if(nowWidget != m_pCurrentWidget
			&& isVisible())
		{
			KillFocusBorder();
		}
		m_pCurrentWidget = nowWidget;
		if (nowWidget->inherits(CTRL_LINEEDIT)) 
		{
			m_pCurrentLineEdit = (QLineEdit *)nowWidget;
			if (!m_pCurrentLineEdit->isReadOnly()) 
			{
				m_strCurrentEditType = CTRL_LINEEDIT;
				ShowPanel(nowWidget);
			} 
			else 
			{
				m_strCurrentEditType = "";
				HidePanel();
			}
		} 
		else if (nowWidget->inherits(CTRL_TEXTEDIT)) 
		{
			m_pCurrentTextEdit = (QTextEdit *)nowWidget;
			if (!m_pCurrentTextEdit->isReadOnly()) 
			{
				m_strCurrentEditType = CTRL_TEXTEDIT;
				ShowPanel(nowWidget);
			} 
			else 
			{
				m_strCurrentEditType = "";
				HidePanel();
			}
		} 
		else 
		{
			if(m_pCurrentTextEdit)
			{
				m_pCurrentTextEdit->setStyleSheet(m_strFoucsOutStyle);
			}
			else if(m_pCurrentLineEdit)
			{
				m_pCurrentLineEdit->setStyleSheet(m_strFoucsOutStyle);
			}
			else if(m_pCurrentPlain)
			{
				m_pCurrentPlain->setStyleSheet(m_strFoucsOutStyle);
			}
			else if(m_pCurrentBrowser)
			{
				m_pCurrentBrowser->setStyleSheet(m_strFoucsOutStyle);
			}

            m_pCurrentWidget = nullptr;
            m_pCurrentLineEdit = nullptr;
            m_pCurrentTextEdit = nullptr;
            m_pCurrentPlain = nullptr;
            m_pCurrentBrowser = nullptr;
			m_strCurrentEditType = "";

			if(!m_bIsWebCtrl)
			{
				HidePanel();
			}
		}

		//Set according to the input method position selected by the user - display in the center - fill at the bottom - display directly below the input box
		int height = this->height();

		//If automatic placement is not enabled, it is only set at the bottom
		if(!m_bAuto)
		{
			if(m_pFatherObject == m_pAnchorWidget)
			{
				this->setGeometry(0, m_nDeskHeight - height, m_nDeskWidth, height);
			}
			else
			{
				QRect rc = m_pAnchorWidget->normalGeometry();
				this->setGeometry(rc.left(), rc.top() + m_nDeskHeight - height, m_nDeskWidth, height);
			}			
		}
	}

	//Set the size and position of the button
	UpdateTopWidget();
}

void Keyboard::UpdateTopWidget()
{
	if(ui->widgetTop->isVisible())
	{
		int nFitWidth = (ui->widgetTop->width() - ui->widgetMain->layout()->spacing() * 7) / 8;
		ui->btnPre->setMaximumWidth(nFitWidth);
		ui->btnPre->setMinimumWidth(nFitWidth);
		ui->btnNext->setMaximumWidth(nFitWidth);
		ui->btnNext->setMinimumWidth(nFitWidth);
		ui->labCh1->setMaximumWidth(nFitWidth);
		ui->labCh1->setMinimumWidth(nFitWidth);
		ui->labCh2->setMaximumWidth(nFitWidth);
		ui->labCh2->setMinimumWidth(nFitWidth);
		ui->labCh3->setMaximumWidth(nFitWidth);
		ui->labCh3->setMinimumWidth(nFitWidth);
		ui->labCh4->setMaximumWidth(nFitWidth);
		ui->labCh4->setMinimumWidth(nFitWidth);
		ui->labCh5->setMaximumWidth(nFitWidth);
		ui->labCh5->setMinimumWidth(nFitWidth);
		ui->labPY->setMaximumWidth(nFitWidth);
		ui->labPY->setMinimumWidth(nFitWidth);
	}
}

void Keyboard::UpdateCurrentLineEdit(QLineEdit* pLineEdit)
{
	m_pCurrentWidget = pLineEdit;
	m_pCurrentLineEdit = pLineEdit;
}

void Keyboard::ShowSmoothEdit()
{
	if(m_pPreSmoothObj)
	{
		ShowPanel(m_pPreSmoothObj);
        m_pPreSmoothObj = nullptr;
	}
}

void Keyboard::SetMinusBtnText(QString strText)
{
	ui->btnMinus->setText(strText);
}

void Keyboard::UpdateShowRect(long nCurX, long nCurY, long nCurWidth, long nCurHeight)
{
	if(!m_bAuto)
	{
		return;
	}

	if(!m_pCurrentWidget && !nCurX && !nCurY && !nCurWidth && !nCurHeight)
	{
		return;
	}

	QRect rcNum = QRect(0, 0, 460, 360);
	QRect rcSymbol = QRect(0, 0, 980, 360);
	if (m_nInputMode == CHINESE)
	{
		rcSymbol = QRect(0, 0, 980, 420);
	}

	long nWidth = 0, nHeight = 0;
	if (m_nInputMode == NUMBER
		|| m_nInputMode == ONLYNUMBER 
		|| m_nInputMode == PERCENTAGE)
	{
		nWidth = rcNum.width();
		nHeight = rcNum.height();
	}
	else
	{
		nWidth = rcSymbol.width();
		nHeight = rcSymbol.height();
	}

	QPoint pt;
	QRect rc;
	QRect rcDesktop = QRect(0, 0, m_pAnchorWidget->width(), m_pAnchorWidget->height());
	if(nCurX || nCurY)
	{
		pt = QPoint(nCurX, nCurY);
	}
	else
	{
		pt = m_pCurrentWidget->mapToGlobal(QPoint(0, 0));
	}

	if(nCurWidth || nCurHeight)
	{
		rc = QRect(pt.x(), pt.y(), nCurWidth, nCurHeight);
	}
	else
	{
		rc = QRect(pt.x(), pt.y(), m_pCurrentWidget->width(), m_pCurrentWidget->height());
	}

	long nLeft = rc.left() - nWidth;
	long nRight = rc.right() + nWidth;
	long nTop = rc.top() - nHeight;
	long nBottom = rc.bottom() + nHeight;

	//*********Determine the placement position in 8 directions around the control
// 1     2     3
// 4    Item   5
// 6     7     8
//******************************************
	
	if(nLeft > rcDesktop.left())
	{
		//Place Left
		if(nTop > rcDesktop.top())
		{
			//Place 1
			setGeometry(nLeft, nTop, nWidth, nHeight);
		}
		else if (nBottom < rcDesktop.bottom())
		{
			//Place 6
			setGeometry(nLeft, rc.bottom(), nWidth, nHeight);
		}
		else
		{
			//Place 4
			setGeometry(nLeft, 0, nWidth, nHeight);//20221227 zhouhj
			//setGeometry(nLeft, rc.top(), nWidth, nHeight);
		}
	}
	else if(nRight < rcDesktop.right())
	{
		if(nTop > rcDesktop.top())
		{
			//Place 3
			setGeometry(rc.right(), nTop, nWidth, nHeight);
		}
		else if (nBottom < rcDesktop.bottom())
		{
			//Place 8
			setGeometry(rc.right(), rc.bottom(), nWidth, nHeight);
		}
		else
		{
			//Place 5
			setGeometry(rc.right(), 0, nWidth, nHeight);//20221227 zhouhj
			//setGeometry(rc.right(), rc.top(), nWidth, nHeight);
		}
	}
	else
	{
		if(nTop > rcDesktop.top())
		{
			//Place 2
			setGeometry(rcDesktop.right() - nWidth, nTop, nWidth, nHeight);
		}
		else
		{
			//Place 7
			setGeometry(rcDesktop.right() - nWidth, rc.bottom(), nWidth, nHeight);
		}
	}
}

void Keyboard::ShowPanel(QObject *objCurr)
{
	if (!isEnabled()) 
	{
		return;
	}

	if(m_bPreSmooth)
	{
		m_pPreSmoothObj = objCurr;
		return;
	}

	m_bIsWebCtrl = false;
	SetFocusBorder();
	this->setVisible(true);
	this->raise();

	UpdateShowRect();
	UpdateBtnsRect();

//	qDebug() << "KeyboardShow";
}

void Keyboard::SetFocusBorder()
{
	return;

	if(m_pCurrentWidget)
	{
		if(m_strFoucsInStyle != m_pCurrentWidget->styleSheet())
		{
			m_strFoucsOutStyle = m_pCurrentWidget->styleSheet();
		}

		QFont font = m_pCurrentWidget->font();
		QString strFontStyle = QString("font-size:%1px;").arg(font.pixelSize());
		m_pCurrentWidget->setStyleSheet(m_strFoucsInStyle + strFontStyle);
	}
}

void Keyboard::KillFocusBorder()
{
	if(m_pCurrentLineEdit)
	{
		m_pCurrentLineEdit->setStyleSheet(m_strFoucsOutStyle);
	}
	else if(m_pCurrentBrowser)
	{
		m_pCurrentBrowser->setStyleSheet(m_strFoucsOutStyle);
	}
	else if(m_pCurrentPlain)
	{
		m_pCurrentPlain->setStyleSheet(m_strFoucsOutStyle);
	}
	else if(m_pCurrentTextEdit)
	{
		m_pCurrentTextEdit->setStyleSheet(m_strFoucsOutStyle);
	}
}

void Keyboard::HidePanel()
{
	if(isVisible())
	{
		if(m_bIsDialog)
		{
			emit sig_clickEnter();
		}
        KillFocusBorder();
        UpdateFocus();
		setVisible(false);
		SetDefaultFunc();
//		qDebug() << "KeyboardHide";
	}
}
//使Fun可用
void Keyboard::SetDefaultFunc()
{
	SetUserFunSetup(true);

	//对当前键盘进行判断，如果是百分比键盘，则修改为数字键盘
	if(m_nInputMode == PERCENTAGE)
	{
		m_nInputMode = ONLYNUMBER;
		ui->btnDot->setText(".");
	}

	ui->btnMinus->setText("-");
	SetUserFunString("", "", "", "");
}

void Keyboard::SetAutoKeyboard(bool bAuto)
{
	m_bAuto = bAuto;
}

QString Keyboard::GetCurWidgetText()
{
	if(m_pCurrentWidget)
	{
		if(m_pCurrentLineEdit)
		{
			return m_pCurrentLineEdit->text();
		}
		else if(m_pCurrentTextEdit)
		{
			return m_pCurrentTextEdit->toPlainText();
		}
		else if (m_pCurrentPlain)
		{
			return m_pCurrentPlain->toPlainText();
		}
	}
	return "";
}

void Keyboard::SetCurWidgetText(const QString& strText)
{
	if(m_pCurrentWidget)
	{
		if(m_pCurrentLineEdit)
		{
			m_pCurrentLineEdit->setText(strText);
		}
		else if(m_pCurrentTextEdit)
		{
			m_pCurrentTextEdit->setPlainText(strText);
		}
		else if (m_pCurrentPlain)
		{
			m_pCurrentPlain->setPlainText(strText);
		}
	}
}

void Keyboard::SetUserFunSetup(bool bSetup)
{
	ui->btnFun1->setVisible(bSetup);
	ui->btnFun2->setVisible(bSetup);
	ui->btnFun3->setVisible(bSetup);
	ui->btnFun4->setVisible(bSetup);

	if (!bSetup)//
	{
		ui->btnMinus->setText("-");

		if (m_nInputMode == PERCENTAGE)
		{
			ui->btnDot->setText(".");
			SetInputType(ONLYNUMBER);
//			CLogPrint::LogFormatString(XLOGLEVEL_RESULT,_T("m_nInputMode == PERCENTAGE"));
		}
	}
//	CLogPrint::LogFormatString(XLOGLEVEL_RESULT,_T("SetUserFunSetup(%d)"),(int)bSetup);
}

void Keyboard::SetUserFunString(const QString& strFun1, const QString& strFun2,
					  const QString& strFun3, const QString& strFun4)
{
	ui->btnFun1->setText(strFun1);
	ui->btnFun2->setText(strFun2);
	ui->btnFun3->setText(strFun3);
	ui->btnFun4->setText(strFun4);

	if(strFun4.length())
	{
		ui->btnFun4->setIcon(QIcon());
	}
	else
	{
		SetInputType(m_nInputMode);
	}
}

void Keyboard::ShowWebPanel(const QString& strValue)
{
	if(!isVisible())
	{
		m_bIsWebCtrl = true;
		m_bSelectAll = false;
		show();
		UpdateBtnsRect();
	}
}

void Keyboard::btnClicked_Web()
{

}

bool Keyboard::GetSelectAll()
{
	return m_bSelectAll;
}

void Keyboard::DeleteValue_Web()
{
	emit sig_deleteChanged();
}

bool Keyboard::IsSameWebCtrlID(const QString &strCtrlID)
{
	return (strCtrlID == m_strCurWebCtrlID);
}

void Keyboard::CheckWebAttribute()
{
	m_strCurWebCtrlID = "";
}

void Keyboard::InsertValue_Web(const QString &v)
{
	m_bSelectAll = true;
	emit sig_updateChanged(v);
}

void Keyboard::mouseMoveEvent(QMouseEvent *e)
{
}

void Keyboard::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {
		m_bMousePressed = true;
		m_ptMousePoint = e->globalPos() - this->pos();
		e->accept();
	}
}

void Keyboard::mouseReleaseEvent(QMouseEvent *)
{
	m_bMousePressed = false;
}

void Keyboard::DealBtnFunEx_Persent()
{
	switch(m_nInputMode)
	{
	case ONLYNUMBER:
		SetInputType(PERCENTAGE);
		break;
	case PERCENTAGE:
		SetInputType(ONLYNUMBER);
		ui->btnFun1->setVisible(true);
		UpdateBtnsRect();
		ui->btnDot->setText(".");
//		ui->btnMinus->setText(g_oKeyBoardBtnDescs);
		SetUserFunSetup(true);
		break;
	default:
		break;
	}
}

void Keyboard::DealBtnFun4()
{
	//Keyboard switch
	switch(m_nInputMode)
	{
	case NUMBER:
		SetInputType(ENGLISH);
		break;
	case CHINESE:
		SetInputType(NUMBER);
		break;
	case ENGLISH:
		SetInputType(SYMBOL);
		break;
	case SYMBOL:
		SetInputType(CHINESE);
		break;
	default:
		break;
	}

	m_bIsUpper = false;
	SetUpper(m_bIsUpper);
}

void Keyboard::DealBtnFun3()
{
	switch(m_nInputMode)
	{
	case SYMBOL:
	case NUMBER:
	case CHINESE:
		break;
	case ENGLISH:
		m_bIsUpper = !m_bIsUpper;
		SetUpper(m_bIsUpper);
		break;
	default:
		break;
	}
}

void Keyboard::DealBtnDelete(const QString& strText)
{
	//If the current mode is Chinese, delete the corresponding pinyin. After deleting the pinyin, delete the contents of the corresponding text input box
	if (m_nInputMode == CHINESE) 
	{
		QString txt = strText;
		int len = txt.length();

		if (len > 0) 
		{
			ui->labPY->setText(txt.left(len - 1));
			SelectChinese();
		} 
		else 
		{
			DeleteValue();
		}
	} 
	else 
	{
		DeleteValue();
	}
}

void Keyboard::DealBtnPre()
{
	if (m_nCurrentPYIndex >= 10) 
	{
		//At most 5 Chinese characters can be displayed each time, so the index will be reduced by 5 * 2 each time you move forward
		if (m_nCurrentPYIndex % 5 == 0) 
		{
			m_nCurrentPYIndex -= 10;
		} 
		else 
		{
			m_nCurrentPYIndex = m_nCurrentPYCount - (m_nCurrentPYCount % 5) - 5;
		}
	} 
	else 
	{
		m_nCurrentPYIndex = 0;
	}

	ShowChinese();
}

void Keyboard::DealBtnNext()
{
	if (m_nCurrentPYIndex < m_nCurrentPYCount - 1) 
	{
		ShowChinese();
	}
}

void Keyboard::DealBtnCancel()
{
	ClearChinese();
	ui->labPY->setText("");
}

void Keyboard::DealBtnSpace(const QString& strText)
{
	//If there is Chinese mode and letters need to be input, determine whether there is Chinese, insert the first Chinese; otherwise, insert letters
	if (m_nInputMode == CHINESE && strText != "") 
	{
		if (ui->labCh1->text().isEmpty()) 
		{
			InsertValue(strText);
		} 
		else 
		{
			SetChinese(0);
		}
	} 
	else 
	{
		InsertValue(" ");
	}
}

void Keyboard::DealBtnEnter(const QString& strText)
{
	//If Chinese mode and letters need to be input, insert letters immediately
	if (m_nInputMode == CHINESE && strText != "") 
	{
		InsertValue(strText);
	}

	if (m_pCurrentLineEdit != 0) 
	{
		UpdateFocus();
		//QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier, QString("\n"));
		//QApplication::sendEvent(m_pCurrentLineEdit, &keyPress);
	} 
	else 
	{
		InsertValue("\n");
	}

	HidePanel();
}

void Keyboard::DealBtnDefault(QString strText)
{
	QString strLabText = ui->labPY->text();
	QPushButton* pBtn = (QPushButton*)sender();
	QString strObjectName = pBtn->objectName();

	//If it is the&button, because the corresponding&is filtered, the real text is to remove the previous&character
	if (strText == "&&") {
		strText = "&";
	}

	//If the current mode is not Chinese, click the button and the corresponding text is the transfer parameter
	if (m_nInputMode!= CHINESE) 
	{
		InsertValue(strText);
	} 
	else 
	{
		//In Chinese mode, it is not allowed to enter special characters. Click the corresponding number key to obtain the current indexed Chinese characters
		if (pBtn->property(PROPERTY_OTHER).toBool()) 
		{
			if (strLabText.length() == 0) 
			{
				InsertValue(strText);
			}
		}
		else if (pBtn->property(PROPERTY_PINYIN).toBool()
			|| pBtn->property(PROPERTY_NUM).toBool()) 
		{
			ui->labPY->setText(strLabText + strText);
			SelectChinese();
		}
	}
}

void Keyboard::btnClicked()
{    
 	if(!m_bIsWebCtrl)
 	{
		//If the current focus control type is null, the return does not need to continue processing
		if (m_strCurrentEditType == "") 
		{
			return;
		}
 	}

	QString strLabText = ui->labPY->text();
	QPushButton* pBtn = (QPushButton*)sender();
	QString strBtnText = pBtn->text();
	QString strObjectName = pBtn->objectName();

    if (strBtnText == QStringLiteral("Ex"))
	{
		DealBtnFunEx_Persent();
	}
	//Process specific buttons according to ObjectName
	else if(strObjectName == "btnFun1")
	{
		emit sig_clickFunc1(pBtn);
		/*QString strBtnText = pBtn->text();
		if(strBtnText == "Ex")
		{
			DealBtnFun1();
		}
		else
		{
		emit sig_clickFunc1(pBtn);
		}*/
	}
	else if(strObjectName == "btnFun2")
	{
		emit sig_clickFunc2(pBtn);
	}
	else if(strObjectName == "btnFun3")
	{
		QString strBtnText = pBtn->text();
		if(!strBtnText.length())
		{
			DealBtnFun3();
		}
		else 
		{
			emit sig_clickFunc3(pBtn);
		}
	}
	else if(strObjectName == "btnFun4")
	{
		QString strBtnText = pBtn->text();
		if(!strBtnText.length())
		{
			DealBtnFun4();
		}
		else 
		{
			emit sig_clickFunc4(pBtn);
		}
	}
	else if (strObjectName == "btnDelete")
	{
		DealBtnDelete(strLabText);
	}
	else if (strObjectName == "btnPre") 
	{
		DealBtnPre();
	} 
	else if (strObjectName == "btnNext") 
	{
		DealBtnNext();
	} 
	else if (strObjectName == "btnCancel") 
	{
		DealBtnCancel();
	} 
	else if (strObjectName == "btnSpace") 
	{
		DealBtnSpace(strLabText);
	} 
	else if (strObjectName == "btnEnter") 
	{
		DealBtnEnter(strLabText);
	} 
	else 
	{
		QString value = pBtn->text();
		DealBtnDefault(value);
	}
}

void Keyboard::SetUpper(bool isUpper)
{
	QList<QPushButton *> btn = ui->widgetMain->findChildren<QPushButton *>();
	foreach (QPushButton *b, btn) 
	{
		if (b->property(PROPERTY_PINYIN).toBool()
			|| b->property(PROPERTY_NUM).toBool()) 
		{
			if (isUpper) 
			{
				b->setText(b->text().toUpper());
			} 
			else 
			{
				b->setText(b->text().toLower());
			}
		}
	}
}


void Keyboard::SelectChinese()
{
	//Clear Chinese Characters
	ClearChinese();

	QString strText = ui->labPY->text();
	QStringList list;

#ifndef _NOT_USE_GOOGLE_PINYIN_
	int count = m_oPinYinGoogle.select(strText);
	for (int i = 0; i < count; i++) 
	{
		QString str = m_oPinYinGoogle.getChinese(i);
		list << str;
	}
#endif

	for (int nIndex = 0;nIndex<list.size();nIndex++)
	{
		strText = list.at(nIndex);

		if (strText.length() > 0) 
		{
			m_listAllPY.append(strText);
			m_nCurrentPYCount++;
		}
	}
// 	foreach (QString txt, list) 
// 	{
// 		if (txt.length() > 0) 
// 		{
// 			m_listAllPY.append(txt);
// 			m_nCurrentPYCount++;
// 		}
// 	}

	//
	ShowChinese();
}

void Keyboard::ShowChinese()
{
	//Each layout can display up to chineseCount Chinese characters
	int count = 0;    
	m_listCurrentPY.clear();

	for (int i = 0; i < m_listCNChar.count(); i++)
	{
		m_listCNChar.at(i)->setText("");
	}

	for (int i = m_nCurrentPYIndex; i < m_nCurrentPYCount; i++) {
		if (count == 5) {
			break;
		}

		QString txt = QString("%1. %2").arg(count + 1).arg(m_listAllPY.at(m_nCurrentPYIndex));
		m_listCurrentPY.append(m_listAllPY.at(m_nCurrentPYIndex));
		m_listCNChar.at(count)->setText(txt);
		count++;
		m_nCurrentPYIndex++;
	}
}

void Keyboard::SetKeyBoardType(long nType)
{
	m_nCurrVolKeyBoardType = nType;
// 	if(nType == 0)
// 	{
// 		m_strKeyBoardType = "Vol";
// 	}
// 	else
// 	{
// 		m_strKeyBoardType = "Cur";
// 	}
}

QString Keyboard::SetPercentTextValue(QString strText)
{
	QString strTemp = strText.left(strText.length()-1);
	//strText = strText.left(strText.length() - 1);
	float nTempNumIV;//记录获取的当前额定电压/电流的值
//	if(m_strKeyBoardType == "Vol")
	if (m_nCurrVolKeyBoardType == Stt_CurrVolKeyboardType_Vol)
	{
		nTempNumIV = *(g_oKeyBoardBtnDescs.m_pfVNom) /* / SQRT3*/;//zhouhj 2023.8.30
	}
	else
	{
		nTempNumIV = *(g_oKeyBoardBtnDescs.m_pfINom);
	}
	float nCount = strTemp.toFloat();
	nCount = nCount / 100 * nTempNumIV;
	strTemp = QString::number(nCount);
	return strTemp;
}

void Keyboard::InsertValue(const QString &v)
{
	if(m_bIsWebCtrl)
	{
		InsertValue_Web(v);

		//Make all Chinese character labels visible
		ui->labCh3->setVisible(true);
		ui->labCh4->setVisible(true);
		ui->labCh5->setVisible(true);
		ui->labPY->setText("");

		return;
	}

	QString value = v;

	//2023/8/15 wjs 是百分比键盘则执行以下内容
	if((m_nInputMode == PERCENTAGE)
		&&(value.contains("%"))&&(value.size()>1))
	{
		value = SetPercentTextValue(value);
		SetCurWidgetText(value);

		ui->labCh3->setVisible(true);
		ui->labCh4->setVisible(true);
		ui->labCh5->setVisible(true);
		ui->labPY->setText("");

		return;
	}

	if (m_strCurrentEditType == CTRL_LINEEDIT) {
		m_pCurrentLineEdit->insert(m_pCurrentLineEdit->property("upper").toBool() ? value.toUpper() : value);
	} else if (m_strCurrentEditType == CTRL_TEXTEDIT) {
		m_pCurrentTextEdit->insertPlainText(m_pCurrentTextEdit->property("upper").toBool() ? value.toUpper() : value);
	}else if (m_strCurrentEditType == CTRL_WIDGET) {
		QStringList list = value.split("");
		foreach (QString str, list) {
			if (!str.isEmpty()) {
				QKeyEvent keyPress(QEvent::KeyPress, 0, Qt::NoModifier, QString(str));
				QApplication::sendEvent(m_pCurrentWidget, &keyPress);
			}
		}
	}

	//Make all Chinese character labels visible
	ui->labCh3->setVisible(true);
	ui->labCh4->setVisible(true);
	ui->labCh5->setVisible(true);
	ui->labPY->setText("");
}


void Keyboard::DeleteValue()
{
	if(m_bIsWebCtrl)
	{
		DeleteValue_Web();
		return;
	}

	if (m_strCurrentEditType == CTRL_LINEEDIT) 
	{
		m_pCurrentLineEdit->backspace();
	} 
	else if (m_strCurrentEditType == CTRL_TEXTEDIT) 
	{
		QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier, QString());
		QApplication::sendEvent(m_pCurrentTextEdit, &keyPress);
	}
}

void Keyboard::SetChinese(int index)
{
	int count = m_listCurrentPY.count();
	if (count > index) 
	{
		InsertValue(m_listCurrentPY.at(index));
		//After adding a Chinese character, clear the current Chinese character information and wait for re input
		ClearChinese();
		ui->labPY->setText("");
	}
}

void Keyboard::ClearChinese()
{
	//Clear Chinese characters, reset index
	for (int i = 0; i < m_listCNChar.count(); i++) 
	{
		m_listCNChar.at(i)->setText("");
	}

	m_listAllPY.clear();
	m_listCurrentPY.clear();
	m_nCurrentPYIndex = 0;
	m_nCurrentPYCount = 0;
}

bool Keyboard::CheckPress()
{
	//Only valid buttons belonging to the input method keyboard can continue processing
	bool bOK = m_btnLongPress->property(PROPERTY_NUM).toBool();
	bOK = bOK && m_btnLongPress->property(PROPERTY_OTHER).toBool();
	bOK = bOK && m_btnLongPress->property(PROPERTY_PINYIN).toBool();
	bOK = bOK && m_btnLongPress->property(PROPERTY_FUN).toBool();
	return bOK;
}

void Keyboard::UpdateFocus()
{
	m_pFatherObject->setFocus();
}

bool Keyboard::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonPress) 
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

		if ((mouseEvent->button() == Qt::LeftButton) /*&&(!m_bIsWebCtrl)*/)
		{
			if (obj == ui->labCh1) 
			{
				SetChinese(0);
			} 
			else if (obj == ui->labCh2) 
			{
				SetChinese(1);
			}
			else if (obj == ui->labCh3) 
			{
				SetChinese(2);
			}
			else if (obj == ui->labCh4) 
			{
				SetChinese(3);
			} 
			else if (obj == ui->labCh5) 
			{
				SetChinese(4);
			} 
			else if (m_strCurrentEditType != "" && obj != ui->btnCancel) 
			{
				QString objName = obj->objectName();
				if (obj->parent() != 0x0 && !obj->property("noinput").toBool() && objName != "frmMainWindow"
					&& objName != "frmInputWindow" && objName != "qt_edit_menu" && objName != "labPY") 
				{
						if (obj->inherits("QGroupBox") || obj->inherits("QFrame") || obj->inherits("QMenu"))
						{
							HidePanel();
						} 
				}
			}
			m_btnLongPress = (QPushButton *)obj;
			if (CheckPress()) 
			{
				m_bIsLongPress = true;
				m_tmDeletePress->start(50);//500->100 ->50
			}
			return false;
		}
	} else if (event->type() == QEvent::MouseButtonRelease) 
	{
		m_btnLongPress = (QPushButton *)obj;

		if (CheckPress())
		{
			m_bIsLongPress = false;
			m_tmDeletePress->stop();
		}

// 		if (m_bIsWebCtrl && m_pCurWebFrame)
// 		{
// 			m_pCurWebFrame->evaluateJavaScript("document.activeElement.setSelectionRange(0, document.activeElement.value.length)");
// 		}

		return false;
	} 
	else if (event->type() == QEvent::KeyPress) 
	{
		//If the input method form is not visible, no processing is required
		if (!isVisible()) 
		{
			return QWidget::eventFilter(obj, event);
		}

		QString labText = ui->labPY->text();
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

		//Shift to switch the input method mode, esc key to close the input method panel, the space takes the first Chinese character, and backspace key to delete

//In Chinese mode, enter to get Pinyin. In Chinese mode, you can enter a space when there is no Pinyin
		if (keyEvent->key() == Qt::Key_Space)
		{
			if (labText != "") 
			{
				ui->labPY->setText("");
				SetChinese(0);
				return true;
			} 
			else 
			{
				return false;
			}
		} else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) 
		{
			if (labText != "") 
			{
				InsertValue(labText);
				ui->labPY->setText("");
				SelectChinese();
			} 
			else 
			{
				//hidePanel(); 
			}
			//return true;
		} 
		else if (keyEvent->key() == Qt::Key_Control) 
		{
			//
		} 
		else if (keyEvent->key() == Qt::Key_Shift)
		{
			ui->btnFun4->click();
			return true;
		} 
		else if (keyEvent->key() == Qt::Key_Escape)
		{
			ui->btnCancel->click();
			return true;
		}
		else if (keyEvent->key() == Qt::Key_Backspace)
		{
			if (m_nInputMode == CHINESE) 
			{
				QString txt = labText;
				int len = txt.length();
				if (len > 0) 
				{
					ui->labPY->setText(txt.left(len - 1));
					SelectChinese();
					return true;
				}
			}
		} 
		else if (keyEvent->key() == Qt::Key_CapsLock)
		{
			ui->btnFun3->click();
			return true;
		} 
		else if (keyEvent->text() == "+" || keyEvent->text() == "=")
		{
			if (labText != "") 
			{
				ui->btnNext->click();
				return true;
			} 
			else 
			{
				return false;
			}
		}
		else if (keyEvent->text() == "-" || keyEvent->text() == "_") 
		{
			if (labText != "")
			{
				ui->btnPre->click();
				return true;
			} 
			else 
			{
				return false;
			}
		} 
		else 
		{
			if (m_strCurrentEditType == "QWidget")
			{
				return false;
			}

			QString key;
			if (m_nInputMode == CHINESE) 
			{
				key = keyEvent->text();
			} else if (m_nInputMode == ENGLISH) 
			{
				if (m_bIsUpper) 
				{
					key = keyEvent->text().toUpper();
				} 
				else
				{
					key = keyEvent->text().toLower();
				}
			}

			if (!key.isEmpty()) 
			{
				QList<QPushButton *> btn = ui->widgetMain->findChildren<QPushButton *>();
				foreach (QPushButton *b, btn)
				{
					QString text = b->text();
					if (!text.isEmpty() && text == key)
					{
						b->click();
						return true;
					}
				}
			}
		}

		return false;
	}

	return QWidget::eventFilter(obj, event);
}
