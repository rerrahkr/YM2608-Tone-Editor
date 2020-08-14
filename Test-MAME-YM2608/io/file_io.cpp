#include "file_io.hpp"
#include <QFile>
#include <QIODevice>
#include "file_io_error.hpp"
#include "original_tone_io.hpp"
#include "original_bank_io.hpp"
#include "bti_io.hpp"
#include "btb_io.hpp"
#include "ff_io.hpp"
#include "wopn/wopn_io.hpp"

Tone* AbstractSingleToneIo::load(const BinaryContainer& container) const
{
	(void)container;
	throw FileUnsupportedError(FileIo::FileType::SingleTone);
}

const BinaryContainer AbstractSingleToneIo::save(const Tone& tone) const
{
	(void)tone;
	throw FileUnsupportedError(FileIo::FileType::SingleTone);
}

std::vector<TonePtr> AbstractToneBankIo::load(const BinaryContainer& container) const
{
	(void)container;
	throw FileUnsupportedError(FileIo::FileType::ToneBank);
}

const BinaryContainer AbstractToneBankIo::save(const std::vector<TonePtr>& bank) const
{
	(void)bank;
	throw FileUnsupportedError(FileIo::FileType::ToneBank);
}

std::unique_ptr<FileIo> FileIo::instance_;

FileIo::FileIo()
{
	SINGLE_TONE_HANDLER_.add(new OriginalToneIo);
	SINGLE_TONE_HANDLER_.add(new BtiIo);

	TONE_BANK_HANDLER_.add(new OriginalBankIo);
	TONE_BANK_HANDLER_.add(new BtbIo);
	TONE_BANK_HANDLER_.add(new FfIo);
	TONE_BANK_HANDLER_.add(new WopnIo);
}

FileIo& FileIo::getInstance()
{
	if (!instance_) instance_.reset(new FileIo);
	return *instance_;
}

FileIo::FileType FileIo::detectFileType(const QString& file) const
{
	const std::string ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.containExtension(ext)) return FileType::SingleTone;
	if (TONE_BANK_HANDLER_.containExtension(ext)) return FileType::ToneBank;

	return FileType::Unknown;
}

QStringList FileIo::getSingleToneLoadFilter() const
{
	return SINGLE_TONE_HANDLER_.getLoadFilterList();
}

QStringList FileIo::getSingleToneSaveFilter() const
{
	return SINGLE_TONE_HANDLER_.getSaveFilterList();
}

Tone* FileIo::loadSingleToneFrom(const QString& file) const
{
	const std::string ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.containExtension(ext)) {
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
	const std::string ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.containExtension(ext)) {
		const BinaryContainer&& container = SINGLE_TONE_HANDLER_.at(ext)->save(tone);
		QFile f(file);
		if (!f.open(QIODevice::WriteOnly)) throw FileOutputError(FileType::SingleTone);
		f.write(container.getPointer(), container.size());
	}
}

QStringList FileIo::getToneBankLoadFilter() const
{
	return TONE_BANK_HANDLER_.getLoadFilterList();
}

QStringList FileIo::getToneBankSaveFilter() const
{
	return TONE_BANK_HANDLER_.getSaveFilterList();
}

std::vector<TonePtr> FileIo::loadToneBankFrom(const QString& file) const
{
	const std::string ext = FileIo::extractExtention(file);

	if (TONE_BANK_HANDLER_.containExtension(ext)) {
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
	const std::string ext = FileIo::extractExtention(file);

	if (TONE_BANK_HANDLER_.containExtension(ext)) {
		const BinaryContainer&& container = TONE_BANK_HANDLER_.at(ext)->save(bank);
		QFile f(file);
		if (!f.open(QIODevice::WriteOnly)) throw FileOutputError(FileType::ToneBank);
		f.write(container.getPointer(), container.size());
	}
}
