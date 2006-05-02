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

using namespace ost;

AudioBase::AudioBase()
{
	memset(&info, 0, sizeof(info));
}

AudioBase::AudioBase(Info *i)
{
	memcpy(&info, i, sizeof(info));
}

AudioBase::~AudioBase()
{
}

ssize_t AudioBase::putNative(Encoded data, size_t bytes)	
{
	swapEncoded(info, data, bytes);
	return putBuffer(data, bytes);
}

ssize_t AudioBase::getNative(Encoded data, size_t bytes)
{
	ssize_t result = getBuffer(data, bytes);

	if(result < 1)
		return result;

	swapEncoded(info, data, result);
	return result;
}
	

