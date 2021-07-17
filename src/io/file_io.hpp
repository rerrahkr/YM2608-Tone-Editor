#pragma once

#include <vector>
#include <memory>
#include <QString>
#include <QStringList>
#include "tone.hpp"

namespace io
{
enum class FileType { SingleTone, ToneBank, SongFile, Unknown };

FileType detectFileType(const QString& file);
QStringList getSingleToneLoadFilter();
QStringList getSingleToneSaveFilter();
Tone* loadSingleToneFrom(const QString& file);
void saveSingleToneFrom(const QString& file, const Tone& tone);
QStringList getToneBankLoadFilter();
QStringList getToneBankSaveFilter();
std::vector<TonePtr> loadToneBankFrom(const QString& file);
void saveToneBankFrom(const QString& file, const std::vector<TonePtr>& bank);
QStringList getSongFileLoadFilter();
std::vector<TonePtr> loadSongFileFrom(const QString& file);
};
