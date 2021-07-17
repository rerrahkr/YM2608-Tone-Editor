#pragma once

#include <stdexcept>
#include <string>
#include "file_io.hpp"

class FileIoError : public std::runtime_error
{
public:
	io::FileType getFileType() const;

protected:
	FileIoError(const std::string& text, const io::FileType type);
	const io::FileType type_;
};

class FileInputError : public FileIoError
{
public:
	explicit FileInputError(const io::FileType type);
};

class FileOutputError : public FileIoError
{
public:
	explicit FileOutputError(const io::FileType type);
};

//class FileVersionError : public FileIOError
//{
//public:
//	explicit FileVersionError(const FileIo::FileType type);
//};

class FileCorruptionError : public FileIoError
{
public:
	explicit FileCorruptionError(const io::FileType type);
};

class FileUnsupportedError : public FileIoError
{
public:
	explicit FileUnsupportedError(const io::FileType type);
};
