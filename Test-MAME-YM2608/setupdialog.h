#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QString>
#include <vector>
#include <map>
#include "settings.hpp"
#include "tone_converter.hpp"

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
	SetupDialog(const Settings& settings, const ToneConverter& converter, QWidget *parent = nullptr);
	~SetupDialog() override;

    unsigned int rate();
    unsigned int duration();
	QString outputFormat();
	std::vector<int> inputOrder();

private slots:
    void on_horizontalSlider_valueChanged(int value);
	void on_upToolButton_clicked();
	void on_downToolButton_clicked();

private:
    Ui::SetupDialog *ui;
	std::map<int, int> rateMap_;
};

#endif // SETUPDIALOG_H
