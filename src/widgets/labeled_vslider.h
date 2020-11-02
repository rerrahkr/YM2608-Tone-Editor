#ifndef LABELED_VSLIDER_H
#define LABELED_VSLIDER_H

#include <QWidget>
#include <QString>

namespace Ui {
	class LabeledVSlider;
}

class LabeledVSlider : public QWidget
{
	Q_OBJECT

public:
	explicit LabeledVSlider(QWidget *parent = 0, const QString str = "");
	~LabeledVSlider();

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
	Ui::LabeledVSlider *ui;
	QString str_;
};

#endif // LABELED_VSLIDER_H
