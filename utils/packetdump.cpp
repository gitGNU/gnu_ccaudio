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

class PacketStream : public AudioStream
{
private:
	char **list;
	char *getContinuation(void);

public:
	PacketStream();
	void open(char **argv);
};

PacketStream::PacketStream() : AudioStream()
{
	list = NULL;
}

void PacketStream::open(char **argv)
{
	AudioStream::open(*(argv++), modeRead, 10);
	if(isOpen())
		list = argv;
}

char *PacketStream::getContinuation(void)
{
	if(!list)
		return NULL;

	return *(list++);
}

void Tool::packetdump(char **argv)
{
	PacketStream packetfile;
	const char *path = *argv;
	Encoded buffer;
	Info info;
	ssize_t count;
	
	packetfile.open(argv);

	if(!packetfile.isOpen())
	{
		cerr << "audiotool: " << path << ": unable to access" << endl;
		exit(-1);
	}

	if(!packetfile.isStreamable())
	{
		cerr << "audiotool: " << path << ": missing needed codec" << endl;
		exit(-1);
	}

	packetfile.getInfo(&info);

	buffer = new unsigned char[maxFramesize(info)];

	while((count = packetfile.getPacket(buffer)) > 0)
		cout << "-- " << count << endl;			

	delete[] buffer;
	packetfile.close();		
	exit(0);
}

