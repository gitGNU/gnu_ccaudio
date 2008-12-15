// Copyright (C) 2006 David Sugar, Tycho Softworks
// Copyright (C) 2006-2008 David Sugar, Tycho Softworks.
//
// This file is part of GNU ccAudio2.
//
// GNU ccAudio2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GNU ccAuydio2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GNU ccAudio2.  If not, see <http://www.gnu.org/licenses/>.

#include "private.h"
#include "audio2.h"

using namespace ost;

AudioPacket::AudioPacket(Info *i, Encoded d)
{
	memcpy(&info, i, sizeof(info));
	if(d) {
		data = new unsigned char [info.framesize];
		memcpy(data, d, info.framesize);
	}
	else
		data = NULL;
}

AudioPacket::~AudioPacket()
{
	if(data) {
		delete[] data;
		data = NULL;
	}
}

bool AudioPacket::isEmpty(void)
{
	if(data)
		return true;

	return false;
}

void AudioPacket::setData(Encoded d, unsigned size)
{
	if(data) {
		delete[] data;
		data = NULL;
	}

	if(!size)
		size = info.framesize;
	else
		info.framesize = size;

	if(d) {
		data = new unsigned char[size];
		memcpy(data, d, size);
	}
}

