#include "custom_slider.h"

CustomSlider::CustomSlider(QWidget *parent) : QSlider(parent) {}

CustomSlider::CustomSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent) {}

void CustomSlider::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        setValue(minimum() + (maximum() - minimum()) * (height() - ev->y()) / height());
    }
    QSlider::mousePressEvent(ev);
}
