#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cttest.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ConnectSignals();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
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

void MainWindow::ConnectSignals()
{
    connect(ui->m_buttonCTTest, &QPushButton::clicked, this, &MainWindow::On_CTTestButton_clicked);
}

void MainWindow::On_CTTestButton_clicked()
{
    CTTest *child = new CTTest(this);

    // Connect the child's signals (including the close-button slot)
    child->ConnectSignals();

    // Show it
    child->show();
}

