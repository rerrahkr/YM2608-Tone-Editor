#ifndef SSGEG_SLIDER_HPP
#define SSGEG_SLIDER_HPP

#include <QWidget>
#include <QString>

namespace Ui {
	class SSGEGSlider;
}

class SSGEGSlider : public QWidget
{
	Q_OBJECT

public:
	explicit SSGEGSlider(QWidget *parent = nullptr);
	~SSGEGSlider();

	int value() const;
	void setValue(int value);

signals:
	void valueChanged(int value);

private slots:
	void on_checkBox_stateChanged(int arg1);
	void on_verticalSlider_valueChanged(int value);

private:
	Ui::SSGEGSlider *ui;
	QString str_;
};

#endif // SSGEG_SLIDER_HPP
