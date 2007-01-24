// Copyright (C) 1999-2001 Open Source Telecom Corporation.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// As a special exception to the GNU General Public License, permission is
// granted for additional uses of the text contained in its release
// of ccaudio.
//
// The exception is that, if you link the ccaudio library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the ccaudio library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
//
// This exception applies only to the code released under the
// name ccaudio.  If you copy code from other releases into a copy of
// ccaudio, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
//
// If you write modifications of your own for ccaudio, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

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

