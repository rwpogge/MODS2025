/** \file getErrorMsg.h
 * \brief Header file for geterrormsg.c. In this file, the different error 
 * messages are declared.
 */

#ifndef _GETERRORMSG_H_
#define _GETERRORMSG_H_

//char *errormessages[300];

//const char *fatalerrormsg[300]=
char *mods_fatalerrormsg[300]=
  {
    "Warning: error test/index",//0 *NOT* an ERROR. it's used for indexing
    "Warning: Host *NOT* available",//1 Start of WARNING errors
    "Warning: Port *NOT* available",//2
    "Warning: Cannot form connection, Server might not be running.",//3
    "Warning: Server did *NOT* responed",//4
    "Warning: Cannot execute quit command - Only EXEC is allowed",//5
    "Warning: Unrecognized argument(s)",//6
    "Warning: Unrecognized axis",//7
    "Warning: AGW Locked",//8
    "Warning: function requires an arguments or cannot execute command",//9
    "Warning: RESET failed on attempt to read config file",//10
    "Warning: CALIB only available to remote hosts as  executive override command",//11
    "Warning: Cannot LOCK, AGW Lock already held by another host",//12
    "Warning: Cannot UNLOCK, Command LOCK held by another host",//13
    "Warning: Unrecognized command",//14
    "Warning: 15",//15
    "Warning: Invalid Y command, must be 0..200.0",//16
    "Warning: Invalid pickoff mirror Y position, must be 0.0 .. 180.50",//17"
    "Warning: Invalid Y-axis drift rate, must be +/-0..10000 um/sec",//18
    "Warning: 19",//19
    "Warning: 20",//20
    "Warning: Invalid X, must be 0 .. 180.0",//21
    "Warning: Invalid pickoff mirror X position, must be -90.0 .. 90.0",//22
    "Warning: Invalid X-axis drift rate, must be +/-0..10000 um/sec",//23
    "Warning: 24",//24
    "Warning: Invalid Filter, must be 1..4",//25
    "Warning: AGW Filter at #1 i.e. Homed",//26
    "Warning: 27",//27
    "Warning: 28",//28
    "Warning: Invalid absolute focus requested, must be 0...30",//29
    "Warning: Invalid Focus, must be 0...30",//30
    "Warning: AGW Focus Homed",//31
    "Warning: Unrecognized argument(s), Usage: AUTOFOCUS <ON|OFF|ZERO>",//32
    "Warning: 33 ",//33
    "Warning: 34 ",//34
    "Warning: 35 ",//35
    "Warning: 36 ",//36
    "Warning: 37 Unrecognized argument, Usage: CALIB <IN|OUT>",//37
    "Warning: 38 ",//38
    "Warning: 39 ",//39
    "Warning: No Drift Vector set, cannot initiate drift",//40
    "Warning: 41 ",//41
    "Warning: Unrecognized argument, Usage: DRIFT <START|STOP>",//42
    "Warning: Cannot change drift rates while drift is enabled",//43
    "Warning: AGW XY Stage, Focus, and Filter Homed",//44
    "Warning: AGW XY Stage Homed",//45
    "Warning: 46 ",//46
    "Warning: 47 ",//47
    "Warning: 48 ",//48
    "Warning: 49 ",//49
    "Warning: Unrecognized argument, Usage: getposition [x|y|focus|filter]",//50
    "Warning: Unrecognized axis, Usage: setposition [x|y|focus|filter]",//51
    "Warning: 52 ",//52
    "Warning: 53 ",//53
    "Warning: 54 ",//54
    "Warning: 55 ",//55
    "Warning: 56 ",//56
    "Warning: 57 ",//57
    "Warning: 58 ",//58
    "Warning: 59 ",//59
    "Warning: 60 ",//60
    "Warning: 61 ",//61
    "Warning: 62 ",//62
    "Warning: 63 ",//63
    "Warning: 64 ",//64
    "Warning: 65 ",//65
    "Warning: 66 ",//66
    "Warning: 67 ",//67
    "Warning: 68 ",//68
    "Warning: 69 ",//69
    "Warning: 70 ",//70
    "Warning: 71 ",//71
    "Warning: 72 ",//72
    "Warning: 73 ",//73
    "Warning: 74 ",//74
    "Warning: 75 ",//75
    "Warning: 76 ",//76
    "Warning: 77 ",//77
    "Warning: 78 ",//78
    "Warning: 79 ",//79
    "Warning: 80 ",//80
    "Warning: 81 ",//81
    "Warning: 82 ",//82
    "Warning: 83 ",//83
    "Warning: 84 ",//84
    "Warning: 85 ",//85
    "Warning: 86 ",//86
    "Warning: 87 ",//87
    "Warning: 88 ",//88
    "Warning: 89 ",//89
    "Warning: 90 ",//90
    "Warning: 91 ",//91
    "Warning: 92 ",//92
    "Warning: 93 ",//93
    "Warning: 94 ",//94
    "Warning: setposition has been disabled",//95
    "Warning: getposition requires an argument, Usage: getposition [x|y|focus|filter] pos",//96
    "Warning: setposition requires 2 arguments, Usage: setposition [x|y|focus|filter] pos",//97
    "Warning: Invalid XS %d, must be 0..180.0",//98
    "Warning: Invalid YS %d, must be 0..200.0",//99
    "Critical: error test",//100 Start of CRITICAL errors.
    "Critical: setfocus would move the Y stage past its lower limit.",//101
    "Critical: 102 ",//102
    "Critical: 103 ",//103
    "Critical: 104 ",//104
    "Critical: 105 ",//105
    "Critical: 106 ",//106
    "Critical: 107 ",//107
    "Critical: 108 ",//108
    "Critical: 109 ",//109
    "Critical: 110 ",//110
    "Critical: 111 ",//111
    "Critical: 112 ",//112
    "Critical: 113 ",//113
    "Critical: 114 ",//114
    "Critical: 115 ",//115
    "Critical: 116 ",//116
    "Critical: 117 ",//117
    "Critical: 118 ",//118
    "Critical: 119 ",//119
    "Critical: 120 ",//120
    "Critical: 121 ",//121
    "Critical: 122 ",//122
    "Critical: 123 ",//123
    "Critical: 124 ",//124
    "Critical: 125 ",//125
    "Critical: 126 ",//126
    "Critical: 127 ",//127
    "Critical: 128 ",//128
    "Critical: 129 ",//129
    "Critical: 130 ",//130
    "Critical: 131 ",//131
    "Critical: 132 ",//132
    "Critical: 133 ",//133
    "Critical: 134 ",//134
    "Critical: 135 ",//135
    "Critical: 136 ",//136
    "Critical: 137 ",//137
    "Critical: 138 ",//138
    "Critical: 139 ",//139
    "Critical: 140 ",//140
    "Critical: 141 ",//141
    "Critical: 142 ",//142
    "Critical: 143 ",//143
    "Critical: 144 ",//144
    "Critical: 145 ",//145
    "Critical: 146 ",//146
    "Critical: 147 ",//147
    "Critical: 148 ",//148
    "Critical: 149 ",//149
    "Critical: 150 ",//150
    "Critical: 151 ",//151
    "Critical: 152 ",//152
    "Critical: 153 ",//153
    "Critical: 154 ",//154
    "Critical: 155 ",//155
    "Critical: 156 ",//156
    "Critical: 157 ",//157
    "Critical: 158 ",//158
    "Critical: 159 ",//159
    "Critical: 160 ",//160
    "Critical: 161 ",//161
    "Critical: 162 ",//162
    "Critical: 163 ",//163
    "Critical: 164 ",//164
    "Critical: 165 ",//165
    "Critical: 166 ",//166
    "Critical: 167 ",//167
    "Critical: 168 ",//168
    "Critical: 169 ",//169
    "Critical: 170 ",//170
    "Critical: 171 ",//171
    "Critical: 172 ",//172
    "Critical: 173 ",//173
    "Critical: 174 ",//174
    "Critical: 175 ",//175
    "Critical: 176 ",//176
    "Critical: 177 ",//177
    "Critical: 178 ",//178
    "Critical: 179 ",//179
    "Critical: 180 ",//180
    "Critical: 181 ",//181
    "Critical: 182 ",//182
    "Critical: 183 ",//183
    "Critical: 184 ",//184
    "Critical: 185 ",//185
    "Critical: 186 ",//186
    "Critical: 187 ",//187
    "Critical: 188 ",//188
    "Critical: 189 ",//189
    "Critical: 190 ",//190
    "Critical: 191 ",//191
    "Critical: 192 ",//192
    "Critical: 193 ",//193
    "Critical: 194 ",//194
    "Critical: 195 ",//195
    "Critical: 196 ",//196
    "Critical: 197 ",//197
    "Critical: 198 ",//198
    "Critical: 199 ",//199
    "Fatal: error test",//200 Start of FATAL errors.
    "Fatal: Calibration Tower is in the beam",//201
    "Fatal: 202 ",//202
    "Fatal: 203 ",//203
    "Fatal: 204 ",//204
    "Fatal: 205 ",//205
    "Fatal: 206 ",//206
    "Fatal: 207 ",//207
    "Fatal: 208 ",//208
    "Fatal: 209 ",//209
    "Fatal: 210 ",//210
    "Fatal: 211 ",//211
    "Fatal: 212 ",//212
    "Fatal: 213 ",//213
    "Fatal: 214 ",//214
    "Fatal: 215 ",//215
    "Fatal: 216 ",//216
    "Fatal: 217 ",//217
    "Fatal: 218 ",//218
    "Fatal: 219 ",//219
    "Fatal: 220 ",//220
    "Fatal: 221 ",//221
    "Fatal: 222 ",//222
    "Fatal: 223 ",//223
    "Fatal: 224 ",//224
    "Fatal: 225 ",//225
    "Fatal: 226 ",//226
    "Fatal: 227 ",//227
    "Fatal: 228 ",//228
    "Fatal: 229 ",//229
    "Fatal: 230 ",//230
    "Fatal: 231 ",//231
    "Fatal: 232 ",//232
    "Fatal: 233 ",//233
    "Fatal: 234 ",//234
    "Fatal: 235 ",//235
    "Fatal: 236 ",//236
    "Fatal: 237 ",//237
    "Fatal: 238 ",//238
    "Fatal: 239 ",//239
    "Fatal: 240 ",//240
    "Fatal: 241 ",//241
    "Fatal: 242 ",//242
    "Fatal: 243 ",//243
    "Fatal: 244 ",//244
    "Fatal: 245 ",//245
    "Fatal: 246 ",//246
    "Fatal: 247 ",//247
    "Fatal: 248 ",//248
    "Fatal: 249 ",//249
    "Fatal: 250 ",//250
    "Fatal: 251 ",//251
    "Fatal: 252 ",//252
    "Fatal: 253 ",//253
    "Fatal: 254 ",//254
    "Fatal: 255 ",//255
    "Fatal: 256 ",//256
    "Fatal: 257 ",//257
    "Fatal: 258 ",//258
    "Fatal: 259 ",//259
    "Fatal: 260 ",//260
    "Fatal: 261 ",//261
    "Fatal: 262 ",//262
    "Fatal: 263 ",//263
    "Fatal: 264 ",//264
    "Fatal: 265 ",//265
    "Fatal: 266 ",//266
    "Fatal: 267 ",//267
    "Fatal: 268 ",//268
    "Fatal: 269 ",//269
    "Fatal: 270 ",//270
    "Fatal: 271 ",//271
    "Fatal: 272 ",//272
    "Fatal: 273 ",//273
    "Fatal: 274 ",//274
    "Fatal: 275 ",//275
    "Fatal: 276 ",//276
    "Fatal: 277 ",//277
    "Fatal: 278 ",//278
    "Fatal: 279 ",//279
    "Fatal: 280 ",//280
    "Fatal: 281 ",//281
    "Fatal: 282 ",//282
    "Fatal: 283 ",//283
    "Fatal: 284 ",//284
    "Fatal: 285 ",//285
    "Fatal: 286 ",//286
    "Fatal: 287 ",//287
    "Fatal: 288 ",//288
    "Fatal: 289 ",//289
    "Fatal: 290 ",//290
    "Fatal: 291 ",//291
    "Fatal: 292 ",//292
    "Fatal: 293 ",//293
    "Fatal: 294 ",//294
    "Fatal: 295 ",//295
    "Fatal: 296 ",//296
    "Fatal: 297 ",//297
    "Fatal: 298 ",//298
    "Fatal: 299 ",//299
  };

#endif
