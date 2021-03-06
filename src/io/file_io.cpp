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
#include "wopn/opni_io.hpp"
#include "dmp_io.hpp"
#include "mucom88_io.hpp"
#include "tfi_io.hpp"
#include "vgi_io.hpp"
#include "y12_io.hpp"
#include "ins_io.hpp"
#include "bnk_io.hpp"
#include "gyb_io.hpp"
#include "vgm_io.hpp"
#include "s98_io.hpp"

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

std::vector<TonePtr> AbstractToneBankIo::load(BinaryContainer& container) const
{
	(void)container;
	throw FileUnsupportedError(FileIo::FileType::ToneBank);
}

const BinaryContainer AbstractToneBankIo::save(const std::vector<TonePtr>& bank) const
{
	(void)bank;
	throw FileUnsupportedError(FileIo::FileType::ToneBank);
}

std::vector<TonePtr> AbstractSongFileIo::load(BinaryContainer& container) const
{
	(void)container;
	throw FileUnsupportedError(FileIo::FileType::SongFile);
}

FileIo::FileIo()
{
	SINGLE_TONE_HANDLER_.add(new OriginalToneIo);
	SINGLE_TONE_HANDLER_.add(new BtiIo);
	SINGLE_TONE_HANDLER_.add(new DmpIo);
	SINGLE_TONE_HANDLER_.add(new OpniIo);
	SINGLE_TONE_HANDLER_.add(new TfiIo);
	SINGLE_TONE_HANDLER_.add(new VgiIo);
	SINGLE_TONE_HANDLER_.add(new Y12Io);
	SINGLE_TONE_HANDLER_.add(new InsIo);

	TONE_BANK_HANDLER_.add(new OriginalBankIo);
	TONE_BANK_HANDLER_.add(new BtbIo);
	TONE_BANK_HANDLER_.add(new FfIo);
	TONE_BANK_HANDLER_.add(new WopnIo);
	TONE_BANK_HANDLER_.add(new Mucom88Io);
	TONE_BANK_HANDLER_.add(new BnkIo);
	TONE_BANK_HANDLER_.add(new GybIo);

	SONG_FILE_HANDLER_.add(new VgmIo);
	SONG_FILE_HANDLER_.add(new S98Io);
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
	if (SONG_FILE_HANDLER_.containExtension(ext)) return FileType::SongFile;

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

QStringList FileIo::getSongFileLoadFilter() const
{
	return SONG_FILE_HANDLER_.getLoadFilterList();
}

std::vector<TonePtr> FileIo::loadSongFileFrom(const QString& file) const
{
	const std::string ext = FileIo::extractExtention(file);

	if (SONG_FILE_HANDLER_.containExtension(ext)) {
		QFile f(file);
		if (!f.open(QIODevice::ReadOnly)) throw FileInputError(FileType::SongFile);
		QByteArray array = f.readAll();
		f.close();
		BinaryContainer ctr(std::vector<char>(array.begin(), array.end()));
		std::vector<TonePtr> list = SONG_FILE_HANDLER_.at(ext)->load(ctr);

		for (TonePtr& tone : list) tone->path = (file + ".tone").toUtf8().toStdString();
		return list;
	}

	throw FileUnsupportedError(FileType::SongFile);
}
