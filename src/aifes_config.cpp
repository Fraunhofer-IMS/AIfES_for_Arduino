/**
 * \file core/aifes_core.c
 * \author Justus Viga
 * \version 2.0alpha
 * \date 15.06.2021
 * \copyright Copyright 2020 by Fraunhofer IMS, Duisburg. \n\n
 Use and modification of this code is allowed only with
 written permission of the Fraunhofer IMS, Duisburg.
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