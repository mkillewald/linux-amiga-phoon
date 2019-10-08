### Phases of the moon
Forked from  https://acme.com/software/phoon/

This is a fork Jef Poskanzer's C source for Phoon (https://acme.com/software/phoon/). I've add some command line options 
which allow you to add/subract time from the command line along with the necessary bits needed to compile it for an Amiga.

Mike Killewald 

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

    Jef Poskanzer  http://www.acme.com/jef/

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
