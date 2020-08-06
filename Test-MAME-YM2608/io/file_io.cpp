#include "file_io.hpp"
#include <QFile>
#include <QIODevice>
#include "file_io_error.hpp"
#include "original_tone_io.hpp"
#include "bti_io.hpp"
#include "btb_io.hpp"

std::unique_ptr<FileIo> FileIo::instance_;

FileIo::FileIo()
{
	SINGLE_TONE_HANDLER_["tone"].reset(new OriginalToneIo);
	SINGLE_TONE_HANDLER_["bti"].reset(new BtiIo);

	TONE_BANK_HANDLER_["btb"].reset(new BtbIo);
}

FileIo& FileIo::getInstance()
{
	if (!instance_) instance_.reset(new FileIo);
	return *instance_;
}

FileIo::FileType FileIo::detectFileType(const QString& file) const
{
	const QString ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.count(ext)) return FileType::SingleTone;
	if (TONE_BANK_HANDLER_.count(ext)) return FileType::ToneBank;

	return FileType::Unknown;
}

Tone* FileIo::loadSingleToneFrom(const QString& file) const
{
	const QString ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.count(ext)) {
		QFile f(file);
		if (!f.open(QIODevice::ReadOnly)) throw FileInputError(FileType::SingleTone);
		QByteArray array = f.readAll();
		f.close();
		BinaryContainer ctr(std::vector<char>(array.begin(), array.end()));
		Tone* tone = SINGLE_TONE_HANDLER_.at(ext)->load(ctr);

		tone->path = (file + ((ext == "tone") ? "" : ".tone")).toUtf8().toStdString();
		return tone;
	}

	throw FileUnsupportedError(FileType::SingleTone);
}

void FileIo::saveSingleToneFrom(const QString& file, const Tone& tone) const
{
	const QString ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.count(ext)) {
		const BinaryContainer&& container = SINGLE_TONE_HANDLER_.at(ext)->save(tone);
		QFile f(file);
		if (!f.open(QIODevice::WriteOnly)) throw FileOutputError(FileType::SingleTone);
		f.write(container.getPointer(), container.size());
	}
}

std::vector<TonePtr> FileIo::loadToneBankFrom(const QString& file) const
{
	const QString ext = FileIo::extractExtention(file);

	if (TONE_BANK_HANDLER_.count(ext)) {
		QFile f(file);
		if (!f.open(QIODevice::ReadOnly)) throw FileInputError(FileType::ToneBank);
		QByteArray array = f.readAll();
		f.close();
		BinaryContainer ctr(std::vector<char>(array.begin(), array.end()));
		std::vector<TonePtr> bank = TONE_BANK_HANDLER_.at(ext)->load(ctr);

		for (TonePtr& tone : bank) tone->path = (file + ".tone").toUtf8().toStdString();
		return bank;
	}

	throw FileUnsupportedError(FileType::ToneBank);
}

void FileIo::saveToneBankFrom(const QString& file, const std::vector<TonePtr>& bank) const
{
	const QString ext = FileIo::extractExtention(file);

	if (TONE_BANK_HANDLER_.count(ext)) {
		const BinaryContainer&& container = TONE_BANK_HANDLER_.at(ext)->save(bank);
		QFile f(file);
		if (!f.open(QIODevice::WriteOnly)) throw FileOutputError(FileType::ToneBank);
		f.write(container.getPointer(), container.size());
	}
}
