#pragma once

#include "Keyboard/Keyboard.h"

#define QModePair QPair<QWidget*, Keyboard::tagMode>

#define  STT_SOFTKEYBOARD_VOL_NORMAL             _T("SoftKeyBoard_Vol_Normal")
#define  STT_SOFTKEYBOARD_VOL_WEAK				 _T("SoftKeyBoard_Vol_Weak")
#define  STT_SOFTKEYBOARD_CUR_NORMAL             _T("SoftKeyBoard_Cur_Normal")
#define  STT_SOFTKEYBOARD_CUR_WEAK			     _T("SoftKeyBoard_Cur_Weak")
#define STT_KEYBOARD_PERCENTAGE				   	_T("KeyBoard-Percentage") //°Ù·Ö±È¼üÅÌ

#define  STT_SOFTKEYBOARD_FUNC1_VALUE							 _T("Func1")
#define  STT_SOFTKEYBOARD_FUNC2_VALUE							 _T("Func2")
#define  STT_SOFTKEYBOARD_FUNC3_VALUE							 _T("Func3")
#define  STT_SOFTKEYBOARD_FuncMinus_VALUE					     _T("FuncMinus")

#define  STT_KEYBOARD_PERCENTAGE_BTN0								_T("Percentage_btn0")
#define  STT_KEYBOARD_PERCENTAGE_BTN1								_T("Percentage_btn1")
#define  STT_KEYBOARD_PERCENTAGE_BTN2								_T("Percentage_btn2")
#define  STT_KEYBOARD_PERCENTAGE_BTN3								_T("Percentage_btn3")
#define  STT_KEYBOARD_PERCENTAGE_BTN4								_T("Percentage_btn4")
#define  STT_KEYBOARD_PERCENTAGE_BTN5								_T("Percentage_btn5")
#define  STT_KEYBOARD_PERCENTAGE_BTN6								_T("Percentage_btn6")
#define  STT_KEYBOARD_PERCENTAGE_BTN7								_T("Percentage_btn7")
#define  STT_KEYBOARD_PERCENTAGE_BTN8								_T("Percentage_btn8")
#define  STT_KEYBOARD_PERCENTAGE_BTN9								_T("Percentage_btn9")
#define  STT_KEYBOARD_PERCENTAGE_BTNDOT							    _T("Percentage_btndot")

class QSoftKeyBoard : public QObject
{
	Q_OBJECT
public:
	QSoftKeyBoard();
	virtual ~QSoftKeyBoard();

private:
	QWidget* m_pMainWidget;
	Keyboard* m_pKeyBoard;
	QTimer m_oTimer;
	QVector<QModePair> m_vecModeStack;

	void Release();
	void GetFuncNum(QString& strText);
	void SetSmoothState(bool bSmooth); 
	void ShowPanel();
	void HidePanel();
	void SetDefaultFunc();
	void SetLanguageFunc();
	void Attach(QWidget* pWidget, Keyboard::tagMode oDefault = Keyboard::NUMBER, bool bStack = true);
	void ReAttach();
	void SetUserFunc(bool bUserFun, const QString& strFun1 = "", const QString& strFun2 = "", const QString& strFun3 = "", const QString& strFun4 = "");
	void SetMinusBtnText(QString strText);
	void SetSoftKeyBoardType(long nType);

protected:
	bool eventFilter(QObject *watched, QEvent *event);

signals:
	void sig_clickFunc1(QString);
	void sig_clickFunc2(QString);
	void sig_clickFunc3(QString);
	void sig_clickFunc4(QString);
	void sig_clickEnter();

private slots:
	void slot_clickFunc1(QPushButton*);
	void slot_clickFunc2(QPushButton*);
	void slot_clickFunc3(QPushButton*);
	void slot_clickFunc4(QPushButton*);
	void slot_clickEnter();

public slots:
	void OnTimer();
	void updateChanged(const QString& strValue);
	void deleteChanged();
	void microFocusChanged();

public:
	static void AttachObj(QWidget* pWidget,Keyboard::tagMode oDefault = Keyboard::ONLYNUMBER);
	static void ReAttachObj();
	static void SetLanguageKeyboard();
	static void SetDefaultKeyboard();
	static void ShowSmoothKeyboard();
	static void HideSmoothKeyboard();
	static void DisableUserFunc();
};

extern QSoftKeyBoard* g_pSoftKeyBoard;

