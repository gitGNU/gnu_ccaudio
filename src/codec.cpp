// Copyright (C) 1999-2005 Open Source Telecom Corporation.
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
// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.
//
// This exception applies only to the code released under the name GNU
// ccAudio.  If you copy code from other releases into a copy of GNU
// ccAudio, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
//
// If you write modifications of your own for GNU ccAudio, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.
//

#include "private.h"
#include "audio2.h"
#include <cmath>
#include <cstdio>

#ifdef	HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifndef	M_PI
#define	M_PI	3.14159265358979323846
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1300
#if defined(_WIN64) 
#define	RLL_SUFFIX ".x64"
#elif defined(_M_IX86)
#define	RLL_SUFFIX ".x86"
#else
#define RLL_SUFFIX ".xlo"
#endif
#endif

#if !defined(RLL_SUFFIX) && defined(W32)  && !defined(__MINGW32__) && !defined(__CYGWIN32__)
#define	RLL_SUFFIX ".rll"
#endif

static ccAudio_Mutex_ lock;

#if defined(HAVE_PTHREAD_H)

ccAudio_Mutex_::ccAudio_Mutex_()
{
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&mutex, &attr);
}

ccAudio_Mutex_::~ccAudio_Mutex_()
{
	pthread_mutex_destroy(&mutex);
}

void ccAudio_Mutex_::enter(void)
{
	pthread_mutex_lock(&mutex);
}

void ccAudio_Mutex_::leave(void)
{
	pthread_mutex_unlock(&mutex);
}

#elif defined(W32)

ccAudio_Mutex_::ccAudio_Mutex_()
{
	InitializeCriticalSection(&mutex);
}

ccAudio_Mutex_::~ccAudio_Mutex_()
{
	DeleteCriticalSection(&mutex);
}

void ccAudio_Mutex_::enter(void)
{
	EnterCriticalSection(&mutex);
}

void ccAudio_Mutex_::leave(void)
{
	LeaveCriticalSection(&mutex);
}

#else

ccAudio_Mutex_::ccAudio_Mutex_() {};
ccAudio_Mutex_::~ccAudio_Mutex_() {};
void ccAudio_Mutex_::enter(void) {};
void ccAudio_Mutex_::leave(void) {};

#endif

using namespace ost;

AudioCodec *AudioCodec::first = NULL;

AudioCodec::AudioCodec(const char *n, Encoding e)
{
	encoding = e;
	name = n;
	next = first;
	first = this;

	info.clear();
	info.format = raw;
	info.encoding = e;
}

AudioCodec::AudioCodec()
{
	name = NULL;

	info.clear();
	info.format = raw;
}

void AudioCodec::endCodec(AudioCodec *codec)
{
	if(!codec->name)
		delete codec;
}

bool AudioCodec::load(Encoding e)
{
	switch(e)
	{
	case mulawAudio:
	case alawAudio:
		return load("g.711");
	case g721ADPCM:
		return load("g.721");
	case g723_2bit:
	case g723_3bit:
	case g723_5bit:
		return load("g.723");
	case mp1Audio:
	case mp2Audio:
	case mp3Audio:
		return load("mpg");
	case gsmVoice:
		return load("gsm");
	case msgsmVoice:
		return load("msgsm");
	case sx73Voice:
	case sx96Voice:
		return load("sx7396");
	default:
		return false;
	}
}

bool AudioCodec::load(const char *name)
{
	char path[256];

#ifdef	RLL_SUFFIX
	char fn[16];
	char *p = fn;
	char *q = fn;
	unsigned len;

	snprintf(fn, sizeof(fn) - 3, "%s", name);
	while(*p)
	{
		if(*p != '.')
			*(q++) = *p;
		++p;
	}
	*q = 0;
	len = (unsigned)strlen(fn);
	snprintf(fn + len, sizeof(fn) - len, RLL_SUFFIX);
	name = fn;

	snprintf(path, sizeof(path), "%s/%s", Audio::getCodecPath(), name);
#else
#ifdef	W32
	snprintf(path, sizeof(path), "%s/%s", Audio::getCodecPath(), name);
#else
	snprintf(path, sizeof(path), "%s/%s.codec", CODEC_LIBPATH, name);
#endif
#endif
	return loadPlugin(path);
}

AudioCodec *AudioCodec::getCodec(Encoding e, const char *format, bool loaded)
{
	AudioCodec *codec;
	lock.enter();
retry:
	codec = first;

	while(codec)
	{
		if(e == codec->encoding)
			break;
		codec = codec->next;
	}

	if(!codec && !loaded)
		if(load(e))
		{
			loaded = true;
			goto retry;
		}

	lock.leave();

	if(codec && format)
		return codec->getByFormat(format);

	return codec;
}

AudioCodec *AudioCodec::getCodec(Info &info, bool loaded)
{
	AudioCodec *codec;
	lock.enter();
retry:
	codec = first;

	while(codec)
	{
		if(info.encoding == codec->encoding)
			break;
		codec = codec->next;
	}

	if(!codec && !loaded)
		if(load(info.encoding))
		{
			loaded = true;
			goto retry;
		}

	lock.leave();

	if(codec)
		return codec->getByInfo(info);

	return codec;
}

bool AudioCodec::isSilent(Level hint, void *data, unsigned samples)
{
	Level power = getImpulse(data, samples);

	if(power < 0)
		return true;

	if(power > hint)
		return false;

	return true;
}

Audio::Level AudioCodec::getImpulse(void *data, unsigned samples)
{
	unsigned long sum = 0;
	Linear ldata = new Sample[samples];
	Linear lptr = ldata;
	long count = decode(ldata, data, samples);

	samples = count;
	while(samples--)
	{
		if(*ldata < 0)
			sum -= *(ldata++);
		else
			sum += *(ldata++);
	}

	delete[] lptr;
	return (Level)(sum / count);
}

Audio::Level AudioCodec::getPeak(void *data, unsigned samples)
{
	Level max = 0, value;
	Linear ldata = new Sample[samples];
	Linear lptr = ldata;
	long count = decode(ldata, data, samples);

	samples = count;
	while(samples--)
	{
		value = *(ldata++);
		if(value < 0)
			value = -value;
		if(value > max)
			max = value;
	}

	delete[] lptr;
	return max;
}

unsigned AudioCodec::getEstimated(void)
{
	return info.framesize;
}

unsigned AudioCodec::getRequired(void)
{
	return info.framecount;
}

unsigned AudioCodec::encodeBuffered(Linear buffer, Encoded source, unsigned samples)
{
	return encode(buffer, source, samples);
}

unsigned AudioCodec::decodeBuffered(Linear buffer, Encoded source, unsigned bytes)
{
	return decode(buffer, source, toSamples(info, bytes));
}

unsigned AudioCodec::getPacket(Encoded packet, Encoded data, unsigned bytes)
{
	if(bytes != info.framesize)
		return 0;

	memcpy(packet, data, bytes);
	return bytes;
}

