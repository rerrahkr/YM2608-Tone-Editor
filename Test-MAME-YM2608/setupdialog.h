#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <map>
#include "settings.hpp"

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(Settings& settings, QWidget *parent = 0);
    ~SetupDialog();

    unsigned int rate();
    unsigned int duration();

private slots:
    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::SetupDialog *ui;
	std::map<int, int> rateMap_;
};

#endif // SETUPDIALOG_H
