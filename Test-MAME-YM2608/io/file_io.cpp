#include "file_io.hpp"
#include <QFile>
#include <QIODevice>
#include "original_tone_io.hpp"

std::unique_ptr<FileIo> FileIo::instance_;

FileIo::FileIo()
{
	SINGLE_TONE_HANDLER_["tone"].reset(new OriginalToneIo);
}

FileIo& FileIo::getInstance()
{
	if (!instance_)
		instance_.reset(new FileIo);
	return *instance_;
}

FileIo::FileType FileIo::detectFileType(const QString& file) const
{
	const QString ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.count(ext)) return FileType::SingleTone;

	return FileType::Unknown;
}

Tone* FileIo::loadSingleToneFrom(const QString& file) const
{
	const QString ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.count(ext)) {
		QFile f(file);
		if (!f.open(QIODevice::ReadOnly)) return nullptr;
		QByteArray array = f.readAll();
		f.close();
		BinaryContainer ctr(std::vector<char>(array.begin(), array.end()));
		Tone* tone = SINGLE_TONE_HANDLER_.at(ext)->load(ctr);
		if (tone) tone->path = file.toUtf8().toStdString();
		return tone;
	}

	return nullptr;
}

void FileIo::saveSingleToneFrom(const QString& file, const Tone& tone) const
{
	const QString ext = FileIo::extractExtention(file);

	if (SINGLE_TONE_HANDLER_.count(ext)) {
		const BinaryContainer&& container = SINGLE_TONE_HANDLER_.at(ext)->save(tone);
		QFile f(file);
		if (f.open(QIODevice::WriteOnly)) {
			f.write(container.getPointer(), container.size());
		}
	}
}
