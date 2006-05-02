// Copyright (C) 2005 David Sugar, Tycho Softworks.
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

using namespace ost;

DTMFTones::DTMFTones(const char *d, Level l, timeout_t duration, timeout_t timer) :
AudioTone(duration)
{
	dtmfframes = (timer / duration);
	frametime = duration;
	level = l;

	if(timer % duration)
		++dtmfframes;

	digits = d;
	remaining = 0;
	reset();

	complete = true;
	if(digits && *digits)
		complete = false;
}

DTMFTones::~DTMFTones()
{
	AudioTone::cleanup();
}

bool DTMFTones::isComplete(void)
{
	return complete;
}

Audio::Linear DTMFTones::getFrame(void)
{
	for(;;)
	{
		if(remaining)
		{
			--remaining;
			return AudioTone::getFrame();
		}

		if(!digits || !*digits)
		{
			complete = true;
			return NULL;
		}

		if(!isSilent())
		{
			remaining = dtmfframes;
			reset();
			continue;
		}

		switch(*(digits++))
		{
		case '!':
		case 'f':
		case 'F':
			return NULL;

		case '1':
			dual(697, 1209, level, level);
			remaining = dtmfframes;
			break;

                case '2':
                        dual(697, 1336, level, level);
                        remaining = dtmfframes;
                        break;

                case '3':      
                        dual(697, 1477, level, level);
                        remaining = dtmfframes;
                        break;  

		case 'A':
                case 'a':
                        dual(697, 1633, level, level);
                        remaining = dtmfframes;
                        break;   

		case '4':
			dual(770, 1209, level, level);
			remaining = dtmfframes;
			break;

                case '5':
                        dual(770, 1336, level, level);
                        remaining = dtmfframes;
                        break;

                case '6':      
                        dual(770, 1477, level, level);
                        remaining = dtmfframes;
                        break;  

		case 'B':
                case 'b':
                        dual(770, 1633, level, level);
                        remaining = dtmfframes;
                        break;   

		case '7':
			dual(852, 1209, level, level);
			remaining = dtmfframes;
			break;

                case '8':
                        dual(852, 1336, level, level);
                        remaining = dtmfframes;
                        break;

                case '9':      
                        dual(852, 1477, level, level);
                        remaining = dtmfframes;
                        break;  

		case 'C':
                case 'c':
                        dual(852, 1633, level, level);
                        remaining = dtmfframes;
                        break;   

		case '*':
			dual(941, 1209, level, level);
			remaining = dtmfframes;
			break;

                case '0':
                        dual(941, 1336, level, level);
                        remaining = dtmfframes;
                        break;

                case '#':      
                        dual(941, 1477, level, level);
                        remaining = dtmfframes;
                        break;  

		case 'D':
                case 'd':
                        dual(941, 1633, level, level);
                        remaining = dtmfframes;
                        break;   

		case 's':
		case 'S':
		case ',':
			remaining = 1000 / frametime;
			reset();
			break;
		case '.':
			remaining = dtmfframes * 2;
			reset();
			break;
		}
	}	
}

MFTones::MFTones(const char *d, Level l, timeout_t duration, timeout_t timer) :
AudioTone(duration)
{
	mfframes = (timer / duration);
	frametime = duration;
	level = l;
	kflag = false;

	if(timer % duration)
		++mfframes;

	digits = d;
	remaining = 0;
	reset();

	complete = true;
	if(digits && *digits)
		complete = false;
}

MFTones::~MFTones()
{
	AudioTone::cleanup();
}

bool MFTones::isComplete(void)
{
	return complete;
}

Audio::Linear MFTones::getFrame(void)
{
	for(;;)
	{
		if(remaining)
		{
			--remaining;
			return AudioTone::getFrame();
		}

		if(!digits || !*digits)
		{
			complete = true;
			return NULL;
		}

		if(!isSilent())
		{
			if(kflag)
				remaining = 100 / frametime;
			else
				remaining = mfframes;
			kflag = false;
			reset();
			continue;
		}

		switch(*(digits++))
		{
		case '!':
		case 'f':
		case 'F':
			return NULL;

		case '1':
			dual(700, 900, level, level);
			remaining = mfframes;
			break;

                case '2':
                        dual(700, 1100, level, level);
                        remaining = mfframes;
                        break;

                case '3':      
                        dual(900, 1100, level, level);
                        remaining = mfframes;
                        break;  

		case '4':
			dual(700, 1300, level, level);
			remaining = mfframes;
			break;

                case '5':
                        dual(900, 1300, level, level);
                        remaining = mfframes;
                        break;

                case '6':      
                        dual(1100, 1300, level, level);
                        remaining = mfframes;
                        break;  

		case '7':
			dual(700, 1500, level, level);
			remaining = mfframes;
			break;

                case '8':
                        dual(900, 1500, level, level);
                        remaining = mfframes;
                        break;

                case '9':      
                        dual(1100, 1500, level, level);
                        remaining = mfframes;
                        break;  

		case 'K':
		case 'k':
		case '#':
			dual(1100, 1700, level, level);
			remaining = 100 / frametime;
			kflag = true;
			break;

                case '0':
                        dual(1300, 1500, level, level);
                        remaining = mfframes;
                        break;

		case 'S':
		case 's':
                case '*':      
                        dual(1500, 1700, level, level);
                        remaining = mfframes;
                        break;  

		case 'B':
                case 'b':
			single(2600, level);
			kflag = true;
			remaining = 1000 / frametime;
                        break;   

		case ',':
			remaining = 1000 / frametime;
			reset();
			break;
		case '.':
			remaining = mfframes * 2;
			reset();
			break;
		}
	}	
}
