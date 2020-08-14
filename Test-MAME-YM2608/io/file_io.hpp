#pragma once

#include <unordered_map>
#include <memory>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include "../tone.hpp"
#include "binary_container.hpp"
#include "../text_conversion.hpp"

class AbstractSingleToneIo
{
public:
	AbstractSingleToneIo(std::string ext, std::string desc, bool loadable, bool savable)
		: ext_(ext), desc_(desc), loadable_(loadable), savable_(savable) {}
	virtual Tone* load(const BinaryContainer& container) const;
	virtual const BinaryContainer save(const Tone& tone) const;
	inline std::string getExtension() const { return ext_; }
	inline std::string getFilterText() const { return desc_ + "(*." + ext_ + ")"; }
	inline bool isLoadable() const { return loadable_; }
	inline bool isSavable() const { return savable_; }

private:
	std::string ext_, desc_;
	bool loadable_, savable_;
};

class AbstractToneBankIo
{
public:
	AbstractToneBankIo(std::string ext, std::string desc, bool loadable, bool savable)
		: ext_(ext), desc_(desc), loadable_(loadable), savable_(savable) {}
	virtual std::vector<TonePtr> load(const BinaryContainer& container) const;
	virtual const BinaryContainer save(const std::vector<TonePtr>& bank) const;
	inline std::string getExtension() const { return ext_; }
	inline std::string getFilterText() const { return desc_ + "(*." + ext_ + ")"; }
	inline bool isLoadable() const { return loadable_; }
	inline bool isSavable() const { return savable_; }

private:
	std::string ext_, desc_;
	bool loadable_, savable_;
};

class FileIo
{
public:
	enum class FileType { SingleTone, ToneBank, Unknown };

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

private:
	FileIo();

	inline std::string extractExtention(const QString& file) const
	{
		return QFileInfo(file).suffix().toLower().toUtf8().toStdString();
	}

	static std::unique_ptr<FileIo> instance_;

	template<class T>
	class IoManagerMap
	{
	public:
		inline void add(T* handler)
		{
			map_[handler->getExtension()].reset(handler);
			QString filter = utf8ToQString(handler->getFilterText());
			if (handler->isLoadable()) ldFilters_.push_back(filter);
			if (handler->isSavable()) svFilters_.push_back(filter);
		}
		inline bool containExtension(std::string ext) const { return map_.count(ext); }
		inline const std::unique_ptr<T>& at(std::string ext) const { return map_.at(ext); }
		inline QStringList getLoadFilterList() const { return ldFilters_; }
		inline QStringList getSaveFilterList() const { return svFilters_; }

	private:
		std::unordered_map<std::string, std::unique_ptr<T>> map_;
		QStringList ldFilters_, svFilters_;
	};

	IoManagerMap<AbstractSingleToneIo> SINGLE_TONE_HANDLER_;
	IoManagerMap<AbstractToneBankIo> TONE_BANK_HANDLER_;
};
