#include "hkspinbox.h"
#include <QLineEdit>
#include <QMouseEvent>
#include <QDebug>

HKSpinBox::HKSpinBox(QWidget *parent)
    : QSpinBox(parent)
{
    // ensure it can accept focus:
    setFocusPolicy(Qt::StrongFocus);
    setReadOnly(false);
    // optional: remove arrow buttons if you want only text
    // setButtonSymbols(QAbstractSpinBox::NoButtons);
}

void HKSpinBox::mousePressEvent(QMouseEvent *event)
{
    // Let the base class handle the default spinbox clicking
    QSpinBox::mousePressEvent(event);

    // Force the line edit to receive focus:
    if (lineEdit()) {
        lineEdit()->setFocus(Qt::OtherFocusReason);
        // (optional) qDebug() << "HKSpinBox line edit forcibly focused";
    }
}
