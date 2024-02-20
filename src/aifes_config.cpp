/**
 * \file core/aifes_core.c
 * \version 2.2.0
 * \date 15.06.2021
 * \copyright  Copyright (C) 2020-2023  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.<br><br>
    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.<br><br>
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.<br><br>
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief
 * \details
 */

#include "aifes_config.h"

#ifdef AIDEBUG_ENABLE_PRINTING

#if defined AIFES_USE_ARDUINO_SERIAL_PRINT
// Use the Serial.print functions for console printing
int (*aiprint)(const char *string) = aifes_print;
int (*aiprint_int)(const char *format, int var) = aifes_print_int;
int (*aiprint_uint)(const char *format, unsigned int var) = aifes_print_uint;
int (*aiprint_long_int)(const char *format, long int var) = aifes_print_long_int;
int (*aiprint_float)(const char *format, float var) = aifes_print_float;
#else
int (*aiprint)(const char *string);
int (*aiprint_int)(const char *format, int var);
int (*aiprint_uint)(const char *format, unsigned int var);
int (*aiprint_long_int)(const char *format, long int var);
int (*aiprint_float)(const char *format, float var);
#endif


int aifes_print(const char *string)
{
  Serial.print((__FlashStringHelper *) string);
  return 0;
}

int aifes_print_int(const char *format, int var)
{
  Serial.print(var);
  return 0;
}

int aifes_print_uint(const char *format, unsigned int var)
{
  Serial.print(var);
  return 0;
}

int aifes_print_long_int(const char *format, long int var)
{
  Serial.print(var);
  return 0;
}

int aifes_print_float(const char *format, float var)
{
  Serial.print(var);
  return 0;
}

#endif // AIDEBUG_ENABLE_PRINTING