/** \file mods_timefuncts.h
 * \brief Headerfile for mods_timefuncts.c
 */

#ifndef _DELAY_H_
#define _DELAY_H_

/**
 * Lets the calling function delay
 * @param microseconds: delay for this time
 */
void delay(long microseconds);

/**
 * Writes the current time to timestring, using formatstring to specify the 
 * format the time should be displayed. Use the same conversion specifications
 * as in strftime, but be sure to use the one for sec last, as ms are 
 * concatenated.
 * @param timestring: the string, the time should be written to
 * @param formatstring: time conversion specifications
 * @return 0 when succeeded
 */
int getCurrentTime(char* timestring, char* formatstring);

#endif
