## phoon - PHase of the mOON v03b (6 Oct 2019)
Forked from  https://acme.com/software/phoon/

phoon - program to display the PHase of the mOON.  Unlike other
such programs, which just tell you how long since first quarter
or something like that, phoon *shows* you the phase with a little
picture.

This is a fork Jef Poskanzer's C source for Phoon (https://acme.com/software/phoon/). I've add some command line options 
which allow you to add/subract time from the command line along with the necessary bits needed to compile it for an Amiga.

### Amiga version

![Amiga phoon](https://github.com/mkillewald/phoon/blob/master/screenshots/amigaphoon.png)

Amiga style arguments:
```
L=LINES/K/N,M=MIN/K/N,H=HOUR/K/N,D=DAY/K/N,MO=MONTH/K/N,Y=YEAR/K/N,SD=SHOWDATE/S,HELP/S,DATETIME/F

phoon: PHase of the mOON 0.3 (6 Oct 2019)

Usage:
phoon [<options>] [<date> [<time>]]

Options:
   L, LINES n    n = lines to generate (default: 23)
   M, MIN n      n = +/- change in minutes
   H, HOUR n     n = +/- change in hours
   D, DAY n      n = +/- change in days
   MO, MONTH n   n = +/- change in months
   Y, YEAR n     n = +/- change in years
   SD, SHOWDATE  display date of moon phase being shown
   HELP          display this message

Where:
   <date>   = start date (DD MMM YYYY)
   <time>   = start time (HH:MM[:SS] [AM|PM] [TZ])

```

### Linux version

![Linux phoon](https://github.com/mkillewald/phoon/blob/master/screenshots/linuxphoon.png)

POSIX style arguments:
```
phoon: PHase of the mOON v03b (6 Oct 2019)

Usage:
phoon [<options>] [<date> [<time>]]

Options:
   -l n     n = lines to generate (default: 23)
   -m n     n = +/- change in minutes
   -h n     n = +/- change in hours
   -d n     n = +/- change in days
   -M n     n = +/- change in months
   -y n     n = +/- change in years
   -s       display date of moon phase being shown
   --help   display this message

Where:
   <date>   = start date (DD MMM YYYY)
   <time>   = start time (HH:MM[:SS] [AM|PM] [TZ])
```


### Contents of Jef's original README file:
```
		phoon - display current moon phase


phoon - program to display the PHase of the mOON.  Unlike other
such programs, which just tell you how long since first quarter
or something like that, phoon *shows* you the phase with a little
picture.  I've put an example at the end of this file.  I first
wrote this program in Pascal / TOPS-20 at CMU in 1979; I translated
it to Ratfor / Software Tools in 1981; and now it's in C / Unix.

Files in this distribution:

    README		this
    Makefile		guess
    phoon.c		phase of moon display
    astro.c		phase of moon calculations
    astro.h		header file
    phoon.1		manual for phase of moon program
    date_parse.c	date-parsing routine
    date_parse.h	header file

Unpack the files, edit Makefile and change the options to suit, make,
and enjoy!  Feedback is welcome - send bug reports, enhancements,
checks, money orders, etc. to the addresses below.

    Jef Poskanzer  jef@mail.acme.com  http://www.acme.com/jef/

                 .--
             .--
          .-' 
       .-'@ 
      /@@@ 
    ./    
   /@@  o
  /@@@@  
  |@@@@@
 /@@@@@ 		Last Quarter + 
 | @@@@ 		4  1:36:10
 |@ @@@			New Moon -     
 |      		3  7:34:53
 \  . @ 
  |     
  \     @
   \  o  
    `\    
      \    
       `-.  
          `-. 
             `--
                 `--

```
