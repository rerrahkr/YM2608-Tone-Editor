#include "operator_sliders.h"
#include "ui_operator_sliders.h"
#include "widgets/labeled_slider.h"

OperatorSliders::OperatorSliders(QWidget *parent, int number) :
    QFrame(parent),
    ui(new Ui::OperatorSliders),
    number_(number),
    arState_(ParameterState::AR),
    drState_(ParameterState::DR),
    srState_(ParameterState::SR),
    rrState_(ParameterState::RR),
    slState_(ParameterState::SL),
    tlState_(ParameterState::TL),
    ksState_(ParameterState::KS),
    mlState_(ParameterState::ML),
    dtState_(ParameterState::DT),
    amState_(ParameterState::AM)
{
    ui->setupUi(this);

    ui->operatorNumberLabel->setText("Op" + QString::number(number_));

    ui->arSlider->setText("AR: ");
    ui->drSlider->setText("DR: ");
    ui->srSlider->setText("SR: ");
    ui->rrSlider->setText("RR: ");
    ui->slSlider->setText("SL: ");
    ui->tlSlider->setText("TL: ");
    ui->ksSlider->setText("KS: ");
    ui->mlSlider->setText("ML: ");
    ui->dtSlider->setText("DT: ");
    ui->amSlider->setText("AM: ");

    ui->arSlider->setMaximum(31);
    ui->drSlider->setMaximum(31);
    ui->srSlider->setMaximum(31);
    ui->rrSlider->setMaximum(15);
    ui->slSlider->setMaximum(15);
    ui->tlSlider->setMaximum(127);
    ui->ksSlider->setMaximum(3);
    ui->mlSlider->setMaximum(15);
    ui->dtSlider->setMaximum(7);
    ui->amSlider->setMaximum(1);

    connect(ui->arSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onARChanged);
    connect(ui->drSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onDRChanged);
    connect(ui->srSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onSRChanged);
    connect(ui->rrSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onRRChanged);
    connect(ui->slSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onSLChanged);
    connect(ui->tlSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onTLChanged);
    connect(ui->ksSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onKSChanged);
    connect(ui->mlSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onMLChanged);
    connect(ui->dtSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onDTChanged);
    connect(ui->amSlider, &LabeledSlider::valueChanged, this, &OperatorSliders::onAMChanged);
}

OperatorSliders::~OperatorSliders()
{
    delete ui;
}

int OperatorSliders::operatorNumber() const
{
    return number_;
}

void OperatorSliders::setOperatorNumber(int number)
{
    number_ = number;
    ui->operatorNumberLabel->setText("Op" + QString::number(number_));
}

int OperatorSliders::parameterValue(ParameterState::Parameter param) const
{
    switch (param) {
    case ParameterState::AR: return ui->arSlider->value();
    case ParameterState::DR: return ui->drSlider->value();
    case ParameterState::SR: return ui->srSlider->value();
    case ParameterState::RR: return ui->rrSlider->value();
    case ParameterState::SL: return ui->slSlider->value();
    case ParameterState::TL: return ui->tlSlider->value();
    case ParameterState::KS: return ui->ksSlider->value();
    case ParameterState::ML: return ui->mlSlider->value();
    case ParameterState::DT: return ui->dtSlider->value();
    case ParameterState::AM: return ui->amSlider->value();
    default:                 return -1; // Error
    }
}

void OperatorSliders::setParameterValue(ParameterState::Parameter param, int value)
{
    switch (param) {
    case ParameterState::AR: ui->arSlider->setValue(value); break;
    case ParameterState::DR: ui->drSlider->setValue(value); break;
    case ParameterState::SR: ui->srSlider->setValue(value); break;
    case ParameterState::RR: ui->rrSlider->setValue(value); break;
    case ParameterState::SL: ui->slSlider->setValue(value); break;
    case ParameterState::TL: ui->tlSlider->setValue(value); break;
    case ParameterState::KS: ui->ksSlider->setValue(value); break;
    case ParameterState::ML: ui->mlSlider->setValue(value); break;
    case ParameterState::DT: ui->dtSlider->setValue(value); break;
    case ParameterState::AM: ui->amSlider->setValue(value); break;
    default:                 break; // Error
    }
}

void OperatorSliders::setStateValue(ParameterState &state, int value)
{
    state.setValue(value);
    emit parameterChanged(number_, state);
}

void OperatorSliders::onARChanged(int value)
{
    setStateValue(arState_, value);
}

void OperatorSliders::onDRChanged(int value)
{
    setStateValue(drState_, value);
}

void OperatorSliders::onSRChanged(int value)
{
    setStateValue(srState_, value);
}

void OperatorSliders::onRRChanged(int value)
{
    setStateValue(rrState_, value);
}

void OperatorSliders::onSLChanged(int value)
{
    setStateValue(slState_, value);
}

void OperatorSliders::onTLChanged(int value)
{
    setStateValue(tlState_, value);
}

void OperatorSliders::onKSChanged(int value)
{
    setStateValue(ksState_, value);
}

void OperatorSliders::onMLChanged(int value)
{
    setStateValue(mlState_, value);
}

void OperatorSliders::onDTChanged(int value)
{
    setStateValue(dtState_, value);
}

void OperatorSliders::onAMChanged(int value)
{
    setStateValue(amState_, value);
}
