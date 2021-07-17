#include "abstract_file_handler.hpp"
#include "file_io.hpp"
#include "file_io_error.hpp"

Tone* AbstractSingleToneIo::load(const BinaryContainer& /*container*/) const
{
	throw FileUnsupportedError(io::FileType::SingleTone);
}

const BinaryContainer AbstractSingleToneIo::save(const Tone& /*tone*/) const
{
	throw FileUnsupportedError(io::FileType::SingleTone);
}

std::vector<TonePtr> AbstractToneBankIo::load(BinaryContainer& /*container*/) const
{
	throw FileUnsupportedError(io::FileType::ToneBank);
}

const BinaryContainer AbstractToneBankIo::save(const std::vector<TonePtr>& /*bank*/) const
{
	throw FileUnsupportedError(io::FileType::ToneBank);
}

std::vector<TonePtr> AbstractSongFileIo::load(BinaryContainer& /*container*/) const
{
	throw FileUnsupportedError(io::FileType::SongFile);
}
