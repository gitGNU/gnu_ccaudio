// Copyright (C) 2006 David Sugar, Tycho Softworks
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

#include <speex/speex.h>

namespace ccAudioCodec {
using namespace ost;

class SpeexCommon: public AudioCodec
{
private:
	SpeexMode *spx_mode;
	SpeexBits enc_bits, dec_bits;
	unsigned int spx_clock, spx_channel;
	void *encoder, *decoder;
	int spx_frame;

public:
	SpeexCommon(Encoding enc, const char *name);
	~SpeexCommon();

	unsigned encode(Linear buffer, void *dest, unsigned lsamples);
	unsigned decode(Linear buffer, void *source, unsigned lsamples);

        AudioCodec *getByInfo(Info &info);
        AudioCodec *getByFormat(const char *format);
};

class SpeexAudio: public SpeexCommon
{
public:
	SpeexAudio();
};

class SpeexVoice: public SpeexCommon
{
public:
	SpeexVoice();
};

SpeexCommon::SpeexCommon(Encoding enc, const char *id) : 
AudioCodec("speex", enc)
{
        info.framesize = 20;
        info.framecount = 160;
        info.rate = 8000;
        info.bitrate = 24000;
        info.annotation = "speex/8000";

	spx_channel = 1;

	switch(enc)
	{
	case speexVoice:
		spx_clock = 8000;		
		spx_mode = &speex_nb_mode;
		break;
	case speexAudio:
		info.annotation = "speex/16000";
		info.framesize = 40;
		info.rate = 16000;
		spx_clock = 16000;
		spx_mode = &speex_wb_mode;
	default:
		break;
	}

	if(id)
		encoder = decoder = NULL;
	else
	{
		speex_bits_init(&dec_bits);
		decoder = speex_decoder_init(spx_mode);
		speex_bits_init(&enc_bits);
		encoder = speex_encoder_init(spx_mode);
		speex_decoder_ctl(decoder, SPEEX_GET_FRAME_SIZE, &spx_frame);
		info.framecount = spx_frame;
		info.set();
	}
}

SpeexCommon::~SpeexCommon()
{
	if(decoder)
	{
		speex_bits_destroy(&dec_bits);
		speex_decoder_destroy(decoder);
	}
	if(encoder)
	{
		speex_bits_destroy(&enc_bits);
		speex_encoder_destroy(encoder);
	}
	decoder = encoder = NULL;
}

AudioCodec *SpeexCommon::getByFormat(const char *format)
{
	if(!strnicmp(format, "speex/16", 8))
		return (AudioCodec *)new SpeexAudio(); 
        return (AudioCodec *)new SpeexVoice();
}

AudioCodec *SpeexCommon::getByInfo(Info &info)
{
	switch(info.encoding)
	{
	case speexAudio:
	        return (AudioCodec *)new SpeexAudio();
	default:
		return (AudioCodec *)new SpeexVoice();
	}
}

unsigned SpeexCommon::decode(Linear buffer, void *src, unsigned lsamples)
{
	unsigned count = lsamples / info.framecount;
	unsigned result = 0;
	char *encoded = (char *)src;

	if(!count)
		return 0;

	while(count--)
	{
		speex_bits_read_from(&dec_bits, encoded, info.framesize);
		if(speex_decode_int(decoder, &dec_bits, buffer))
			break;
		result += info.framesize;
	}
	return result;
}

unsigned SpeexCommon::encode(Linear buffer, void *dest, unsigned lsamples)
{
	unsigned count = lsamples / info.framecount;
	unsigned result = 0;
	char *encoded = (char *)dest;
	
	if(!count)
		return 0;
	
	while(count--)
	{
		speex_bits_reset(&enc_bits);
		speex_encoder_ctl(encoder, SPEEX_SET_SAMPLING_RATE, &spx_clock);
		speex_encode_int(encoder, buffer, &enc_bits);
		int nb = speex_bits_write(&enc_bits, encoded, info.framesize);
		buffer += 160;
		encoded += nb;
		result += nb;
	}
	return result;
}	

SpeexAudio::SpeexAudio() : 
SpeexCommon(speexAudio, NULL)
{
}

SpeexVoice::SpeexVoice() :
SpeexCommon(speexVoice, NULL)
{
}

static SpeexCommon codec(Audio::speexVoice, "speex");

// namespace
}

