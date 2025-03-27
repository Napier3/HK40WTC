#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ConnectSignals();
public slots:
    void On_CTTestButton_clicked();

protected:
    // 1) Declare an override for paintEvent
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
};
