#ifndef ITERM_H
#define ITERM_H

//
// Parameters of the ITerm Command Agent
//

#define ITERM_SESSION_NAME "ITerm Control Panel" //!< Window Name for banner
#define ITERM_REV_NUMBER   "v1.0.2"              //!< Our Revision number
#define ITERM_REV_DATE     "2010 June 20"        //!< Revision date

// Runtime configuration files

#define ITERM_CONFIG_FILE "/home/mods/Config/iterm.ini" //!< Default config file

// ISIS Server and Client info defaults

#define ISIS_SERVER_ID            "IS" //!< Default ISIS server ID
#define ISIS_SERVER_HOST "192.169.139.131" //!< Default ISIS server Host at LBT
#define ISIS_SERVER_PORT          6600 //!< Default ISIS server Port

#define ITERM_ID                  "IT" //!< Default client ID
#define ITERM_PORT                   0 //!< Default ITerm client Port (0="any free port")

// User Interface Time intervals

#define ITERM_STATUSMSG_TIME     10 //!< Status message persistence in time seconds
#define ITERM_BASE_FONT "Helvetica" //!< iTerm tool base font
#define ITERM_BASE_FONTSIZE      12 //!< iTerm default font size

// Runtime Flags - place to enable/disable features

#undef  ITERM_DEBUG_MODE

#endif
