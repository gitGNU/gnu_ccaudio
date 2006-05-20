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

extern "C" {

#ifdef	HAVE_GSM_GSM_H
#include <gsm/gsm.h>
#else
#include <gsm.h>
#endif

}

namespace ccAudioCodec {
using namespace ost;

class GSMCodec : protected AudioCodec
{
protected:
	gsm encoder, decoder;

public:
	AudioCodec *getByInfo(Info &info);
	AudioCodec *getByFormat(const char *format);	
	
	unsigned encode(Linear data, void *dest, unsigned samples);
	unsigned decode(Linear data, void *source, unsigned samples);

	GSMCodec(const char *id, Encoding e);
	GSMCodec();
	~GSMCodec();
 
};

GSMCodec::GSMCodec()
{
        encoder = gsm_create();
        decoder = gsm_create();
        info.framesize = 33;
        info.framecount = 160;
        info.rate = 8000;
        info.bitrate = 13200;
        info.annotation = "gsm";
}

GSMCodec::GSMCodec(const char *id, Encoding e) : AudioCodec(id, e)
{
	encoder = gsm_create();	
	decoder = gsm_create();
	info.framesize = 33;
	info.framecount = 160;
	info.rate = 8000;
	info.bitrate = 13200;
	info.annotation = "gsm";
}

GSMCodec::~GSMCodec()
{
	gsm_destroy(encoder);
	gsm_destroy(decoder);
}

AudioCodec *GSMCodec::getByInfo(Info &info)
{
	return (AudioCodec *)new GSMCodec();
}

AudioCodec *GSMCodec::getByFormat(const char *format)
{
	return (AudioCodec *)new GSMCodec();
}

unsigned GSMCodec::encode(Linear from, void *dest, unsigned samples)
{
	unsigned count = samples / 160;
	unsigned result = count * 33;
	gsm_byte *encoded = (gsm_byte *)dest;

	if(!count)
		return 0;

	while(count--)
	{
		gsm_encode(encoder, from, encoded);
		from += 160;
		encoded += 33;
	}
	return result;
}

unsigned GSMCodec::decode(Linear dest, void *from, unsigned samples)
{
	unsigned count = samples / 160;
	unsigned result = count * 33;
	gsm_byte *encoded = (gsm_byte *)from;
	if(!count)
		return 0;

	while(count--)
	{
		gsm_decode(decoder, encoded, dest);
		encoded += 160;
		dest += 160;
	}
	return result;
}

static GSMCodec codec("gsm", Audio::gsmVoice);

}
