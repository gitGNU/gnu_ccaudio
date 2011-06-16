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

using namespace UCOMMON_NAMESPACE;

static shell::flagopt helpflag('h',"--help",    _TEXT("display this list"));
static shell::flagopt althelp('?', NULL, NULL);
static shell::stringopt lang('l', "--lang", _TEXT("specify language"), "language", "C");
static AudioRule *ruleset;

static void display(char **args)
{
    union {
        audiorule_t rule;
        char buf[1024];
    } state;

    while(*args) {
        AudioRule::init(&state.rule, sizeof(state));
        const char **out = &state.rule.list[0];
        const char *arg = *(args++);

        if(eq(arg, "-number=", 8))
            ruleset->number(arg + 8, &state.rule);
        else if(eq(arg, "-spell=", 7))
            ruleset->spell(arg + 7, &state.rule);
        else
            ruleset->literal(arg, &state.rule);

        if(*out == NULL)
            printf("*** %s: failed", arg);
        else while(*out)
            printf("%s ", *(out++));
        printf("\n");
    }
}

PROGRAM_MAIN(argc, argv)
{
    shell::bind("phrasebook");
    shell args(argc, argv);

    if(is(helpflag) || is(althelp)) {
        printf("%s\n", _TEXT("Usage: phrasebook [options] command arguments..."));
        printf("%s\n\n", _TEXT("Phrasebook operations"));
        printf("%s\n", _TEXT("Options:"));
        shell::help();
        printf("\n%s\n", _TEXT("Report bugs to dyfet@gnu.org"));
        PROGRAM_EXIT(0);
    }

    if(!args())
        shell::errexit(1, "*** phrasebook: %s\n",
            _TEXT("no command specified"));

    Audio::init();
    const char *cmd = args[0];

    argv = args.argv();
    ++argv;

    if(is(lang))
        ruleset = AudioRule::find(*lang);
    else
        ruleset = AudioRule::find(NULL);

    if(!ruleset)
        shell::errexit(3, "*** phrasebook: %s: %s\n",
            *lang, _TEXT("language not found"));

    if(case_eq(cmd, "display"))
        display(argv);
    else
        shell::errexit(2, "*** phrasebook: %s: %s\n",
            cmd, _TEXT("unknown command"));

    PROGRAM_EXIT(0);
}

