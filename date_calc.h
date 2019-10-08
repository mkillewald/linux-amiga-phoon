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

#ifndef _DATE_CALC_H_
#define _DATE_CALC_H_

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
void date_calc(struct tm* t, long dmin, long dhour, long dday, long dmonth, long dyear);

#endif /* _DATE_CALC_H_ */
