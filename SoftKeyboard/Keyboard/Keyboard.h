#pragma once

#include <QtGui>
#include <QtWidgets>
#ifndef _NOT_USE_GOOGLE_PINYIN_
#include "googlepinyin.h"
#endif
#include "OSInterface.h"

#define Stt_CurrVolKeyboardType_Vol                       0
#define Stt_CurrVolKeyboardType_Curr                      1


namespace Ui
{
    class Keyboard;
}

typedef struct stt_keyboardbtndescs
{
public:
	stt_keyboardbtndescs();
	~stt_keyboardbtndescs();

        QString m_strVolFunc1;
        QString m_strVolFunc2;
        QString m_strVolFunc3;
        QString m_strVolMinus;
        QString m_strCurFunc1;
        QString m_strCurFunc2;
        QString m_strCurFunc3;
        QString m_strCurMinus;
        QString m_strFreqFunc1;
        QString m_strFreqFunc2;

        QString m_strPercentageBtn0;
        QString m_strPercentageBtn1;
        QString m_strPercentageBtn2;
        QString m_strPercentageBtn3;
        QString m_strPercentageBtn4;
        QString m_strPercentageBtn5;
        QString m_strPercentageBtn6;
        QString m_strPercentageBtn7;
        QString m_strPercentageBtn8;
        QString m_strPercentageBtn9;
        QString m_strPercentageBtnDot;

	float*	m_pfVNom;			//额定线电压正常100V
	float*	m_pfINom;            //额定电流正常1A

public:
	void Init();

}Stt_KeyboardBtnDescs;

class Keyboard : public QWidget
{
    Q_OBJECT

public:
	enum tagMode
	{
		NUMBER,
		ENGLISH,
		SYMBOL,
		CHINESE,
		ONLYNUMBER,
        PERCENTAGE
	};

	QString m_strOriginalText;//刚弹出键盘时的原始字符内容
private:
    Ui::Keyboard *ui;
    static Keyboard *m_pSelf;

    QWidget* m_pFatherObject;
	QWidget* m_pAnchorWidget;

    int m_nFramWidth;				//窗体宽度
    int m_nFramHeight;				//窗体高度
    int m_nDeskWidth;				//桌面宽度
    int m_nDeskHeight;				//桌面高度
    int m_nIconWidth;				//图标宽度
    int m_nIconHeight;				//图标高度
    int m_nSpacing;					//间距
    int m_nTopHeight;				//顶部高度

    QString m_strDBPath;			//数据库路径

    QPoint m_ptMousePoint;			//鼠标拖动自定义标题栏时的坐标
    bool m_bMousePressed;			//鼠标是否按下
    bool m_bIsUpper;				//当前大小写状态
    bool m_bIsLongPress;			//是否长按退格键
    QPushButton *m_btnLongPress;	//长按按钮
    QTimer *m_tmDeletePress;		//退格键定时器

	bool m_bPreSmooth;				//界面准备滑动,取消焦点
	QObject* m_pPreSmoothObj;		//滑动前最后的焦点obj

	bool m_bIsDialog;				//对象窗口是否是dialog
	Qt::WindowFlags m_flagPre;
    QWidget *m_pCurrentWidget;		//当前焦点的对象
    QLineEdit *m_pCurrentLineEdit;	//当前焦点的单行文本框
    QTextEdit *m_pCurrentTextEdit;	//当前焦点的多行文本框
    QPlainTextEdit *m_pCurrentPlain;//当前焦点的富文本框
    QTextBrowser *m_pCurrentBrowser;//当前焦点的文本浏览框
    QString m_strCurrentEditType;	//当前焦点控件的类型
    QString m_strCurrentFlag;		//当前焦点控件属性

	bool m_bAuto;					//键盘自动放置
	tagMode m_nInputMode;			//当前输入法类型
	QList<QLabel *>m_listCNChar;	//汉字标签数组
	QStringList m_listAllPY;			//所有拼音链表  
	QStringList m_listCurrentPY;		//当前拼音链表  
	int m_nCurrentPYIndex;			//当前拼音索引  
	int m_nCurrentPYCount;			//当前拼音数量
#ifndef _NOT_USE_GOOGLE_PINYIN_
	GooglePinYin m_oPinYinGoogle;			//谷歌拼音内核
#endif

	//焦点控件样式
	QString m_strFoucsOutStyle;
	QString m_strFoucsInStyle;

	QIcon m_icoUpper;
	QIcon m_icoChinese;
	QIcon m_icoEnglish;
	QIcon m_icoSymbol;
	QIcon m_icoNumber;

public:
	//单例模式,保证一个程序只存在一个输入法实例对象
	static Keyboard *Instance(QWidget *parent);
	Keyboard(QWidget *parent = 0,tagMode oMode = NUMBER);
	virtual ~Keyboard();

private:
	void InitUI(tagMode oMode);					//初始化界面
	void InitCNList();				//初始化中文可选词组Label
	void InitProperty();			//初始化控件属性
	void InitAttr();				//初始化WEB属性

	void SetFont(const QString &strFontName, int nBtnFontSize, int nLabFontSize);	//设置字体名称及大小
	void SetIconSize(int nWidth, int nHeight);										//设置图标尺寸
	void SetFramSize(int nWidth, int nHeight);										//设置键盘尺寸
	void SetSpacing(int nSpacing);													//设置按钮之间的空隙间隔
	void SetTopHeight(int nTopHeight);												//设置顶部汉字区域高度
	void SetDefaultStyle();															//设置键盘默认风格
	void SetColor(const QString &strMainBackgroundColor, const QString &strMainTextColor, 
		const QString &strBtnBackgroundColor, const QString &strBtnHoveColor, 
		const QString &strBtnHoveTextColor, const QString &strLabHoveColor, 
		const QString &strLabHoveTextColor);										//改变输入法面板样式

	void UpdateFocus();
	void UpdateTopWidget();
	void UpdateVisible(tagMode nMode); //在不同风格下隐藏或显示按钮组
	void SetUpper(bool isUpper); //设置大小写

	void SelectChinese();//查询汉字
	void ShowChinese();//显示查询到的汉字
	void InsertValue(const QString &value);//插入值到当前焦点控件
	void DeleteValue();//删除当前焦点控件的一个字符
	void SetChinese(int index);//设置当前汉字
	void ClearChinese(); //清空当前汉字信息
	bool CheckPress();

	void SetFocusBorder();
	void KillFocusBorder();
//************针对按钮click响应的处理函数************
private:
	void DealBtnFun3();
	void DealBtnFun4();
	void DealBtnDelete(const QString& strText);
	void DealBtnPre();
	void DealBtnNext();
	void DealBtnCancel();
	void DealBtnSpace(const QString& strText);
	void DealBtnEnter(const QString& strText);
	void DealBtnDefault(QString strText);

	void DealBtnFunEx_Persent();//切换到扩展键盘-百分比
	QString SetPercentTextValue(QString strText);
	long m_nCurrVolKeyBoardType;//电压电流键盘类别
//	QString m_strKeyBoardType;
public:
	//获取当前是电压还是电流键盘
	void SetKeyBoardType(long nType);
//***************************************************

private slots:
	void focusChanged(QWidget *oldWidget, QWidget *nowWidget); //焦点改变事件槽函数处理
	void btnClicked();//输入法面板按键处理

Q_SIGNALS:
	void sig_clickFunc1(QPushButton*);
	void sig_clickFunc2(QPushButton*);
	void sig_clickFunc3(QPushButton*);
	void sig_clickFunc4(QPushButton*);
	void sig_clickEnter();

public:
	void SetInputType(tagMode nMode);
	void ShowPanel(QObject *objCurr);
	void HidePanel();
	void SetAutoKeyboard(bool bAuto);
	void SetDefaultFunc();
	void SetUserFunSetup(bool bSetup);
	void SetUserFunString(const QString& strFun1, const QString& strFun2,
		const QString& strFun3, const QString& strFun4);
	QString GetCurWidgetText();
	void SetCurWidgetText(const QString& strText);
    void UpdateShowRect(long nCurX = NULL, long nCurY = NULL, long nCurWidth = NULL, long nCurHeight = NULL);
	void UpdateBtnsRect(); //在不同风格下布局按钮

	void SetIsDialog(bool bIsDialog) { m_bIsDialog = bIsDialog; } 
    //void SetPreFlags(Qt::WindowFlags flags) { m_flagPre = false; }
	void UpdateCurrentLineEdit(QLineEdit* pLineEdit);

	void SetSmooth(bool bSmooth) { m_bPreSmooth = bSmooth; } //准备滑动,但不一定滑动
	void ShowSmoothEdit(); //展示确定滑动之前的键盘
	void SetMinusBtnText(QString strText);

//************针对网页控件的响应************
private:
	struct tagEnableAttribute
	{
		bool m_bMax;
		bool m_bMin;
		bool m_bSize;
	}m_unEnAttri;
	struct tagWebAttribute
	{
		float m_fMax;
		float m_fMin;
		long m_nSize;
	}m_unAttri;

	QString m_strCurWebCtrlID;
	bool m_bIsWebCtrl;
	bool m_bSelectAll;

signals:
	void sig_updateChanged(const QString& strValue);
	void sig_deleteChanged();
private:
	void DeleteValue_Web();
	void InsertValue_Web(const QString &v);

public:
	bool GetSelectAll();
	void btnClicked_Web();
	void ShowWebPanel(const QString& strValue);
    void CheckWebAttribute();
	bool IsSameWebCtrlID(const QString &strCtrlID);
//*******************************************

protected:
	void mouseMoveEvent(QMouseEvent *);//鼠标拖动事件
	void mousePressEvent(QMouseEvent *);//鼠标按下事件
	void mouseReleaseEvent(QMouseEvent *);//鼠标松开事件
	bool eventFilter(QObject *obj, QEvent *event);//事件过滤器,处理鼠标在汉字标签处单击操作
};

extern Stt_KeyboardBtnDescs g_oKeyBoardBtnDescs;
