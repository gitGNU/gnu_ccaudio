// Copyright (C) 1999-2005 Open Source Telecom Corporation.
// Copyright (C) 2006-2011 David Sugar, Tycho Softworks.
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

#include <ccaudio2.h>
#include <config.h>
#ifdef  HAVE_ENDIAN_H
#include <endian.h>
#endif

#if !defined(__BIG_ENDIAN)
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412
#define __BYTE_ORDER    __LITTLE_ENDIAN
#endif

using namespace UCOMMON_NAMESPACE;

static const char *delfile = NULL;
static shell::flagopt helpflag('h',"--help",    _TEXT("display this list"));
static shell::flagopt althelp('?', NULL, NULL);

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

class PacketStream : public AudioStream
{
private:
    char **list;
    char *getContinuation(void);

public:
    PacketStream();
    void open(char **argv);
};

class PlayStream : public AudioStream
{
private:
    char **list;
    char *getContinuation(void);

public:
    PlayStream();
    void open(char **argv);
};

PlayStream::PlayStream() : AudioStream()
{
    list = NULL;
}

void PlayStream::open(char **argv)
{
    AudioStream::open(*(argv++), modeRead, 10);
    if(is_open())
        list = argv;
}

char *PlayStream::getContinuation(void)
{
    if(!list)
        return NULL;

    return *(list++);
}

PacketStream::PacketStream() : AudioStream()
{
    list = NULL;
}

void PacketStream::open(char **argv)
{
    AudioStream::open(*(argv++), modeRead, 10);
    if(is_open())
        list = argv;
}

char *PacketStream::getContinuation(void)
{
    if(!list)
        return NULL;

    return *(list++);
}


AudioBuild::AudioBuild() : AudioStream()
{
    list = NULL;
}

void AudioBuild::open(char **argv)
{
    AudioStream::open(*(argv++), modeRead, 10);
    if(is_open())
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
    unsigned long samples, copied;
    Linear buffer, source;
    unsigned pages, npages;
    Info from, to;
    bool mono = true;
    AudioResample *resampler = NULL;
    Linear resample = NULL;

    input.getInfo(&from);
    output.getInfo(&to);

    if(is_stereo(from.encoding) || is_stereo(to.encoding))
        mono = false;

    samples = input.getCount();


    if(mono)
        buffer = new Audio::Sample[samples];
    else
        buffer = new Audio::Sample[samples * 2];

    source = buffer;

    if(from.rate != to.rate) {
        resampler = new AudioResample((Audio::Rate)from.rate, (Audio::Rate)to.rate);
        resample = new Audio::Sample[resampler->estimate(samples)];
        source = resample;
    }

    for(;;)
    {
        if(mono)
            pages = input.getMono(buffer, 1);
        else
            pages = input.getStereo(buffer, 1);

        if(!pages)
            break;

        if(resampler)
            copied = resampler->process(buffer, resample, samples);
        else
            copied = samples;

        if(mono)
            npages = output.bufMono(source, copied);
        else
            npages = output.bufStereo(source, copied);

        // if(!npages)
        //  break;
    }

    delete[] buffer;

    if(resampler)
        delete resampler;

    if(resample)
        delete[] resample;
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

    if(to.order && from.order && to.order != from.order && is_linear(from.encoding))
        endian = true;

    bufsize = from.framesize;
    buffer = new unsigned char[bufsize];

    while(status > 0) {
        status = input.getNative(buffer, bufsize);
        if(status < 1)
            break;

        status = output.putNative(buffer, status);
    }

    delete[] buffer;
}


static void stop(void)
{
    if(delfile) {
        remove(delfile);
        delfile = NULL;
    }
}

static void plugins(void)
{
    printf("%s\n", Audio::getPluginPath());
    exit(0);
}

static void codecs(void)
{
    linked_pointer<AudioCodec> cp = AudioCodec::begin();

    while(is(cp)) {
        printf("%s - %s\n", cp->getName(), cp->getDescription());
        cp.next();
    }
    exit(0);
}

static void version(void)
{
    printf("%s\n", VERSION);
    exit(0);
}

static void soundcard(unsigned index)
{
    AudioDevice *soundcard = Audio::getDevice(index);
    const Audio::Info *info;

    if(!Audio::is_available(index) && !soundcard)
        printf("%s\n", _TEXT("Sound device inaccessible or unsupported"));
    else if(!soundcard)
        printf("%s\b", _TEXT("Sound device unavailable"));
    else {
        info = soundcard->getInfo();
        printf("%s: %s\n", _TEXT("Soundcard Driver"), info->annotation);
        if(Audio::is_stereo(info->encoding))
            printf("%s: 2\n", _TEXT("Default Channels"));
        else
            printf("%s: 1\n", _TEXT("Default Channels"));
        printf("%s: %s\n", _TEXT("Default Encoding"), Audio::getName(info->encoding));
        printf("%s: %d\n", _TEXT("Default Buffers"), info->framecount);
        printf("%s: %ldms\n", _TEXT("Default Framing"), info->framing);
        printf("%s: %ld %s\n", _TEXT("Default Rate"), info->rate, _TEXT(" samples per second"));
    }
    exit(0);
}

static void showendian(void)
{
    if(__BYTE_ORDER == __BIG_ENDIAN)
        printf("%s\n", _TEXT("big"));
    else
        printf("%s\n", _TEXT("little"));
    exit(0);
}

static const char *fname(const char *cp)
{
    const char *fn = strrchr(cp, '/');
    if(!fn)
        fn = strrchr(cp, '\\');
    if(fn)
        return ++fn;
    return cp;
}

static void rewrite(const char *source, char *target, size_t max)
{
    char *fn;
    char buffer[256];

#ifdef  WIN32
    char *ext;
    snprintf(buffer, sizeof(buffer), "%s", source);
    while(NULL != (fn = strchr(buffer, '\\')))
        *fn = '/';

    fn = strrchr(buffer, '/');
    if(fn) {
        *(fn++) = 0;
        ext = strrchr(fn, '.');
        if(ext)
            *ext = 0;
        snprintf(target, max, "%s/%s.tmp", buffer, fn);
    }
    else {
        ext = strrchr(buffer, '.');
        if(ext)
            *ext = 0;
        snprintf(target, max, "%s.tmp", buffer);
    }
#else
    snprintf(buffer, sizeof(buffer), "%s", source);
    fn = strrchr(buffer, '/');
    if(fn) {
        *(fn++) = 0;
        snprintf(target, max, "%s/.tmp.%s", buffer, fn);
    }
    else
        snprintf(target, max, ".tmp.%s", source);
#endif
}

static void chart(char **argv)
{
    AudioFile file;
    Audio::Info info;
    AudioCodec *codec = NULL;
    char *fn;
    timeout_t framing = 20;
    Audio::Level silence = 0;
    unsigned char *buffer;
    short max, current;
    unsigned long sum;
    unsigned long count;

retry:
    if(!*argv) {
        shell::errexit(2, "*** audiotool: -chart: %s\n",
            _TEXT("missing arguments"));
    }

    fn = *argv;

    if(eq(fn, "--")) {
        ++argv;
        goto skip;
    }

    if(eq(fn, "--", 2))
        ++fn;

    if(eq(fn, "-framing=", 9)) {
        framing = atoi(fn + 9);
        ++argv;
        goto retry;
    }
    else if(eq(fn, "-framing"))
    {
        ++argv;
        if(!*argv) {
            shell::errexit(3, "*** audiotool: -chart: -framing: %s\n",
                _TEXT("missing argument"));
        }
        framing = atoi(*(argv++));
        goto retry;
    }

    if(eq(fn, "-silence=", 9)) {
        silence = atoi(fn + 9);
        ++argv;
        goto retry;
    }
    else if(eq(fn, "-silence"))
    {
        ++argv;
        if(!*argv) {
            shell::errexit(3, "*** audiotool: -chart: -silence: %s\n",
                _TEXT("missing argument"));
        }
        silence = atoi(*(argv++));
        goto retry;
    }

skip:

    if(!framing)
        framing = 20;

    while(*argv) {
        if(!fsys::isfile(*argv)) {
            printf("%s: %s\n",
                fname(*(argv++)), _TEXT("invalid"));
            continue;
        }
        if(fsys::access(*argv, R_OK)) {
            printf("%s: %s\n",
                fname(*(argv++)), _TEXT("inaccessable"));
            continue;
        }
        file.open(*argv, Audio::modeRead, framing);
        file.getInfo(&info);
        if(!Audio::is_linear(info.encoding))
            codec = AudioCodec::get(info);
        if(!Audio::is_linear(info.encoding) && !codec) {
            printf("%s: %s\n",
                fname(*(argv++)), _TEXT("cannot load codec"));
            continue;
        }

        printf("%s: ", fname(*(argv++)));
        buffer = new unsigned char[info.framesize];

        max = 0;
        sum = 0;
        count = 0;

        // autochart for silence value

        while(!silence) {
            if(file.getBuffer(buffer, info.framesize) < (int)info.framesize)
                break;
            ++count;
            if(codec)
                sum += codec->impulse(buffer, info.framecount);
            else
                sum += Audio::impulse(info, buffer, info.framecount);
        }

        if(!silence && count)
            silence = (Audio::Level)(((sum / count) * 2) / 3);

        max = 0;
        sum = 0;
        count = 0;

        file.setPosition(0);

        for(;;) {
            if(file.getBuffer(buffer, info.framesize) < (int)info.framesize)
                break;
            ++count;
            if(codec) {
                current = codec->peak(buffer, info.framecount);
                if(current > max)
                    max = current;
                sum += codec->impulse(buffer, info.framecount);
                if(codec->is_silent(silence, buffer, info.framecount)) {
                    if(codec->peak(buffer, info.framecount) >= silence)
                        printf("^");
                    else
                        printf(".");
                }
                else
                    printf("+");
                fflush(stdout);
                continue;
            }

            current = Audio::peak(info, buffer, info.framecount);
            if(current > max)
                max = current;

            sum += Audio::impulse(info, buffer, info.framecount);
            if(Audio::impulse(info, buffer, info.framecount) < silence) {
                if(Audio::peak(info, buffer, info.framecount) >= silence)
                    printf("^");
                else
                    printf(".");
            }
            else
                printf("+");
            fflush(stdout);
        }
        printf("\n");
        if(count)
            printf("%s = %d, %s = %ld, %s = %d\n",
                _TEXT("silence threashold"), silence,
                _TEXT("avg frame energy"), (sum / count),
                _TEXT("peak level"), max);

        if(buffer)
            delete[] buffer;


        if(codec)
            AudioCodec::release(codec);

        codec = NULL;
        buffer = NULL;

        file.close();
    }
    exit(0);
}

static void info(char **argv)
{
    AudioFile au;
    Audio::Info info;
    const char *fn;
    timeout_t framing = 0;
    unsigned long size, end;
    unsigned long minutes, seconds, subsec, scale;

    fn = *argv;
    if(eq(fn, "--", 2))
        ++fn;
    if(eq(fn, "-framing=", 9)) {
        framing = atoi(fn + 9);
        ++argv;
    }
    else if(eq(fn, "-framing"))
    {
        framing = atoi(*(++argv));
        ++argv;
    }

    while(*argv) {
        if(!fsys::isfile(*argv)) {
            printf("audiotool: %s: %s\n",
                fname(*(argv++)), _TEXT("invalid"));
            continue;
        }
        if(fsys::access(*argv, R_OK)) {
            printf("audiotool: %s: %s\n",
                fname(*(argv++)), _TEXT("inaccessable"));
            continue;
        }
        au.open(*argv, Audio::modeInfo, framing);
        au.getInfo(&info);
        au.setPosition();
        size = end = au.getPosition();
        printf("%s\n", fname(*(argv++)));
        fn = Audio::getMIME(info);
        if(!fn)
            switch(info.format) {
            case Audio::raw:
                fn = "raw audio";
                break;
            case Audio::snd:
                fn = "sun audio";
                break;
            case Audio::riff:
                fn = "riff";
                break;
            case Audio::wave:
                fn = "ms wave";
                break;
            case Audio::mpeg:
                fn = "mpeg audio";
                break;
            }


        if(fn)
            printf("    %s: %s\n", _TEXT("Format"), fn);
        else
            printf("    %s: %s\n", _TEXT("Format"), _TEXT("unknown"));

        printf("    %s: %s\n", _TEXT("Encoding"), Audio::getName(info.encoding));
        if(Audio::is_stereo(info.encoding))
            printf("    %s: 2\n", _TEXT("Channels"));
        else
            printf("    %s: 1\n", _TEXT("Channels"));
        if(info.framing)
            printf("    %s: %ldms\n", _TEXT("Frame Size"), info.framing);
        if(Audio::is_linear(info.encoding)) {
            if(info.order == __BIG_ENDIAN)
                printf("    %s: %s\n", _TEXT("Byte Order"), _TEXT("big"));
            else if(info.order == __LITTLE_ENDIAN)
                printf("    %s: %s\n", _TEXT("Byte Order"), _TEXT("little"));
            else
                printf("    %s: %s\n", _TEXT("Byte Order"), _TEXT("native"));
        }
        printf("    %s: %ld\n", _TEXT("Sample Rate"), info.rate);
        printf("    %s: %ld\n", _TEXT("Bit Rate"), info.bitrate);
        printf("    %s: %ld\n", _TEXT("Samples"), end);

        scale = info.rate / 1000;

        subsec = (end % info.rate) / scale;

        end /= info.rate;
        seconds = end % 60;
        end /= 60;
        minutes = end % 60;
        end /= 60;
        printf("    %s %02ld:%02ld:%02ld.%03ld\n",
            _TEXT("Duration"), end, minutes, seconds, subsec);

        if(info.headersize)
            printf("    %s: %u, %s=%u, %s=%u\n",
                _TEXT("Computed Frame Size"),
                info.framesize - info.headersize - info.padding,
                _TEXT("header"), info.headersize,
                _TEXT("padding"), info.padding);;

        au.close();
    }
    exit(0);
}

static void strip(char **argv)
{
    AudioFile file, tmp;
    Audio::Info info;
    AudioCodec *codec = NULL;
    char *fn;
    timeout_t framing = 20;
    short silence = 0;
    int rtn;
    unsigned char *buffer;
    Audio::Level max, current;
    unsigned long sum;
    unsigned long count;
    char target[256];

retry:
    if(!*argv) {
        shell::errexit(2, "*** audiotool: -strip: %s\n",
            _TEXT("missing arguments"));
    }

    fn = *argv;

    if(eq(fn, "--")) {
        ++argv;
        goto skip;
    }

    if(eq(fn, "--", 2))
        ++fn;
    if(eq(fn, "-framing=", 9)) {
        framing = atoi(fn + 9);
        ++argv;
        goto retry;
    }
    else if(eq(fn, "-framing"))
    {
        ++argv;
        if(!*argv) {
            shell::errexit(2, "*** audiotool: -strip: -framing: %s\n",
                _TEXT("missing argument"));
        }
        framing = atoi(*(argv++));
        goto retry;
    }

    if(eq(fn, "-silence=", 9)) {
        silence = atoi(fn + 9);
        ++argv;
        goto retry;
    }
    else if(eq(fn, "-silence"))
    {
        ++argv;
        if(!*argv) {
            shell::errexit(2, "*** audiotool: -strip: -silence: %s\n",
                _TEXT("missing argument"));
        }
        silence = atoi(*(argv++));
        goto retry;
    }

skip:

    if(!framing)
        framing = 20;

    while(*argv) {
        if(!fsys::isfile(*argv)) {
            printf("%s: %s\n",
                *(argv++), _TEXT("invalid"));
            continue;
        }
        if(fsys::access(*argv, R_OK)) {
            printf("%s: %s\n",
                *(argv++), _TEXT("inaccessable"));
            continue;
        }
        rewrite(*argv, target, sizeof(target));
        delfile = target;
        file.open(*argv, Audio::modeRead, framing);
        file.getInfo(&info);
        if(!Audio::is_linear(info.encoding))
            codec = AudioCodec::get(info);
        if(!Audio::is_linear(info.encoding) && !codec) {
            printf("%s: %s\n",
                *(argv++), _TEXT("cannot load codec"));
            continue;
        }

        buffer = new unsigned char[info.framesize];

        max = 0;
        sum = 0;
        count = 0;

        // compute silence value

        while(!silence) {
            rtn = file.getBuffer(buffer, info.framesize);
            if(rtn < (int)info.framesize)
                break;
            ++count;
            if(codec)
                sum += codec->impulse(buffer, info.framecount);
            else
                sum += Audio::impulse(info, buffer, info.framecount);
        }

        if(!silence && count)
            silence = (Audio::Level)(((sum / count) * 2) / 3);

        max = 0;
        sum = 0;
        count = 0;

        file.setPosition(0);


        tmp.create(target, &info);
        if(!tmp.is_open()) {
            printf("%s: %s\n",
                *(argv++), _TEXT("cannot rewrite"));
            continue;
        }

        for(;;)
        {
            rtn = file.getBuffer(buffer, info.framesize);
            if(rtn < (int)info.framesize)
                break;
            ++count;
            if(codec) {
                if(codec->is_silent(silence, buffer, info.framecount)) {
                    if(codec->peak(buffer, info.framecount) >= silence)
                        tmp.putBuffer(buffer, info.framesize);
                }
                else
                    tmp.putBuffer(buffer, info.framesize);
                continue;
            }

            current = Audio::peak(info, buffer, info.framecount);
            if(current > max)
                max = current;

            sum += Audio::impulse(info, buffer, info.framecount);
            if(Audio::impulse(info, buffer, info.framecount) < silence) {
                if(Audio::peak(info, buffer, info.framecount) >= silence)
                    tmp.putBuffer(buffer, info.framecount);
            }
            else
                tmp.putBuffer(buffer, info.framecount);
        }
        if(buffer)
            delete[] buffer;

        if(codec)
            AudioCodec::release(codec);

        codec = NULL;
        buffer = NULL;

        file.close();
        tmp.close();

        rtn = rename(target, *argv);
        remove(target);
        delfile = NULL;

        if(rtn)
            printf("%s: %s\n",
                *argv, _TEXT("could not be replaced"));

        ++argv;
    }
    exit(0);
}

static void trim(char **argv)
{
    AudioFile file, tmp;
    unsigned long first = 0, last = 0, total = 0, padding = 0;
    Audio::Info info;
    AudioCodec *codec = NULL;
    char *fn;
    timeout_t framing = 20;
    Audio::Level silence = 0;
    int rtn;
    unsigned char *buffer;
    Audio::Linear samples = NULL;
    Audio::Level max;
    unsigned long sum;
    unsigned long count;
    char target[256];
    bool use = false;

retry:
    if(!*argv) {
        shell::errexit(2, "*** audiotool: -trim: %s\n",
            _TEXT("missing arguments"));
        exit(-1);
    }

    fn = *argv;

    if(eq(fn, "--")) {
        ++argv;
        goto skip;
    }

    if(eq(fn, "--", 2))
        ++fn;
    if(eq(fn, "-framing=", 9)) {
        framing = atoi(fn + 9);
        ++argv;
        goto retry;
    }
    else if(eq(fn, "-framing"))
    {
        ++argv;
        if(!*argv) {
            shell::errexit(3, "*** audiotool: -trim: -framing: %s\n",
                _TEXT("missing argument"));
        }
        framing = atoi(*(argv++));
        goto retry;
    }

    if(eq(fn, "-padding=", 9)) {
        padding = atoi(fn + 9);
        ++argv;
        goto retry;
    }
    else if(eq(fn, "-padding"))
    {
        ++argv;
        if(!*argv) {
            shell::errexit(3, "*** audiotool: -trim: -padding: %s\n",
                _TEXT("missing argument"));
        }
        padding = atol(*(argv++));
        goto retry;
    }

    if(eq(fn, "-silence=", 9)) {
        silence = atoi(fn + 9);
        ++argv;
        goto retry;
    }
    else if(eq(fn, "-silence"))
    {
        ++argv;
        if(!*argv) {
            shell::errexit(3, "*** audiotool: -trim: -silence: %s\n",
                _TEXT("missing argument"));
        }
        silence = atoi(*(argv++));
        goto retry;
    }

skip:

    if(!framing)
        framing = 20;

    while(*argv) {
        if(!fsys::isfile(*argv)) {
            printf("%s: %s\n",
                *(argv++), _TEXT("invalid"));
            continue;
        }
        if(fsys::access(*argv, R_OK)) {
            printf("%s: %s\n",
                *(argv++), _TEXT("inaccessable"));
            continue;
        }
        rewrite(*argv, target, sizeof(target));
        delfile = target;
        file.open(*argv, Audio::modeRead, framing);
        file.getInfo(&info);
        if(!Audio::is_linear(info.encoding))
            codec = AudioCodec::get(info);
        if(!Audio::is_linear(info.encoding) && !codec) {
            printf("%s: %s\n",
                *(argv++), _TEXT("cannot load codec"));
            continue;
        }

        buffer = new unsigned char[info.framesize];

        max = 0;
        sum = 0;
        count = 0;

        // compute silence value

        while(!silence) {
            rtn = file.getBuffer(buffer, info.framesize);
            if(rtn < (int)info.framesize)
                break;
            ++count;
            if(codec)
                sum += codec->impulse(buffer, info.framecount);
            else
                sum += Audio::impulse(info, buffer, info.framecount);
        }

        if(!silence && count)
            silence = (Audio::Level)(((sum / count) * 2) / 3);

        max = 0;
        sum = 0;
        count = 0;

        file.setPosition(0);

        for(;;)
        {
            rtn = file.getBuffer(buffer, info.framesize);
            if(rtn < (int)info.framesize)
                break;

            ++total;
            if(codec) {
                if(codec->is_silent(silence, buffer, info.framecount)) {
                    use = false;
                    if(codec->peak(buffer, info.framecount) >= silence)
                        use = true;
                }
                else
                    use = true;
            }
            if(use && !first)
                first = total;
            if(use)
                last = total;
        }

        if(!last || !first) {
            printf("%s: %s\n",
                *(argv++), _TEXT("all silent, skipping"));
            continue;
        }

        --first;
        total = last - first;
        file.setPosition(first * info.framecount);

        tmp.create(target, &info);
        if(!tmp.is_open()) {
            printf("%s: %s\n",
                *(argv++), _TEXT("cannot rewrite"));
            continue;
        }

        while(total--) {
            rtn = file.getBuffer(buffer, info.framesize);
            if(rtn < (int)info.framesize)
                break;
            tmp.putBuffer(buffer, info.framesize);
        }

        if(padding) {
            if(!codec)
                memset(buffer, 0, info.framesize);
            else if(Audio::is_stereo(info.encoding))
            {
                samples = new Audio::Sample[info.framecount * 2];
                memset(samples, 0, info.framecount * 2);
                codec->encode(samples, buffer, info.framecount);
            }
            else {
                samples = new Audio::Sample[info.framecount];
                memset(samples, 0, info.framecount);
                codec->encode(samples, buffer, info.framecount);
            }
        }

        while(padding--) {
            rtn = tmp.putBuffer(buffer, info.framesize);
        }

        if(samples)
            delete[] samples;

        if(buffer)
            delete[] buffer;

        if(codec)
            AudioCodec::release(codec);

        codec = NULL;
        buffer = NULL;

        file.close();
        tmp.close();

        rtn = rename(target, *argv);
        remove(target);
        delfile = NULL;

        if(rtn)
            printf("%s: %s\n",
                *argv, _TEXT("could not be replaced"));

        ++argv;
    }
    exit(0);
}

static void size(char **argv)
{
    char *fn = *(argv++);
    AudioFile file;
    Audio::Info info;
    unsigned long pos;

    if(!fn) {
        shell::errexit(2, "*** audiotool: -size: %s\n",
            _TEXT("no file specified"));
    }

    file.open(fn, Audio::modeRead);
    if(!file.is_open()) {
        shell::errexit(3, "*** audiotool: %s: %s\n",
            fname(fn), _TEXT("cannot access"));
    }
    file.getInfo(&info);
    file.setPosition();
    pos = file.getPosition();
    pos /= info.rate;
    printf("%ld\n", pos);
    exit(0);
}

static void play(char **argv)
{
    AudioDevice *dev;
    PlayStream playfile;
    const char *path = *argv;
    Audio::Linear buffer;
    Audio::Info info;
    unsigned bufcount, pages;

    dev = Audio::getDevice();

    if(!Audio::is_available() && !dev) {
        shell::errexit(10, "*** audiotool: %s\n",
            _TEXT("no device supported"));
    }
    else if(!dev) {
        shell::errexit(10, "*** audiotool: %s\n",
            _TEXT("device unavailable"));
    }

    playfile.open(argv);

    if(!playfile.is_open()) {
        shell::errexit(10, "*** audiotool: %s: %s\n",
            fname(path), _TEXT("unable to access"));
    }

    if(!playfile.is_streamable()) {
        shell::errexit(10, "*** audiotool: %s: %s\n",
            fname(path), _TEXT("missing codec"));
    }

    playfile.getInfo(&info);
    if(!dev->setAudio((Audio::Rate)info.rate, Audio::is_stereo(info.encoding), 10)) {
        shell::errexit(10, "*** audiotool: %s\n",
            _TEXT("sound device does not support rate"));
        exit(-1);
    }

    bufcount = playfile.getCount();
    if(Audio::is_stereo(info.encoding))
        buffer = new Audio::Sample[bufcount * 2];
    else
        buffer = new Audio::Sample[bufcount];

    for(;;) {
        if(Audio::is_stereo(info.encoding))
            pages = playfile.getStereo(buffer, 1);
        else
            pages = playfile.getMono(buffer, 1);

        if(!pages)
            break;

        dev->putSamples(buffer, bufcount);
    }

    dev->sync();
    delete dev;
    playfile.close();
    exit(0);
}

static void packetdump(char **argv)
{
    PacketStream packetfile;
    const char *path = *argv;
    Audio::Encoded buffer;
    Audio::Info info;
    ssize_t count;

    packetfile.open(argv);

    if(!packetfile.is_open()) {
        shell::errexit(2, "*** audiotool: %s: %s\n",
            fname(path), _TEXT("cannot access"));
    }

    if(!packetfile.is_streamable()) {
        shell::errexit(2, "*** audiotool: %s: %s\n",
            fname(path), _TEXT("missing codec needed"));
    }

    packetfile.getInfo(&info);

    buffer = new unsigned char[Audio::maxFramesize(info)];

    while((count = packetfile.getPacket(buffer)) > 0)
        printf("-- %ld\n", (long)count);

    delete[] buffer;
    packetfile.close();
    exit(0);
}


static void note(char **argv)
{
    char *fn = *(argv++);
    char *ann = NULL;
    AudioFile file, tmp;
    Audio::Info info;
    char target[256];
    unsigned char buffer[4096];
    int rtn;

    if(!fn) {
        shell::errexit(2, "*** audiotool: -notation: %s\n",
            _TEXT("no file specified"));
    }

    ann = *argv;

    file.open(fn, Audio::modeRead);
    if(!file.is_open()) {
        shell::errexit(4, "*** audiotool: %s: %s\n:",
            fname(fn), _TEXT("cannot access"));
    }
    file.getInfo(&info);
    if(info.annotation && !ann)
        printf("%s\n", info.annotation);
    if(!ann)
        exit(0);
    rewrite(fn, target, sizeof(target));
    info.annotation = ann;
    delfile = target;
    remove(target);
    tmp.create(target, &info);
    if(!tmp.is_open()) {
        shell::errexit(5, "*** audiotool: %s: %s\n",
            fname(target), _TEXT("unable to create"));
    }
    for(;;) {
        rtn = file.getBuffer(buffer, sizeof(buffer));
        if(!rtn)
            break;
        if(rtn < 0) {
            remove(target);
            shell::errexit(6, "*** audiotool: %s: %s\n",
                fname(fn), _TEXT("read failed"));
        }
        rtn = tmp.putBuffer(buffer, rtn);
        if(rtn < 1) {
            remove(target);
            shell::errexit(6, "*** audiotool: %s: %s\n",
                fname(fn), _TEXT("write failed"));
        }
    }
    file.close();
    tmp.close();
    rtn = rename(target, fn);
    remove(target);
    delfile = NULL;
    if(rtn) {
        shell::errexit(6, "*** audiotool: %s: %s\n",
            fname(fn), _TEXT("could not replace"));
    }

    exit(0);
}

static void build(char **argv)
{
    AudioBuild input;
    AudioStream output;
    Audio::Info info, make;
    const char *target;
    char *option;
    char *encoding = NULL;
    Audio::Rate rate = Audio::rateUnknown;

retry:
    if(!*argv) {
        shell::errexit(2, "*** audiotool: -build: %s\n",
            _TEXT("missing arguments"));
    }

    option = *argv;
    if(eq("--", option)) {
        ++argv;
        goto skip;
    }

    if(eq("--", option, 2))
        ++option;

    if(eq(option, "-encoding=", 10)) {
        encoding = option + 10;
        ++argv;
        goto retry;
    }

    if(eq(option, "-rate=", 6)) {
        rate = (Audio::Rate)atol(option + 6);
        ++argv;
        goto retry;
    }

    if(eq(option, "-encoding")) {
        ++argv;
        if(!*argv) {
            shell::errexit(3, "*** audiotool: -build: -encoding: %s\n",
                _TEXT("missing argument"));
        }
        encoding = *(argv++);
        goto retry;
    }

    if(eq(option, "-rate")) {
        ++argv;
        if(!*argv) {
            shell::errexit(3, "*** audiotool: -build: -rate: %s\n",
                _TEXT("missing argument"));
        }
        rate = (Audio::Rate)atol(*(argv++));
        goto retry;
    }

skip:
    if(*argv && **argv == '-') {
        shell::errexit(2, "*** auditool: -build: %s: %s\n",
            *argv, _TEXT("unknown option"));
    }

    if(*argv)
        target = *(argv++);

    if(!*argv) {
        shell::errexit(4, "*** audiotool: -build: %s\n",
            _TEXT("no files specified"));
    }

    input.open(argv);
    if(!input.is_open()) {
        shell::errexit(4, "*** audiotool: %s: %s\n",
            *argv, _TEXT("cannot access"));
    }
    input.getInfo(&info);
    input.getInfo(&make);
    remove(target);
    if(encoding)
        make.encoding = Audio::getEncoding(encoding);

    if(rate != Audio::rateUnknown)
        make.setRate(rate);

    output.create(target, &make, false, 10);
    if(!output.is_open()) {
        shell::errexit(5, "*** audiotool: %s: %s\n",
            target, _TEXT("cannot create"));
    }
    output.getInfo(&make);

    if(make.encoding == info.encoding && make.rate == info.rate)
        AudioBuild::copyDirect(input, output);
    else {
        if(!input.is_streamable()) {
            remove(target);
            shell::errexit(6, "*** audiotool: %s: %s\n",
                *argv, _TEXT("cannot load codec"));
        }
        if(!output.is_streamable()) {
            remove(target);
            shell::errexit(6, "*** audiotool: %s: %s\n",
                target, _TEXT("cannot load codec"));
        }
        AudioBuild::copyConvert(input, output);
    }
    input.close();
    output.close();
    exit(0);
}

static void append(char **argv)
{
    AudioBuild input;
    AudioStream output;
    Audio::Info info, make;
    const char *target;
    char *option;
    char *offset = NULL;

retry:
    option = *argv;

    if(eq("--", option)) {
        ++argv;
        goto skip;
    }

    if(eq("--", option, 2))
        ++option;

    if(eq(option, "-offset=", 8)) {
        offset = option + 8;
        ++argv;
        goto retry;
    }

    if(eq(option, "-offset")) {
        ++argv;
        if(!*argv) {
            shell::errexit(3, "*** audiotool: -append: -offset: %s\n",
                _TEXT("missing argument"));
            exit(-1);
        }
        offset = *(argv++);
        goto retry;
    }

skip:
    if(*argv && **argv == '-') {
        shell::errexit(2, "*** auditool: -append: %s: %s\n",
            *argv, _TEXT("unknown option"));
    }

    if(*argv)
        target = *(argv++);

    if(!*argv) {
        shell::errexit(4, "*** audiotool: -append: %s\n",
            _TEXT("no files specified"));
    }

    input.open(argv);
    if(!input.is_open()) {
        shell::errexit(4, "*** audiotool: %s: %s\n",
            *argv, _TEXT("cannot access"));
    }
    input.getInfo(&info);
    output.open(target, Audio::modeWrite, 10);
    if(!output.is_open()) {
        shell::errexit(4, "*** audiotool: %s: %s\n",
            target, _TEXT("cannot access"));
    }
    output.getInfo(&make);

    if(offset)
        output.setPosition(atol(offset));
    else
        output.setPosition();

    if(make.encoding == info.encoding)
        AudioBuild::copyDirect(input, output);
    else {
        if(!input.is_streamable()) {
            shell::errexit(6, "*** audiotool: %s: %s\n",
                *argv, _TEXT("cannot load codec"));
        }
        if(!output.is_streamable()) {
            shell::errexit(6, "*** audiotool: %s: %s\n",
                target, _TEXT("cannot load codec"));
        }
        AudioBuild::copyConvert(input, output);
    }
    input.close();
    output.close();
    exit(0);
}

PROGRAM_MAIN(argc, argv)
{
    shell::bind("audiotool");
    shell args(argc, argv);

    if(is(helpflag) || is(althelp)) {
        printf("%s\n", _TEXT("Usage: audiotool [options] command arguments..."));
        printf("%s\n\n", _TEXT("Audio tool and audio file operations"));
        printf("%s\n", _TEXT("Options:"));
        shell::help();
        printf("\n%s\n", _TEXT("Report bugs to dyfet@gnu.org"));
        PROGRAM_EXIT(0);
    }

    if(!args())
        shell::errexit(1, "*** audiotool: %s\n",
            _TEXT("no command specified"));

    Audio::init(args);
    const char *cp = args[0];

    argv = args.argv();
    ++argv;

    shell::exiting(stop);

    if(eq(cp, "version"))
        version();
    else if(eq(cp, "endian"))
        showendian();
    else if(eq(cp, "soundcard")) {
        if(*(argv))
            soundcard(atoi(*argv));
        else
            soundcard(0);
    }
    else if(eq(cp, "build"))
        build(argv);
    else if(eq(cp, "append"))
        append(argv);
    else if(eq(cp, "chart"))
        chart(argv);
    else if(eq(cp, "info"))
        info(argv);
    else if(eq(cp, "note") || eq(cp, "annotate") || eq(cp, "notation"))
        note(argv);
    else if(eq(cp, "packets") || eq(cp, "dump") || eq(cp, "packetdump"))
        packetdump(argv);
    else if(eq(cp, "play"))
        play(argv);
    else if(eq(cp, "strip"))
        strip(argv);
    else if(eq(cp, "trim"))
        trim(argv);
    else if(eq(cp, "size"))
        size(argv);
    else if(eq(cp, "codecs"))
        codecs();
    else if(eq(cp, "plugins"))
        plugins();

    shell::errexit(2, "*** audiotool: %s: %s\n",
        *argv, _TEXT("unknown option"));
    PROGRAM_EXIT(0);
}
