/** \file agw_datacodes.h
 * \brief Codes especially useful for the getdata() and getErrorMsg() 
 * library functions but also for all other ones provided by the 
 * oacontrol-API.
 */

#ifndef AGW_DATACODES_H_
#define AGW_DATACODES_H_

/** @name States of the AGW unit
 * These defines are representing the different states an AGW unit can be in.
 * They are used to encode the general_state word in #data
 * defined in agwcomm.h
 */
/*@{*/
/** AGW unit is ok */
#define AGW_OK			0x1
/** DEPRECATED!! AGW unit is ok. This define 
 * therefore basically 
 * means the same as the above on and exists only for compatibility reasons.*/
#define UMAC_OK			0x1
/** The 1-Wire flow sensor indicates that cooling is active
 */
#define COOLING_ACTIVE		0x2
/** AGW unit is not ok, an error occured */
#define AGW_ERROR		0x100
/** DEPRECATED!! AGW unit is not ok, an error occured. This define 
 * therefore basically 
 * means the same as the above on and exists only for compatibility reasons.*/
#define UMAC_ERROR		0x100
/*@}*/

/** @name Errors triggered by the AIP Service Boards and 1-Wire
 * These defines are offering the possibility for a more detailed description 
 * of errors that
 * eventually occurred on an AGW unit.
 * They are used to encode the general_error word in #data
 * defined in agwcomm.h
 */
/*@{*/
/** 24V loss in UMAC Rack */
#define ERR_24V_LOSS		0x1
/** Service board not free */
#define ERR_BOARD_NOT_FREE	0x2
/** Crash switch on rotational axis afar the motor */
#define ERR_CS_YF		0x4
/** Crash switch on rotational axis near the motor */
#define ERR_CS_YN		0x8
/** Crash switch on radial axis afar the motor */
#define ERR_CS_XF		0x10
/** Crash switch on radial axis near the motor */
#define ERR_CS_XN		0x20
/** Crash switch on focus axis afar the motor */
#define ERR_CS_FF		0x40
/** Crash switch on focus axis near the motor */
#define ERR_CS_FN		0x80
/** Temperature in E-Box is too low */
#define ERR_UMACTEMP_TOO_LOW   	0x100  //UMAC
/** Temperature in E-Box is too high */
#define ERR_UMACTEMP_TOO_HIGH	0x200
/** Ambient temperature is too low */
#define ERR_TEMP_TOO_LOW	0x400
/** Ambient temperature is too high */
#define ERR_TEMP_TOO_HIGH	0x800
/** Humidity in E-Box is too high */
#define ERR_HUMIDITY_UMAC	0x1000
/** 12V loss in UMAC Rack */
#define ERR_12V_LOSS		0x2000
/** -12V loss in UMAC Rack */
#define ERR_M12V_LOSS		0x4000
/** 5V loss in UMAC Rack */
#define ERR_5V_LOSS		0x8000
/** Humidity in E-Box is about to get too high */
#define ERR_UMAC_NOT_CONFIGURED	0x200000
/*@}*/

/** @name Warnings
 */
/*@{*/
/** Temperature in E-Box is about to get too low */
#define WAR_UMACTEMP_TOO_LOW   	0x10000
/** Temperature in E-Box is about to get too high */
#define WAR_UMACTEMP_TOO_HIGH	0x20000
/** Ambient temperature is about to get too low */
#define WAR_TEMP_TOO_LOW	0x40000
/** Ambient temperature is about to get too high */
#define WAR_TEMP_TOO_HIGH	0x80000
/** Humidity in E-Box is about to get too high */
#define WAR_HUMIDITY_UMAC	0x100000
/*@}*/

/** @name States of the different axes
 * These defines are used to encode the different motor state words in #data
 * defined in agwcomm.h
 */
/*@{*/
/** All axis in coordinate system are in position */
#define ISINPOSITION		0x1
/** All axis in coordinate system are homed */
#define ISHOMED			0x2
/** At least one of the axis in coordinate system is moving */
#define ISMOVING		0x4
/** Move incremental. THIS BIT IS CURRENTLY NOT INTERPRETED */
#define MODE_INC		0x8
/** Move in servo mode without braking. THIS BIT IS CURRENTLY NOT INTERPRETED */
#define MODE_SERVO		0x10
/** An error occured at a motor */
#define MOTOR_ERROR		0x100
/*@}*/

/** @name Motor errors
 * These defines are used to encode the different motor error words in #data
 * defined in agwcomm.h
 */
/*@{*/
/** Following error */
#define ERR_FOLLOWING		0x1
/** Amplifier fault error */
#define ERR_AMPLIFIER		0x2
/** I2T protection error */
#define ERR_I2T			0x4
/** Integrated fatal following error */
#define ERR_INTEGRATED		0x8
/** Phasing search error */
#define ERR_PHASINGSEARCH	0x10
/** Data block error */
#define	ERR_DATA_BLOCK		0x20
/*@}*/

/** @name Motor warnings
 */
/*@{*/
/** Position limit reached */
#define WAR_LIMIT		0x10000
/** Positive end limit set */
#define WAR_PLIMIT		0x20000
/** Negative end limit set */
#define WAR_MLIMIT		0x40000
/** Warning following error */
#define WAR_FOLLOWING		0x80000
/*@}*/

/** @name Motion errors
 * These error codes are returned by the appropriate library functions.
 */
/*@{*/
/** Axes of this coordinate system already in Motion */
#define ERR_ISMOVING	250
/** There is an error on UMAC */
#define	ERR_UMAC	251
/** There is a motor error on UMAC */
#define ERR_MOTOR	252
/** The specified axes are not in position */
#define ERR_NINPOSITION	253
/** The specified axes are not homed */
#define ERR_NHOMED	254
/** The requested position is out of range */
#define ERR_OUTOFRANGE	255
/** One of the motors ran into a limit switch */
#define ERR_POSLIMIT	256
/** One of the motors ran into a crash switch */
#define ERR_CRASH	257
/*@}*/

/** @name Other (communication) errors 
 * These error codes are returned by the appropriate library functions.
 */
/*@{*/
/** General unspecified error */
#define ERR_GENERAL	200
/** Error executing a remote procedure call */
#define ERR_RPC		201
/** Network error */
#define ERR_NETWORK	202
/** Memory allocation error */
#define ERR_MEMORY	210
/** This error is returned when an item searched for is not available
 * in the linked lists
 * where the config file data is stored
 * */
#define ERR_ITEMNOTFOUND	220
/*@}*/


/** @name Drive modes
 * Different modes to drive axes with on UMAC
 */
/*@{*/
/** Incremental move mode. This mode is not currently working. */
#define OPT_INC		0x1
/** Servo move mode. This mode is not currently working. */
#define OPT_SERVO	0x2
/** Blocking move mode. The move function will return when the target position 
 * is reached or an error occured */
#define OPT_BLOCK	0x4
/** Nonblocking move mode. The move function will return immediately after
 * issuing the move command to UMAC
 */
#define OPT_NONBLOCK	0x8
/** Interpret positions as to be in counts. This mode is not currently working.
 */
#define OPT_COUNTS	0x10
/*@}*/

/** @name Options for init() routine
 */
/*@{*/
/** Start the AGW unit. This means to switch the power for the whole
 * AGW ring on by triggering the appropriate bit in an 8bit 1-Wire switch. */
#define AGW_START	0x1
/** Stop the AGW unit. This means to switch the power off the whole
 * AGW ring on by triggering the appropriate bit in an 8bit 1-Wire switch. */
#define AGW_STOP	0x2
/*@}*/

/* used in transformation cmds for converting rad into arcsec
 * (180 * 3600 / pi)
 */
#define RAD_TO_ARCSEC_FACTOR 206264.8063

#endif
