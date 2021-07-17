#include "gyb_io.hpp"
#include "file_io_error.hpp"

GybIo::GybIo() : AbstractToneBankIo("gyb", "GYB bank", true, false) {}

std::vector<TonePtr> GybIo::load(BinaryContainer& container) const
{
	std::vector<TonePtr> bank;

	size_t csr = 0;
	uint8_t sig1 = container.readUint8(csr++);
	uint8_t sig2 = container.readUint8(csr++);
	if (sig1 != 26 || sig2 != 12) throw FileCorruptionError(io::FileType::ToneBank);

	uint8_t version = container.readUint8(csr++);
	if (version < 3) {
		uint8_t mCnt = container.readUint8(csr++);
		uint8_t dCnt = container.readUint8(csr++);
		uint8_t nTone = mCnt + dCnt;
		bank.reserve(nTone);
		csr += 0x100;	// Instrument map
		uint8_t lfoFreq = (version == 2) ? container.readUint8(csr++) : 0;
		for (size_t i = 0; i < nTone; ++i) {
			TonePtr tone(new Tone);
			uint8_t tmp;
			const size_t opOffs[] = { 0, 2, 1, 3 };
			for (int o = 0; o < 4; ++o) {
				Operator& op = tone->op[o];
				size_t opCsr = csr + opOffs[o];
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.DT = (tmp & 0x70) >> 4;
				op.ML = tmp & 0x0f;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.TL = tmp & 0x7f;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.KS = tmp >> 6;
				op.AR = tmp & 0x1f;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.AM = (tmp & 0x80) >> 7;
				op.DR = tmp & 0x1f;
				op.SR = container.readUint8(opCsr);
				opCsr += 4;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.SL = tmp >> 4;
				op.RR = tmp & 0x0f;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.SSGEG = tmp & 15;
			}
			csr += 28;
			tmp = container.readUint8(csr++);
			tone->FB = tmp >> 3;
			tone->AL = tmp & 0x07;
			if (version == 2) {
				tmp = container.readUint8(csr++);
				tone->PMS_LFO = tmp & 7;
				tone->AMS_LFO = (tmp >> 4) & 3;
				tone->FREQ_LFO = lfoFreq;
				csr += 2;
			}
			else {
				csr++;
			}
			bank.push_back(std::move(tone));
		}
		for (TonePtr& tone: bank) {
			uint8_t strLen = container.readUint8(csr++);
			tone->name = container.readString(csr, strLen);
			csr += strLen;
		}
	}
	else {
		uint8_t lfoFreq = container.readUint8(csr++);
		if (container.readUint32(csr) != container.size())
			throw FileCorruptionError(io::FileType::ToneBank);
		csr += 4;
		csr = container.readUint32(csr);	// Jump to instrument bank
		uint16_t cnt = container.readUint16(csr);
		csr += 2;
		for (size_t i = 0; i < cnt; ++i) {
			TonePtr tone(new Tone);
			uint16_t instOfs = container.readUint16(csr);
			size_t instCsr = csr + 2;
			uint8_t tmp;
			const size_t opOffs[] = { 0, 2, 1, 3 };
			for (int o = 0; o < 4; ++o) {
				Operator& op = tone->op[o];
				size_t opCsr = instCsr + opOffs[o];
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.DT = (tmp & 0x70) >> 4;
				op.ML = tmp & 0x0f;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.TL = tmp & 0x7f;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.KS = tmp >> 6;
				op.AR = tmp & 0x1f;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.AM = (tmp & 0x80) >> 7;
				op.DR = tmp & 0x1f;
				op.SR = container.readUint8(opCsr);
				opCsr += 4;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.SL = tmp >> 4;
				op.RR = tmp & 0x0f;
				tmp = container.readUint8(opCsr);
				opCsr += 4;
				op.SSGEG = tmp & 15;
			}
			instCsr += 28;
			tmp = container.readUint8(instCsr++);
			tone->FB = tmp >> 3;
			tone->AL = tmp & 0x07;
			tmp = container.readUint8(instCsr++);
			tone->PMS_LFO = tmp & 7;
			tone->AMS_LFO = (tmp >> 4) & 3;
			tone->FREQ_LFO = lfoFreq;
			instCsr++;
			uint8_t flags = container.readUint8(instCsr++);
			if (flags & 1) {	// Chord
				uint8_t n = container.readUint8(instCsr++);
				instCsr += n;
			}
			uint8_t nameLen = container.readUint8(instCsr++);
			tone->name = container.readString(instCsr, nameLen);
			instCsr += nameLen;
			bank.push_back(std::move(tone));
			csr += instOfs;
		}
	}

	return bank;
}
