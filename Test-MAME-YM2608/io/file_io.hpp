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
	AbstractSingleToneIo(std::string ext, std::string desc) : ext_(ext), desc_(desc) {}
	virtual Tone* load(const BinaryContainer& container) const = 0;
	virtual const BinaryContainer save(const Tone& tone) const = 0;
	inline std::string getExtension() const { return ext_; }
	inline std::string getFilterText() const { return desc_ + "(*." + ext_ + ")"; }

private:
	std::string ext_, desc_;
};

class AbstractToneBankIo
{
public:
	AbstractToneBankIo(std::string ext, std::string desc) : ext_(ext), desc_(desc) {}
	virtual std::vector<TonePtr> load(const BinaryContainer& container) const = 0;
	virtual const BinaryContainer save(const std::vector<TonePtr>& bank) const = 0;
	inline std::string getExtension() const { return ext_; }
	inline std::string getFilterText() const { return desc_ + "(*." + ext_ + ")"; }

private:
	std::string ext_, desc_;
};

class FileIo
{
public:
	enum class FileType { SingleTone, ToneBank, Unknown };

	static FileIo& getInstance();

	FileType detectFileType(const QString& file) const;
	QStringList getSingleToneFilter() const;
	Tone* loadSingleToneFrom(const QString& file) const;
	void saveSingleToneFrom(const QString& file, const Tone& tone) const;
	QStringList getToneBankFilter() const;
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
			filters_.push_back(utf8ToQString(handler->getFilterText()));
		}
		inline bool containExtension(std::string ext) const { return map_.count(ext); }
		inline const std::unique_ptr<T>& at(std::string ext) const { return map_.at(ext); }
		inline QStringList getFilterList() const { return filters_; }

	private:
		std::unordered_map<std::string, std::unique_ptr<T>> map_;
		QStringList filters_;
	};

	IoManagerMap<AbstractSingleToneIo> SINGLE_TONE_HANDLER_;
	IoManagerMap<AbstractToneBankIo> TONE_BANK_HANDLER_;
};
