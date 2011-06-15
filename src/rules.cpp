// Copyright (C) 2005 David Sugar, Tycho Softworks.
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
#include <ctype.h>
#include <ucommon/export.h>
#include <ccaudio2.h>

using namespace UCOMMON_NAMESPACE;

static LinkedObject *primary = NULL;
static LinkedObject *secondary = NULL;

AudioRule::AudioRule(bool pri) :
LinkedObject()
{
    if(pri)
        enlist(&primary);
    else
        enlist(&secondary);
}

static class __LOCAL _default_ : public AudioRule
{
public:
    _default_();

    bool id(const char *lang);

} _default_rule;

AudioRule *AudioRule::find(const char *lang)
{
    linked_pointer<AudioRule> rp;

    if(!lang) {
#ifdef  _MSWINDOWS_
#else
        lang = getenv("LANG");
#endif
    }
    if(!lang)
        return &_default_rule;

    rp = secondary;
    while(is(rp)) {
        if(rp->id(lang))
            return *rp;
        rp.next();
    }

    rp = primary;
    while(is(rp)) {
        if(rp->id(lang))
            return *rp;
        rp.next();
    }

    return &_default_rule;
}

_default_::_default_() :
AudioRule(true)
{
}

bool _default_::id(const char *lang)
{
    if(eq(lang, "en_", 3))
        return true;

    if(eq(lang, "C"))
        return true;

    if(eq(lang, "en"))
        return true;

    return false;
}

static void _add(const char *text, audiorule_t *state)
{
    if(state->pos >= state->max - 1)
        state->list[0] = NULL;
    else {
        state->list[state->pos++] = text;
        state->list[state->pos] = NULL;
    }
}

static const char *_0to19[] =
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
     "10", "11", "12", "13", "14", "15", "16", "17", "18", "19"};

static const char *_tens[] =
    {"", "10", "20", "30", "40", "50", "60", "70", "80", "90"};

static void _lownumber(int num, audiorule_t *state)
{
    if(num >= 100)
    {
        _add(_0to19[num / 100], state);
        _add("hundred", state);
        num %= 100;
        if(!num)
            return;
    }
    if(num < 20)
    {
        _add(_0to19[num], state);
        return;
    }
    _add(_tens[num / 10], state);
    if(num % 10)
        _add(_0to19[num % 10], state);
}

void AudioRule::number(const char *text, audiorule_t *state)
{
    unsigned long num;

    state->zeroflag = true;

    if(!text || !*text)
        return;

    if(*text == '-')
    {
        _add("negative", state);
        ++text;
    }
    num = atol(text);
    if(num > 999999999)
    {
        state->zeroflag = false;
        _lownumber(num / 1000000000, state);
        _add("billion", state);
        num %= 1000000000;
    }
    if(num > 999999)
    {
        state->zeroflag = false;
        _lownumber(num / 1000000, state);
        _add("million", state);
        num %= 1000000;
    }
    if(num > 999)
    {
        state->zeroflag = false;
        _lownumber(num / 1000, state);
        _add("thousand", state);
        num %= 1000;
    }
    if(num || state->zeroflag)
        _lownumber(num, state);

    if(num)
        state->zeroflag = false;

    text = strchr(text, '.');
    if(!text)
        return;
    _add("point", state);
    while(*(++text)) {
        if(*text >= '0' && *text <= '9')
            _add(_0to19[(*text - '0')], state);
    }
    state->zeroflag = false;
}

void AudioRule::order(const char *text, audiorule_t *state)
{
    static const char *low[] = { "th",
        "1st", "2nd", "3rd", "4th", "5th",
        "6th", "7th", "8th", "9th", "10th",
        "11th", "12th", "13th", "14th", "15th",
        "16th", "17th", "18th", "19th"};

    static const char *hi[] = {"", "", "20th", "30th", "40th", "50th", "60th", "70th", "80th", "90th"};

    unsigned num = atoi(text);

    if(num > 100)
    {
        if(num % 100) {
            _add(_0to19[num / 100], state);
            _add("hundred", state);
        }
        else {
            _add(_0to19[num / 100], state);
            _add("hundred", state);
            _add("th", state);
        }
        num %= 100;
    }
    if(num > 19)
    {
        if(num % 10)
            _add(_tens[num / 10], state);
        else
            _add(hi[num / 10], state);
        num %= 10;
    }
    if(num)
        _add(low[num], state);
}

