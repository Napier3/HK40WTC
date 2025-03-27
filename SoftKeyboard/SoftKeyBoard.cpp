#include "SoftKeyBoard.h"
#include "MathApi.h"

QSoftKeyBoard* g_pSoftKeyBoard = nullptr;


QSoftKeyBoard::QSoftKeyBoard()
{
    m_pKeyBoard = nullptr;
    m_pMainWidget = nullptr;
}

QSoftKeyBoard::~QSoftKeyBoard()
{
	Release();
}

void QSoftKeyBoard::SetUserFunc(bool bUserFun, const QString& strFun1, const QString& strFun2,
				 const QString& strFun3, const QString& strFun4)
{
	m_pKeyBoard->SetUserFunSetup(bUserFun);
	if(bUserFun)
	{
		m_pKeyBoard->SetUserFunString(strFun1, strFun2, strFun3, strFun4);
	}
}

void QSoftKeyBoard::DisableUserFunc()
{
	if(g_pSoftKeyBoard)
	{
		g_pSoftKeyBoard->SetUserFunc(false);
	}
}

void QSoftKeyBoard::SetDefaultFunc()
{
	m_pKeyBoard->SetUserFunSetup(false);
	m_pKeyBoard->SetInputType(Keyboard::ONLYNUMBER);
}

void QSoftKeyBoard::SetLanguageFunc()
{
	m_pKeyBoard->SetDefaultFunc();
	m_pKeyBoard->SetInputType(Keyboard::NUMBER);
}

void QSoftKeyBoard::SetLanguageKeyboard()
{
	if(g_pSoftKeyBoard)
	{
		g_pSoftKeyBoard->SetLanguageFunc();
	}
}

void QSoftKeyBoard::SetDefaultKeyboard()
{
	if(g_pSoftKeyBoard)
	{
		g_pSoftKeyBoard->SetDefaultFunc();
	}
}

void QSoftKeyBoard::SetSmoothState(bool bSmooth)
{
	m_pKeyBoard->SetSmooth(bSmooth);
}

void QSoftKeyBoard::ShowPanel()
{
	m_pKeyBoard->ShowSmoothEdit();
}

void QSoftKeyBoard::HidePanel()
{
	m_pKeyBoard->HidePanel();
}

void QSoftKeyBoard::AttachObj(QWidget* pWidget,Keyboard::tagMode oDefault)
{
	if(g_pSoftKeyBoard)
	{
		g_pSoftKeyBoard->Attach(pWidget,oDefault);
	}
}

void QSoftKeyBoard::ReAttach()
{
	if (m_vecModeStack.size()>0)
	{
		m_vecModeStack.pop_back();

		if(m_vecModeStack.size()>0)
		{
			QModePair pair = m_vecModeStack.at(m_vecModeStack.size() - 1);
			Attach(pair.first, pair.second, false);
		}
	}
}

void QSoftKeyBoard::ReAttachObj()
{
	if(g_pSoftKeyBoard)
	{
		g_pSoftKeyBoard->ReAttach();
	}
}

void QSoftKeyBoard::Attach(QWidget* pWidget, Keyboard::tagMode oDefault, bool bStack)
{
	Release();

	m_pKeyBoard = new Keyboard(pWidget,oDefault);
	if(bStack)
	{
		bool bSame = false;
		if(m_vecModeStack.size())
		{
			//确定是否在同一个界面记录了两次一模一样的键盘配置
			QModePair lastPair = m_vecModeStack.at(m_vecModeStack.size() - 1);
			if(lastPair.first == pWidget && lastPair.second == oDefault)
			{
				bSame = true;
			}
		}

		if(!bSame)
		{
			QModePair pair(pWidget, oDefault);
			m_vecModeStack.push_back(pair);
		}
	}
	
	m_pMainWidget = pWidget;
	pWidget->installEventFilter(this);
	m_pKeyBoard->setAttribute(Qt::WA_ShowWithoutActivating,true);
	m_pKeyBoard->setVisible(false);
	m_pKeyBoard->installEventFilter(pWidget);

	connect(m_pKeyBoard, SIGNAL(sig_clickFunc1(QPushButton*)), this, SLOT(slot_clickFunc1(QPushButton*)));
	connect(m_pKeyBoard, SIGNAL(sig_clickFunc2(QPushButton*)), this, SLOT(slot_clickFunc2(QPushButton*)));
	connect(m_pKeyBoard, SIGNAL(sig_clickFunc3(QPushButton*)), this, SLOT(slot_clickFunc3(QPushButton*)));
	connect(m_pKeyBoard, SIGNAL(sig_clickFunc4(QPushButton*)), this, SLOT(slot_clickFunc4(QPushButton*)));
	connect(m_pKeyBoard, SIGNAL(sig_clickEnter()), this, SLOT(slot_clickEnter()));

	connect(m_pKeyBoard, SIGNAL(sig_updateChanged(const QString&)), this, SLOT(updateChanged(const QString&)));
	connect(m_pKeyBoard, SIGNAL(sig_deleteChanged()), this, SLOT(deleteChanged()));

	if(pWidget->inherits("QDialog"))
	{
		m_pKeyBoard->SetIsDialog(true);
		m_pKeyBoard->setWindowFlags(Qt::ToolTip);
//  	connect(&m_oTimer, SIGNAL(timeout()), this, SLOT(OnTimer()));
//  	m_oTimer.start(10);
	}
}

void QSoftKeyBoard::Release()
{
	if(m_pKeyBoard)
	{
		disconnect(&m_oTimer, SIGNAL(timeout()), this, SLOT(OnTimer()));
		delete m_pKeyBoard;
        m_pKeyBoard = nullptr;
	}
}

bool QSoftKeyBoard::eventFilter(QObject *watched, QEvent *event)
{
	if(watched == m_pMainWidget)
	{
		switch (event->type()) 
		{
		case QEvent::MouseButtonPress:
			m_pKeyBoard->HidePanel();
			m_pMainWidget->setFocus();
			break;
		default:
			break;
		}
	}

	if(watched == m_pKeyBoard)
	{
		switch (event->type()) 
		{
		case QEvent::Hide:
			break;
		case QEvent::Show:
			m_pKeyBoard->HidePanel();
			return false;  
		default:
			break;
		}
	}
	return QObject::eventFilter(watched,event);
}

void QSoftKeyBoard::OnTimer()
{
	if(!m_pMainWidget->isActiveWindow())
	{
		m_pMainWidget->activateWindow();
	}
}

void QSoftKeyBoard::updateChanged(const QString& strValue)
{
	QKeyEvent keyPress(QEvent::KeyPress, 0, Qt::NoModifier, QString(strValue));
}

void QSoftKeyBoard::deleteChanged()
{
	QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier, QString());
}

void QSoftKeyBoard::microFocusChanged()
{
    m_pKeyBoard->CheckWebAttribute();
    m_pKeyBoard->setVisible(false);
}

void QSoftKeyBoard::GetFuncNum(QString& strText)
{
	if(strText.indexOf("V") == strText.length() - 1)
	{
		strText = strText.left(strText.length() - 1);
	}
	else if(strText.indexOf("A") == strText.length() - 1)
	{
		strText = strText.left(strText.length() - 1);
	}
	else if(strText.indexOf("Hz") == strText.length() - 2)
	{
		strText = strText.left(strText.length() - 2);
	}
}

void QSoftKeyBoard::slot_clickFunc1(QPushButton* pBtn)
{
	//向外转发
	long nSig = receivers(SIGNAL(sig_clickFunc1(QString)));
	if(nSig)
	{
		emit sig_clickFunc1(m_pKeyBoard->GetCurWidgetText());
	}
	else
	{
		QString strText = pBtn->text();
		GetFuncNum(strText);
		m_pKeyBoard->SetCurWidgetText(strText);
	}
}

void QSoftKeyBoard::slot_clickFunc2(QPushButton* pBtn)
{
	//向外转发
	long nSig = receivers(SIGNAL(sig_clickFunc2(QString)));
	if(nSig)
	{
		emit sig_clickFunc2(m_pKeyBoard->GetCurWidgetText());
	}
	else
	{
		QString strText = pBtn->text();
		GetFuncNum(strText);
		m_pKeyBoard->SetCurWidgetText(strText);
	}
}

void QSoftKeyBoard::slot_clickFunc3(QPushButton* pBtn)
{
	//向外转发
	long nSig = receivers(SIGNAL(sig_clickFunc3(QString)));
	if(nSig)
	{
		emit sig_clickFunc3(m_pKeyBoard->GetCurWidgetText());
	}
	else
	{
		QString strText = pBtn->text();
		GetFuncNum(strText);
		m_pKeyBoard->SetCurWidgetText(strText);
	}
}

void QSoftKeyBoard::slot_clickEnter()
{
	emit sig_clickEnter();
}

void QSoftKeyBoard::slot_clickFunc4(QPushButton* pBtn)
{
	//向外转发
	long nSig = receivers(SIGNAL(sig_clickFunc4(QString)));
	if(nSig)
	{
		emit sig_clickFunc4(m_pKeyBoard->GetCurWidgetText());
	}
	else
	{
		QString strText = pBtn->text();
		GetFuncNum(strText);
		m_pKeyBoard->SetCurWidgetText(strText);
	}
}

void QSoftKeyBoard::ShowSmoothKeyboard()
{
	if(g_pSoftKeyBoard)
	{
		g_pSoftKeyBoard->SetSmoothState(false);
		g_pSoftKeyBoard->ShowPanel();
	}
}

void QSoftKeyBoard::HideSmoothKeyboard()
{
	if(g_pSoftKeyBoard)
	{
		g_pSoftKeyBoard->SetSmoothState(true);
	}
}

void QSoftKeyBoard::SetSoftKeyBoardType(long nType)
{
	m_pKeyBoard->SetKeyBoardType(nType);
}

void QSoftKeyBoard::SetMinusBtnText(QString strText)
{
	m_pKeyBoard->SetMinusBtnText(strText);
}
