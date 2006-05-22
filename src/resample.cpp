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
#include <cstdio>

using namespace ost;

AudioResample::AudioResample(Rate div, Rate mul)
{
	bool common = true;
	while(common)
	{
		common = false;

		while(!(mul & 0x01) && !(div & 0x01))
		{
			mul = (Rate)(mul >> 1);
			div = (Rate)(div >> 1);
			common = true;
		}

		while(!(mul % 3) && !(div % 3))
		{
			mul = (Rate)(mul / 3);
			div = (Rate)(div / 3);
			common = true;
		}

		while(!(mul % 5) && !(div %5)) 	
		{
			mul = (Rate)(mul / 5);
			div = (Rate)(div / 5);
			common = true;
		}
	}
		

	mfact = (unsigned)mul;
	dfact = (unsigned)div;

	max = mfact;
	if(dfact > mfact)
		max = dfact;

	++max;
	buffer = new Sample[max];
	ppos = gpos = 0;
	memset(buffer, 0, max * 2);
	last = 0;
}

AudioResample::~AudioResample()
{
	delete[] buffer;
};

size_t AudioResample::estimate(size_t count)
{
	count *= mfact;
	count += (mfact - 1);
	return (count / dfact) + 1;
}

size_t AudioResample::process(Linear from, Linear dest, size_t count)
{
	size_t saved = 0;
	Sample diff, current;
	unsigned pos;
	unsigned dpos;

	while(count--)
	{
		current = *(from++);
		diff = (current - last) / mfact;
		pos = mfact;
		while(pos--)
		{
			last += diff;
			buffer[ppos++] = current;			
			if(ppos >= max)
				ppos = 0;

			if(gpos < ppos)
				dpos = ppos - gpos;
			else
				dpos = max - (gpos - ppos);
			if(dpos >= dfact)
			{
				*(dest++) = buffer[gpos];
				++saved;
				gpos += dfact;
				if(gpos >= max)
					gpos = gpos - max;
			}
		}
		last = current;
	}
	return saved;
}
