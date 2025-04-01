#pragma once

#include <QSpinBox>

class HKSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    explicit HKSpinBox(QWidget *parent = nullptr);

protected:
    // Override mousePressEvent to force focus onto the line edit
    void mousePressEvent(QMouseEvent *event) override;
};
