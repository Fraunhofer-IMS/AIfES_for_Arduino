/**
 * \file aifes_config.h
 * \version 2.0alpha
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
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
 * \brief 	AIfES 2 configurations
 *
 * In this file you can configure debug settings and functions to use that are platform and use case dependent
 */

#ifndef AIFES_CONFIG
#define AIFES_CONFIG


// Add hardware specific packages
#if __arm__
//#   define AIFES_WITH_CMSIS
#endif // __arm__
#if __AVR__
//#   define AIFES_WITH_AVR_PGM
#endif // __AVR__


#define AIDEBUG_SHAPE_CHECKS /**< Enable checking for tensor shapes before performing math operations on them */
#define AIDEBUG_GENERAL_CHECKS /**< Enable general checks for all kind of situations */

#define AIFES_MEMORY_ALIGNMENT  sizeof(int) /** Set the memory alignment size used by AIfES functions when distributing or calculating memory */
#define AIFES_ALIGN_INTEGER(variable, alignment)  while(variable % alignment != 0) variable ++		/** Define the code to align a variable in memory (e.g. a pointer) to the given size */

#define AIDEBUG_ENABLE_PRINTING /**< Enable printing to the console (switch off to save memory) */


#ifdef AIDEBUG_ENABLE_PRINTING

    #define AISTRING_STORAGE_WRAPPER(S)     const char S[]  /**< Specifies the storage type of strings like logs or names */

    #define AIDEBUG_PRINT_MODULE_SPECS  /**< Enable printing of the module (layer, loss, optimizer) specifications */
    #define AIDEBUG_PRINT_ERROR_MESSAGES /**< Enable logging of error messages (with AILOG_E) */

    #define AILOG_E(MESSAGE)                ailog_e(MESSAGE)  /**< Logging function for errors */

    #define AIPRINT(STRING)                 aiprint(STRING)                 /**< Used to print strings */
    #define AIPRINT_INT(FORMAT, VAR)        aiprint_int(FORMAT, VAR)        /**< Used to print (printf-style formated) integer */
    #define AIPRINT_UINT(FORMAT, VAR)       aiprint_uint(FORMAT, VAR)       /**< Used to print (printf-style formated) unsigned integer */
    #define AIPRINT_LONG_INT(FORMAT, VAR)   aiprint_long_int(FORMAT, VAR)   /**< Used to print (printf-style formated) long integer */
    #define AIPRINT_FLOAT(FORMAT, VAR)      aiprint_float(FORMAT, VAR)      /**< Used to print (printf-style formated) float */

    /** @name Print function pointers
     * With these pointers you can dynamically set the print functions of AIfES.
     */
    ///@{
    extern int (*ailog_e)(const char *message);
    extern int (*aiprint)(const char *string);
    extern int (*aiprint_int)(const char *format, int var);
    extern int (*aiprint_uint)(const char *format, unsigned int var);
    extern int (*aiprint_long_int)(const char *format, long int var);
    extern int (*aiprint_float)(const char *format, float var);
    ///@}

    /** @name Print functions
     * You can use these functions to define individual print functions for your platform.
     * Do this by overwriting the functions and setting them to the print function pointers.
     */
    ///@{
    int aifes_log_e(const char *message);
    int aifes_print(const char *string);
    int aifes_print_int(const char *format, int var);
    int aifes_print_uint(const char *format, unsigned int var);
    int aifes_print_long_int(const char *format, long int var);
    int aifes_print_float(const char *format, float var);
    ///@}

#else // AIDEBUG_ENABLE_PRINTING

    #define AISTRING_STORAGE_WRAPPER(S)       /**< Specifies the storage type of strings like logs or names */

    #define AILOG_E(MESSAGE)                  /**< Logging function for errors */

    #define AIPRINT(STRING)                   /**< Used to print strings */
    #define AIPRINT_INT(FORMAT, VAR)          /**< Used to print (printf-style formated) integer */
    #define AIPRINT_UINT(FORMAT, VAR)         /**< Used to print (printf-style formated) unsigned integer */
    #define AIPRINT_LONG_INT(FORMAT, VAR)     /**< Used to print (printf-style formated) long integer */
    #define AIPRINT_FLOAT(FORMAT, VAR)        /**< Used to print (printf-style formated) float */

#endif // AIDEBUG_ENABLE_PRINTING

#endif // AIFES_CONFIG
