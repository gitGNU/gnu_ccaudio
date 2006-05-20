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

#include "codecs.h"

namespace ccAudioCodec {
using namespace ost;

class g72x
{
protected:
        long yl;        /* Locked or steady state step size multiplier. */
        short yu;       /* Unlocked or non-steady state step size multiplier. */        short dms;      /* Short term energy estimate. */
        short dml;      /* Long term energy estimate. */
        short ap;       /* Linear weighting coefficient of 'yl' and 'yu'. */

        short a[2];     /* Coefficients of pole portion of prediction filter. */
        short b[6];     /* Coefficients of zero portion of prediction filter. */        short pk[2];    /*
                         * Signs of previous two samples of a partially
                         * reconstructed signal.
                         */
        short dq[6];    /*
                         * Previous 6 samples of the quantized difference
                         * signal represented in an internal floating point
                         * format.
                         */
        short sr[2];    /*
                         * Previous 2 samples of the quantized difference
                         * signal represented in an internal floating point
                         * format.
                         */
        char td;        /* delayed tone detect, new in 1988 version */

	g72x();
	
	int predictor_zero();
	int predictor_pole();
	int step_size();
	void update(int code_size, int y, int wi, int fi, int dq,int sr, int dqsex);
};
	
class g723_16 : protected g72x, protected AudioCodec
{
protected:
	friend class g723;

	static short _dqlntab[4];
	static short _witab[4];
	static short _fitab[4];
	static short _qtab[1];

	g723_16();

	Sample decoder(int code);
	unsigned char encoder(Sample sample);
	unsigned encode(Linear buffer, void *dest, unsigned lsamples, bool buffered);
	unsigned decode(Linear buffer, void *source, unsigned lsamples);
};

class g723_24 : protected g72x, protected AudioCodec
{
protected:
	friend class g723;

	g723_24();

        unsigned encode(Linear buffer, void *source, unsigned lsamples, bool buffered );
        unsigned decode(Linear buffer, void *dest, unsigned lsamples);
};

class g723_40 : protected g72x, protected AudioCodec
{
protected:
	friend class g723;

	g723_40();

        unsigned encode(Linear buffer, void *source, unsigned lsamples, bool buffered);
        unsigned decode(Linear buffer, void *dest, unsigned lsamples);
};

static class g723 : public AudioCodec
{
	friend class g723_16;
	friend class g723_24;
	friend class g723_40;

public:
	g723(Encoding encoding);

	unsigned encode(Linear buffer, void *source, unsigned lsamples, bool buffered)
		{return 0;};

	unsigned decode(Linear buffer, void *dest, unsigned lsamples)
		{return 0;};

	AudioCodec *getByFormat(const char *format);

	AudioCodec *getByInfo(Info &info);

} g723_3(Audio::g723_3bit), g723_2(Audio::g723_2bit), g723_5(Audio::g723_5bit);

g723::g723(Encoding encoding) : AudioCodec("g.723", encoding)
{
	info.framesize = getFrame(encoding);
	info.framecount = getCount(encoding);
	info.rate = 8000;
	info.bitrate = info.framesize * 64000l / info.framecount;
	info.annotation = "g.723";
}

AudioCodec *g723::getByInfo(Info &info)
{
	switch(info.encoding)
	{
	case g723_2bit:
		return new g723_16();
	case g723_3bit:
		return new g723_24();
	case g723_5bit:
		return new g723_40();
	default:
		return NULL;
	}
}

AudioCodec *g723::getByFormat(const char *options)
{
	return getByInfo(info);
}

static short power2[15] = {
	1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80,
	0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000};

static int quan(int val, short *table, int size)
{
        int             i;

        for (i = 0; i < size; i++)
                if (val < *table++)
                        break;
        return (i);
}

static int fmult(int an, int srn)
{
        short           anmag, anexp, anmant;
        short           wanexp, wanmant;
        short           retval;

        anmag = (an > 0) ? an : ((-an) & 0x1FFF);
        anexp = quan(anmag, power2, 15) - 6;
        anmant = (anmag == 0) ? 32 :
            (anexp >= 0) ? anmag >> anexp : anmag << -anexp;
        wanexp = anexp + ((srn >> 6) & 0xF) - 13;

        wanmant = (anmant * (srn & 077) + 0x30) >> 4;
        retval = (wanexp >= 0) ? ((wanmant << wanexp) & 0x7FFF) :
            (wanmant >> -wanexp);

        return (((an ^ srn) < 0) ? -retval : retval);
}

static int quantize(int d, int y, short *table, int size)
{
        short           dqm;    /* Magnitude of 'd' */
        short           exp;    /* Integer part of base 2 log of 'd' */
        short           mant;   /* Fractional part of base 2 log */
        short           dl;     /* Log of magnitude of 'd' */
        short           dln;    /* Step size scale factor normalized log */
        int             i;

        /*
         * LOG
         *
         * Compute base 2 log of 'd', and store in 'dl'.
         */
        dqm = abs(d);
        exp = quan(dqm >> 1, power2, 15);
        mant = ((dqm << 7) >> exp) & 0x7F;      /* Fractional portion. */
        dl = (exp << 7) + mant;

        /*
         * SUBTB
         *
         * "Divide" by step size multiplier.
         */
        dln = dl - (y >> 2);

        /*
         * QUAN
         *
         * Obtain codword i for 'd'.
         */
        i = quan(dln, table, size);
        if (d < 0)                      /* take 1's complement of i */
                return ((size << 1) + 1 - i);
        else if (i == 0)                /* take 1's complement of 0 */
                return ((size << 1) + 1); /* new in 1988 */
        else
                return (i);
}

static int reconstruct(int sign, int dqln, int y)
{
        short           dql;    /* Log of 'dq' magnitude */
        short           dex;    /* Integer part of log */
        short           dqt;
        short           dq;     /* Reconstructed difference signal sample */

        dql = dqln + (y >> 2);  /* ADDA */

        if (dql < 0)
                return ((sign) ? -0x8000 : 0);

                /* ANTILOG */
        dex = (dql >> 7) & 15;
        dqt = 128 + (dql & 127);
        dq = (dqt << 7) >> (14 - dex);
        return ((sign) ? (dq - 0x8000) : dq);
}

g72x::g72x()
{
	int i;

	yl = 34816L;
	yu = 544;
	dms = dml = 0;
	ap = 0;
	for(i = 0; i < 6; ++i)
	{
		if(i < 2)
		{
			a[i] = 0;
			pk[i] = 0;
			sr[i] = 0;
		}
		b[i] = 0;
		dq[i] = 0;
	}
	td = 0;
}

int g72x::predictor_zero()
{
	int i, sum = 0;

	for(i = 0; i < 6; ++i)
		sum += fmult(b[i] >> 2, dq[i]);

	return sum;
}

int g72x::predictor_pole()
{
	int i, sum = 0;
	for(i = 0; i < 2; ++i)
		sum += fmult(a[i] >> 2, sr[i]);

	return sum;
}

int g72x::step_size()
{
	int y, dif, al;

	if(ap >= 256)
		return yu;

	y = yl >> 6;
	dif = yu - y;
	al = ap >> 2;
	if(dif > 0)
		y += (dif * al) >> 6;
	else if(dif < 0)
		y += (dif * al + 0x3f) >> 6;
	return y;
}


/*
 * update()
 *
 * updates the state variables for each output code
 */
void g72x::update(int code_size, int y, int wi, int fi, int dq, int sr, int dqsez)
{
        int             cnt;
        short           mag, exp;       /* Adaptive predictor, FLOAT A */
        short           a2p=0;          /* LIMC */
        short           a1ul;           /* UPA1 */
        short           pks1;           /* UPA2 */
        short           fa1;
        char            tr;             /* tone/transition detector */
        short           ylint, thr2, dqthr;
        short           ylfrac, thr1;
        short           pk0;

        pk0 = (dqsez < 0) ? 1 : 0;      /* needed in updating predictor poles */

        mag = dq & 0x7FFF;              /* prediction difference magnitude */
        /* TRANS */
        ylint = g72x::yl >> 15;    /* exponent part of yl */
        ylfrac = (g72x::yl >> 10) & 0x1F;  /* fractional part of yl */
        thr1 = (32 + ylfrac) << ylint;          /* threshold */
        thr2 = (ylint > 9) ? 31 << 10 : thr1;   /* limit thr2 to 31 << 10 */
        dqthr = (thr2 + (thr2 >> 1)) >> 1;      /* dqthr = 0.75 * thr2 */
        if (g72x::td == 0)         /* signal supposed voice */
                tr = 0;
        else if (mag <= dqthr)          /* supposed data, but small mag */
                tr = 0;                 /* treated as voice */
        else                            /* signal is data (modem) */
                tr = 1;

        /*
         * Quantizer scale factor adaptation.
         */

        /* FUNCTW & FILTD & DELAY */
        /* update non-steady state step size multiplier */
        g72x::yu = y + ((wi - y) >> 5);

        /* LIMB */
        if (g72x::yu < 544)        /* 544 <= yu <= 5120 */
                g72x::yu = 544;
        else if (g72x::yu > 5120)
                g72x::yu = 5120;

        /* FILTE & DELAY */
        /* update steady state step size multiplier */
        g72x::yl += g72x::yu + ((-g72x::yl) >> 6);

        /*
         * Adaptive predictor coefficients.
         */
        if (tr == 1) {                  /* reset a's and b's for modem signal */
                g72x::a[0] = 0;
                g72x::a[1] = 0;
                g72x::b[0] = 0;
                g72x::b[1] = 0;
                g72x::b[2] = 0;
                g72x::b[3] = 0;
                g72x::b[4] = 0;
                g72x::b[5] = 0;
        } else {                        /* update a's and b's */
                pks1 = pk0 ^ g72x::pk[0];          /* UPA2 */

                /* update predictor pole a[1] */
                a2p = g72x::a[1] - (g72x::a[1] >> 7);
                if (dqsez != 0) {
                        fa1 = (pks1) ? g72x::a[0] : -g72x::a[0];
                        if (fa1 < -8191)        /* a2p = function of fa1 */
                                a2p -= 0x100;
                        else if (fa1 > 8191)
                                a2p += 0xFF;
                        else
                                a2p += fa1 >> 5;

                        if (pk0 ^ g72x::pk[1])
                        {
                                /* LIMC */
                                if (a2p <= -12160)
                                        a2p = -12288;
                                else if (a2p >= 12416)
                                        a2p = 12288;
                                else
                                        a2p -= 0x80;
                        }
                        else if (a2p <= -12416)
                                a2p = -12288;
                        else if (a2p >= 12160)
                                a2p = 12288;
                        else
                                a2p += 0x80;
                }

                /* Possible bug: a2p not initialized if dqsez == 0) */
                /* TRIGB & DELAY */
                g72x::a[1] = a2p;

                /* UPA1 */
                /* update predictor pole a[0] */
                g72x::a[0] -= g72x::a[0] >> 8;
                if (dqsez != 0)
                {
                        if (pks1 == 0)
                                g72x::a[0] += 192;
                        else
                                g72x::a[0] -= 192;
                }
                /* LIMD */
                a1ul = 15360 - a2p;
                if (g72x::a[0] < -a1ul)
                        g72x::a[0] = -a1ul;
                else if (g72x::a[0] > a1ul)
                        g72x::a[0] = a1ul;

                /* UPB : update predictor zeros b[6] */
                for (cnt = 0; cnt < 6; cnt++) {
                        if (code_size == 5)             /* for 40Kbps G.723 */
                                g72x::b[cnt] -= g72x::b[cnt] >> 9;
                        else                    /* for G.721 and 24Kbps G.723 */
                                g72x::b[cnt] -= g72x::b[cnt] >> 8;
                        if (dq & 0x7FFF) {                      /* XOR */
                                if ((dq ^ g72x::dq[cnt]) >= 0)
                                        g72x::b[cnt] += 128;
                                else
                                        g72x::b[cnt] -= 128;
                        }
                }
        }

        for (cnt = 5; cnt > 0; cnt--)
                g72x::dq[cnt] = g72x::dq[cnt-1];
        /* FLOAT A : convert dq[0] to 4-bit exp, 6-bit mantissa f.p. */
        if (mag == 0) {
                g72x::dq[0] = (dq >= 0) ? 0x20 : 0xFC20;
        } else {
                exp = quan(mag, power2, 15);
                g72x::dq[0] = (dq >= 0) ?
                    (exp << 6) + ((mag << 6) >> exp) :
                    (exp << 6) + ((mag << 6) >> exp) - 0x400;
        }

        g72x::sr[1] = g72x::sr[0];
        /* FLOAT B : convert sr to 4-bit exp., 6-bit mantissa f.p. */
        if (sr == 0) {
                g72x::sr[0] = 0x20;
        } else if (sr > 0) {
                exp = quan(sr, power2, 15);
                g72x::sr[0] = (exp << 6) + ((sr << 6) >> exp);
        } else if (sr > -32768L) {
                mag = -sr;
                exp = quan(mag, power2, 15);
                g72x::sr[0] =  (exp << 6) + ((mag << 6) >> exp) - 0x400;
        } else
                g72x::sr[0] = 0xFC20;

        /* DELAY A */
        g72x::pk[1] = g72x::pk[0];
        g72x::pk[0] = pk0;

        /* TONE */
        if (tr == 1)            /* this sample has been treated as data */
                g72x::td = 0;      /* next one will be treated as voice */
        else if (a2p < -11776)  /* small sample-to-sample correlation */
                g72x::td = 1;      /* signal may be data */
        else                            /* signal is voice */
                g72x::td = 0;

        /*
         * Adaptation speed control.
         */
        g72x::dms += (fi - g72x::dms) >> 5;           /* FILTA */
        g72x::dml += (((fi << 2) - g72x::dml) >> 7);  /* FILTB */

        if (tr == 1)
                g72x::ap = 256;
        else if (y < 1536)                                      /* SUBTC */
                g72x::ap += (0x200 - g72x::ap) >> 4;
        else if (g72x::td == 1)
                g72x::ap += (0x200 - g72x::ap) >> 4;
        else if (abs((g72x::dms << 2) - g72x::dml) >=
            (g72x::dml >> 3))
                g72x::ap += (0x200 - g72x::ap) >> 4;
        else
                g72x::ap += (-g72x::ap) >> 4;
}

short g723_16::_dqlntab[4] = { 116, 365, 365, 116};
short g723_16::_witab[4] = {-704, 14048, 14048, -704};
short g723_16::_fitab[4] = {0, 0xE00, 0xE00, 0};
short g723_16::_qtab[1] = {261};

g723_16::g723_16() : g72x(), AudioCodec()
{
	memcpy(&info, &g723_2.info, sizeof(info));
}

g723_24::g723_24() : g72x(), AudioCodec()
{
        memcpy(&info, &g723_3.info, sizeof(info));
}

g723_40::g723_40() : g72x(), AudioCodec()
{
        memcpy(&info, &g723_5.info, sizeof(info));
}

Audio::Sample g723_16::decoder(int i)
{
        short           sezi, sei, sez, se;     /* ACCUM */
        short           y;                      /* MIX */
        short           sr;                     /* ADDB */
        short           dq;
        short           dqsez;

        i &= 0x03;                      /* mask to get proper bits */
        sezi = predictor_zero();
        sez = sezi >> 1;
        sei = sezi + predictor_pole();
        se = sei >> 1;                  /* se = estimated signal */


        y = step_size();       /* adaptive quantizer step size */
        dq = reconstruct(i & 0x02, _dqlntab[i], y); /* unquantize pred diff */

        sr = (dq < 0) ? (se - (dq & 0x3FFF)) : (se + dq); /* reconst. signal */

        dqsez = sr - se + sez;                  /* pole prediction diff. */

        update(2, y, _witab[i], _fitab[i], dq, sr, dqsez);

	return sr << 2;
}

unsigned char g723_16::encoder(Sample sl)
{
        short           sei, sezi, se, sez;     /* ACCUM */
        short           d;                      /* SUBTA */
        short           y;                      /* MIX */
        short           sr;                     /* ADDB */
        short           dqsez;                  /* ADDC */
        short           dq, i;

	sl >>= 2;

        sezi = predictor_zero();
        sez = sezi >> 1;
        sei = sezi + predictor_pole();
        se = sei >> 1;                  /* se = estimated signal */

        d = sl - se;                    /* d = estimation diff. */

        /* quantize prediction difference d */
        y = step_size();       /* quantizer step size */
        i = quantize(d, y, _qtab, 1);  /* i = ADPCM code */

              /* Since quantize() only produces a three level output
               * (1, 2, or 3), we must create the fourth one on our own
               */
        if (i == 3)                          /* i code for the zero region */
          if ((d & 0x8000) == 0)             /* If d > 0, i=3 isn't right... */
            i = 0;


        dq = reconstruct(i & 2, _dqlntab[i], y); /* quantized diff. */

        sr = (dq < 0) ? se - (dq & 0x3FFF) : se + dq; /* reconstructed signal */
        dqsez = sr + sez - se;          /* pole prediction diff. */

        update(2, y, _witab[i], _fitab[i], dq, sr, dqsez);

	return i;

}

unsigned g723_16::decode(Linear buffer, void *source, unsigned lsamples)
{
	unsigned char code, *dp = (unsigned char *)source;
	unsigned count = 0;

	while(lsamples > 3)
	{
		code = *(dp++);
		*(buffer++) = (code >> 6);
		*(buffer++) = (code >> 4) & 0x03;
		*(buffer++) = (code >> 2) & 0x03;
		*(buffer++) = code & 0x03;
		lsamples -= 4;
		count += 4;
	}
	return count;
}
	
unsigned g723_16::encode(Linear buffer, void *dest, unsigned lsamples, bool buffered)
{
	unsigned char *dp = (unsigned char *)dest, code;
	unsigned count = 0;

	while(lsamples > 3)
	{
		code = encoder(*(buffer++));
		code = code << 2 | encoder(*(buffer++));
		code = code << 2 | encoder(*(buffer++));
		code = code << 2 | encoder(*(buffer++));
		*(dp++) = code;
		count += 4;
		lsamples -= 4;
	}
	return count;
}

}
