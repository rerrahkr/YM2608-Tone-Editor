#include "file_io_error.hpp"

FileIoError::FileIoError(std::string text, const FileIo::FileType type)
	: std::runtime_error(text), type_(type)
{
}

FileIo::FileType FileIoError::getFileType() const
{
	return type_;
}

/******************************/
FileInputError::FileInputError(const FileIo::FileType type)
	: FileIoError("File input error", type)
{
}

/******************************/
FileOutputError::FileOutputError(const FileIo::FileType type)
	: FileIoError("File output error", type)
{
}

/******************************/
//FileVersionError::FileVersionError(const FileIo::FileType type)
//	: FileIOError("File version error", type)
//{
//}

/******************************/
FileCorruptionError::FileCorruptionError(const FileIo::FileType type)
	: FileIoError("File corruption error", type)
{
}

/******************************/
FileUnsupportedError::FileUnsupportedError(const FileIo::FileType type)
	: FileIoError("File unsupported error", type)
{
}
