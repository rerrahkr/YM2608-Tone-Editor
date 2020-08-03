#pragma once

#include <unordered_map>
#include <memory>
#include <QString>
#include <QFileInfo>
#include "../tone.hpp"
#include "binary_container.hpp"

class AbstractSingleToneIo
{
public:
	virtual Tone* load(const BinaryContainer& container) const = 0;
	virtual const BinaryContainer save(const Tone& tone) const = 0;
};

class FileIo
{
public:
	enum class FileType { SingleTone, ToneBank, Unknown };

	static FileIo& getInstance();

	FileType detectFileType(const QString& file) const;
	Tone* loadSingleToneFrom(const QString& file) const;
	void saveSingleToneFrom(const QString& file, const Tone& tone) const;

private:
	FileIo();

	inline QString extractExtention(const QString& file) const
	{
		return QFileInfo(file).suffix().toLower();
	}

	static std::unique_ptr<FileIo> instance_;

	std::unordered_map<QString, std::unique_ptr<AbstractSingleToneIo>> SINGLE_TONE_HANDLER_;
};
