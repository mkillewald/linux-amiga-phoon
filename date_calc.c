/* date_calc - add minutes, hours, days, months or years to a given date
**
** Copyright © 2019 by Mike Killewald <mkillewald yahoo.com>
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

#include <time.h>

#include "date_calc.h"

static int
is_leap(int y) {
    return ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0);
}

static int
get_days_in_month(int m, int y) {
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (is_leap(y)) days[1] = 29;
    return days[m];
}

/*
 * DATE_CALC  -- Add or subtract minutes, hours, days, months, or
 *     years to or from a given date.
 * 
 * @param t      the time struct we will be performing calculations on  
 * @param dmin   the +/- change in minutes
 * @param dhour  the +/- change in hours
 * @param dday   the +/- change in days
 * @param dmonth the +/- change in months
 * @param dyear  the +/- change in years
 *  
 */
void
date_calc(struct tm* t, long dmin, long dhour, long dday, long dmonth, long dyear)
{
    int days_in_month;

    /* Add 'deltas' to the date structure */

    /* minutes, range 0 to 59 */ 
    t->tm_min += dmin;
    while (t->tm_min < 0)
    {
        dhour--;
        t->tm_min += 60;
    }
    while (t->tm_min > 59)
    {
        dhour++;
        t->tm_min -= 60;
    }       
    
    /* hours, range 0 to 23 */
    t->tm_hour += dhour;
    while (t->tm_hour < 0)
    {
        dday--;
        t->tm_hour += 24;
    }
    while (t->tm_hour > 23)
    {
        dday++;
        t->tm_hour -= 24;
    }
    
    /* day of the month, range 1 to 31 */
    t->tm_mday += dday;
    days_in_month = get_days_in_month(t->tm_mon, t->tm_year+1900);
    while (t->tm_mday < 1)
    {
        if(t->tm_mon > 0)
            t->tm_mon--;
        else
        {
            t->tm_mon = 11;
            t->tm_year--;
        }
        days_in_month = get_days_in_month(t->tm_mon, t->tm_year+1900);     
        t->tm_mday += days_in_month;
    }
    while (t->tm_mday > days_in_month)
    {
        if(t->tm_mon < 11)
            t->tm_mon++;
        else
        {
            t->tm_mon = 0;
            t->tm_year++;
        }
        t->tm_mday  -= days_in_month;
        days_in_month = get_days_in_month(t->tm_mon, t->tm_year+1900);
    }    

    /* month, range 0 to 11 */
    t->tm_mon += dmonth;
    while (t->tm_mon < 0)
    {
        dyear--;
        t->tm_mon += 12;
    }
    while (t->tm_mon > 11)
    {
        dyear++;
        t->tm_mon -= 12;
    }
    
    /* years since 1900 */
    t->tm_year += dyear;
    
}
