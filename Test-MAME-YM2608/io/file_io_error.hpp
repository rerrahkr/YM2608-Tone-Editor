#pragma once

#include <stdexcept>
#include <string>
#include "file_io.hpp"

class FileIoError : public std::runtime_error
{
public:
	FileIo::FileType getFileType() const;

protected:
	FileIoError(std::string text, const FileIo::FileType type);
	const FileIo::FileType type_;
};

class FileInputError : public FileIoError
{
public:
	FileInputError(const FileIo::FileType type);
};

class FileOutputError : public FileIoError
{
public:
	FileOutputError(const FileIo::FileType type);
};

//class FileVersionError : public FileIOError
//{
//public:
//	FileVersionError(const FileIo::FileType type);
//};

class FileCorruptionError : public FileIoError
{
public:
	FileCorruptionError(const FileIo::FileType type);
};

class FileUnsupportedError : public FileIoError
{
public:
	FileUnsupportedError(const FileIo::FileType type);
};
