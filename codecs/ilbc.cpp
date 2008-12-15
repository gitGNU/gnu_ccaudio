// Copyright (C) 1999-2001 Open Source Telecom Corporation.
// Copyright (C) 2006-2008 David Sugar, Tycho Softworks.
//
// This file is part of GNU ccAudio2.
//
// GNU ccAudio2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GNU ccAuydio2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GNU ccAudio2.  If not, see <http://www.gnu.org/licenses/>.

#include "codecs.h"

#include <ilbc/iLBC_decode.h>
#include <ilbc/iLBC_encode.h>

namespace ccAudioCodec {
using namespace ost;

class ilbcCodec : public AudioCodec
{
private:
	iLBC_Dec_Inst_t ilbc_decoder;
	iLBC_Enc_Inst_t ilbc_encoder;

public:
	AudioCodec *getByInfo(Info &info);
	AudioCodec *getByFormat(const char *format);

	unsigned decode(Linear buffer, void *from, unsigned lsamples);
	unsigned encode(Linear buffer, void *dest, unsigned lsamples);

	ilbcCodec(const char *id, Encoding e);
	ilbcCodec();
	~ilbcCodec();
};

ilbcCodec::ilbcCodec() : AudioCodec()
{

	info.framesize = 50;
	info.framecount = 240;
	info.rate = 8000;
	info.annotation = "ilbc";
	info.bitrate = 13333;

	memset(&ilbc_encoder, 0, sizeof(ilbc_encoder));
	memset(&ilbc_decoder, 0, sizeof(ilbc_decoder));
	initEncode(&ilbc_encoder, 30);
	initDecode(&ilbc_decoder, 30, USE_ILBC_ENHANCER);
}

ilbcCodec::ilbcCodec(const char *id, Encoding e) : AudioCodec(id, e)
{

	info.framesize = 50;
	info.framecount = 240;
	info.rate = 8000;
	info.annotation = "ilbc";
	info.bitrate = 13333;

	memset(&ilbc_encoder, 0, sizeof(ilbc_encoder));
	memset(&ilbc_decoder, 0, sizeof(ilbc_decoder));
	initEncode(&ilbc_encoder, 30);
	initDecode(&ilbc_decoder, 30, USE_ILBC_ENHANCER);
}

ilbcCodec::~ilbcCodec()
{}

unsigned ilbcCodec::encode(Linear buffer, void *coded, unsigned lsamples)
{
	unsigned count = (lsamples / 2) * 2;
	short data;
	bool hi = false;
	int diff, step, nib;
	unsigned char byte = 0;
	Encoded dest = (Encoded)coded;

	while(count--) {
		data = (*(buffer++)) >> 4;
		step = steps[encode_state.ssindex];
		diff = data - encode_state.signal;

		if(diff < 0) {
			nib = (-diff << 2) / step;
			if(nib > 7)
				nib = 7;
			nib |= 0x08;
		}
		else {
			nib = (diff << 2) / step;
			if(nib > 7)
				nib = 7;
		}
		coder(&encode_state, nib);
		if(hi) {
			byte |= nib;
			*(dest++) = byte;
			hi = false;
		}
		else {
			byte = (unsigned char)(nib << 4);
			hi = true;
		}

	}
	return (lsamples / 2) * 2;
}

unsigned g721Codec::decode(Linear buffer, void *from, unsigned lsamples)
{
	Encoded src = (Encoded)from;
	unsigned count = lsamples / 2;
	int nib;

	while(count--) {
		nib = (*src >> 4) & 0x0f;
		*(buffer++) = coder(&decode_state, nib);
		nib = *src & 0x0f;
		*(buffer++) = coder(&decode_state, nib);
		++src;
	}
	return (lsamples / 2) * 2;
}

AudioCodec *g721Codec::getByInfo(Info &info)
{
	return (AudioCodec *)new g721Codec();
}

AudioCodec *g721Codec::getByFormat(const char *format)
{
	return (AudioCodec *)new g721Codec();
}

static g721Codec codec("adpcm", Audio::g721ADPCM);

} // namespace

