#include "original_bank_io.hpp"
#include "file_io_error.hpp"

OriginalBankIo::OriginalBankIo() : AbstractToneBankIo("bank", "FM bank file", true, true) {}

std::vector<TonePtr> OriginalBankIo::load(BinaryContainer& container) const
{
	std::vector<TonePtr> bank;

	size_t csr = 0;
	std::string ident = container.readString(csr, 4);
	if (ident != "BANK" && ident != "BKV2") throw FileCorruptionError(FileIo::FileType::ToneBank);
	csr += 4;

	uint16_t cnt = container.readUint16(csr);
	csr += 2;
	bank.reserve(cnt);
	for (size_t i = 0; i < cnt; ++i) {
		TonePtr tone(new Tone);
		uint8_t strLen = container.readUint8(csr++);
		if (strLen) {
			tone->name = container.readString(csr, strLen);
			csr += strLen;
		}
		tone->AL = container.readUint8(csr++);
		tone->FB = container.readUint8(csr++);
		for (auto& op : tone->op) {
			op.AR = container.readUint8(csr++);
			op.DR = container.readUint8(csr++);
			op.SR = container.readUint8(csr++);
			op.RR = container.readUint8(csr++);
			op.SL = container.readUint8(csr++);
			op.TL = container.readUint8(csr++);
			op.KS = container.readUint8(csr++);
			op.ML = container.readUint8(csr++);
			op.DT = container.readUint8(csr++);
			op.AM = container.readUint8(csr++);
			if (ident == "BKV2") op.SSGEG = container.readUint8(csr++);
		}
		bank.push_back(std::move(tone));
	}

	return bank;
}

const BinaryContainer OriginalBankIo::save(const std::vector<TonePtr>& bank) const
{
	BinaryContainer container;
	container.appendString("BKV2");
	container.appendUint16(static_cast<uint16_t>(bank.size()));
	for (const TonePtr& tone : bank) {
		uint8_t nameLen = static_cast<uint8_t>(tone->name.size());
		container.appendUint8(nameLen);
		if (nameLen) container.appendString(tone->name);
		container.appendUint8(tone->AL);
		container.appendUint8(tone->FB);
		for (auto& op : tone->op) {
			container.appendUint8(op.AR);
			container.appendUint8(op.DR);
			container.appendUint8(op.SR);
			container.appendUint8(op.RR);
			container.appendUint8(op.SL);
			container.appendUint8(op.TL);
			container.appendUint8(op.KS);
			container.appendUint8(op.ML);
			container.appendUint8(op.DT);
			container.appendUint8(op.AM);
			container.appendUint8(op.SSGEG);
		}
	}
	return container;
}
