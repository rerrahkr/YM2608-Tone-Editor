#include "file_io_error.hpp"

FileIoError::FileIoError(const std::string& text, const io::FileType type)
	: std::runtime_error(text), type_(type)
{
}

io::FileType FileIoError::getFileType() const
{
	return type_;
}

/******************************/
FileInputError::FileInputError(const io::FileType type)
	: FileIoError("File input error", type)
{
}

/******************************/
FileOutputError::FileOutputError(const io::FileType type)
	: FileIoError("File output error", type)
{
}

/******************************/
//FileVersionError::FileVersionError(const FileIo::FileType type)
//	: FileIOError("File version error", type)
//{
//}

/******************************/
FileCorruptionError::FileCorruptionError(const io::FileType type)
	: FileIoError("File corruption error", type)
{
}

/******************************/
FileUnsupportedError::FileUnsupportedError(const io::FileType type)
	: FileIoError("File unsupported error", type)
{
}
