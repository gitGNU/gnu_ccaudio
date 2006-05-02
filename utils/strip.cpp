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

#include "audiotool.h"

void Tool::strip(char **argv)
{
	AudioFile file, tmp;
	Info info;
	AudioCodec *codec = NULL;
	char *fn;
	timeout_t framing = 20;
	short silence = 0;
	int rtn;
	unsigned char *buffer;
	Level max, current;
	unsigned long sum;
	unsigned long count;
	char target[PATH_MAX];
	
retry:
	if(!*argv)
	{
		cerr << "audiotool: -strip: missing arguments" << endl;
		exit(-1);
	}

	fn = *argv;

	if(!strcmp(fn, "--"))
	{
		++argv;
		goto skip;
	}

	if(!strnicmp(fn, "--", 2))
		++fn;
	if(!strnicmp(fn, "-framing=", 9))
	{
		framing = atoi(fn + 9);
		++argv;
		goto retry;
	}
	else if(!stricmp(fn, "-framing"))
	{
		++argv;
		if(!*argv)
		{
			cerr << "audiotool: -strip: -framing: missing argument" << endl;
			exit(-1);
		}
		framing = atoi(*(argv++));
		goto retry;
	} 

	if(!strnicmp(fn, "-silence=", 9))
	{
		silence = atoi(fn + 9);
		++argv;
		goto retry;
	}
	else if(!stricmp(fn, "-silence"))
	{
		++argv;
		if(!*argv)
		{
			cerr << "audiotool: -strip: -silence: argument missing" << endl;
			exit(-1);
		}
		silence = atoi(*(argv++));
		goto retry;
	}		

skip:

	if(!framing)
		framing = 20;

	while(*argv)
	{
		if(!isFile(*argv))
		{
			cout << *(argv++) << ": invalid" << endl;
			continue;
		}
		if(!canAccess(*argv))
		{
			cout << *(argv++) << ": inaccessable" << endl;
			continue;
		}
		rewrite(*argv, target, PATH_MAX);
		setDelete(target);
		file.open(*argv, modeRead, framing);
		file.getInfo(&info);
		if(!isLinear(info.encoding))
			codec = AudioCodec::getCodec(info, false);
		if(!isLinear(info.encoding) && !codec)
		{
			cout << *(argv++) << ": cannot load codec" << endl;
			continue;
		}

		buffer = new unsigned char[info.framesize];

		max = 0;
		sum = 0;
		count = 0;

		// compute silence value

		while(!silence)
		{
			rtn = file.getBuffer(buffer, info.framesize);
			if(rtn < (int)info.framesize)
				break;
			++count;
			if(codec)
				sum += codec->getImpulse(buffer, info.framecount);
			else
				sum += getImpulse(info, buffer, info.framecount);
		}

		if(!silence && count)
			silence = (Level)(((sum / count) * 2) / 3);

		max = 0;
		sum = 0;
		count = 0;

		file.setPosition(0);

		
		tmp.create(target, &info);
		if(!tmp.isOpen())
		{
			cout << *(argv++) << ": cannot rewrite" << endl;
			continue;
		}

		for(;;)
		{
			rtn = file.getBuffer(buffer, info.framesize);
			if(rtn < (int)info.framesize)
				break;
			++count;
			if(codec)
			{
				if(codec->isSilent(silence, buffer, info.framecount))
				{
					if(codec->getPeak(buffer, info.framecount) >= silence)
						tmp.putBuffer(buffer, info.framesize);
				}
				else
					tmp.putBuffer(buffer, info.framesize);
				continue;
			}

			current = getPeak(info, buffer, info.framecount);
			if(current > max)
				max = current;

			sum += getImpulse(info, buffer, info.framecount);
			if(getImpulse(info, buffer, info.framecount) < silence)
			{
				if(getPeak(info, buffer, info.framecount) >= silence)
					tmp.putBuffer(buffer, info.framecount);
			}
			else
				tmp.putBuffer(buffer, info.framecount);
		}
		if(buffer)
			delete[] buffer;

		if(codec)
			AudioCodec::endCodec(codec);

		codec = NULL;
		buffer = NULL;
		
		file.close();
		tmp.close();

		rtn = rename(target, *argv);
		remove(target);
		setDelete(NULL);

		if(rtn)
			cout << *argv << ": could not be replaced" << endl;

		++argv;
	}
	exit(0);
}
