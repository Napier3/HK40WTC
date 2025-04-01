#include "cttest.h"
#include "ui_cttest.h"
#include "SoftKeyBoard.h"

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
    ui->m_plot->plotLayout()->insertRow(0);
    m_plotTitle = new QCPTextElement(ui->m_plot, "", QFont("SimHei", 14, QFont::Bold));
    ui->m_plot->plotLayout()->addElement(0, 0, m_plotTitle);
    initTabWidget();
    initTableWidget(ui->tableWidget);
    initTableWidget(ui->tableWidget_2);
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

void CTTest::initTabWidget()
{
    // 1) Create a label for the corner
    QLabel *cornerLabel = new QLabel(QStringLiteral("试验数据"), this);
    QFont cornerFont("SimHei", 12, QFont::Bold);
    cornerLabel->setFont(cornerFont);

    // 2) Place it in the top-left corner of the tabWidget
    ui->tabWidget->setCornerWidget(cornerLabel, Qt::TopLeftCorner);

    // 3) Align the tabs themselves on the right side
    ui->tabWidget->setStyleSheet("QTabBar { alignment: right; }");

    // If needed, ensure tabs are on top
    ui->tabWidget->setTabPosition(QTabWidget::North);
}

void CTTest::initTableWidget(QTableWidget *table)
{
    // Example: 4 total rows (1 "header" row + 3 data rows),
        //          7 columns
    table->setRowCount(3);
    table->setColumnCount(7);

    // Hide the built-in headers
    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setVisible(false);

    // Let columns/rows stretch to fill the available space
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Fill row 0 with "header" text
    table->setItem(0, 0, new QTableWidgetItem(QStringLiteral("VA")));
    table->setItem(0, 1, new QTableWidgetItem(QStringLiteral("COSφ")));
    table->setItem(0, 2, new QTableWidgetItem(QStringLiteral("1%")));
    table->setItem(0, 3, new QTableWidgetItem(QStringLiteral("5%")));
    table->setItem(0, 4, new QTableWidgetItem(QStringLiteral("20%")));
    table->setItem(0, 5, new QTableWidgetItem(QStringLiteral("100%")));
    table->setItem(0, 6, new QTableWidgetItem(QStringLiteral("120%")));
    table->setSelectionBehavior(QAbstractItemView::SelectItems);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked | QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed);
}

/* 励磁特性曲线: use LOG scale on both X and Y, with sub-grid lines
   We'll set a fixed range, e.g. x: 1e-4..10, y: 1e-2..100
   Then plot some example data. */
void CTTest::plotExcitationData()
{
    // Clear old data
    ui->m_plot->clearGraphs();
    if (m_plotTitle)
    {
        m_plotTitle->setText(QString::fromLocal8Bit("励磁特性曲线"));
    }

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
    if (m_plotTitle)
    {
        m_plotTitle->setText(QString::fromLocal8Bit("5%误差曲线"));
    }

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
    if (m_plotTitle)
    {
        m_plotTitle->setText(QString::fromLocal8Bit("10%误差曲线"));
    }

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
    QFont axisFont(QStringLiteral("Microsoft YaHei"), 12);
    ui->m_plot->xAxis->setLabelFont(axisFont);
    ui->m_plot->yAxis->setLabelFont(axisFont);

    // If your .cpp is UTF-8, this is safe:
    ui->m_plot->xAxis->setLabel(QString::fromLocal8Bit("Z(Ω)"));
    ui->m_plot->yAxis->setLabel(QString::fromLocal8Bit("kalf(倍)"));

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
