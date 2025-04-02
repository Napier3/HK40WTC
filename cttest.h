#pragma once
#include <QMainWindow>
#include "qcustomplot.h"

class QCPTextElement; // forward declare

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

    void plotExcitationData();
    void plot5PercentData();
    void plot10PercentData();

private:
    void initTableWidget(QTableWidget *table);

private:
    Ui::CTTest *ui;
    QCPTextElement *m_plotTitle = nullptr;
};
