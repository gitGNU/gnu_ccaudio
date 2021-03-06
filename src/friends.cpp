// Copyright (C) 1999-2005 Open Source Telecom Corporation.
// Copyright (C) 2006-2008 David Sugar, Tycho Softworks.
//
// This file is part of GNU ccAudio2.
//
// GNU ccAudio2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GNU ccAudio2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GNU ccAudio2.  If not, see <http://www.gnu.org/licenses/>.

#include <ucommon/ucommon.h>
#include <config.h>
#include <math.h>
#include <ctype.h>
#ifdef  HAVE_ENDIAN_H
#include <endian.h>
#endif
#include <ucommon/export.h>
#include <ccaudio2.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#if !defined(__BIG_ENDIAN)
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412
#define __BYTE_ORDER    __LITTLE_ENDIAN
#endif

using namespace UCOMMON_NAMESPACE;

#ifdef  _MSWINDOWS_
static const char *plugins = DEFAULT_LIBPATH;
#else
static const char *plugins = DEFAULT_LIBPATH "/ccaudio";
#endif

#if _MSC_VER > 1400        // windows broken dll linkage issue...
#else
const unsigned Audio::ndata = (unsigned)(-1);
#endif

const char *Audio::getPluginPath(void)
{
    return plugins;
}

Audio::Level Audio::tolevel(float dbm)
{
    double l = pow(10.0, (dbm - M_PI)/20.0)*(32768.0*0.70711);
    return (Level)(l*l);
}

float Audio::todbm(Level l)
{
    return (float)(log10(sqrt((float)l)/(32768.0*0.70711))*20.0 + M_PI);
}

const char *Audio::getExtension(Encoding encoding)
{
    switch(encoding) {
    case cdaStereo:
    case pcm16Stereo:
    case pcm32Stereo:
    case cdaMono:
    case pcm16Mono:
    case pcm32Mono:
        return ".wav";
    case alawAudio:
        return ".al";
    case gsmVoice:
        return ".gsm";
    case msgsmVoice:
        return ".wav";
    case mp1Audio:
        return ".mp1";
    case mp2Audio:
        return ".mp2";
    case mp3Audio:
        return ".mp3";
    case voxADPCM:
        return ".vox";
    case speexVoice:
    case speexAudio:
        return ".spx";
    case sx73Voice:
    case sx96Voice:
        return ".sx";
    case g721ADPCM:
        return ".adpcm";
    case g723_2bit:
        return ".a16";
    case g723_3bit:
        return ".a24";
    case g723_5bit:
        return ".a40";
    case g729Audio:
        return ".g729";
    case ilbcAudio:
        return ".ilbc";
    default:
        return ".au";
    }
}

Audio::Encoding Audio::getMono(Encoding encoding)
{
    switch(encoding) {
    case cdaStereo:
        return cdaMono;
    case pcm8Stereo:
        return pcm8Mono;
    case pcm16Stereo:
        return pcm16Mono;
    case pcm32Stereo:
        return pcm32Mono;
    default:
        return encoding;
    }
}

Audio::Encoding Audio::getStereo(Encoding encoding)
{
    switch(encoding) {
    case cdaStereo:
    case pcm8Stereo:
    case pcm16Stereo:
    case pcm32Stereo:
        return encoding;
    case cdaMono:
        return cdaStereo;
    case pcm8Mono:
        return pcm8Stereo;
    case pcm16Mono:
        return pcm16Stereo;
    case pcm32Mono:
        return pcm32Stereo;
    default:
        return unknownEncoding;
    }
}

Audio::Encoding Audio::getEncoding(const char *name)
{
    if(!stricmp(name, "ulaw") || !stricmp(name, "mulaw") || !stricmp(name, "pcmu"))
        return mulawAudio;
    else if(!stricmp(name, "alaw") || !stricmp(name, "pcma"))
        return alawAudio;
    else if(!stricmp(name, "linear") || !stricmp(name, "pcm16") || !stricmp(name, "pcm") || !stricmp(name, "l16"))
        return pcm16Mono;
    else if(!stricmp(name, "stereo"))
        return pcm16Stereo;
    else if(!stricmp(name, "cda"))
        return cdaStereo;
    else if(!stricmp(name, "gsm"))
        return gsmVoice;
    else if(!stricmp(name, "msgsm"))
        return msgsmVoice;
    else if(!stricmp(name, "pcm8") || !stricmp(name, "l8"))
        return pcm8Mono;
    else if(!stricmp(name, "pcm32"))
        return pcm32Mono;
    else if(!stricmp(name, "adpcm"))
        return g721ADPCM;
    else if(!stricmp(name, "g721") || !stricmp(name, "g.721"))
        return g721ADPCM;
    else if(!stricmp(name, "g726-32"))
        return g721ADPCM;
    else if(!stricmp(name, "g729") || !stricmp(name, "g.729"))
        return g729Audio;
    else if(!stricmp(name, "ilbc"))
        return ilbcAudio;
    else if(!stricmp(name, "mp1"))
        return mp1Audio;
    else if(!stricmp(name, "mp2"))
        return mp2Audio;
    else if(!stricmp(name, "mp3"))
        return mp3Audio;
    else if(!stricmp(name, "oki"))
        return okiADPCM;
    else if(!stricmp(name, "vox"))
        return voxADPCM;
    else if(!stricmp(name, "sx73"))
        return sx73Voice;
    else if(!stricmp(name, "sx96"))
        return sx96Voice;
    else if(!stricmp(name, "spx") || !stricmp(name, "speex"))
        return speexVoice;
    else if(!stricmp(name, "g723-16") || !stricmp(name, "g.723-16"))
        return g723_2bit;
    else if(!stricmp(name, "g723-24") || !stricmp(name, "g.723-24"))
        return g723_3bit;
    else if(!stricmp(name, "g723-40") || !stricmp(name, "g.723-40"))
        return g723_5bit;
    else if(!stricmp(name, ".al") || !stricmp(name, ".alaw"))
        return alawAudio;
    else if(!stricmp(name, ".ul") || !stricmp(name, ".ulaw") || !stricmp(name, ".mulaw"))
        return mulawAudio;
    else if(!stricmp(name, ".sw") || !stricmp(name, ".raw") || !stricmp(name, ".pcm"))
        return pcm16Mono;
    else if(!stricmp(name, ".vox") || !stricmp(name, "vox"))
        return voxADPCM;
    else if(!stricmp(name, ".adpcm"))
        return g721ADPCM;
    else if(!stricmp(name, ".g721"))
        return g721ADPCM;
    else if(!stricmp(name, ".a32") || !stricmp(name, "a32"))
        return g721ADPCM;
    else if(!stricmp(name, ".a24") || !stricmp(name, "a24"))
        return g723_3bit;
    else if(!stricmp(name, ".a16") || !stricmp(name, "a16"))
        return g723_2bit;
    else if(!stricmp(name, ".a40") || !stricmp(name, "a40"))
        return g723_5bit;
    else if(!stricmp(name, ".g723"))
        return g723_3bit;
    else if(!stricmp(name, ".g729"))
        return g729Audio;
    else if(!stricmp(name, ".ilbc"))
        return ilbcAudio;
    else if(!stricmp(name, ".a24"))
        return g723_3bit;
    else if(!stricmp(name, ".a40"))
        return g723_5bit;
    else if(!stricmp(name, ".cda"))
        return cdaStereo;
    else if(!stricmp(name, ".sx"))
        return sx96Voice;
    else if(!stricmp(name, ".gsm"))
        return gsmVoice;
    else if(!stricmp(name, ".mp1"))
        return mp1Audio;
    else if(!stricmp(name, ".mp2"))
        return mp2Audio;
    else if(!stricmp(name, ".mp3"))
        return mp3Audio;
    else
        return unknownEncoding;
}

const char *Audio::getMIME(Info &info)
{
    if(info.format == wave)
        return "audio/x-wav";

    if(info.format == snd) {
        switch(info.encoding) {
        case g721ADPCM:
            return "audio/x-adpcm";
        default:
            return "audio/basic";
        }
    }

    if(info.format == riff)
        return "audio/x-riff";

    switch(info.encoding) {
    case speexVoice:
    case speexAudio:
        return "application/x-spx";
    case mp1Audio:
    case mp2Audio:
    case mp3Audio:
        return "audio/x-mpeg";
    case pcm16Mono:
        return "audio/l16";
    case voxADPCM:
        return "audio/x-vox";
    case gsmVoice:
        return "audio/x-gsm";
    case g729Audio:
        return "audio/g729";
    case ilbcAudio:
        return "audio/iLBC";
    default:
        return NULL;
    }
}

const char *Audio::getName(Encoding encoding)
{
    switch(encoding) {
    case pcm8Stereo:
    case pcm8Mono:
        return "pcm8";
    case cdaStereo:
    case cdaMono:
    case pcm16Stereo:
    case pcm16Mono:
        return "pcm16";
    case pcm32Stereo:
    case pcm32Mono:
        return "pcm32";
    case mulawAudio:
        return "pcmu";
    case alawAudio:
        return "pcma";
    case g721ADPCM:
        return "adpcm";
    case ilbcAudio:
        return "ilbc";
    case g729Audio:
        return "g.729";
    case g722Audio:
    case g722_7bit:
    case g722_6bit:
        return "g.722";
    case g723_2bit:
    case g723_3bit:
    case g723_5bit:
        return "g.723";
    case gsmVoice:
        return "gsm";
    case msgsmVoice:
        return "msgsm";
    case mp1Audio:
        return "mp1";
    case mp2Audio:
        return "mp2";
    case mp3Audio:
        return "mp3";
    case okiADPCM:
        return "oki";
    case voxADPCM:
        return "vox";
    case sx73Voice:
        return "sx73";
    case sx96Voice:
        return "sx96";
    case speexVoice:
    case speexAudio:
        return "speex";
    default:
        return "unknown";
    }
}

bool Audio::is_buffered(Encoding encoding)
{
    switch(encoding) {
    case mp1Audio:
    case mp2Audio:
    case mp3Audio:
        return true;
    default:
        return false;
    }
}

bool Audio::is_linear(Encoding encoding)
{
    switch(encoding) {
    case cdaStereo:
    case cdaMono:
    case pcm16Stereo:
    case pcm16Mono:
        return true;
    default:
        return false;
    }
}

Audio::Level Audio::impulse(Encoding encoding, void *buffer, unsigned samples)
{
    unsigned long sum = 0;
    unsigned long count;
    Linear sp;
    int mb;
    unsigned char *sv = (unsigned char *)&mb, *s1, *s2;

    if(!samples)
        samples = getCount(encoding);

    switch(encoding) {
    case cdaStereo:
    case pcm16Stereo:
        samples *= 2;
    case pcm16Mono:
    case cdaMono:
        count = samples;
        if(__BYTE_ORDER == __LITTLE_ENDIAN) {
            sp = (Linear)buffer;
            while(samples--) {
                if(*sp < 0)
                    sum -= *(sp++);
                else
                    sum += *(sp++);
            }
            return (Level)(sum/count);
        }

        s1 = (unsigned char *)buffer;
        s2 = s1 + 1;
        while(samples--) {
            sv[0] = *s2;
            sv[1] = *s1;
            s1 += 2;
            s2 += 2;
            if(mb < 0)
                sum -= mb;
            else
                sum += mb;
        }
        return (Level)(sum / count);
    default:
        return -1;
    }
}

Audio::Level Audio::impulse(Info &info, void *buffer, unsigned samples)
{
    unsigned long sum = 0;
    unsigned long count;
    Sample *sp, mb;
    snd16_t *up, ub;
    unsigned char *sv = (unsigned char *)&mb, *s1, *s2;
    unsigned char *uv = (unsigned char *)&ub;

    if(!samples)
        samples = info.framecount;

    if(!samples)
        samples = getCount(info.encoding);

    switch(info.encoding) {
    case cdaStereo:
    case pcm16Stereo:
        samples *= 2;
    case pcm16Mono:
    case cdaMono:
        count = samples;
        if(info.format == snd && (info.order == __BYTE_ORDER || !info.order)) {
            count *= 2;
            up = (snd16_t *)buffer;
            while(samples--)
                sum += *(up++);
            return (Level)(sum / count);
        }
        if(info.format == snd) {
            count *= 2;
            s1 = (unsigned char *)buffer;
            s2 = s1 + 1;
            while(samples--) {
                uv[0] = *s2;
                uv[1] = *s1;
                s2 += 2;
                s1 += 2;
                sum += ub;
            }
            return (Level)(sum / count);
        }
        if(__BYTE_ORDER == info.order || !info.order) {
            sp = (Linear)buffer;
            while(samples--) {
                if(*sp < 0)
                    sum -= *(sp++);
                else
                    sum += *(sp++);
            }
            return (Level)(sum/count);
        }

        s1 = (unsigned char *)buffer;
        s2 = s1 + 1;
        while(samples--) {
            sv[0] = *s2;
            sv[1] = *s1;
            s1 += 2;
            s2 += 2;
            if(mb < 0)
                sum -= mb;
            else
                sum += mb;
        }
        return (Level)(sum / count);
    default:
        return -1;
    }
}

Audio::Level Audio::peak(Encoding encoding, void *buffer, unsigned samples)
{
    Level max = 0, value;
    unsigned long count;
    Sample *sp, mb;
    unsigned char *sv = (unsigned char *)&mb, *s1, *s2;

    if(!samples)
        samples = getCount(encoding);

    switch(encoding) {
    case cdaStereo:
    case pcm16Stereo:
        samples *= 2;
    case pcm16Mono:
    case cdaMono:
        count = samples;
        if(__BYTE_ORDER == __LITTLE_ENDIAN) {
            sp = (Linear)buffer;
            while(samples--) {
                value = *(sp++);
                if(value < 0)
                    value = -value;
                if(value > max)
                    max = value;
            }
            return max;
        }

        s1 = (unsigned char *)buffer;
        s2 = s1 + 1;
        while(samples--) {
            sv[0] = *s2;
            sv[1] = *s1;
            s1 += 2;
            s2 += 2;
            if(mb < 0)
                mb = -mb;
            if(mb > max)
                max = mb;
        }
        return max;
    default:
        return -1;
    }
}

Audio::Level Audio::peak(Info &info, void *buffer, unsigned samples)
{
    Level max = 0, value;
    unsigned long count;
    Sample *sp, mb;
    snd16_t *up, ub;
    unsigned char *sv = (unsigned char *)&mb, *s1, *s2;
    unsigned char *uv = (unsigned char *)&ub;

    if(!samples)
        samples = info.framecount;

    if(!samples)
        samples = getCount(info.encoding);

    switch(info.encoding) {
    case cdaStereo:
    case pcm16Stereo:
        samples *= 2;
    case pcm16Mono:
    case cdaMono:
        count = samples;
        if(info.format == snd && (info.order == __BYTE_ORDER || !info.order)) {
            count *= 2;
            up = (snd16_t *)buffer;
            while(samples--) {
                value = (Level)(*(up++) / 2);
                if(value > max)
                    max = value;
            }
            return max;
        }
        if(info.format == snd) {
            count *= 2;
            s1 = (unsigned char *)buffer;
            s2 = s1 + 1;
            while(samples--) {
                uv[0] = *s2;
                uv[1] = *s1;
                s2 += 2;
                s1 += 2;
                ub /= 2;
                if((Level)ub > max)
                    max = ub;
            }
            return max;
        }
        if(__BYTE_ORDER == info.order || !info.order) {
            sp = (Linear)buffer;
            while(samples--) {
                value = *(sp++);
                if(value < 0)
                    value = -value;
                if(value > max)
                    max = value;
            }
            return max;
        }

        s1 = (unsigned char *)buffer;
        s2 = s1 + 1;
        while(samples--) {
            sv[0] = *s2;
            sv[1] = *s1;
            s1 += 2;
            s2 += 2;
            if(mb < 0)
                mb = -mb;
            if(mb > max)
                max = mb;
        }
        return max;
    default:
        return -1;
    }
}

void Audio::swapEncoded(Info &info, Encoded buffer, size_t bytes)
{
    char buf;

    if(!is_linear(info.encoding))
        return;

    if(!info.order || info.order == __BYTE_ORDER)
        return;

    // options for machine optimized should be inserted here

    bytes /= 2;
    while(bytes--) {
        buf = buffer[1];
        buffer[1] = *buffer;
        *buffer = buf;
        buffer += 2;
    }
}

bool Audio::swapEndian(Encoding encoding, void *buffer, unsigned samples)
{
    unsigned char buf;
    unsigned char *s1, *s2, *s3, *s4;
    if(is_stereo(encoding))
        samples *= 2;

    switch(encoding) {
    case pcm16Mono:
    case pcm16Stereo:
    case cdaMono:
    case cdaStereo:
        if(__BYTE_ORDER == __LITTLE_ENDIAN)
            return true;

        s1 = (unsigned char *)buffer;
        s2 = s1 + 1;
        while(samples--) {
            buf = *s1;
            *s1 = *s2;
            *s2 = buf;
            s1 += 2;
            s2 += 2;
        }
        return false;
    case pcm32Mono:
    case pcm32Stereo:
        if(__BYTE_ORDER == __LITTLE_ENDIAN)
            return true;

        s1 = (unsigned char *)buffer;
        s2 = s1 + 1;
        s3 = s2 + 1;
        s4 = s3 + 1;
        while(samples--) {
            buf = *s1;
            *s1 = *s4;
            *s4 = buf;
            buf = *s2;
            *s2 = *s3;
            *s3 = buf;
            s1 += 4;
            s2 += 4;
            s3 += 4;
            s4 += 4;
        }
        return false;
    default:
        return true;
    }
}

bool Audio::swapEndian(Info &info, void *buffer, unsigned samples)
{
    unsigned char buf;
    unsigned char *s1, *s2, *s3, *s4;
    if(is_stereo(info.encoding))
        samples *= 2;

    switch(info.encoding) {
    case pcm16Mono:
    case pcm16Stereo:
    case cdaMono:
    case cdaStereo:
        if(__BYTE_ORDER == info.order || !info.order)
            return true;

        s1 = (unsigned char *)buffer;
        s2 = s1 + 1;
        while(samples--) {
            buf = *s1;
            *s1 = *s2;
            *s2 = buf;
            s1 += 2;
            s2 += 2;
        }
        return false;
    case pcm32Mono:
    case pcm32Stereo:
        if(__BYTE_ORDER == info.order || !info.order)
            return true;

        s1 = (unsigned char *)buffer;
        s2 = s1 + 1;
        s3 = s2 + 1;
        s4 = s3 + 1;
        while(samples--) {
            buf = *s1;
            *s1 = *s4;
            *s4 = buf;
            buf = *s2;
            *s2 = *s3;
            *s3 = buf;
            s1 += 4;
            s2 += 4;
            s3 += 4;
            s4 += 4;
        }
        return false;
    default:
        return true;
    }
}


bool Audio::is_endian(Encoding encoding)
{
    switch(encoding) {
    case cdaMono:
    case cdaStereo:
    case pcm32Mono:
    case pcm32Stereo:
    case pcm16Stereo:
    case pcm16Mono:
        if(__BYTE_ORDER != __LITTLE_ENDIAN)
            return false;
    default:
        return true;
    }
}

bool Audio::is_endian(Info &info)
{
    switch(info.encoding) {
    case cdaStereo:
    case cdaMono:
    case pcm16Stereo:
    case pcm16Mono:
    case pcm32Stereo:
    case pcm32Mono:
        if(info.order && __BYTE_ORDER != info.order)
            return false;
    default:
        return true;
    }
}

timeout_t Audio::getFraming(Info &info, timeout_t timeout)
{
    timeout_t fa = info.framing;
    unsigned long frames;

    if(!timeout)
        return fa;

    if(!fa)
        return timeout;

    frames = timeout / fa;
    return frames * fa;
}

timeout_t Audio::getFraming(Encoding encoding, timeout_t timeout)
{
    timeout_t fa = 0;
    unsigned long frames;

    switch(encoding) {
    case mp1Audio:
        fa = 8;
        break;
    case mp2Audio:
    case mp3Audio:
        fa = 26;
        break;
    case msgsmVoice:
        fa = 40;
        break;
    case g729Audio:
        fa = 10;
        break;
    case gsmVoice:
    case speexVoice:
    case speexAudio:
    case speexUltra:
        fa = 20;
        break;
    case ilbcAudio:
        fa = 30;
        break;
    case sx96Voice:
    case sx73Voice:
        fa = 15;
    default:
        break;
    }
    if(!timeout)
        return fa;

    if(!fa)
        return timeout;

    frames = timeout / fa;
    return frames * fa;
}

int Audio::getCount(Encoding encoding)
{
    switch(encoding) {
    case mp1Audio:
        return 384;
    case mp2Audio:
    case mp3Audio:
        return 1152;
    case msgsmVoice:
        return 320;
    case gsmVoice:
        return 160;
    case ilbcAudio:
        return 240;
    case g729Audio:
        return 80;
    case sx73Voice:
    case sx96Voice:
        return 120;
    case speexVoice:
        return 160;
    case speexAudio:
        return 320;
    case speexUltra:
        return 640;
    case unknownEncoding:
        return 0;
    case g723_2bit:
        return 4;
    case g723_3bit:
    case g723_5bit:
        return 8;
    case g721ADPCM:
    case okiADPCM:
    case voxADPCM:
        return 2;
    default:
        return 1;
    }
}

int Audio::getFrame(Encoding encoding, int samples)
{
    int framing = 0;
    switch(encoding) {
    case sx73Voice:
        framing = 14;
        break;
    case sx96Voice:
        framing = 18;
        break;
    case msgsmVoice:
        framing = 65;
        break;
    case speexVoice:
        framing = 20;
        break;
    case speexAudio:
        framing = 40;
        break;
    case gsmVoice:
        framing = 33;
        break;
    case ilbcAudio:
        framing = 50;
        break;
    case g729Audio:
        framing = 10;
        break;
    case g723_2bit:
        framing = 1;
        break;
    case g723_3bit:
        framing = 3;
        break;
    case g723_5bit:
        framing = 5;
        break;
    case unknownEncoding:
        return 0;
    case pcm32Stereo:
        return 8;
    case pcm32Mono:
    case pcm16Stereo:
    case cdaStereo:
        framing = 4;
        break;
    case pcm8Stereo:
    case pcm16Mono:
    case cdaMono:
        framing = 2;
        break;
    default:
        framing = 1;
    }
    if(!samples)
        return framing;

    return (samples / framing) * framing;
}

void Audio::fill(unsigned char *addr, int samples, Encoding encoding)
{
    int frame = getFrame(encoding);
    int count = getCount(encoding);

    if(!frame || !count)
        return;

    while(samples >= count) {
        switch(encoding) {
        case mulawAudio:
            *addr = 0xff;
            break;
        case alawAudio:
            *addr = 0x55;
            break;
        default:
            memset(addr, 0, frame);
            break;
        }
        addr += frame;
        samples -= count;
    }
}

Audio::Rate Audio::getRate(Encoding encoding, Rate request)
{
    if((long)request == (long)0)
        request = getRate(encoding);

    switch(encoding) {
    case pcm8Stereo:
    case pcm8Mono:
    case pcm16Stereo:
    case pcm16Mono:
    case pcm32Stereo:
    case pcm32Mono:
    case mulawAudio:
    case alawAudio:
        return request;
    case voxADPCM:
        if(request == rate8khz)
            return rate8khz;
        return rate6khz;
    default:
        break;
    };
    return getRate(encoding);
}

Audio::Rate Audio::getRate(Encoding encoding)
{
    switch(encoding) {
    case pcm8Stereo:
    case pcm8Mono:
    case pcm16Stereo:
    case pcm16Mono:
    case pcm32Stereo:
    case pcm32Mono:
    case unknownEncoding:
        return rateUnknown;
    case voxADPCM:
        return rate6khz;
    case cdaStereo:
    case cdaMono:
        return rate44khz;
    case speexAudio:
        return rate16khz;
    case speexUltra:
        return rate32khz;
    default:
        return rate8khz;
    }
}

unsigned long Audio::toSamples(Encoding encoding, size_t bytes)
{
    unsigned long sf = getFrame(encoding);
    if(!bytes || !sf)
        return 0;
    unsigned long frames = (unsigned long)(bytes / sf);
    return frames * getCount(encoding);
}

unsigned long Audio::toSamples(Info &info, size_t bytes)
{
    if(!bytes)
        return 0;

    unsigned long frames = (unsigned long)(bytes / info.framesize);
    return frames * info.framecount;
}

size_t Audio::toBytes(Encoding encoding, unsigned long samples)
{
    unsigned long sc = getCount(encoding);
    if(!samples || !sc)
        return 0;
    unsigned long frames = samples / sc;
    return frames * getFrame(encoding);
}

size_t Audio::maxFramesize(Info &info)
{
    switch(info.encoding) {
    case mp1Audio:
        return 682;
    case mp2Audio:
    case mp3Audio:
        return 1735;
    default:
        return info.framesize;
    }
}

size_t Audio::toBytes(Info &info, unsigned long samples)
{
    if(!samples)
        return 0;
    unsigned long frames = samples / info.framecount;
    return frames * info.framesize;
}

bool Audio::is_mono(Encoding encoding)
{
    switch(encoding) {
    case pcm8Stereo:
    case pcm16Stereo:
    case pcm32Stereo:
    case cdaStereo:
        return false;
    default:
        return true;
    }
}

bool Audio::is_stereo(Encoding encoding)
{
    return !is_mono(encoding);
}

#ifdef  _MSWINDOWS_
#define snprintf _snprintf
#endif

void Audio::toTimestamp(timeout_t duration, char *buf, size_t len)
{
    timeout_t msec = duration % 1000;
    timeout_t secs = (duration / 1000) % 60;
    timeout_t mins = (duration / 60000) % 60;
    timeout_t hours = (duration /3600000);

    snprintf(buf, len, "%ld:%02ld:%02ld.%03ld", hours, mins, secs, msec);
}

timeout_t Audio::toTimeout(const char *buf)
{
    const char *cp, *sp;
    timeout_t msec = 0;
    timeout_t sec = 0;

    cp = strchr(buf, '.');
    if(cp) {
        msec = atol(cp + 1);
        sp = --cp;
    }
    else
        sp = strrchr(buf, ':');

    if(!sp) {
        sp = buf;
        while(*sp && isdigit(*sp))
            ++sp;
        if(*sp && tolower(*sp) == 'm' && tolower(sp[1] == 's'))
            return atol(buf);
        if(tolower(*sp) == 'h')
            return atol(buf) * 3600000;
        if(tolower(*sp) == 'm')
            return atol(buf) * 60000;
        return atol(buf) * 1000l;
    }
    while(*sp != ':' && sp > buf)
        --sp;

    if(sp == buf)
        return atol(buf) * 1000 + msec;

    sec = atol(sp + 1) * 1000;
    --sp;
    while(*sp != ':' && sp > buf)
        --sp;

    if(sp == buf)
        return atol(buf) * 60000 + sec + msec;

    return atol(buf) * 3600000 + atol(++sp) * 60000 + sec + msec;
}

static const char *prefix_path = NULL;
static const char *voices_path = NULL;
static const char *suffix_ext = NULL;
static AudioRule *locale_rule = NULL;

void Audio::prefix(const char *path)
{
    if(path)
        prefix_path = strdup(path);
}

void Audio::suffix(const char *ext)
{
    if(ext && *ext == '.')
        suffix_ext = strdup(ext);
}

void Audio::voices(const char *path, AudioRule *locale)
{
    if(!locale)
        locale = AudioRule::find(NULL);

    voices_path = strdup(path);
    locale_rule = locale;
}

string_t Audio::path(const char *name, AudioRule *locale)
{
    bool fp = false;
    const char *ext;
    const char *ls = strrchr(name, '/');
    if(*name == '/')
        fp = true;
#ifdef  _MSWINDOWS_
    const char *lbs = strrchr(name, '\\');
    if(*name == '\\')
        fp = true;
    if(!lbs && name[1] == ':') {
        lbs = name + 1;
        fp = true;
    }
    if(lbs > ls)
        ls = lbs;
#endif
    if(ls)
        ext = strrchr(ls, '.');
    else
        ext = strrchr(name, '.');
    if(!ls && voices_path && (locale || locale_rule)) {
        if(!locale)
            locale = locale_rule;
        if(ext)
            return str(voices_path) + str(locale->path()) + str(name);
        else
            return str(voices_path) + str(locale->path()) + str(name) + str(suffix_ext);
    }
    if((!ls || fp || !prefix_path) && ext)
        return str(name);
    if(!ls || fp || !prefix_path)
        return str(name) + str(suffix_ext);

    if(ext)
        return str(prefix_path) + str("/") + str(name);

    return str(prefix_path) + str("/") + str(name) + str(suffix_ext);
}

void Audio::init(shell& args)
{
#ifdef  _MSWINDOWS_
    plugins = _STR(str(args.execdir()) + "../ccaudio");
#else
    const char *dp = strrchr(args.execdir(), '/');
    if(dp && (eq(dp, "/.") || eq(dp, "/utils") || eq(dp, "/.libs")))
        plugins = args.execdir();
#endif
    init();
}

void Audio::init(void)
{
#ifndef BUILD_STATIC
    char path[256];
    fsys dir;
    const char *dp = getPluginPath();
    char filename[65];

    if(!dp)
        return;

    fsys::open(dir, dp, fsys::ACCESS_DIRECTORY);

    while(is(dir) && fsys::read(dir, filename, sizeof(filename)) > 0) {
        if(filename[0] == '.')
            continue;

        const char *ext = strrchr(filename, '.');
        if(!ext || !case_eq(ext, MODULE_EXT))
            continue;

        if(case_eq(filename, "lib", 3))
                continue;

        snprintf(path, sizeof(path), "%s/%s", dp, filename);
        fsys::load(path);
    }
    fsys::close(dir);
#endif
}

// if no soundcard hardware, we don't bind soundcard fetch

#if !defined(HAVE_SYS_SOUNDCARD_H) && !defined(_MSWINDOWS_) && !defined(OSX_AUDIO)

bool Audio::hasDevice(unsigned index)
{
    return false;
}

AudioDevice *Audio::getDevice(unsigned index, DeviceMode mode)
{
    return NULL;
}

#endif

