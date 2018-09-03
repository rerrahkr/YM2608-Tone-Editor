#include "labeled_slider.h"
#include "ui_labeled_slider.h"
#include "sliderstyle.hpp"

LabeledSlider::LabeledSlider(QWidget *parent, const QString str) :
    QFrame(parent),
    ui(new Ui::LabeledSlider),
    str_(str)
{
    ui->setupUi(this);
    ui->label->setText(str_ + QString::number(ui->verticalSlider->value()));
	ui->verticalSlider->setStyle(new SliderStyle());
}

LabeledSlider::~LabeledSlider()
{
    delete ui;
}

int LabeledSlider::value() const
{
    return ui->verticalSlider->value();
}

void LabeledSlider::setValue(int value)
{
    ui->verticalSlider->setValue(value);
}

int LabeledSlider::maximum() const
{
    return ui->verticalSlider->maximum();
}

void LabeledSlider::setMaximum(int value)
{
    ui->verticalSlider->setMaximum(value);
}

int LabeledSlider::minimum() const
{
    return ui->verticalSlider->minimum();
}

void LabeledSlider::setMinimum(int value)
{
    ui->verticalSlider->setMinimum(value);
}

QString LabeledSlider::text() const
{
    return str_;
}

void LabeledSlider::setText(const QString str)
{
    str_ = str;
    ui->label->setText(str_ + QString::number(ui->verticalSlider->value()));
}

void LabeledSlider::on_verticalSlider_valueChanged(int value)
{
    ui->label->setText(str_ + QString::number(value));

    emit valueChanged(value);
}
