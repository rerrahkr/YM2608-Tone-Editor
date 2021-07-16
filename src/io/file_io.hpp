#pragma once

#include <memory>
#include <QHash>
#include <QString>
#include <QStringList>
#include "../tone.hpp"
#include "binary_container.hpp"
#include "../text_conversion.hpp"

class AbstractSingleToneIo
{
public:
	AbstractSingleToneIo(const QStringList& exts, const QString& desc, bool loadable, bool savable)
		: exts_(exts), desc_(desc), loadable_(loadable), savable_(savable) {}
	AbstractSingleToneIo(const QString& ext, const QString& desc, bool loadable, bool savable)
		: AbstractSingleToneIo(QStringList{ ext }, desc, loadable, savable) {}
	virtual ~AbstractSingleToneIo() = default;
	virtual Tone* load(const BinaryContainer& container) const;
	virtual const BinaryContainer save(const Tone& tone) const;
	QStringList getExtensions() const { return exts_; }
	QString getFilterText() const
	{
		QStringList list = exts_;
		std::transform(list.begin(), list.end(), list.begin(), [](QString& s) { return "*." + s; });
		return desc_ + "(" + list.join(" ") + ")";
	}
	bool isLoadable() const { return loadable_; }
	bool isSavable() const { return savable_; }

private:
	const QStringList exts_;
	const QString desc_;
	bool loadable_, savable_;
};

class AbstractToneBankIo
{
public:
	AbstractToneBankIo(const QStringList& exts, const QString& desc, bool loadable, bool savable)
		: exts_(exts), desc_(desc), loadable_(loadable), savable_(savable) {}
	AbstractToneBankIo(const QString& ext, const QString& desc, bool loadable, bool savable)
		: AbstractToneBankIo(QStringList{ ext }, desc, loadable, savable) {}
	virtual ~AbstractToneBankIo() = default;
	virtual std::vector<TonePtr> load(BinaryContainer& container) const;
	virtual const BinaryContainer save(const std::vector<TonePtr>& bank) const;
	QStringList getExtensions() const { return exts_; }
	QString getFilterText() const
	{
		QStringList list = exts_;
		std::transform(list.begin(), list.end(), list.begin(), [](QString& s) { return "*." + s; });
		return desc_ + "(" + list.join(" ") + ")";
	}
	bool isLoadable() const { return loadable_; }
	bool isSavable() const { return savable_; }

private:
	const QStringList exts_;
	const QString desc_;
	bool loadable_, savable_;
};

class AbstractSongFileIo
{
public:
	AbstractSongFileIo(const QStringList& exts, const QString& desc)
		: exts_(exts), desc_(desc) {}
	AbstractSongFileIo(const QString& ext, const QString& desc)
		: AbstractSongFileIo(QStringList{ ext }, desc) {}
	virtual ~AbstractSongFileIo() = default;
	virtual std::vector<TonePtr> load(BinaryContainer& container) const;
	QStringList getExtensions() const { return exts_; }
	QString getFilterText() const
	{
		QStringList list = exts_;
		std::transform(list.begin(), list.end(), list.begin(), [](QString& s) { return "*." + s; });
		return desc_ + "(" + list.join(" ") + ")";
	}
	bool isLoadable() const { return true; }
	bool isSavable() const { return false; }

private:
	const QStringList exts_;
	const QString desc_;
};

class FileIo
{
public:
	enum class FileType { SingleTone, ToneBank, SongFile, Unknown };

	static FileIo& getInstance();

	FileType detectFileType(const QString& file) const;
	QStringList getSingleToneLoadFilter() const;
	QStringList getSingleToneSaveFilter() const;
	Tone* loadSingleToneFrom(const QString& file) const;
	void saveSingleToneFrom(const QString& file, const Tone& tone) const;
	QStringList getToneBankLoadFilter() const;
	QStringList getToneBankSaveFilter() const;
	std::vector<TonePtr> loadToneBankFrom(const QString& file) const;
	void saveToneBankFrom(const QString& file, const std::vector<TonePtr>& bank) const;
	QStringList getSongFileLoadFilter() const;
	std::vector<TonePtr> loadSongFileFrom(const QString& file) const;

private:
	FileIo();

	static inline std::unique_ptr<FileIo> instance_;

	template<class T>
	class IoManagerMap
	{
	public:
		void add(T* handler)
		{
			std::shared_ptr<T> ptr(handler);
			for (const auto& ext : handler->getExtensions()) map_[ext] = ptr;
			QString filter = handler->getFilterText();
			if (handler->isLoadable()) ldFilters_.push_back(filter);
			if (handler->isSavable()) svFilters_.push_back(filter);
		}
		bool containExtension(const QString& ext) const { return map_.contains(ext); }
		const std::shared_ptr<T>& at(const QString& ext) const { return map_[ext]; }
		QStringList getLoadFilterList() const { return ldFilters_; }
		QStringList getSaveFilterList() const { return svFilters_; }

	private:
		QHash<QString, std::shared_ptr<T>> map_;
		QStringList ldFilters_, svFilters_;
	};

	IoManagerMap<AbstractSingleToneIo> SINGLE_TONE_HANDLER_;
	IoManagerMap<AbstractToneBankIo> TONE_BANK_HANDLER_;
	IoManagerMap<AbstractSongFileIo> SONG_FILE_HANDLER_;
};
