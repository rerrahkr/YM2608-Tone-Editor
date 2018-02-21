//#include <iostream>
//#include <fstream>
//#include <iomanip>
//#include <string>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "types.h"
#include "chips/opna.hpp"
#include "sound_thread.hpp"


//void Out(int16* buf, size_t len, int rate);

struct Operator {
	uint8 AR;
	uint8 DR;
	uint8 SR;
	uint8 RR;
	uint8 SL;
	uint8 TL;
	uint8 KS;
	uint8 ML;
	uint8 DT;
	uint8 AM;
};

struct Voice {
	std::string name;
	uint8 AL;
	uint8 FB;
	Operator op[4];
};

void SetFMVoice(chip::OPNA &opna, struct Voice voice, int channel);
void SetFMKeyOn(chip::OPNA &opna, int channel);
void SetFMKeyOff(chip::OPNA &opna, int channel);
void SetFMTone(chip::OPNA &opna, int channel, uint32 octave, uint32 keynum);
void SetPSGTone(chip::OPNA &opna, int channel, uint32 octave, uint32 keynum);
void InitPan(chip::OPNA& opna);

void JamKeyOn(chip::OPNA& opna, uint32 keynum);
void JamKeyOff(chip::OPNA& opna);


int main(int argc, char* argv[])
{
	chip::OPNA chip(3993600 * 2, 0);

#ifndef REGION
	Voice voice;
	voice.name = "detune";
	voice.AL = 5;
	voice.FB = 5;
	voice.op[0].AR = 31;
	voice.op[0].DR = 5;
	voice.op[0].SR = 0;
	voice.op[0].RR = 7;
	voice.op[0].SL = 12;
	voice.op[0].TL = 22;
	voice.op[0].KS = 0;
	voice.op[0].ML = 0;
	voice.op[0].DT = 0;
	voice.op[0].AM = 0;
	voice.op[1].AR = 31;
	voice.op[1].DR = 5;
	voice.op[1].SR = 0;
	voice.op[1].RR = 7;
	voice.op[1].SL = 6;
	voice.op[1].TL = 12;
	voice.op[1].KS = 0;
	voice.op[1].ML = 2;
	voice.op[1].DT = 3;
	voice.op[1].AM = 0;
	voice.op[2].AR = 31;
	voice.op[2].DR = 5;
	voice.op[2].SR = 0;
	voice.op[2].RR = 7;
	voice.op[2].SL = 7;
	voice.op[2].TL = 21;
	voice.op[2].KS = 0;
	voice.op[2].ML = 3;
	voice.op[2].DT = 0;
	voice.op[2].AM = 0;
	voice.op[3].AR = 31;
	voice.op[3].DR = 0;
	voice.op[3].SR = 0;
	voice.op[3].RR = 7;
	voice.op[3].SL = 0;
	voice.op[3].TL = 0;
	voice.op[3].KS = 0;
	voice.op[3].ML = 0;
	voice.op[3].DT = 0;
	voice.op[3].AM = 0;
#endif

	chip.setRegister(0x29, 0x80);	// Init interrupt
	InitPan(chip);
	SetFMVoice(chip, voice, 1);

	//JamKeyOn(chip, 0);
	//chip.mix(buffer.get(), soundBufferCount);
	//Out(buffer.get(), soundBufferByteSize, chip.getRate());

	if (SDL_Init(SDL_INIT_VIDEO)) return 1;

	int h = 500;
	int w = 500;
	SDL_Window* window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, 0);

	if (window != nullptr) {

		SDL_Surface* surface = SDL_GetWindowSurface(window);
		if (surface != nullptr) {
			SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));
			SDL_Rect rect{ 10, 10, 10, 10 };
			SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0xff, 0xff, 0xff));

			SDL_UpdateWindowSurface(window);
		}

		thread::SoundThread at(chip, 40);

		while (true) {
			SDL_Event ev;
			while (SDL_PollEvent(&ev)) {
				switch (ev.type) {
				case SDL_KEYDOWN:	// Keydown event
					switch (ev.key.keysym.sym) {
					case SDLK_z: JamKeyOn(chip, 0);		break;
					case SDLK_s: JamKeyOn(chip, 1);		break;
					case SDLK_x: JamKeyOn(chip, 2);		break;
					case SDLK_d: JamKeyOn(chip, 3);		break;
					case SDLK_c: JamKeyOn(chip, 4);		break;
					case SDLK_v: JamKeyOn(chip, 5);		break;
					case SDLK_g: JamKeyOn(chip, 6);		break;
					case SDLK_b: JamKeyOn(chip, 7);		break;
					case SDLK_h: JamKeyOn(chip, 8);		break;
					case SDLK_n: JamKeyOn(chip, 9);		break;
					case SDLK_j: JamKeyOn(chip, 10);	break;
					case SDLK_m: JamKeyOn(chip, 11);	break;
					case SDLK_r:	// Reset sound
						at.stopSound();
						chip.reset();
						at.restartSound();

						chip.setRegister(0x29, 0x80);	// Init interrupt
						InitPan(chip);
						SetFMVoice(chip, voice, 1);
						break;
					case SDLK_ESCAPE:
						goto END_LABEL;
					default:
						break;
					}
					break;
				case SDL_KEYUP:		// Keyup event
					JamKeyOff(chip);
					break;
				default:
					break;
				}
			}
		}

	END_LABEL:
		SDL_DestroyWindow(window);
	}

	SDL_Quit();

	return 0;
}


//void Out(int16* buf, size_t len, int rate)
//{
//	std::ofstream ofs("out.wav", std::ios::out | std::ios::binary | std::ios::trunc);
//	if (!ofs) {
//		std::cerr << "error" << std::endl;
//		int error;
//		std::cin >> error;
//		return;
//	}
//
//	ofs.seekp(0, std::ios::beg);
//
//	uint32 u32buf;
//	uint16 u16buf;
//
//	uint32 sampleSize = len;	// L&R
//
//	// RIFF headder
//	ofs.write("RIFF", 4);
//	u32buf = sampleSize + 36;
//	ofs.write(reinterpret_cast<char*>(&u32buf), sizeof(uint32));
//	ofs.write("WAVE", 4);
//
//	// fmt chunk
//	ofs.write("fmt ", 4);
//	u32buf = 16;
//	ofs.write(reinterpret_cast<char*>(&u32buf), sizeof(uint32));
//	u16buf = 1;
//	ofs.write(reinterpret_cast<char*>(&u16buf), sizeof(uint16));
//	uint16 channel = 2;
//	ofs.write(reinterpret_cast<char*>(&channel), sizeof(uint16));
//	uint32 samp_freq = rate;
//	ofs.write(reinterpret_cast<char*>(&samp_freq), sizeof(uint32));
//	u32buf = 2 * channel * samp_freq;
//	ofs.write(reinterpret_cast<char*>(&u32buf), sizeof(uint32));
//	u16buf = 2 * channel;
//	ofs.write(reinterpret_cast<char*>(&u16buf), sizeof(uint16));
//	u16buf = 16;
//	ofs.write(reinterpret_cast<char*>(&u16buf), sizeof(uint16));
//
//	// data chunk
//	ofs.write("data", 4);
//	ofs.write(reinterpret_cast<char*>(&sampleSize), sizeof(uint32));
//
//	char* tb = reinterpret_cast<char*>(buf);
//	for (size_t i = 0; i < len; i++)
//	{
//		ofs.write(tb + i, sizeof(char));
//	}
//
//	ofs.close();
//}

#define FMSOUND
//#define PSGSOUND

void JamKeyOn(chip::OPNA& opna, uint32 keynum)
{
#ifdef FMSOUND
	SetFMTone(opna, 1, 4, keynum);
	SetFMKeyOn(opna, 1);
#endif // FMSOUND
#ifdef PSGSOUND
	SetPSGTone(opna, 1, 3, keynum);
	opna.setRegister(0x08, 0x0f);
	opna.setRegister(0x07, 0x7e);
#endif // PSGSOUND
}


void JamKeyOff(chip::OPNA& opna)
{
#ifdef FMSOUND
	SetFMKeyOff(opna, 1);
#endif // FMSOUND
#ifdef PSGSOUND
	opna.setRegister(0x08, 0x00);
	opna.setRegister(0x07, 0x7f);
#endif // PSGSOUND
}


void SetFMVoice(chip::OPNA &opna, struct Voice voice, int channel)
{
	uint32 bch;	// Bank and channel offset
	switch (channel) {
	case 1:
	case 2:
	case 3:
		bch = channel - 1;
		break;
	case 4:
	case 5:
	case 6:
		bch = 0x100 + (channel % 3) - 1;
		break;
	}

	uint32 data;

	data = voice.FB << 3;
	data += voice.AL;
	opna.setRegister(0xb0 + bch, data);

	for (int i = 0; i < 4; i++) {
		uint32 offset;
		switch (i) {
		case 0: offset = 0;  break;
		case 1: offset = 8;  break;
		case 2: offset = 4;  break;
		case 3: offset = 12; break;
		}

		data = voice.op[i].DT << 4;
		data |= voice.op[i].ML;
		opna.setRegister(0x30 + bch + offset, data);

		data = voice.op[i].TL;
		opna.setRegister(0x40 + bch + offset, data);

		data = voice.op[i].KS << 6;
		data |= voice.op[i].AR;
		opna.setRegister(0x50 + bch + offset, data);

		data = voice.op[i].AM << 7;
		data |= voice.op[i].DR;
		opna.setRegister(0x60 + bch + offset, data);

		data = voice.op[i].SR;
		opna.setRegister(0x70 + bch + offset, data);

		data = voice.op[i].SL << 4;
		data |= voice.op[i].RR;
		opna.setRegister(0x80 + bch + offset, data);
	}
}

void SetFMKeyOn(chip::OPNA &opna, int channel)
{
	uint32 slot = 0x0f;
	uint32 ch;
	switch (channel) {
	case 1: ch = 0x00; break;
	case 2: ch = 0x01; break;
	case 3: ch = 0x02; break;
	case 4: ch = 0x04; break;
	case 5: ch = 0x05; break;
	case 6: ch = 0x06; break;
	}
	opna.setRegister(0x28, (slot << 4) | ch);
}

void SetFMKeyOff(chip::OPNA &opna, int channel)
{
	uint32 slot = 0x00;
	uint32 ch;
	switch (channel) {
	case 1: ch = 0x00; break;
	case 2: ch = 0x01; break;
	case 3: ch = 0x02; break;
	case 4: ch = 0x04; break;
	case 5: ch = 0x05; break;
	case 6: ch = 0x06; break;
	}
	opna.setRegister(0x28, (slot << 4) | ch);
}

const uint32 fm_tune_tbl[] = {
	0x026a,	// C
	0x028f,	// C#
	0x02b6,	// D
	0x02df,	// D#
	0x030b,	// E
	0x0339,	// F
	0x036a,	// F#
	0x039e,	// G
	0x03d5,	// G#
	0x0410,	// A
	0x044e,	// A#
	0x048f	// B
};

const int psg_tune_tbl[] = {
	0x0ee8,	// C
	0x0e12,	// C#
	0x0d48,	// D
	0x0c89,	// D#
	0x0bd5,	// E
	0x0b2b,	// F
	0x0a8a,	// F#
	0x09f3,	// G
	0x0964,	// G#
	0x08dd,	// A
	0x085e,	// A#
	0x07e6,	// B
};

void SetFMTone(chip::OPNA &opna, int channel, uint32 octave, uint32 keynum)
{
	uint32 bch;	// Bank and channel offset
	switch (channel) {
	case 1:
	case 2:
	case 3:
		bch = channel - 1;
		break;
	case 4:
	case 5:
	case 6:
		bch = 0x100 + (channel % 3) - 1;
		break;
	}

	// A4(440Hz)
	uint32 data;
	uint32 block = octave;	// Octave
	uint32 fnum = fm_tune_tbl[keynum];
	data = block << 3;
	data = data | (fnum >> 8);
	opna.setRegister(0xa4 + bch, data);
	data = fnum & 0xff;
	opna.setRegister(0xa0 + bch, data);
}

void SetPSGTone(chip::OPNA &opna, int channel, uint32 octave, uint32 keynum)
{
	uint32 offset = 2 * (channel - 1);

	uint32 data = psg_tune_tbl[keynum];
	if (octave > 0) {
		data = (data + 1) >> octave;
	}

	opna.setRegister(0x00 + offset, data & 0xff);
	opna.setRegister(0x01 + offset, data >> 8);
}

void InitPan(chip::OPNA& opna)
{
	opna.setRegister(0xb4, 0xc0);
	opna.setRegister(0xb5, 0xc0);
	opna.setRegister(0xb6, 0xc0);
}