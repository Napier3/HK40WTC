#ifndef CTTEST_H
#define CTTEST_H

#include <QMainWindow>

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
    // 1) Declare an override for paintEvent
    void paintEvent(QPaintEvent *event) override;

public slots:
    void On_CloseButton_Clicked();

private:
    Ui::CTTest *ui;
};

#endif // CTTEST_H
