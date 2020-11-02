#ifndef LABELED_HSLIDER_HPP
#define LABELED_HSLIDER_HPP

#include <QWidget>
#include <QString>

namespace Ui {
	class LabeledHSlider;
}

class LabeledHSlider : public QWidget
{
	Q_OBJECT

public:
	explicit LabeledHSlider(QWidget *parent = nullptr, const QString str = "");
	~LabeledHSlider();

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
	void on_horizontalSlider_valueChanged(int value);

private:
	Ui::LabeledHSlider *ui;
	QString str_;
};

#endif // LABELED_HSLIDER_HPP
