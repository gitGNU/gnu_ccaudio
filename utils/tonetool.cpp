// Copyright (C) 2006-2011 David Sugar, Tycho Softworks.
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

#include <ccaudio2.h>
#include <config.h>

using namespace UCOMMON_NAMESPACE;

static const char *delfile = NULL;
static shell::flagopt helpflag('h',"--help",    _TEXT("display this list"));
static shell::flagopt althelp('?', NULL, NULL);
static shell::stringopt encoding('e', "--encoding", _TEXT("audio format"), "type", "pcmu");
static shell::numericopt framing('f', "--framing", _TEXT("audio framing"), "msec", 20);
static shell::numericopt interdigit('i', "--interdigit", _TEXT("interdigit timing"), "msec", 60);
static shell::numericopt level('l', "--level", _TEXT("audio level"), "level", 30000);
static shell::numericopt maxtime('m', "--max", _TEXT("max time"), "msec", 60000);
static shell::numericopt offset('s', "--offset", _TEXT("offset into stream"), "msec", 0);

static AudioTone *getTone(char **argv, Audio::Level l, timeout_t framing, timeout_t interdigit)
{
    TelTone::tonekey_t *key;
    char *name, *locale;

    if(!stricmp(*argv, "dtmf"))
        return new DTMFTones(*(++argv), l, framing, interdigit);
    else if(!stricmp(*argv, "mf"))
        return new MFTones(*(++argv), l, framing, interdigit);

    name = *(argv++);
    locale = *(argv);
    key = TelTone::find(name, locale);
    if(!key)
        return NULL;

    return new TelTone(key, l, framing);
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

void writeTones(char **argv, bool append)
{
    AudioTone *tone;
    AudioStream output;
    Audio::Info info, make;
    const char *target;
    unsigned maxframes;
    Audio::Linear buffer;
    char *filename = (char *)"tones.conf";

    TelTone::load(filename);

    if(*argv)
        target = *(argv++);

    if(!*argv) {
        shell::errexit(3, "*** tonetool: %s\n",
            _TEXT("no tone spec to use"));
    }

    if(!append)
        make.encoding = Audio::getEncoding(*encoding);

    make.rate = Audio::rate8khz;

    if(!append) {
        remove(target);
        output.create(target, &make, false, *framing);
    }
    else {
        output.open(target, Audio::modeWrite, *framing);
        if(is(offset))
            output.setPosition(*offset);
        else
            output.setPosition();
    }

    if(!output.is_open()) {
        shell::errexit(3, "*** tonetool: %s: %s\n",
            fname(target), _TEXT("cannot access"));
    }

    if(!output.is_streamable()) {
        shell::errexit(4, "*** tonetool: %s: %s\n",
            fname(target), _TEXT("unable to load codec"));
    }

    output.getInfo(&info);

    tone = getTone(argv, ((Audio::Level)*level), info.framing, *interdigit);

    if(!tone) {
        shell::errexit(5, "*** tonetool: %s\n",
            _TEXT("unrecognized tone spec used"));
    }

    printf("%s: ", fname(target));
    maxframes = *maxtime / info.framing;

    while(maxframes--) {
        buffer = tone->getFrame();
        if(!buffer) {
            if(tone->is_complete())
                break;
            printf("!");
            continue;
        }
        output.putMono(buffer);
        if(tone->is_silent())
            printf(".");
        else
            printf("+");
    }
    printf("\n");
    output.close();
    exit(0);
}


void listTones(char **argv)
{
    const char *filename = "tones.conf";
    const char *locale = NULL;
    const char *option;
    TelTone::tonekey_t *key;
    TelTone::tonedef_t *def;

retry:
    option = *argv;

    if(eq("--", option)) {
        ++argv;
        goto skip;
    }

    if(eq("--", option, 2))
        ++option;

    if(eq(option, "-file=", 6)) {
        filename = option + 6;
        ++argv;
        goto retry;
    }

    if(eq(option, "-file")) {
        ++argv;
        if(!(*argv)) {
            shell::errexit(2, "*** tonetool: -file: %s\n",
                _TEXT("missing argument"));
        }

        filename = *(argv++);
        goto retry;
    }

    if(eq(option, "-locale=", 8)) {
        locale = option + 8;
        ++argv;
        goto retry;
    }

    if(eq(option, "-locale")) {
        ++argv;
        if(!(*argv)) {
            shell::errexit(2, "*** tonetool: -locale: %s\n",
                _TEXT("missing argument"));
        }

        locale = *(argv++);
        goto retry;
    }


skip:
    if(*argv && **argv == '-') {
        shell::errexit(1, "*** tonetool: %s: %s\n",
            *argv, _TEXT("unknown option"));
    }

    if(!*argv) {
        shell::errexit(3, "*** tonetool: %s\n",
            _TEXT("tone name missing"));
    }

    if(!TelTone::load(filename)) {
        shell::errexit(4, "*** tonetool: %s: %s\n",
            fname(filename), _TEXT("unable to load"));
    }

    while(*argv) {
        printf("%s:", *argv);
        key = TelTone::find(*(argv++), locale);
        if(!key) {
            printf(" %s\n", _TEXT("not found"));
            continue;
        }
        printf("\n");
        def = key->first;
        while(def) {
            printf("   ");
            if(def->f2 && def->f1)
                printf("%d+%d", def->f1, def->f2);
            else
                printf("%d", def->f1);

            if(def->duration)
                printf(" %ldms\n", def->duration);
            else {
                printf(" forever\n");
                break;
            }

            if(def->silence)
                printf("   silence %ldms\n", def->silence);

            if(def == key->last) {
                if(def->next == key->first)
                    printf("   repeat full\n");
                else if(def->next)
                    printf("   repeat partial\n");
                break;
            }
            def = def->next;
        }
    }
    exit(0);
}

void toneDetect(char **argv)
{
    DTMFDetect *dtmf;
    AudioStream input;
    Audio::Info info, make;
    const char *target;
    Audio::Linear buffer;
    char result[128];

    while(*argv) {
        target = *(argv++);

        input.open(target, Audio::modeRead, *framing);

        if(!input.is_open()) {
            shell::errexit(3, "*** tonetool: %s: %s\n",
                fname(target), _TEXT("cannot access"));
        }

        if(!input.is_streamable()) {
            shell::errexit(4, "*** tonetool: %s: %s\n",
                fname(target), _TEXT("unable to load codec"));
        }

        input.getInfo(&info);
        dtmf = new DTMFDetect;

        buffer = new Audio::Sample[input.getCount()];
        for(;;)
        {
            if(!input.getMono(buffer, 1))
                break;

            dtmf->putSamples(buffer, input.getCount());
        }

        dtmf->getResult(result, sizeof(result));
        printf("%s: %s\n", fname(target), result);

        input.close();
        delete[] buffer;
        delete dtmf;
    }
    exit(0);
}

static void stop(void)
{
    if(delfile) {
        ::remove(delfile);
        delfile = NULL;
    }
}

PROGRAM_MAIN(argc, argv)
{
    shell::bind("tonetool");
    shell args(argc, argv);

    if(is(helpflag) || is(althelp)) {
        printf("%s\n", _TEXT("Usage: tonetool [options] command [arguments...]"));
        printf("%s\n\n", _TEXT("Tone tool operations"));
        printf("%s\n", _TEXT("Options:"));
        shell::help();
        printf("\n%s\n", _TEXT("Report bugs to dyfet@gnu.org"));
        PROGRAM_EXIT(0);
    }

    if(!args())
        shell::errexit(1, "*** tonetool: %s\n",
            _TEXT("no command specified"));

    Audio::init(args);
    const char *cp = args[0];

    argv = args.argv();
    ++argv;

    shell::exiting(stop);

    if(eq(cp, "list"))
        listTones(argv);
    else if(eq(cp, "detect"))
        toneDetect(argv);
    else if(eq(cp, "create"))
        writeTones(argv, false);
    else if(eq(cp, "append"))
        writeTones(argv, true);

    shell::errexit(1, "*** tonetool: %s: %s\n",
        cp, _TEXT("unknown option"));

    PROGRAM_EXIT(0);
}

