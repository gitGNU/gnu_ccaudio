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

extern int _oss_ccaudio_dummy;
int _oss_ccaudio_dummy = 0;

#ifdef	HAVE_SYS_SOUNDCARD_H

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/soundcard.h>

using namespace ost;

class OSSAudioDevice : public AudioDevice
{
private:
	int dsp, mixer, channels;
	int volspeaker, volpcm, volsynth, volmic, volline;
	Linear sendbuf;
	unsigned sendpos, bufsize;

public:
	OSSAudioDevice(int fdsp, int fmixer, DeviceMode mode);
	~OSSAudioDevice();

	bool setAudio(Rate rate, bool stereo, timeout_t timeout);

	unsigned putSamples(Linear buffer, unsigned count);
	unsigned getSamples(Linear buffer, unsigned count);

	void flush(void);
	void resetPlay(void);
	void mutePlay(void);
	void resetRecord(void);
	void enableRecord(void);
	void disableRecord(void);
};

OSSAudioDevice::OSSAudioDevice(int fdsp, int fmixer, DeviceMode mode)
{
	long flags = fcntl(fdsp, F_GETFL) & ~O_NDELAY;

	dsp = fdsp;
	mixer = fmixer;

	fcntl(dsp, F_SETFL, flags);

	ioctl(mixer, MIXER_READ(SOUND_MIXER_SPEAKER), &volspeaker);
        ioctl(mixer, MIXER_READ(SOUND_MIXER_PCM), &volpcm);
        ioctl(mixer, MIXER_READ(SOUND_MIXER_MIC), &volmic);
        ioctl(mixer, MIXER_READ(SOUND_MIXER_SYNTH), &volsynth);
        ioctl(mixer, MIXER_READ(SOUND_MIXER_VOLUME), &volline);

	switch(mode)
	{
	case PLAY:
		disableRecord();
		break;
	case RECORD:
		mutePlay();
		enableRecord();
		break;
	case PLAYREC:
		enableRecord();
		break;
	}
	enabled = false;
	sendbuf = NULL;
	sendpos = 0;
}

OSSAudioDevice::~OSSAudioDevice()
{
	long flags;

	flags = fcntl(dsp, F_GETFL) | O_NDELAY;
	fcntl(dsp, F_SETFL, flags);

	resetPlay();
	resetRecord();

	::close(mixer);
	::close(dsp);

	if(sendbuf)
		delete[] sendbuf;
}

void OSSAudioDevice::flush(void)
{
	unsigned pos;

	if(sendpos && sendbuf)
	{
		for(pos = sendpos; pos < bufsize / 2; ++pos)
			sendbuf[pos] = 0;
		if(::write(dsp, sendbuf, bufsize) < (ssize_t)(bufsize))
			abort();
	}

	sendpos = 0;
}

bool OSSAudioDevice::setAudio(Rate rate, bool stereo, timeout_t framing)
{
	int srate = (int)rate;
	int div = 1;
	int blksize;

	if(stereo)
	{
		channels = 2;
		info.encoding = pcm16Stereo;
	}
	else
	{
		info.encoding = pcm16Mono;
		channels = 1;
	}

	info.rate = srate;
	info.bitrate = srate * 16 * channels;
	info.order = __BYTE_ORDER;
	info.format = raw;
	info.annotation = "OSS Soundcard";

	flush();

#if __BYTE_ORDER == __LITTLE_ENDIAN
	int codec = AFMT_S16_LE;
#else
	int codec = AFMT_S16_BE;
#endif

	enabled = false;

	ioctl(dsp, SOUND_PCM_RESET, NULL);
	ioctl(dsp, SOUND_PCM_SYNC, NULL);
	ioctl(dsp, SOUND_PCM_SUBDIVIDE, &div);
	if(ioctl(dsp, SOUND_PCM_SETFMT, &codec))
		return false;

	if(ioctl(dsp, SOUND_PCM_WRITE_RATE, &srate))
		return false;

	if(ioctl(dsp, SOUND_PCM_WRITE_CHANNELS, &channels))
		return false;

	ioctl(dsp, SOUND_PCM_READ_RATE, &srate);
	ioctl(dsp, SOUND_PCM_READ_CHANNELS, &channels);
	ioctl(dsp, SNDCTL_DSP_GETBLKSIZE, &blksize);
	info.framesize = blksize;	
	info.framecount = toSamples(info.encoding, blksize);
	info.framing = (info.framecount * 1000l) / srate;

	bufsize = info.framecount * channels;

	if(sendbuf)
		delete[] sendbuf;

	sendbuf = new Sample[bufsize];
	sendpos = 0;

	enabled = true;
	return true;
}
	
void OSSAudioDevice::resetPlay(void)
{
	ioctl(mixer, MIXER_WRITE(SOUND_MIXER_PCM), &volpcm);
        ioctl(mixer, MIXER_WRITE(SOUND_MIXER_SPEAKER), &volspeaker);
        ioctl(mixer, MIXER_WRITE(SOUND_MIXER_SYNTH), &volsynth);
        ioctl(mixer, MIXER_WRITE(SOUND_MIXER_VOLUME), &volline);
}

void OSSAudioDevice::mutePlay(void)
{
	int zero = 0;

        ioctl(mixer, MIXER_WRITE(SOUND_MIXER_PCM), &zero);
        ioctl(mixer, MIXER_WRITE(SOUND_MIXER_SPEAKER), &zero);
        ioctl(mixer, MIXER_WRITE(SOUND_MIXER_SYNTH), &zero);
        ioctl(mixer, MIXER_WRITE(SOUND_MIXER_VOLUME), &zero);
}

void OSSAudioDevice::resetRecord(void)
{
	ioctl(mixer, MIXER_WRITE(SOUND_MIXER_MIC), &volmic); 
}

void OSSAudioDevice::enableRecord(void)
{
	int level = 0x5c5c;

        ioctl(mixer, MIXER_WRITE(SOUND_MIXER_MIC), &level);
}

void OSSAudioDevice::disableRecord(void)
{
	int zero = 0;

	ioctl(mixer, MIXER_WRITE(SOUND_MIXER_MIC), &zero);
}

unsigned OSSAudioDevice::getSamples(Linear samples, unsigned count)
{
	size_t bytes = (count * 2 * channels);
	ssize_t result;

	if(!enabled)
		return 0;

	result = ::read(dsp, samples, bytes);

	if(result < 0)
		return 0;

	return (result / 2 / channels);
}

unsigned OSSAudioDevice::putSamples(Linear samples, unsigned count)
{
	unsigned total = 0, fill = 0;

	if(!enabled)
		return 0;

	count *= channels;

	if(sendpos)
	{
		fill = bufsize - sendpos;
		if(fill > count)
			fill = count;

		memcpy(&sendbuf[sendpos], samples, fill * 2);
		count -= fill;
		samples += fill;
		sendpos += fill;
	}
 
	if(sendpos == bufsize)
	{
		sendpos = 0;
		if(::write(dsp, sendbuf, bufsize * 2) < (ssize_t)(bufsize * 2))
			return 0;
		total += info.framecount;
	}
	else if(sendpos)
		return fill / channels;
	
	fill = count / bufsize;
	
	if(fill)
	{
		fill *= bufsize;
		if(::write(dsp, samples, fill * 2) < (ssize_t)(fill * 2))
			return 0;

		total += (fill / channels);
	}
	
	if(fill)
	{
		count -= fill;
		samples += fill;
	}
	
	if(count)
	{
		memcpy(sendbuf, samples, count * 2);
		sendpos = count;
		total += (count / channels);
	}

	return total;
}	

bool Audio::hasDevice(unsigned index)
{
	char fname[33];

	if(index)
		snprintf(fname, sizeof(fname), "/dev/dsp%d", index);
	else
		snprintf(fname, sizeof(fname), "/dev/dsp");

	if(access(fname, W_OK))
		return true;
	return false;
}

AudioDevice *Audio::getDevice(unsigned index, DeviceMode mode)
{
	int dsp, mixer;
	char fname[33];
	AudioDevice *dev;

	if(index)
		snprintf(fname, sizeof(fname), "/dev/dsp%d", index);
	else
		snprintf(fname, sizeof(fname), "/dev/dsp");

	dsp = ::open(fname, O_RDWR | O_NDELAY);
	if(dsp < 0)
		return NULL;

	if(index)
		snprintf(fname, sizeof(fname), "/dev/mixer%d", index);
	else
		snprintf(fname, sizeof(fname), "/dev/mixer");

	mixer = ::open(fname, O_RDWR | O_NONBLOCK);
	if(mixer < 0)
	{
		::close(dsp);
		return NULL;
	}

	dev = new OSSAudioDevice(dsp, mixer, mode);
	if(!dev->setAudio(rate8khz, false))
		dev->setAudio(rate44khz, true);
	return dev;
}

#endif
