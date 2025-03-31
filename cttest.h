#pragma once
#include <QMainWindow>

class QCustomPlot; // forward declaration

namespace Ui {
class CTTest;
}

class CTTest : public QMainWindow
{
    Q_OBJECT

public:
    explicit CTTest(QWidget *parent = nullptr);
    ~CTTest();

    void ConnectSignals();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void On_CloseButton_Clicked();

    // Example: two slots to plot different graphs on m_plot:
    void plotExcitationData();
    void plot5PercentData();
    void plot10PercentData();

private:
    Ui::CTTest *ui;
};
