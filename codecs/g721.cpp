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

namespace ccAudioCodec {
using namespace ost;

static int index[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

static int steps[49] = {
  16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73,
  80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279,
  307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963,
  1060, 1166, 1282, 1411, 1552
};

class g721Codec : public AudioCodec
{
private:
	typedef struct state
	{
		int ssindex;
		int signal;
		int next_flag;
	}	state_t;

	state_t encode_state, decode_state;
		
public:
	AudioCodec *getByInfo(Info &info);
	AudioCodec *getByFormat(const char *format);

	unsigned decode(Linear buffer, void *from, unsigned lsamples);
	unsigned encode(Linear buffer, void *dest, unsigned lsamples);
	short coder(state_t *state, int nib);

	g721Codec(const char *id, Encoding e);
	g721Codec();
	~g721Codec();
};

g721Codec::g721Codec() : AudioCodec()
{
	info.framesize = 1;
	info.framecount = 2;
	info.rate = 8000;
	info.bitrate = 32000;
	info.annotation = "g.721";

	memset(&encode_state, 0, sizeof(encode_state));
        memset(&decode_state, 0, sizeof(decode_state));
}

g721Codec::g721Codec(const char *id, Encoding e) : AudioCodec(id, e)
{
	info.framesize = 1;
	info.framecount = 2;
	info.rate = 8000;
	info.bitrate = 32000;
	info.annotation = "g.721";
}

g721Codec::~g721Codec()
{}

short g721Codec::coder(state_t *state, int nib)
{
	int step, sign, diff;

	step = steps[state->ssindex];
	sign = nib & 0x08;
	nib &= 0x07;

	diff = (((nib << 1) + 1) * step) >> 3;
		
	if(sign)
		diff = -diff;

	if(state->next_flag & 0x01)
		state->signal -= 8;
	else if(state->next_flag & 0x02)
		state->signal += 8;

	state->signal += diff;
	if (state->signal > 2047)
                state->signal = 2047;
	else if (state->signal < -2047)
               	state->signal = -2047;

        state->next_flag = 0;
	state->ssindex += index[nib];
	if (state->ssindex < 0)
		state->ssindex = 0;
	else if(state->ssindex > 48)
		state->ssindex = 48;

	return state->signal << 4;	
}

unsigned g721Codec::encode(Linear buffer, void *coded, unsigned lsamples)
{
	unsigned count = (lsamples / 2) * 2;
	short data;
	bool hi = false;
	int diff, step, nib;
	unsigned char byte = 0;
	Encoded dest = (Encoded)coded;

	while(count--)
	{
		data = (*(buffer++)) >> 4;
		step = steps[encode_state.ssindex];
		diff = data - encode_state.signal;

		if(diff < 0)
		{
			nib = (-diff << 2) / step;
			if(nib > 7)
				nib = 7;
			nib |= 0x08;
		}
		else
		{
			nib = (diff << 2) / step;
			if(nib > 7)
				nib = 7;
		}
		coder(&encode_state, nib);
		if(hi)
		{
			byte |= nib;
			*(dest++) = byte;
			hi = false;
		}
		else
		{
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

	while(count--)
	{
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

