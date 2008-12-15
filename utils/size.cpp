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

void Tool::size(char **argv)
{
	char *fn = *(argv++);
	AudioFile file;
	Info info;
	unsigned long pos;

	if(!fn) {
		cerr << "audiotool: --size: no file specified" << endl;
		exit(-1);
	}

	file.open(fn, modeRead);
	if(!file.isOpen()) {
		cerr << "*** " << fn << ": cannot access or invalid" << endl;
		exit(-1);
	}
	file.getInfo(&info);
	file.setPosition();
	pos = file.getPosition();
	pos /= info.rate;
	cout << pos << endl;
	exit(0);
}
