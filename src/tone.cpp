// Copyright (C) 2000-2005 Open Source Telecom Corporation.
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

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

using namespace ost;

AudioTone::AudioTone(timeout_t duration, Rate r)
{
	rate = r;
	df1 = df2 = 0;
	samples = (duration *(long)rate) / 1000;
	frame = new Sample[samples];
	silencer = true;

	reset();
}

AudioTone::AudioTone(unsigned freq, Level l, timeout_t duration, Rate r)
{
	rate = r;
    df1 = (freq * M_PI * 2) / (long)rate;
    df2 = (freq * M_PI * 2) / (long)rate;
    p1 = 0, p2 = 0;
    samples = (duration * (long)rate) / 1000;
	m1 = l / 2;
	m2 = l / 2;
	silencer = false;

	frame = new Sample[samples];
}

AudioTone::AudioTone(unsigned f1, unsigned f2, Level l1, Level l2, timeout_t duration, Rate r)
{
	rate = r;
    df1 = (f1 * M_PI * 2) / (long)r;
    df2 = (f2 * M_PI * 2) / (long)r;
    p1 = 0, p2 = 0;
    samples = (duration * (long)r) / 1000;
	m1 = l1 / 2;
	m2 = l2 / 2;
	silencer = false;

	frame = new Sample[samples];
}

unsigned AudioTone::getFrames(Linear buffer, unsigned pages)
{
	unsigned count = 0;
	Linear save = frame;

	while(count < pages)
	{
		frame = buffer;
		buffer += samples;
		if(!getFrame())
			break;
		++count;
	}

	if(count && count < pages)
		memset(buffer, 0, samples * (pages - count) * 2);

	frame = save;
	return count;
}	


bool AudioTone::isComplete(void)
{
	return false;
}

void AudioTone::silence(void)
{
	silencer = true;
}

void AudioTone::reset(void)
{
	m1 = m2 = 0;
	p1 = p2 = 0;
}

void AudioTone::single(unsigned freq, Level l)
{
    df1 = (freq * M_PI * 2) / (long)rate;
    df2 = (freq * M_PI * 2) / (long)rate;
        m1 = l / 2; 
        m2 = l / 2; 
	silencer = false;
}

void AudioTone::dual(unsigned f1, unsigned f2, Level l1, Level l2)
{
    df1 = (f1 * M_PI * 2) / (long)rate;
    df2 = (f2 * M_PI * 2) / (long)rate;
        m1 = l1 / 2;
        m2 = l2 / 2;
	silencer = false;
}

Audio::Linear AudioTone::getFrame(void)
{
	unsigned count = samples;
	Linear data = frame;

	if(isSilent() && !p1 && !p2)
	{
		if(!p1 && !p2)
		{
			memset(frame, 0, samples * 2);
			return frame;
		}
	}
	else if(silencer)
	{
		while(count--)
		{
			if(p1 <= 0 && df1 >= p1)
			{
				p1 = 0;
		 		df1 = 0;
				m1 = 0;
			}
			if(p1 >= 0 && -df1 >= p1)
			{
				p1 = 0;
				df1 = 0;
				m1 = 0;
			}
                        if(p2 <= 0 && df2 >= p1)
                        {
                                p2 = 0;
                                df2 = 0;
				m2 = 0;
                        }
                        if(p2 >= 0 && -df2 >= p1)
                        {
                                p2 = 0;
                                df2 = 0;
				m2 = 0;
                        }

			if(!m1 && !m2)
			{
				*(data++) = 0;
				continue;
			}
		
	                *(data++) = (Level)(sin(p1) * (double)m1) +
        	                (Level)(sin(p2) * (double)m2);

			p1 += df1;
			p2 += df2;
		}
	}
	else
	{
		while(count--)
		{
                	*(data++) = (Level)(sin(p1) * (double)m1) +
                        	(Level)(sin(p2) * (double)m2);

	                p1 += df1;
        	        p2 += df2;
		}
	}

	return frame;
}

AudioTone::~AudioTone()
{
	cleanup();
}

void AudioTone::cleanup(void)
{
	if(frame)
	{
		delete[] frame;
		frame = NULL;
	}
}

bool AudioTone::isSilent(void)
{
	if(!m1 && !m2)
		return true;

	return false;
}


