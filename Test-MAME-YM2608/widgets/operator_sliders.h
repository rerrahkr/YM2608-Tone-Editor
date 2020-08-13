#ifndef OPERATOR_SLIDERS_H
#define OPERATOR_SLIDERS_H

#include <QWidget>
#include "parameter_state.hpp"

namespace Ui {
	class OperatorSliders;
}

class OperatorSliders : public QWidget
{
	Q_OBJECT

public:
	explicit OperatorSliders(QWidget *parent = 0, int number = 0);
	~OperatorSliders();

	int operatorNumber() const;
	void setOperatorNumber(int number);
	int parameterValue(OperatorParameter param) const;
	void setParameterValue(OperatorParameter param, int value);

signals:
	void parameterChanged(int op, OperatorParameter state, int value);

private:
	Ui::OperatorSliders *ui;

	int number_;
};

#endif // OPERATOR_SLIDERS_H
