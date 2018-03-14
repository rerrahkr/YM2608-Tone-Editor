#ifndef LABELED_SLIDER_H
#define LABELED_SLIDER_H

#include <QFrame>
#include <QString>

namespace Ui {
class LabeledSlider;
}

class LabeledSlider : public QFrame
{
    Q_OBJECT

public:
    explicit LabeledSlider(QWidget *parent = 0, const QString str = "");
    ~LabeledSlider();

    int value() const;
    void setValue(int value);

    int maximum() const;
    void setMaximum(int value);
    int minimum() const;
    void setMinimum(int value);

    QString text() const;
    void setText(const QString str);

signals:
    void valueChanged(int value);

private slots:
    void on_verticalSlider_valueChanged(int value);

private:
    Ui::LabeledSlider *ui;
    QString str_;
};

#endif // LABELED_SLIDER_H
