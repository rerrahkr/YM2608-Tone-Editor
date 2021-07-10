#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <QMainWindow>
#include <QString>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFontMetrics>
#include "chips/opna.hpp"
#include "audio_stream.hpp"
#include "parameter_state.hpp"
#include "tone.hpp"
#include "widgets/operator_sliders.h"
#include "tone_converter.hpp"
#include "settings.hpp"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	Ui::MainWindow *ui;

	Settings settings_;

	chip::OPNA chip_;
	AudioStream audio_;

	int octave_;
	std::vector<int> jamKeyOnTableFM_, jamKeyOnTablePSG_;
	QString pressedKeyNameFM_, pressedKeyNamePSG_;

	OperatorSliders* sliders_[4];
	std::unique_ptr<QFontMetrics> nameFontMet_;

	ToneConverter converter_;

	void SetFMTone(int channel);
	void SetFMKeyOn(int channel);
	void SetPSGKeyOn(int channel);
	void SetFMKeyOff(int channel);
	void SetPSGKeyOff(int channel);
	void SetFMKey(int channel, int octave, int keynum);
	void SetPSGKey(int channel, int octave, int keynum);

	void JamKeyOnFM(int jamKeyNumber, bool isRepeat);
	void JamKeyOnPSG(int jamKeyNumber, bool isRepeat);
	void(MainWindow::*jamKeyOnFunc_)(int, bool);
	void JamKeyOffFM(int jamKeyNumber, bool isRepeat);
	void JamKeyOffPSG(int jamKeyNumber, bool isRepeat);
	void(MainWindow::*jamKeyOffFunc_)(int, bool);

	QString keyNumberToNameString(int jamKeyNumber);
	QString modifyDisplayedToneName(const QString& src) const;

	static const std::unordered_map<int, int> NOTE_NUM_MAP_;
	static const QString NOTE_NAME_TBL_[12];

	static constexpr int FM_FNUM_TBL_[12] = {
		0x026a,	// C
		0x028f,	// C#
		0x02b6,	// D
		0x02df,	// D#
		0x030b,	// E
		0x0339,	// F
		0x036a,	// F#
		0x039e,	// G
		0x03d5,	// G#
		0x0410,	// A
		0x044e,	// A#
		0x048f	// B
	};

	static constexpr int PSG_PITCH_TBL_[12] = {
		0x0ee8,	// C
		0x0e12,	// C#
		0x0d48,	// D
		0x0c89,	// D#
		0x0bd5,	// E
		0x0b2b,	// F
		0x0a8a,	// F#
		0x09f3,	// G
		0x0964,	// G#
		0x08dd,	// A
		0x085e,	// A#
		0x07e6,	// B
	};

	void loadSingleTone(const QString& file);
	void loadToneBank(const QString& file);
	void loadSongFile(const QString& file);

	void addToneTo(int n);
	void addToneTo(int n, Tone* tone);
	void addToneTo(int n, TonePtr tone);
	void removeToneAt(int n);

	TonePtr getCurrentTone() const;

	inline QMessageBox::StandardButton showSaveWarning()
	{
		return QMessageBox::warning(this, "Warning", "Do you want to save changes?",
									QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
	}

private slots:
	void on_lfoGroupBox_clicked(bool checked);
	void onFreqChanged(int value);
	void onPMSChanged(int value);
	void onAMSChanged(int value);
	void onALChanged(int value);
	void onFBChanged(int value);
	void onParameterChanged(int op, OperatorParameter param, int value);
	void on_actionOpen_O_triggered();
	void on_actionSave_S_triggered();
	void on_actionExit_X_triggered();
	void on_actionSave_As_triggered();
	void on_nameButton_clicked();
	void on_actionConvert_To_Text_C_triggered();
	void on_actionSetup_E_triggered();
	void on_actionAbout_A_triggered();
	void on_actionRead_Text_R_triggered();
	void on_newTonePushButton_clicked();
	void on_removeTonePushButton_clicked();
	void on_listWidget_currentRowChanged(int currentRow);
	void on_actionSave_Bank_As_triggered();
	void on_actionO_pen_Bank_triggered();
	void on_lineEdit_textChanged(const QString &arg1);

	void on_actionOpe_n_Song_triggered();

private:
	bool saveTone();
	bool saveToneAs();
};

#endif // MAINWINDOW_H
