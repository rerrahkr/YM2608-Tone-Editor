#include "file_io.hpp"
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include "abstract_file_handler.hpp"
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
#include "pmd_io.hpp"
#include "fmp_io.hpp"
#include "fui_io.hpp"

namespace io
{
namespace
{
template<class T>
class IoManagerMap
{
public:
	IoManagerMap(std::initializer_list<T*> handlers)
	{
		QStringList ldExts, svExts;
		for (T* handler : handlers) {
			std::shared_ptr<T> ptr(handler);
			QStringList exts = handler->getExtensions();
			for (const auto& ext : exts) map_[ext] = ptr;
			QString filter = handler->getFilterText();
			std::transform(exts.begin(), exts.end(), exts.begin(), [](QString& s) { return "*." + s; });
			if (handler->isLoadable()) {
				ldFilters_.push_back(filter);
				ldExts += exts;
			}
			if (handler->isSavable()) {
				svFilters_.push_back(filter);
				svExts += exts;
			}
		}
		const QString allSupp = "All supported formats (%1)";
		ldFilters_.push_front(allSupp.arg(ldExts.join(" ")));
		svFilters_.push_front(allSupp.arg(svExts.join(" ")));

		const QString all = "All files (*)";
		ldFilters_.push_back(all);
		svFilters_.push_back(all);
	}

	bool containExtension(const QString& ext) const { return map_.contains(ext); }
	const std::shared_ptr<T> operator[](const QString& ext) const { return map_[ext]; }
	QStringList getLoadFilterList() const noexcept { return ldFilters_; }
	QStringList getSaveFilterList() const noexcept { return svFilters_; }

private:
	QHash<QString, std::shared_ptr<T>> map_;
	QStringList ldFilters_, svFilters_;
	QString ldExts_, svExts_;
};

IoManagerMap<AbstractSingleToneIo> SINGLE_TONE_HANDLERS = {
	new OriginalToneIo,
	new BtiIo,
	new DmpIo,
	new OpniIo,
	new TfiIo,
	new VgiIo,
	new Y12Io,
	new InsIo,
	new FuiIO,
};

IoManagerMap<AbstractToneBankIo> TONE_BANK_HANDLERS = {
	new OriginalBankIo,
	new BtbIo,
	new FfIo,
	new WopnIo,
	new Mucom88Io,
	new BnkIo,
	new GybIo,
};

IoManagerMap<AbstractSongFileIo> SONG_FILE_HANDLERS = {
	new VgmIo,
	new S98Io,
	new PmdIo,
	new FmpIo
};

inline QString extractExtention(const QString& file)
{
	return QFileInfo(file).suffix().toLower();
}
}

FileType detectFileType(const QString& file)
{
	const QString ext = extractExtention(file);

	if (SINGLE_TONE_HANDLERS.containExtension(ext)) return FileType::SingleTone;
	if (TONE_BANK_HANDLERS.containExtension(ext)) return FileType::ToneBank;
	if (SONG_FILE_HANDLERS.containExtension(ext)) return FileType::SongFile;

	return FileType::Unknown;
}

QStringList getSingleToneLoadFilter()
{
	return SINGLE_TONE_HANDLERS.getLoadFilterList();
}

QStringList getSingleToneSaveFilter()
{
	return SINGLE_TONE_HANDLERS.getSaveFilterList();
}

Tone* loadSingleToneFrom(const QString& file)
{
	const QString ext = extractExtention(file);

	if (SINGLE_TONE_HANDLERS.containExtension(ext)) {
		QFile f(file);
		if (!f.open(QIODevice::ReadOnly)) throw FileInputError(FileType::SingleTone);
		QByteArray array = f.readAll();
		f.close();
		BinaryContainer ctr(std::vector<char>(array.begin(), array.end()));
		Tone* tone = SINGLE_TONE_HANDLERS[ext]->load(ctr);

		tone->path = (file + ((ext == "tone") ? "" : ".tone")).toUtf8().toStdString();
		return tone;
	}

	throw FileUnsupportedError(FileType::SingleTone);
}

void saveSingleToneFrom(const QString& file, const Tone& tone)
{
	const QString ext = extractExtention(file);

	if (SINGLE_TONE_HANDLERS.containExtension(ext)) {
		const BinaryContainer&& container = SINGLE_TONE_HANDLERS[ext]->save(tone);
		QFile f(file);
		if (!f.open(QIODevice::WriteOnly)) throw FileOutputError(FileType::SingleTone);
		f.write(container.getPointer(), container.size());
	}
}

QStringList getToneBankLoadFilter()
{
	return TONE_BANK_HANDLERS.getLoadFilterList();
}

QStringList getToneBankSaveFilter()
{
	return TONE_BANK_HANDLERS.getSaveFilterList();
}

std::vector<TonePtr> loadToneBankFrom(const QString& file)
{
	const QString ext = extractExtention(file);

	if (TONE_BANK_HANDLERS.containExtension(ext)) {
		QFile f(file);
		if (!f.open(QIODevice::ReadOnly)) throw FileInputError(FileType::ToneBank);
		QByteArray array = f.readAll();
		f.close();
		BinaryContainer ctr(std::vector<char>(array.begin(), array.end()));
		std::vector<TonePtr> bank = TONE_BANK_HANDLERS[ext]->load(ctr);

		for (TonePtr& tone : bank) tone->path = (file + ".tone").toUtf8().toStdString();
		return bank;
	}

	throw FileUnsupportedError(FileType::ToneBank);
}

void saveToneBankFrom(const QString& file, const std::vector<TonePtr>& bank)
{
	const QString ext = extractExtention(file);

	if (TONE_BANK_HANDLERS.containExtension(ext)) {
		const BinaryContainer&& container = TONE_BANK_HANDLERS[ext]->save(bank);
		QFile f(file);
		if (!f.open(QIODevice::WriteOnly)) throw FileOutputError(FileType::ToneBank);
		f.write(container.getPointer(), container.size());
	}
}

QStringList getSongFileLoadFilter()
{
	return SONG_FILE_HANDLERS.getLoadFilterList();
}

std::vector<TonePtr> loadSongFileFrom(const QString& file)
{
	const QString ext = extractExtention(file);

	if (SONG_FILE_HANDLERS.containExtension(ext)) {
		QFile f(file);
		if (!f.open(QIODevice::ReadOnly)) throw FileInputError(FileType::SongFile);
		QByteArray array = f.readAll();
		f.close();
		BinaryContainer ctr(std::vector<char>(array.begin(), array.end()));
		std::vector<TonePtr> list = SONG_FILE_HANDLERS[ext]->load(ctr);

		for (TonePtr& tone : list) tone->path = (file + ".tone").toUtf8().toStdString();
		return list;
	}

	throw FileUnsupportedError(FileType::SongFile);
}
}
