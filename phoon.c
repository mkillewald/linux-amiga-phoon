/* phoon - show the phase of the moon
**
** ver  date   who remarks
** --- ------- --- -------------------------------------------------------------
** 03B 08oct19 MK  Mike Killewald <mkillewald yahoo com>
**                 Added #ifdef AMIGA (astro.c, date_parse.c, phoon.c)
**                 Added phasehunt6 (astro.c)
**                 Added hubert23, pumpkin23 (phoon.c)
**                 Added parse_args(), parse_amiga_args() (phoon.c)
**                 Added trim() (phoon.c)
**                 Added date_calc.c
**                 Updated putseconds() so it handles negative input (phoon.c)
**                 Updated putmoon() so tabs line up properly at all times
** 03A 01apr95 JP  Updated to use date_parse.
** 02A 07jun88 JP  Changed the phase calculation to use code from John Walker's
**                 "moontool", increasing accuracy tremendously.
**                 Got rid of SINFORCOS, since Apple has probably fixed A/UX
**                 by now.
** 01I 03feb88 JP  Added 32 lines.
** 01H 14jan88 JP  Added 22 lines.
** 01G 05dec87 JP  Added random sabotage to generate Hubert.
**                 Defeated text stuff for moons 28 or larger.
** 01F 13oct87 JP  Added pumpkin19 in October.  Added hubert29.
** 01E 14may87 JP  Added #ifdef SINFORCOS to handle broken A/UX library.
** 01D 02apr87 JP  Added 21 lines.
** 01C 26jan87 JP  Added backgrounds for 29 and 18 lines.
** 01B 28dec86 JP  Added -l flag, and backgrounds for 19 and 24 lines.
** 01A 08nov86 JP  Translated from the ratfor version of 12nov85, which itself
**                 was translated from the Pascal version of 05apr79.
**
** Copyright (C) 1986,1987,1988,1995 by Jef Poskanzer <jef@mail.acme.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#ifdef AMIGA
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <proto/dos.h>
#include <clib/exec_protos.h>
#include <libraries/mathlibrary.h>
#include <libraries/mathieeedp.h>
#include <clib/mathieeedoubbas_protos.h>
#include <clib/mathieeedoubtrans_protos.h>
#else
#include <unistd.h>
#endif /* AMIGA */

#include "date_calc.h"
#include "date_parse.h"
#include "astro.h"

/* Global defines and declarations. */
#define TITLE "phoon: PHase of the mOON"
#define VERSION "v03b"
#define VDATE "(8 Oct 2019)"

#ifdef AMIGA
#undef VERSION
#define VERSION "0.3"
#define VERSTAG "\0$VER: " TITLE " " VERSION
#define TEMPLATE "L=LINES/K/N,M=MIN/K/N,H=HOUR/K/N,D=DAY/K/N,MO=MONTH/K/N,Y=YEAR/K/N,SD=SHOWDATE/S,HELP/S,DATETIME/F"
#define USAGE "Usage:\n"\
              "%s [<options>] [<date> [<time>]]\n\n"\
              "Options:\n"\
              "   L, LINES n    n = lines to generate (default: 23)\n"\
              "   M, MIN n      n = +/- change in minutes\n"\
              "   H, HOUR n     n = +/- change in hours\n"\
              "   D, DAY n      n = +/- change in days\n"\
              "   MO, MONTH n   n = +/- change in months\n"\
              "   Y, YEAR n     n = +/- change in years\n"\
              "   SD, SHOWDATE  display date of moon phase being shown\n"\
              "   HELP          display this message\n\n"\
              "Where:\n"\
              "   <date>   = start date (DD MMM YYYY)\n"\
              "   <time>   = start time (HH:MM[:SS] [AM|PM] [TZ])\n\n"
#else
#define USAGE "Usage:\n"\
              "%s [<options>] [<date> [<time>]]\n\n"\
              "Options:\n"\
              "   -l n     n = lines to generate (default: 23)\n"\
              "   -m n     n = +/- change in minutes\n"\
              "   -h n     n = +/- change in hours\n"\
              "   -d n     n = +/- change in days\n"\
              "   -M n     n = +/- change in months\n"\
              "   -y n     n = +/- change in years\n"\
              "   -s       display date of moon phase being shown\n"\
              "   --help   display this message\n\n"\
              "Where:\n"\
              "   <date>   = start date (DD MMM YYYY)\n"\
              "   <time>   = start time (HH:MM[:SS] [AM|PM] [TZ])\n\n"
#endif /* AMIGA */

#define SECSPERMINUTE 60
#define SECSPERHOUR (60 * SECSPERMINUTE)
#define SECSPERDAY (24 * SECSPERHOUR)

#define KPI 3.1415926535897932384626433

/* If you change the aspect ratio, the canned backgrounds won't work. */
#define ASPECTRATIO 0.5

#define LINELENGTH 80
#define DEFAULTNUMLINES 23
#define MAXNUMLINES (LINELENGTH * ASPECTRATIO)

#ifdef AMIGA
/* parse Amiga style command line arguments */
static int
parse_amiga_args(char** argv, int* lines, LONG* dmin, LONG* dhour, LONG* dday,
                 LONG* dmonth, LONG* dyear, int* showdate, STRPTR datetime)
{
    struct RDArgs *rdargs;
    LONG  params[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    rdargs = ReadArgs(TEMPLATE, params, NULL);

    if (rdargs)
    {
        if (params[0])
        {
          *lines = *(int *)params[0];
        }
        if (params[1])
        {
          *dmin = *(LONG *)params[1];
        }
        if (params[2])
        {
          *dhour = *(LONG *)params[2];
        }
        if (params[3])
        {
          *dday = *(LONG *)params[3];
        }
        if (params[4])
        {
          *dmonth = *(LONG *)params[4];
        }
        if (params[5])
        {
          *dyear = *(LONG *)params[5];
        }
        if (params[6])
        {
          *showdate = 1;
        }
        if (params[7])
        {
          printf("%s %s %s\n\n", TITLE, VERSION, VDATE);
          printf(USAGE, argv[0]);
          return -1;
        }
        if (params[8])
        {
          memcpy(datetime, (STRPTR)params[8], 30);
        }

        FreeArgs(rdargs);
        return 0;
    }
    else
    {
        fprintf(stderr, "%s %s %s\n\n", TITLE, VERSION, VDATE);
        fprintf(stderr, "\n**Invalid arguments\n\n");
        fprintf(stderr, USAGE, argv[0]);

        return -1;
    }
}

#else

/* parse POSIX style commnand line arguments */
static int
parse_args(int argc, char** argv, int* lines, long* dmin, long* dhour,
               long* dday, long* dmonth, long* dyear, int* showdate, char* datetime)
{
    int opt, index;

    opterr = 0;

    while ((opt = getopt(argc, argv, "l:m:h:d:M:y:s")) != -1)
    {
        switch (opt)
        {
        case 'l':
            *lines = (int) strtol(optarg, NULL, 10);
            break;
        case 'm':
            *dmin = strtol(optarg, NULL, 10);
            break;
        case 'h':
            *dhour = strtol(optarg, NULL, 10);
            break;
        case 'd':
            *dday = strtol(optarg, NULL, 10);
            break;
        case 'M':
            *dmonth = strtol(optarg, NULL, 10);
            break;
        case 'y':
            *dyear = strtol(optarg, NULL, 10);
            break;
        case 's':
            *showdate = 1;
            break;
        case '?':
            fprintf(stderr, "%s %s %s\n\n", TITLE, VERSION, VDATE);

            if (optopt == 'l' || optopt == 'm' || optopt == 'h' || optopt == 'd' ||
                optopt == 'M' || optopt == 'y' )
              fprintf (stderr, "Option -%c requires an argument.\n\n", optopt);
            else if (optopt == '-') /* "--help" lands here */
              ; /* no operation */
            else if (isprint (optopt))
              fprintf (stderr, "Unknown option `-%c'.\n\n", optopt);
            else
              fprintf (stderr, "Unknown option character `\\x%x'.\n\n", optopt);

            fprintf (stderr, USAGE, argv[0]);
            return -1;
        default:
            abort();
        }
    }

    /* at this point, optind points to the first non option argument */
    for (index=optind; index < argc; index++)
    {
        if (strnlen(datetime, 30)>=30 || index > optind+5) break;
        strncat(datetime, argv[index], 8);
        strncat(datetime, " ", 1);
    }

    return 0;
}
#endif /* AMIGA */

static void
trim(char *str)
{
    int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace((unsigned char) str[begin]))
        begin++;

    while ((end >= begin) && isspace((unsigned char) str[end]))
        end--;

    /* Shift all characters back to the start of the string array. */
    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];

    str[i - begin] = '\0'; /* Null terminate string. */
}

static void
putseconds( long secs )
{
  long days, hours, minutes;

  if (secs < 0 )
  {
    days = -1 + (secs / SECSPERDAY);
    secs = (days * SECSPERDAY)*-1 + secs;
  } else {
    days = secs / SECSPERDAY;
    secs = secs - (days * SECSPERDAY);
  }
  hours = secs / SECSPERHOUR;
  secs = secs - (hours * SECSPERHOUR);
  minutes = secs / SECSPERMINUTE;
  secs = secs - (minutes * SECSPERMINUTE);

  printf( "%ld %2ld:%02ld:%02ld", days, hours, minutes, secs );
}


static void
putmoon( time_t t, int numlines, char* atfiller )
{
  static char background18[18][37] = {
    "             .----------.            ",
    "         .--'   o    .   `--.        ",
    "       .'@  @@@@@@ O   .   . `.      ",
    "     .'@@  @@@@@@@@   @@@@   . `.    ",
    "   .'    . @@@@@@@@  @@@@@@    . `.  ",
    "  / @@ o    @@@@@@.   @@@@    O   @\\ ",
    "  |@@@@               @@@@@@     @@| ",
    " / @@@@@   `.-.    . @@@@@@@@  .  @@\\",
    " | @@@@   --`-'  .  o  @@@@@@@      |",
    " |@ @@                 @@@@@@ @@@   |",
    " \\      @@    @   . ()  @@   @@@@@  /",
    "  |   @      @@@         @@@  @@@  | ",
    "  \\  .   @@  @\\  .      .  @@    o / ",
    "   `.   @@@@  _\\ /     .      o  .'  ",
    "     `.  @@    ()---           .'    ",
    "       `.     / |  .    o    .'      ",
    "         `--./   .       .--'        ",
    "             `----------'            "};
  static char background19[19][39] = {
    "              .----------.             ",
    "          .--'   o    .   `--.         ",
    "       .-'@  @@@@@@ O   .   . `-.      ",
    "     .' @@  @@@@@@@@   @@@@   .  `.    ",
    "    /     . @@@@@@@@  @@@@@@     . \\   ",
    "   /@@  o    @@@@@@.   @@@@    O   @\\  ",
    "  /@@@@                @@@@@@     @@@\\ ",
    " . @@@@@   `.-./    . @@@@@@@@  .  @@ .",
    " | @@@@   --`-'  .      @@@@@@@       |",
    " |@ @@        `      o  @@@@@@ @@@@   |",
    " |      @@        o      @@   @@@@@@  |",
    " ` .  @       @@     ()   @@@  @@@@   '",
    "  \\     @@   @@@@        . @@   .  o / ",
    "   \\   @@@@  @@\\  .           o     /  ",
    "    \\ . @@     _\\ /    .      .-.  /   ",
    "     `.    .    ()---        `-' .'    ",
    "       `-.    ./ |  .   o     .-'      ",
    "          `--./   .       .--'         ",
    "              `----------'             "};
  static char pumpkin19[19][39] = {
    "              @@@@@@@@@@@@             ",
    "          @@@@@@@@@@@@@@@@@@@@         ",
    "       @@@@@@@@@@@@@@@@@@@@@@@@@@      ",
    "     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ",
    "    @@@@        @@@@@@@@        @@@@   ",
    "   @@@@@@      @@@@@@@@@@      @@@@@@  ",
    "  @@@@@@@@    @@@@@@@@@@@@    @@@@@@@@ ",
    " @@@@@@@@@@  @@@@@@  @@@@@@  @@@@@@@@@@",
    " @@@@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@@@",
    " @@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@",
    " @@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@",
    " @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
    "  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ ",
    "   @@@@@                @@      @@@@@  ",
    "    @@@@@@                    @@@@@@   ",
    "     @@@@@@@@              @@@@@@@@    ",
    "       @@@@@@@@@        @@@@@@@@@      ",
    "          @@@@@@@@@@@@@@@@@@@@         ",
    "              @@@@@@@@@@@@             "};
  static char background21[21][43] = {
    "                .----------.               ",
    "           .---'   O   . .  `---.          ",
    "        .-'@ @@@@@@  .  @@@@@    `-.       ",
    "      .'@@  @@@@@@@@@  @@@@@@@   .  `.     ",
    "     /   o  @@@@@@@@@  @@@@@@@      . \\    ",
    "    /@  o   @@@@@@@@@.  @@@@@@@   O    \\   ",
    "   /@@@  .   @@@@@@o   @@@@@@@@@@     @@\\  ",
    "  /@@@@@            . @@@@@@@@@@@@@ o @@@\\ ",
    " .@@@@@ O  `.-./ .     @@@@@@@@@@@@    @@ .",
    " | @@@@   --`-'      o    @@@@@@@@ @@@@   |",
    " |@ @@@       `   o     .  @@  . @@@@@@@  |",
    " |      @@  @        .-.    @@@  @@@@@@@  |",
    " `  . @       @@@    `-'  . @@@@  @@@@  o '",
    "  \\     @@   @@@@@ .         @@  .       / ",
    "   \\   @@@@  @\\@@    /  . O   .    o  . /  ",
    "    \\o  @@     \\ \\  /       .   .      /   ",
    "     \\    .    .\\.-.___  .     .  .-. /    ",
    "      `.         `-'             `-'.'     ",
    "        `-.  o  / |    o   O  .  .-'       ",
    "           `---.    .    .  .---'          ",
    "                `----------'               "};
  static char background22[22][45] = {
    "                .------------.               ",
    "            .--'   o     . .  `--.           ",
    "         .-'    .    O   .      . `-.        ",
    "       .'@    @@@@@@@   .  @@@@@     `.      ",
    "     .'@@@  @@@@@@@@@@@   @@@@@@@  .   `.    ",
    "    /     o @@@@@@@@@@@   @@@@@@@      . \\   ",
    "   /@@  o   @@@@@@@@@@@.   @@@@@@@   O    \\  ",
    "  /@@@@   .   @@@@@@@o    @@@@@@@@@@    @@@\\ ",
    "  |@@@@@               . @@@@@@@@@@@@  @@@@| ",
    " /@@@@@  O  `.-./  .      @@@@@@@@@@@   @@  \\",
    " | @@@@    --`-'      o    . @@@@@@@ @@@@   |",
    " |@ @@@  @@  @ `   o  .-.     @@  . @@@@@@  |",
    " \\             @@@    `-'  .   @@@  @@@@@@  /",
    "  | . @  @@   @@@@@ .          @@@@  @@@ o | ",
    "  \\     @@@@  @\\@@    /  .  O   @@ .     . / ",
    "   \\  o  @@     \\ \\  /          . . o     /  ",
    "    \\      .    .\\.-.___   .  .  .  .-.  /   ",
    "     `.           `-'              `-' .'    ",
    "       `.    o   / |     o   O   .   .'      ",
    "         `-.    /     .      .    .-'        ",
    "            `--.        .     .--'           ",
    "                `------------'               "};
  static char background23[23][47] = {
    "                 .------------.                ",
    "             .--'  o     . .   `--.            ",
    "          .-'   .    O   .       . `-.         ",
    "       .-'@   @@@@@@@   .  @@@@@      `-.      ",
    "      /@@@  @@@@@@@@@@@   @@@@@@@   .    \\     ",
    "    ./    o @@@@@@@@@@@   @@@@@@@       . \\.   ",
    "   /@@  o   @@@@@@@@@@@.   @@@@@@@   O      \\  ",
    "  /@@@@   .   @@@@@@@o    @@@@@@@@@@     @@@ \\ ",
    "  |@@@@@               . @@@@@@@@@@@@@ o @@@@| ",
    " /@@@@@  O  `.-./  .      @@@@@@@@@@@@    @@  \\",
    " | @@@@    --`-'       o     @@@@@@@@ @@@@    |",
    " |@ @@@        `    o      .  @@   . @@@@@@@  |",
    " |       @@  @         .-.     @@@   @@@@@@@  |",
    " \\  . @        @@@     `-'   . @@@@   @@@@  o /",
    "  |      @@   @@@@@ .           @@   .       | ",
    "  \\     @@@@  @\\@@    /  .  O    .     o   . / ",
    "   \\  o  @@     \\ \\  /         .    .       /  ",
    "    `\\     .    .\\.-.___   .      .   .-. /'   ",
    "      \\           `-'                `-' /     ",
    "       `-.   o   / |     o    O   .   .-'      ",
    "          `-.   /     .       .    .-'         ",
    "             `--.       .      .--'            ",
    "                 `------------'                "};
  static char hubert23[23][47] = {
    "                 .------------.                ",
    "             .--'  o     . .   `--.            ",
    "          .-'   .    O   .       . `-.         ",
    "       .-'@   ___====-_  _-====___    `-.      ",
    "      / _--~~~#####//      \\\\#####~~~--_ \\     ",
    "    ./-~##########// (    ) \\\\##########~-\\.   ",
    "   /-############//  :\\^^/:  \\\\############-\\  ",
    "  /~############//   (G::R)   \\\\############~\\ ",
    "  |############((     \\\\//     ))############| ",
    " /##############\\\\    (oo)    //##############\\",
    " |###############\\\\  / \"\" \\  //###############|",
    " |################\\\\/      \\//################|",
    " |#########/\\######(   /\\   )######/\\#########|",
    " \\/\\#/\\#/\\/  \\#/\\##\\  :  :  /##/\\#/  \\/\\#/\\#/\\/",
    "  | \"  \" @@   \"  \\#\\: :  : :/#/  \"      \"  \" | ",
    "  \\     @@@@   \\@ / : :  : : \\         o   . / ",
    "   \\  o  @@     \\(  : :  : :  ).    .       /  ",
    "    `\\     .    __\\ : :  : : /__  .   .-. /'   ",
    "      \\        (vvv(VVV)(VVV)vvv)    `-' /     ",
    "        -.   o   / |     o    O   .   .-'      ",
    "          `-.   /     .       .    .-'         ",
    "             `--.       .      .--'            ",
    "                 `------------'                "};
  static char pumpkin23[23][47] = {
    "                 .------------.                ",
    "             .--'  o     . .   `--.            ",
    "          .-'   .    O   .       . `-.         ",
    "        -'@   @@@@@@@   .  @@@@@      `-.      ",
    "      /@@@  @@@@@@@@@@@   @@@@@@@   .    \\     ",
    "    ./   .--------.@@@@   @@@.--------. . \\.   ",
    "   /@@  o \\      /@@@@@.   @@@\\      /      \\  ",
    "   @@@@   .\\    /@@@@o    @@@@@\\    /    @@@ \\ ",
    "  |@@@@@    \\  /       . @@@@@@@\\  /@@ o @@@@| ",
    " /@@@@@  O  `\\/./  .   /\\ @@@@@@@\\/@@@    @@  \\",
    " | @@@@    --`-'      /  \\   @@@@@@@@ @@@@    |",
    " |@ @@@        `    o/    \\.  @@   . @@@@@@@  |",
    " |       @@  @      /      \\   @@@   @@@@@@@  |",
    " \\  . @        @@@ `--------'. @@@@   @@@@  o /",
    "  |      @@   @@@@@ .           @@   .       | ",
    "  \\     @____________________    ______o   . / ",
    "   \\  o  \\                   \\  /     /     /  ",
    "    `\\    \\                   \\/     /.-. /'   ",
    "      \\    `\\       /\\             /'`-' /     ",
    "       `-.   `-.___/  \\_________.-'   .-'      ",
    "          `-.   /             .    .-'         ",
    "             `--.       .      .--'            ",
    "                 `------------'                "};
  static char background24[24][49] = {
    "                  .------------.                 ",
    "             .---' o     .  .   `---.            ",
    "          .-'   .    O    .       .  `-.         ",
    "        .'@   @@@@@@@   .   @@@@@       `.       ",
    "      .'@@  @@@@@@@@@@@    @@@@@@@   .    `.     ",
    "     /    o @@@@@@@@@@@    @@@@@@@       .  \\    ",
    "    /@  o   @@@@@@@@@@@.    @@@@@@@   O      \\   ",
    "   /@@@   .   @@@@@@@o     @@@@@@@@@@     @@@ \\  ",
    "  /@@@@@               .  @@@@@@@@@@@@@ o @@@@ \\ ",
    "  |@@@@  O  `.-./  .       @@@@@@@@@@@@    @@  | ",
    " / @@@@    --`-'       o      @@@@@@@@ @@@@     \\",
    " |@ @@@     @  `           .   @@     @@@@@@@   |",
    " |      @           o          @      @@@@@@@   |",
    " \\       @@            .-.      @@@    @@@@  o  /",
    "  | . @        @@@     `-'    . @@@@           | ",
    "  \\      @@   @@@@@ .            @@   .        / ",
    "   \\    @@@@  @\\@@    /  .   O    .     o   . /  ",
    "    \\ o  @@     \\ \\  /          .    .       /   ",
    "     \\     .    .\\.-.___    .      .   .-.  /    ",
    "      `.          `-'                 `-' .'     ",
    "        `.   o   / |      o    O   .    .'       ",
    "          `-.   /      .       .     .-'         ",
    "             `---.       .      .---'            ",
    "                  `------------'                 "};
  static char background29[29][59] = {
    "                      .--------------.                     ",
    "                 .---'  o        .    `---.                ",
    "              .-'    .    O  .         .   `-.             ",
    "           .-'     @@@@@@       .             `-.          ",
    "         .'@@   @@@@@@@@@@@       @@@@@@@   .    `.        ",
    "       .'@@@  @@@@@@@@@@@@@@     @@@@@@@@@         `.      ",
    "      /@@@  o @@@@@@@@@@@@@@     @@@@@@@@@     O     \\     ",
    "     /        @@@@@@@@@@@@@@  @   @@@@@@@@@ @@     .  \\    ",
    "    /@  o      @@@@@@@@@@@   .  @@  @@@@@@@@@@@     @@ \\   ",
    "   /@@@      .   @@@@@@ o       @  @@@@@@@@@@@@@ o @@@@ \\  ",
    "  /@@@@@                  @ .      @@@@@@@@@@@@@@  @@@@@ \\ ",
    "  |@@@@@    O    `.-./  .        .  @@@@@@@@@@@@@   @@@  | ",
    " / @@@@@        --`-'       o        @@@@@@@@@@@ @@@    . \\",
    " |@ @@@@ .  @  @    `    @            @@      . @@@@@@    |",
    " |   @@                         o    @@   .     @@@@@@    |",
    " |  .     @   @ @       o              @@   o   @@@@@@.   |",
    " \\     @    @       @       .-.       @@@@       @@@      /",
    "  |  @    @  @              `-'     . @@@@     .    .    | ",
    "  \\ .  o       @  @@@@  .              @@  .           . / ",
    "   \\      @@@    @@@@@@       .                   o     /  ",
    "    \\    @@@@@   @@\\@@    /        O          .        /   ",
    "     \\ o  @@@       \\ \\  /  __        .   .     .--.  /    ",
    "      \\      .     . \\.-.---                   `--'  /     ",
    "       `.             `-'      .                   .'      ",
    "         `.    o     / | `           O     .     .'        ",
    "           `-.      /  |        o             .-'          ",
    "              `-.          .         .     .-'             ",
    "                 `---.        .       .---'                ",
    "                      `--------------'                     "};
  static char hubert29[29][59] = {
    "                      .--------------.                     ",
    "                 .---'  o        .    `---.                ",
    "              .-'    .    O  .         .   `-.             ",
    "           .-'     @@@@@@       .             `-.          ",
    "         .'@@   @@@@@@@@@@@       @@@@@@@   .    `.        ",
    "       .'@@@  @@@@@ ___====-_  _-====___ @         `.      ",
    "      /@@@  o _--~~~#####//      \\\\#####~~~--_ O     \\     ",
    "     /     _-~##########// (    ) \\\\##########~-_  .  \\    ",
    "    /@  o -############//  :\\^^/:  \\\\############-  @@ \\   ",
    "   /@@@ _~############//   (@::@)   \\\\############~_ @@ \\  ",
    "  /@@@ ~#############((     \\\\//     ))#############~ @@ \\ ",
    "  |@@ -###############\\\\    (oo)    //###############- @ | ",
    " / @ -#################\\\\  / \"\" \\  //#################- . \\",
    " |@ -###################\\\\/      \\//###################-  |",
    " | _#/:##########/\\######(   /\\   )######/\\##########:\\#_ |",
    " | :/ :#/\\#/\\#/\\/  \\#/\\##\\  :  :  /##/\\#/  \\/\\#/\\#/\\#: \\: |",
    " \\ \"  :/  V  V  \"   V  \\#\\: :  : :/#/  V   \"  V  V  \\:  \" /",
    "  | @ \"   \"  \"      \"   / : :  : : \\   \"      \"  \"   \"   | ",
    "  \\ .  o       @  @@@@ (  : :  : :  )  @@  .           . / ",
    "   \\      @@@    @@@@ __\\ : :  : : /__            o     /  ",
    "    \\    @@@@@   @@\\@(vvv(VVV)(VVV)vvv)       .        /   ",
    "     \\ o  @@@       \\ \\  /  __        .   .     .--.  /    ",
    "      \\      .     . \\.-.---                   `--'  /     ",
    "       `.             `-'      .                   .'      ",
    "         `.    o     / | `           O     .     .'        ",
    "           `-.      /  |        o             .-'          ",
    "              `-.          .         .     .-'             ",
    "                 `---.        .       .---'                ",
    "                      `--------------'                     "};
  static char background32[32][65] = {
    "                         .--------------.                        ",
    "                   .----'  o        .    `----.                  ",
    "                .-'     .    O  .          .   `-.               ",
    "             .-'      @@@@@@       .              `-.            ",
    "           .'@     @@@@@@@@@@@       @@@@@@@@    .   `.          ",
    "         .'@@    @@@@@@@@@@@@@@     @@@@@@@@@@         `.        ",
    "       .'@@@ o   @@@@@@@@@@@@@@     @@@@@@@@@@      o    `.      ",
    "      /@@@       @@@@@@@@@@@@@@  @   @@@@@@@@@@  @@     .  \\     ",
    "     /            @@@@@@@@@@@   .  @@   @@@@@@@@@@@@     @@ \\    ",
    "    /@  o     .     @@@@@@ o       @   @@@@@@@@@@@@@@ o @@@@ \\   ",
    "   /@@@                        .       @@@@@@@@@@@@@@@  @@@@@ \\  ",
    "  /@@@@@                     @      .   @@@@@@@@@@@@@@   @@@   \\ ",
    "  |@@@@@     o      `.-./  .             @@@@@@@@@@@@ @@@    . | ",
    " / @@@@@           __`-'       o          @@       . @@@@@@     \\",
    " |@ @@@@ .        @    `    @            @@    .     @@@@@@     |",
    " |   @@       @                    o       @@@   o   @@@@@@.    |",
    " |          @                             @@@@@       @@@       |",
    " |  . .  @      @  @       o              @@@@@     .    .      |",
    " \\            @                .-.      .  @@@  .           .   /",
    "  |    @   @   @      @        `-'                     .       / ",
    "  \\   .      @   @                   .            o            / ",
    "   \\     o          @@@@   .                .                 /  ",
    "    \\       @@@    @@@@@@        .                    o      /   ",
    "     \\     @@@@@   @@\\@@    /         o           .         /    ",
    "      \\  o  @@@       \\ \\  /  ___         .   .     .--.   /     ",
    "       `.      .       \\.-.---                     `--'  .'      ",
    "         `.             `-'       .                    .'        ",
    "           `.    o     / |              O      .     .'          ",
    "             `-.      /  |         o              .-'            ",
    "                `-.           .         .      .-'               ",
    "                   `----.        .       .----'                  ",
    "                         `--------------'                        "};

  static char qlits[4][16] = {
    "New Moon +     ",
    "First Quarter +",
    "Full Moon +    ",
    "Last Quarter + "};
  static char nqlits[4][16] = {
    "New Moon -     ",
    "First Quarter -",
    "Full Moon -    ",
    "Last Quarter - "};

  struct tm* tmP;
  double jd, pctphase, angphase, cphase, aom, cdist, cangdia, csund, csuang;
  double phases[2], which[2];
  long clocknow;
  int atflrlen, atflridx, numcols, lin, col, midlin;
  double mcap, yrad, xrad, y, xright, xleft;
  int colright, colleft, i, j;
  char c, line[LINELENGTH], infoline[4][LINELENGTH];

  int maxline = 0;

  /* Find the length of the atfiller string. */
  atflrlen = strlen( atfiller );

  /* Figure out the phase. */
  jd = unix_to_julian( t );
  pctphase = phase( jd, &cphase, &aom, &cdist, &cangdia, &csund, &csuang );
  angphase = pctphase * 2.0 * KPI;
  mcap = -cos( angphase );

  /* Get now for use as a random number. */
  (void) time( (time_t *)&clocknow );

  /* Randomly cheat and generate Hubert. */
  if ( clocknow % 13 == 3 && cphase > 0.8 )
  {
    clocknow = 3;
  }

  /* Figure out how big the moon is. */
  yrad = numlines / 2.0;
  xrad = yrad / ASPECTRATIO;
  numcols = xrad * 2;

  /* Figure out some other random stuff. */
  midlin = numlines / 2;
  phasehunt2( jd, phases, which );

  /* Now build the moon, a slice at a time. */
  atflridx = 0;

  for ( lin = 0; lin < numlines; lin++ )
  {
    line[0] = '\0';
    /* Compute the edges of this slice. */
    y = lin + 0.5 - yrad;
    xright = xrad * sqrt( 1.0 - ( y * y ) / ( yrad * yrad ) );
    xleft = -xright;
    if ( angphase >= 0.0 && angphase < KPI )
    {
      xleft = mcap * xleft;
    }
    else
    {
      xright = mcap * xright;
    }
    colleft = (int) (xrad + 0.4999) + (int) (xleft + 0.4999);
    colright = (int) (xrad + 0.4999) + (int) (xright + 0.4999);

    /* Now build the slice. */
    for ( col = 0; col < colleft; ++col )
    {
      strcat(line, " ");
    }
    for ( ; col <= colright; ++col )
    {
      switch ( numlines )
      {
      case 18:
        c = background18[lin][col];
        break;
      case 19:
        tmP = localtime( &t );
        if ( tmP->tm_mon == 9 && clocknow % ( 33 - tmP->tm_mday ) == 1 && cphase > 0.6 )
        {
          c = pumpkin19[lin][col];
        }
        else
        {
          c = background19[lin][col];
        }
        break;
      case 21:
        c = background21[lin][col];
        break;
      case 22:
        c = background22[lin][col];
        break;
      case 23:
        tmP = localtime( &t );
        if ( tmP->tm_mon == 9 && clocknow % ( 33 - tmP->tm_mday ) == 1 && cphase > 0.6 )
        {
          c = pumpkin23[lin][col];
        }
        else if ( clocknow % 23 == 3 && cphase > 0.8 )
        {
          c = hubert23[lin][col];
        }
        else
        {
          c = background23[lin][col];
        }
        break;
      case 24:
        c = background24[lin][col];
        break;
      case 29:
        if ( clocknow % 23 == 3 )
        {
          c = hubert29[lin][col];
        }
        else
        {
          c = background29[lin][col];
        }
        break;
      case 32:
        c = background32[lin][col];
        break;
      default:
        c = '@';
      }

      if ( c != '@' )
      {
        strncat(line, &c, 1);
      }
      else
      {
        strncat(line, &atfiller[atflridx], 1);
        atflridx = ( atflridx + 1 ) % atflrlen;
      }
    }

    #ifdef notdef
    for ( ; col <= numcols; ++col )
    {
      strcat(line, " ");
    }
    #endif /* notdef */

    /* check if this line is an info line */
    if ( numlines <= 27 && lin >= midlin - 2 && lin <= midlin + 1 )
    {
      /* this is an info line, get length and compare to maxlength */
      #ifdef AMIGA
      if (strlen(line) > maxline )
      {
        maxline = strlen(line);
      }
      #else
      if (strnlen(line, LINELENGTH) > maxline )
      {
        maxline = strnlen(line, LINELENGTH);
      }
          #endif /* AMIGA */

      /* store line until all 4 info lines have been measured */
      strncpy(infoline[2 + lin - midlin], line, LINELENGTH);

      /* check if we have reached the last info line */
      if ( lin == midlin + 1 )
      {
        /* last info line reached, process all four info lines */
        for ( i = 0; i <= 3; i++ )
        {
          /* normalize length of the info lines */
          #ifdef AMIGA
          if ( strlen(infoline[i]) < maxline )
          {
            for ( j=1; (j=maxline-strlen(infoline[i])); j++ )
            {
              strcat(infoline[i], " ");
            }
          }
          #else
          if ( strnlen(infoline[i], LINELENGTH) < maxline )
          {
            for ( j=1; (j=maxline-strnlen(infoline[i], LINELENGTH)); j++ )
            {
              strcat(infoline[i], " ");
            }
          }
          #endif /* AMIGA */

          /* display moon line and a trailing tab with a space */
          fputs( infoline[i], stdout );
          fputs("\t ", stdout);

          /* display the end-of-line information. */
          if ( i == 0 )
          {
            fputs( qlits[(int) (which[0] * 4.0 + 0.001)], stdout );
          }
          else if ( i == 1 )
          {
            putseconds( (int) ( ( jd - phases[0] ) * SECSPERDAY ) );
          }
          else if ( i == 2 )
          {
            fputs( nqlits[(int) (which[1] * 4.0 + 0.001)], stdout );
          }
          else if ( i == 3 )
          {
            putseconds( (int) ( ( phases[1] - jd ) * SECSPERDAY ) );
          }

          putchar('\n');
        }
      }
    } else {
      /* not an info line, display moon line */
      fputs(line, stdout);
      putchar('\n');
    }
  }
}

/* Main program. */

int
main( int argc, char** argv )
{
  time_t t, start_time;
  int numlines, showdate;
  long dmin, dhour, dday, dmonth, dyear;
  char datetime[30], adj_time_str[50];
  struct tm *tm;
  
  #ifdef AMIGA
  const char __ver[40] = VERSTAG;
  #endif
  
  numlines = DEFAULTNUMLINES;
  dmin = 0;
  dhour = 0;
  dday = 0;
  dmonth = 0;
  dyear = 0;
  showdate = 0;
  datetime[0] = '\0';

  #ifdef AMIGA /* parse Amiga style commamnd line args */
  if (parse_amiga_args(argv, &numlines, &dmin, &dhour, &dday, &dmonth, &dyear, &showdate, (STRPTR)datetime) != 0)
      exit(EXIT_FAILURE);
  #else /* parse POSIX style command line args */
  if (parse_args(argc, argv, &numlines, &dmin, &dhour, &dday, &dmonth, &dyear, &showdate, datetime) != 0)
      exit(EXIT_FAILURE);
  #endif /* AMIGA */

  if ( numlines >= MAXNUMLINES ) numlines = MAXNUMLINES-1;

  if (datetime[0] != '\0')
    /* start time supplied from command line */
    start_time = date_parse(datetime);
  else
    /* start time is current time */
    start_time= time(NULL);

  if (start_time == ((time_t)-1))
  {
    fprintf(stderr, "%s %s %s\n\n", TITLE, VERSION, VDATE);
    fprintf(stderr, "Failure to obtain the start time.\n\n");
    fprintf(stderr, USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Represent the date as struct tm  */
  /*tm = gmtime(&start_time);*/
  tm = localtime(&start_time);

  date_calc(tm, dmin, dhour, dday, dmonth, dyear);

  if (showdate == 1)
  {
    #ifdef AMIGA
    /*strftime(adj_time_str, sizeof(adj_time_str), "%e %b %Y %I:%M:%S %p GMT", at);*/
    strftime(adj_time_str, sizeof(adj_time_str), "%e %b %Y %I:%M:%S %p %Z", tm);
    #else
    strftime(adj_time_str, sizeof(adj_time_str), "%e %b %Y %I:%M:%S %p %Z", tm);
    #endif /* AMIGA */

    trim(adj_time_str);
    printf("%s\n", adj_time_str);
  }

  t = mktime(tm);

  /* Pseudo-randomly decide what the moon is made of, and print it. */
  if ( t % 17 == 3 )
  {
    putmoon( t, numlines, "GREENCHEESE" );
  }
  else
  {
    putmoon( t, numlines, "@" );
  }

  /* All done. */
  exit( 0 );
}
