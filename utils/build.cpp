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

class AudioBuild : public AudioStream
{
private:
	char **list;
	char *getContinuation(void);
public:
	AudioBuild();
	void open(char **argv);

	static void copyDirect(AudioStream &input, AudioStream &output);
	static void copyConvert(AudioStream &input, AudioStream &output);
};

AudioBuild::AudioBuild() : AudioStream()
{
	list = NULL;
}

void AudioBuild::open(char **argv)
{
	AudioStream::open(*(argv++), modeRead, 10);
	if(isOpen())
		list = argv;
}

char *AudioBuild::getContinuation(void)
{
	if(!list)
		return NULL;

	return *(list++);
}

void AudioBuild::copyConvert(AudioStream &input, AudioStream &output)
{
	unsigned long samples;
	Linear buffer;
	unsigned pages, npages;
	Info from, to;
	bool mono = true;
	
	input.getInfo(&from);
	output.getInfo(&to);

	if(isStereo(from.encoding) || isStereo(to.encoding))
		mono = false;

	samples = input.getCount();


	if(mono)
		buffer = new Sample[samples];
	else
		buffer = new Sample[samples * 2];

	for(;;)
	{
		if(mono)
			pages = input.getMono(buffer, 1);
		else
			pages = input.getStereo(buffer, 1);

		if(!pages)
			break;

		if(mono)
			npages = output.bufMono(buffer, samples);
		else
			npages = output.bufStereo(buffer, samples);

		// if(!npages)
		//	break;
	}

	delete[] buffer;
}


void AudioBuild::copyDirect(AudioStream &input, AudioStream &output)
{
	unsigned bufsize;
	Encoded buffer;
	Info from, to;
	bool endian = false;
	ssize_t status = 1;

	input.getInfo(&from);
	output.getInfo(&to);

        if(to.order && from.order && to.order != from.order && isLinear(from.encoding))
                endian = true;
	
        bufsize = from.framesize;
        buffer = new unsigned char[bufsize];

	while(status > 0)
        {
		status = input.getNative(buffer, bufsize);
		if(status < 1)
			break;

                status = output.putNative(buffer, status);
        }

	delete[] buffer;
}	
	
void Tool::build(char **argv) 
{
	AudioBuild input;
	AudioStream output;
	Info info, make;
	const char *target;
	char *option;
	char *encoding = NULL;

retry:
	if(!*argv)
	{
		cerr << "audiotool: -build: missing arguments" << endl;
		exit(-1);
	}

	option = *argv;
	if(!strcmp("--", option))
	{
		++argv;
		goto skip;
	}

	if(!strnicmp("--", option, 2))
		++option;

	if(!strnicmp(option, "-encoding=", 10))
	{
		encoding = option + 10;
		++argv;
		goto retry;
	}

	if(!stricmp(option, "-encoding"))
	{
		++argv;
		if(!*argv)
		{
			cerr << "audiotool: -build: -encoding: missing argument" << endl;
			exit(-1);
		}
		encoding = *(argv++);
		goto retry;
	}

skip:
      	if(*argv && **argv == '-')
      	{
        	cerr << "audiotool: -build: " << *argv << ": unkown option" << endl;
                exit(-1);
        }

	if(*argv)
		target = *(argv++);

	if(!*argv)
	{
		cerr << "audiotool: no files to build from" << endl;
		exit(-1);
	}

	input.open(argv);
	if(!input.isOpen())
	{
		cerr << "audiotool: " << *argv << ": cannot access" << endl;
		exit(-1);
	}
	input.getInfo(&info);
	input.getInfo(&make);
	remove(target);
	if(encoding)
		make.encoding = getEncoding(encoding);

	output.create(target, &make, false, 10);
	if(!output.isOpen())
	{
		cerr << "audiotool: " << target << ": cannot create" << endl;
 		exit(-1);
	}
	output.getInfo(&make);

	if(make.encoding == info.encoding)
		AudioBuild::copyDirect(input, output);
	else
	{
		if(!input.isStreamable())
		{
			remove(target);
			cerr << "audiotool: " << *argv << ": cannot load codec" << endl;
			exit(-1);
		}
		if(!output.isStreamable())
		{
			remove(target);
			cerr << "audiotool: " << target << ": cannot load codec" << endl;
			exit(-1);
		}
		AudioBuild::copyConvert(input, output);
	}
	input.close();
	output.close();
	exit(0);
}
	
void Tool::append(char **argv) 
{
	AudioBuild input;
	AudioStream output;
	Info info, make;
	const char *target;
	char *option;
	char *offset = NULL;

retry:
	option = *argv;

	if(!strcmp("--", option))
	{
		++argv;
		goto skip;
	}

	if(!strnicmp("--", option, 2))
		++option;

	if(!strnicmp(option, "-offset=", 8))
	{
		offset = option + 8;
		++argv;
		goto retry;
	}

	if(!stricmp(option, "-offset"))
	{
		++argv;
		if(!*argv)
		{
			cerr << "audiotool: -append: -offset: argument missing" << endl;
			exit(-1);
		}
		offset = *(argv++);
		goto retry;
	}


skip:
        if(*argv && **argv == '-')
        {
		cerr << "audiotool: -append: " << *argv << ": unknown option" << endl;
                exit(-1);
        }

	if(*argv)
		target = *(argv++);

	if(!*argv)
	{
		cerr << "audiotool: no files to append from" << endl;
		exit(-1);
	}

	input.open(argv);
	if(!input.isOpen())
	{
		cerr << "audiotool: " << *argv << ": cannot access" << endl;
		exit(-1);
	}
	input.getInfo(&info);
	output.open(target, modeWrite, 10);
	if(!output.isOpen())
	{
		cerr << "audiotool: " << target << ": cannot access" << endl;
 		exit(-1);
	}
	output.getInfo(&make);

	if(offset)
		output.setPosition(atol(offset));
	else
		output.setPosition();

	if(make.encoding == info.encoding)
		AudioBuild::copyDirect(input, output);
	else
	{
		if(!input.isStreamable())
		{
			cerr << "audiotool: " << *argv << ": cannot load codec" << endl;
			exit(-1);
		}
		if(!output.isStreamable())
		{
			cerr << "audiotool: " << target << ": cannot load codec" << endl;
			exit(-1);
		}
		AudioBuild::copyConvert(input, output);
	}
	input.close();
	output.close();
	exit(0);
}

