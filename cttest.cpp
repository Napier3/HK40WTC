#include "cttest.h"
#include "ui_cttest.h"
#include "SoftKeyBoard.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

CTTest::CTTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CTTest)
{
    ui->setupUi(this);
    g_pSoftKeyBoard = new QSoftKeyBoard();
    QSoftKeyBoard::AttachObj(this);
}

CTTest::~CTTest()
{
    delete ui;
    delete g_pSoftKeyBoard;
}

void CTTest::paintEvent(QPaintEvent *event)
{
    // Create a QPainter to draw on this widget
    QPainter painter(this);

    // Load your background image (adjust path as needed)
    QPixmap bg("/eric/icons/Background.png");
    // Optionally scale it to fill or expand to the window size
    bg = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // Draw the pixmap at 0,0
    painter.drawPixmap(0, 0, bg);

    // Call the base class implementation last
    QMainWindow::paintEvent(event);
}

void CTTest::ConnectSignals()
{
    connect(ui->m_buttonClose, &QPushButton::clicked, this, &CTTest::On_CloseButton_Clicked);
    //QLineEdit *spinLineEdit = ui->spinBox->findChild<QLineEdit *>();
    //if (spinLineEdit)
    //{
    //    QSoftKeyBoard::AttachObj(spinLineEdit, Keyboard::NUMBER);
    //}
}

void CTTest::On_CloseButton_Clicked()
{
    close();
}
