#include "btb_io.hpp"
#include <unordered_map>
#include "file_io_error.hpp"

BtbIo::BtbIo() : AbstractToneBankIo("btb", "BambooTracker bank", true, true) {}

std::vector<TonePtr> BtbIo::load(BinaryContainer& container) const
{
	std::vector<TonePtr> bank;

	size_t globCsr = 0;
	if (container.readString(globCsr, 16) != "BambooTrackerBnk")
		throw FileCorruptionError(io::FileType::ToneBank);
	globCsr += 16;
	/*size_t eofOfs = */container.readUint32(globCsr);
	globCsr += 8;	// Skip also file version


	/***** Instrument section *****/
	std::unordered_map<int, std::vector<size_t>> envToneMap;
	std::unordered_map<int, std::vector<size_t>> lfoToneMap;
	if (container.readString(globCsr, 8) != "INSTRMNT")
		throw FileCorruptionError(io::FileType::ToneBank);
	globCsr += 8;
	size_t instOfs = container.readUint32(globCsr);
	size_t instCsr = globCsr + 4;
	uint8_t instCnt = container.readUint8(instCsr);
	instCsr += 1;
	for (uint8_t i = 0; i < instCnt; ++i) {
		instCsr++;	// Skip index
		size_t iOfs = container.readUint32(instCsr);
		size_t iCsr = instCsr + 4;
		size_t nameLen = container.readUint32(iCsr);
		iCsr += 4;
		std::string name = u8"";
		if (nameLen > 0) {
			name = container.readString(iCsr, nameLen);
			iCsr += nameLen;
		}
		if (container.readUint8(iCsr++) == 0x00) {	// FM instrument
			uint8_t envNum = container.readUint8(iCsr++);
			if (envToneMap.count(envNum))
				envToneMap[envNum].push_back(bank.size());
			else
				envToneMap[envNum] = { bank.size() };
			TonePtr tone = std::make_shared<Tone>();
			tone->name = name;
			uint8_t lfoNum = container.readUint8(iCsr++);
			if (!(lfoNum & 0x80)) {
				if (lfoToneMap.count(lfoNum))
					lfoToneMap[lfoNum].push_back(bank.size());
				else
					lfoToneMap[lfoNum] = { bank.size() };
			}
			bank.push_back(tone);
		}
		instCsr += iOfs;	// Jump to next
	}
	globCsr += instOfs;

	/***** Instrument property section *****/
	if (container.readString(globCsr, 8) != "INSTPROP")
		throw FileCorruptionError(io::FileType::ToneBank);
	globCsr += 8;
	size_t instPropOfs = container.readUint32(globCsr);
	size_t instPropCsr = globCsr + 4;
	globCsr += instPropOfs;
	while (instPropCsr < globCsr) {
		uint8_t secId = container.readUint8(instPropCsr++);
		if (secId == 0x00) {	// FM envelope
			uint8_t cnt = container.readUint8(instPropCsr++);
			for (size_t i = 0; i < cnt; ++i) {
				uint8_t idx = container.readUint8(instPropCsr++);
				uint8_t ofs = container.readUint8(instPropCsr);
				if (envToneMap.count(idx)) {
					std::vector<size_t> toneIdcs = envToneMap.at(idx);
					size_t csr = instPropCsr + 1;
					uint8_t alfb = container.readUint8(csr++);
					for (const size_t toneIdx : toneIdcs) {
						bank[toneIdx]->AL = alfb >> 4;
						bank[toneIdx]->FB = alfb & 0x0f;
					}
					for (size_t o = 0; o < 4; ++o) {
						uint8_t ar = container.readUint8(csr++);
						uint8_t ksdr = container.readUint8(csr++);
						uint8_t dtsr = container.readUint8(csr++);
						uint8_t slrr = container.readUint8(csr++);
						uint8_t tl = container.readUint8(csr++);
						uint8_t egml = container.readUint8(csr++);
						for (const size_t toneIdx : toneIdcs) {
							Operator& op = bank[toneIdx]->op[o];
							op.AR = ar & 0x1f;
							op.KS = ksdr >> 5;
							op.DR = ksdr & 0x1f;
							op.DT = dtsr >> 5;
							op.SR = dtsr & 0x1f;
							op.SL = slrr >> 4;
							op.RR = slrr & 0x0f;
							op.TL = tl;
							op.ML = egml & 0x0f;
							op.SSGEG = (egml >> 4) ^ 0x08;
						}
					}
				}
				instPropCsr += ofs;
			}
			break;
		}
		else if (secId == 0x01) {	// FM LFO
			uint8_t cnt = container.readUint8(instPropCsr++);
			for (size_t i = 0; i < cnt; ++i) {
				uint8_t idx = container.readUint8(instPropCsr++);
				uint8_t ofs = container.readUint8(instPropCsr);
				if (lfoToneMap.count(idx)) {
					std::vector<size_t> toneIdcs = lfoToneMap.at(idx);
					size_t csr = instPropCsr + 1;
					uint8_t tmp = container.readUint8(csr++);
					uint8_t freq = (tmp >> 4) | 8;
					uint8_t pms = tmp & 7;
					uint8_t amst = container.readUint8(csr++);
					uint8_t ams = amst & 3;
					for (const size_t toneIdx : toneIdcs) {
						TonePtr& tone = bank[toneIdx];
						tone->FREQ_LFO = freq;
						tone->PMS_LFO = pms;
						tone->AMS_LFO = ams;
						if (amst & 0x10) tone->op[0].AM = 1;
						if (amst & 0x20) tone->op[1].AM = 1;
						if (amst & 0x40) tone->op[2].AM = 1;
						if (amst & 0x80) tone->op[3].AM = 1;
					}
				}
				instPropCsr += ofs;
			}
		}
		else if (secId == 0x40) {	// ADPCM sample
			uint8_t cnt = container.readUint8(instPropCsr++);
			for (size_t i = 0; i < cnt; ++i) {
				instPropCsr++;	// Skip index
				uint32_t ofs = container.readUint32(instPropCsr);
				instPropCsr += ofs;
			}
		}
		else if (secId <= 0x29 || (0x30 <= secId && secId <= 0x34)
				 || (0x41 <= secId && secId <= 0x43)) {	// Sequence
			uint8_t cnt = container.readUint8(instPropCsr++);
			for (size_t i = 0; i < cnt; ++i) {
				instPropCsr++;	// Skip index
				uint16_t ofs = container.readUint16(instPropCsr);
				instPropCsr += ofs;
			}
		}
		else {
			throw FileCorruptionError(io::FileType::ToneBank);
		}
	}

	return bank;
}

const BinaryContainer BtbIo::save(const std::vector<TonePtr>& bank) const
{
	BinaryContainer container;

	container.appendString("BambooTrackerBnk");
	size_t eofOfs = container.size();
	container.appendUint32(0);	// Dummy EOF offset
	uint32_t fileVersion = 0x00010200;	// v1.2.0
	container.appendUint32(fileVersion);


	/***** Instrument section *****/
	container.appendString("INSTRMNT");
	size_t instOfs = container.size();
	container.appendUint32(0);	// Dummy instrument section offset
	container.appendUint8(bank.size());
	std::vector<size_t> lfoUser;
	for (size_t i = 0; i < bank.size(); ++i) {
		TonePtr tone = bank[i];
		container.appendUint8(static_cast<uint8_t>(i));
		size_t iOfs = container.size();
		container.appendUint32(0);	// Dummy instrument block offset
		std::string name = tone->name;
		container.appendUint32(name.length());
		if (!name.empty()) container.appendString(name);
		container.appendUint8(0x00);	// FM
		container.appendUint8(static_cast<uint8_t>(i));	// Envelope number
		if (tone->FREQ_LFO & 8) {	// LFO number
			container.appendUint8(static_cast<uint8_t>(lfoUser.size()));
			lfoUser.push_back(i);
		}
		else {
			container.appendUint8(0x80);
		}
		for (int i = 0; i < 40; ++i) container.appendUint8(0x80);
		container.appendUint8(0);
		for (int i = 0; i < 8; ++i) container.appendUint8(0x80);
		container.writeUint32(iOfs, container.size() - iOfs);
	}
	container.writeUint32(instOfs, container.size() - instOfs);


	/***** Instrument property section *****/
	container.appendString("INSTPROP");
	size_t instPropOfs = container.size();
	container.appendUint32(0);	// Dummy instrument property section offset

	// FM envelope
	container.appendUint8(0x00);
	container.appendUint8(static_cast<uint8_t>(bank.size()));
	for (size_t i = 0; i < bank.size(); ++i) {
		container.appendUint8(static_cast<uint8_t>(i));
		container.appendUint8(26);
		TonePtr tone = bank[i];
		container.appendUint8((tone->AL << 4) | tone->FB);
		// Operators
		for (size_t o = 0; o < 4; ++o) {
			const Operator& op = tone->op[o];
			container.appendUint8(0x20 | op.AR);
			container.appendUint8((op.KS << 5) | op.DR);
			container.appendUint8((op.DT << 5) | op.SR);
			container.appendUint8((op.SL << 4) | op.RR);
			container.appendUint8(op.TL);
			container.appendUint8(((op.SSGEG ^ 8) << 4)| op.ML);
		}
	}

	// FM LFO
	if (!lfoUser.empty()) {
		container.appendUint8(0x01);
		container.appendUint8(static_cast<uint8_t>(lfoUser.size()));
		for (size_t i = 0; i < lfoUser.size(); ++i) {
			container.appendUint8(static_cast<uint8_t>(i));
			container.appendUint8(4);
			TonePtr tone = bank[i];
			container.appendUint8(((tone->FREQ_LFO & 7) << 4) | tone->PMS_LFO);
			uint8_t am = 0;
			for (size_t i = 0; i < 4; i++) if (tone->op[i].AM) am |= (0x10 << i);
			container.appendUint8(am | tone->AMS_LFO);
			container.appendUint8(0);
		}
	}

	container.writeUint32(instPropOfs, container.size() - instPropOfs);

	container.writeUint32(eofOfs, container.size() - eofOfs);

	return container;
}
