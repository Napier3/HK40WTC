#include "cttest.h"
#include "ui_cttest.h"
#include "SoftKeyBoard.h"
#include "qcustomplot.h"

#include <QButtonGroup>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

// if you have an extern QSoftKeyBoard
extern QSoftKeyBoard* g_pSoftKeyBoard;

CTTest::CTTest(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CTTest)
{
    ui->setupUi(this);

    // Set up soft keyboard (optional)
    g_pSoftKeyBoard = new QSoftKeyBoard();
    QSoftKeyBoard::AttachObj(this);

    // Mark these 5 push buttons exclusive
    QButtonGroup *group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(ui->m_buttonParaSetting);
    group->addButton(ui->m_buttonAutoTest);
    group->addButton(ui->m_buttonTestData);
    group->addButton(ui->m_buttonDataChange);
    group->addButton(ui->m_buttonClose);

    // We'll connect signals to stackedWidget pages & our new plot slots
    ConnectSignals();
}

CTTest::~CTTest()
{
    delete ui;
    delete g_pSoftKeyBoard;
}

void CTTest::ConnectSignals()
{
    // Close button
    connect(ui->m_buttonClose, &QPushButton::clicked,
            this, &CTTest::On_CloseButton_Clicked);

    // Switch pages in stackedWidget
    connect(ui->m_buttonParaSetting, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->m_buttonAutoTest, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->m_buttonTestData, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(2);
    });
    connect(ui->m_buttonDataChange, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(3);
    });

    connect(ui->m_buttonEXCurve, &QPushButton::clicked,
            this, &CTTest::plotExcitationData);
    connect(ui->m_buttonFiveDev, &QPushButton::clicked,
            this, &CTTest::plot5PercentData);
    connect(ui->m_buttonTenDev, &QPushButton::clicked,
            this, &CTTest::plot10PercentData);
}

/* 励磁特性曲线: use LOG scale on both X and Y, with sub-grid lines
   We'll set a fixed range, e.g. x: 1e-4..10, y: 1e-2..100
   Then plot some example data. */
void CTTest::plotExcitationData()
{
    // Clear old data
    ui->m_plot->clearGraphs();

    // Set up log scale for xAxis
    ui->m_plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->m_plot->xAxis->setRange(1e-4, 10);  // from 0.0001 to 10
    // Use a log ticker so we get nice decade lines
    QSharedPointer<QCPAxisTickerLog> logTickerX(new QCPAxisTickerLog);
    ui->m_plot->xAxis->setTicker(logTickerX);
    ui->m_plot->xAxis->setLabel("Irms(A)");
    ui->m_plot->xAxis->grid()->setSubGridVisible(true);  // extra lines
    ui->m_plot->xAxis->setSubTicks(true);

    // log scale for yAxis
    ui->m_plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->m_plot->yAxis->setRange(1e-2, 100); // from 0.01 to 100
    QSharedPointer<QCPAxisTickerLog> logTickerY(new QCPAxisTickerLog);
    ui->m_plot->yAxis->setTicker(logTickerY);
    ui->m_plot->yAxis->setLabel("Urms(V)");
    ui->m_plot->yAxis->grid()->setSubGridVisible(true);
    ui->m_plot->yAxis->setSubTicks(true);

    // Add a graph
    ui->m_plot->addGraph();
    ui->m_plot->graph(0)->setName(QString::fromLocal8Bit("励磁特性"));
    ui->m_plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    // optionally set a scatter style
    // ui->m_plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);

    // Example data from x=1e-4..10 in 100 steps
    QVector<double> x(100), y(100);
    double xStart = 1e-4;
    double xEnd   = 10;
    for (int i=0; i<100; ++i)
    {
        double t = i / 99.0; // goes 0..1
        x[i] = xStart * pow(xEnd/xStart, t); // geometric interpolation
        // example function: y = 0.1 * x^2 + 1 (just a placeholder)
        y[i] = 0.1 * pow(x[i], 2) + 1;
    }
    ui->m_plot->graph(0)->setData(x, y);

    // Refresh
    ui->m_plot->replot();
}

/* 5%误差曲线: linear scale, fixed range x: 0..210, y: 0..44
   We'll put sub-grid lines so you get many horizontal/vertical lines. */
void CTTest::plot5PercentData()
{
    // Clear old data/graphs:
    ui->m_plot->clearGraphs();

    // 1) Set linear scales:
    ui->m_plot->xAxis->setScaleType(QCPAxis::stLinear);
    ui->m_plot->yAxis->setScaleType(QCPAxis::stLinear);

    // 2) Fix the axis ranges:
    ui->m_plot->xAxis->setRange(0, 210);
    ui->m_plot->yAxis->setRange(0, 44);

    // 3) Make sure we have axis ticks and sub-grid lines:
    ui->m_plot->xAxis->setTicks(true);
    ui->m_plot->yAxis->setTicks(true);
    ui->m_plot->xAxis->grid()->setSubGridVisible(true);
    ui->m_plot->yAxis->grid()->setSubGridVisible(true);

    // 4) Optionally use a "fixed" ticker to show ticks at each 10 ohms on X, 4 on Y
    {
        QSharedPointer<QCPAxisTickerFixed> tickerX(new QCPAxisTickerFixed);
        tickerX->setTickStep(10.0);
        tickerX->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        ui->m_plot->xAxis->setTicker(tickerX);

        QSharedPointer<QCPAxisTickerFixed> tickerY(new QCPAxisTickerFixed);
        tickerY->setTickStep(4.0);
        tickerY->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        ui->m_plot->yAxis->setTicker(tickerY);
    }

    // 5) Set axis labels:
    ui->m_plot->xAxis->setLabel(QString::fromLocal8Bit("Z(Ω)"));
    ui->m_plot->yAxis->setLabel(QString::fromLocal8Bit("kalf (倍)"));

    // 6) Make the grid lines dashed:
    QPen gridPen(Qt::gray, 1, Qt::DotLine);
    ui->m_plot->xAxis->grid()->setPen(gridPen);
    ui->m_plot->yAxis->grid()->setPen(gridPen);

    // 7) Optionally also set sub-grid lines dashed or dotted:
    QPen subGridPen(Qt::lightGray, 1, Qt::DotLine);
    ui->m_plot->xAxis->grid()->setSubGridPen(subGridPen);
    ui->m_plot->yAxis->grid()->setSubGridPen(subGridPen);

    // 8) Add a graph (line or scatter)
    ui->m_plot->addGraph();
    ui->m_plot->graph(0)->setName("额定负荷");
    ui->m_plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    // If you want scatter:
    // ui->m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

    // 9) Provide your data:
    //   x: 0..210, y: 0..44
    //   We'll just do a small example:
    QVector<double> x(211), y(211);
    for (int i=0; i<=210; i++)
    {
        x[i] = i;
        // For demonstration, let's do a small pseudo function
        // e.g. y = (i * 0.2) or something
        y[i] = i * 0.2;
    }
    ui->m_plot->graph(0)->setData(x, y);

    // 10) Redraw
    ui->m_plot->replot();
}

void CTTest::plot10PercentData()
{
    // Clear old data/graphs:
    ui->m_plot->clearGraphs();

    // 1) Set linear scales:
    ui->m_plot->xAxis->setScaleType(QCPAxis::stLinear);
    ui->m_plot->yAxis->setScaleType(QCPAxis::stLinear);

    // 2) Fix the axis ranges:
    ui->m_plot->xAxis->setRange(0, 210);
    ui->m_plot->yAxis->setRange(0, 44);

    // 3) Make sure we have axis ticks and sub-grid lines:
    ui->m_plot->xAxis->setTicks(true);
    ui->m_plot->yAxis->setTicks(true);
    ui->m_plot->xAxis->grid()->setSubGridVisible(true);
    ui->m_plot->yAxis->grid()->setSubGridVisible(true);

    // 4) Optionally use a "fixed" ticker to show ticks at each 10 ohms on X, 4 on Y
    {
        QSharedPointer<QCPAxisTickerFixed> tickerX(new QCPAxisTickerFixed);
        tickerX->setTickStep(10.0);
        tickerX->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        ui->m_plot->xAxis->setTicker(tickerX);

        QSharedPointer<QCPAxisTickerFixed> tickerY(new QCPAxisTickerFixed);
        tickerY->setTickStep(4.0);
        tickerY->setScaleStrategy(QCPAxisTickerFixed::ssNone);
        ui->m_plot->yAxis->setTicker(tickerY);
    }

    // 5) Set axis labels:
    ui->m_plot->xAxis->setLabel(QString::fromLocal8Bit("Z(Ω)"));
    ui->m_plot->yAxis->setLabel(QString::fromLocal8Bit("kalf (倍)"));

    // 6) Make the grid lines dashed:
    QPen gridPen(Qt::gray, 1, Qt::DotLine);
    ui->m_plot->xAxis->grid()->setPen(gridPen);
    ui->m_plot->yAxis->grid()->setPen(gridPen);

    // 7) Optionally also set sub-grid lines dashed or dotted:
    QPen subGridPen(Qt::lightGray, 1, Qt::DotLine);
    ui->m_plot->xAxis->grid()->setSubGridPen(subGridPen);
    ui->m_plot->yAxis->grid()->setSubGridPen(subGridPen);

    // 8) Add a graph (line or scatter)
    ui->m_plot->addGraph();
    ui->m_plot->graph(0)->setName("额定负荷");
    ui->m_plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    // If you want scatter:
    // ui->m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

    // 9) Provide your data:
    //   x: 0..210, y: 0..44
    //   We'll just do a small example:
    QVector<double> x(211), y(211);
    for (int i=0; i<=210; i++)
    {
        x[i] = i;
        // For demonstration, let's do a small pseudo function
        // e.g. y = (i * 0.2) or something
        y[i] = i * 0.2;
    }
    ui->m_plot->graph(0)->setData(x, y);

    // 10) Redraw
    ui->m_plot->replot();
}


void CTTest::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPixmap bg("/eric/icons/Background.png");
    bg = bg.scaled(size(),
                   Qt::KeepAspectRatioByExpanding,
                   Qt::SmoothTransformation);
    painter.drawPixmap(0, 0, bg);

    QMainWindow::paintEvent(event);
}

void CTTest::On_CloseButton_Clicked()
{
    close();
}
