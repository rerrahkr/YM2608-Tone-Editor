#pragma once

#include <QString>
#include <QStringList>
#include "../tone.hpp"
#include "binary_container.hpp"

class AbstractSingleToneIo
{
public:
	AbstractSingleToneIo(const QStringList& exts, const QString& desc, bool loadable, bool savable)
		: exts_(exts), desc_(desc), loadable_(loadable), savable_(savable) {}
	AbstractSingleToneIo(const QString& ext, const QString& desc, bool loadable, bool savable)
		: AbstractSingleToneIo(QStringList{ ext }, desc, loadable, savable) {}
	virtual ~AbstractSingleToneIo() = default;
	[[nodiscard]] virtual Tone* load(const BinaryContainer& container) const;
	virtual const BinaryContainer save(const Tone& tone) const;
	QStringList getExtensions() const { return exts_; }
	QString getFilterText() const
	{
		QStringList list = exts_;
		std::transform(list.begin(), list.end(), list.begin(), [](QString& s) { return "*." + s; });
		return desc_ + "(" + list.join(" ") + ")";
	}
	bool isLoadable() const { return loadable_; }
	bool isSavable() const { return savable_; }

private:
	const QStringList exts_;
	const QString desc_;
	bool loadable_, savable_;
};

class AbstractToneBankIo
{
public:
	AbstractToneBankIo(const QStringList& exts, const QString& desc, bool loadable, bool savable)
		: exts_(exts), desc_(desc), loadable_(loadable), savable_(savable) {}
	AbstractToneBankIo(const QString& ext, const QString& desc, bool loadable, bool savable)
		: AbstractToneBankIo(QStringList{ ext }, desc, loadable, savable) {}
	virtual ~AbstractToneBankIo() = default;
	virtual std::vector<TonePtr> load(BinaryContainer& container) const;
	virtual const BinaryContainer save(const std::vector<TonePtr>& bank) const;
	QStringList getExtensions() const { return exts_; }
	QString getFilterText() const
	{
		QStringList list = exts_;
		std::transform(list.begin(), list.end(), list.begin(), [](QString& s) { return "*." + s; });
		return desc_ + "(" + list.join(" ") + ")";
	}
	bool isLoadable() const { return loadable_; }
	bool isSavable() const { return savable_; }

private:
	const QStringList exts_;
	const QString desc_;
	bool loadable_, savable_;
};

class AbstractSongFileIo
{
public:
	AbstractSongFileIo(const QStringList& exts, const QString& desc)
		: exts_(exts), desc_(desc) {}
	AbstractSongFileIo(const QString& ext, const QString& desc)
		: AbstractSongFileIo(QStringList{ ext }, desc) {}
	virtual ~AbstractSongFileIo() = default;
	virtual std::vector<TonePtr> load(BinaryContainer& container) const;
	QStringList getExtensions() const { return exts_; }
	QString getFilterText() const
	{
		return desc_ + "(*." + exts_.join(" *.") + ")";
	}
	bool isLoadable() const { return true; }
	bool isSavable() const { return false; }

private:
	const QStringList exts_;
	const QString desc_;
};
