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
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release
// of ccaudio.
//
// The exception is that, if you link the ccaudio library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the ccaudio library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
// 
// This exception applies only to the code released under the 
// name ccaudio.  If you copy code from other releases into a copy of
// ccaudio, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for ccaudio, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.  

#define __EXPORT __declspec(dllexport)

#define	HAVE_MODULES	1
#define	HAVE_ALLOCA_H	1

#define	_WIN32_WINNT	0x0400

#include <windows.h>
#include <malloc.h>
#include <cstring>

#define	snprintf	_snprintf
#define	vsnprintf	_vsnprintf

#define	HAVE_SNPRINTF

#if	defined(_M_PPC) || defined(_M_MPPC)
#define	__BYTE_ORDER	4321
#endif

#ifndef	__BYTE_ORDER	// may be overriden in .dsp target
#define	__BYTE_ORDER	1234
#endif

#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321

class ccAudio_Mutex_
{
private:
	CRITICAL_SECTION mutex;

public:
	ccAudio_Mutex_();
	~ccAudio_Mutex_();
	void enter(void);
	void leave(void);
};

#include "audio2.h"
