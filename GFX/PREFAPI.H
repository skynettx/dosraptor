#ifndef _PREF_H
#define _PREF_H

     
/****************************************************************************
* INI_InitPreference - Specify the preference program file
*----------------------------------------------------------------------------
* Returns:
*   TRUE:   File found
*   FALSE:  File not found
*****************************************************************************/
short
INI_InitPreference(
    char            *profile    /* INPUT:  Name of program file             */
    );

/****************************************************************************
* INI_GetPreferenceLong - Retrieve a long integer value from the program file
*----------------------------------------------------------------------------
* Returns:
*   Value retrieved
*****************************************************************************/
long
INI_GetPreferenceLong(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    long            def         /* INPUT:   Default value for preference    */
    );

/****************************************************************************
* INI_GetPreferenceHex - Retrieve a Hex value from the program file
*----------------------------------------------------------------------------
* Returns:
*   Value retrieved
*****************************************************************************/
long
INI_GetPreferenceHex(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    long            def         /* INPUT:   Default value for preference    */
    );

/****************************************************************************
* INI_GetPreferenceBool - Retrieve a boolean value from the program file
*----------------------------------------------------------------------------
* Returns:
*   1 for TRUE
*   0 for FALSE
*****************************************************************************/
short
INI_GetPreferenceBool(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    short           def         /* INPUT:   Default value for preference    */
    );

/****************************************************************************
* INI_GetPreference - Retrieve a character string from the program file
*----------------------------------------------------------------------------
* Returns:
*   Pointer to character buffer
*****************************************************************************/
char *
INI_GetPreference(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    char            *buf,       /* OUTPUT:  buffer to place preference      */
    int             buflen,     /* INPUT:   Maximum size of buffer          */
    char            *def        /* INPUT:   Default value for preference    */
    );

/****************************************************************************
* INI_PutPreferenceLong - Write a long integer value to the program file
*----------------------------------------------------------------------------
* Returns:
*   SUCCESS:    TRUE
*   FAIL:       FALSE
*****************************************************************************/
short
INI_PutPreferenceLong(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    long            val         /* INPUT:   Value to write                  */
    );
 
/****************************************************************************
* INI_PutPreferenceHex - Write a Hex value to the program file
*----------------------------------------------------------------------------
* Returns:
*   SUCCESS:    1
*   FAIL:       0
*****************************************************************************/
short
INI_PutPreferenceHex(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    long            val         /* INPUT:   Value to write                  */
    );

/****************************************************************************
* INI_PutPreferenceBool - Write a Boolean value to the program file
*----------------------------------------------------------------------------
* Returns:
*   SUCCESS:    TRUE
*   FAIL:       FALSE
*****************************************************************************/
short
INI_PutPreferenceBool(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    short           val         /* INPUT:   Value to write                  */
    );

/****************************************************************************
* INI_PutPreference - Write a string value to the program file
*----------------------------------------------------------------------------
* Returns:
*   SUCCESS:    TRUE
*   FAIL:       FALSE
*****************************************************************************/
short
INI_PutPreference(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    char            *val        /* INPUT:   Value to write                  */
    );

/****************************************************************************
* INI_DeletePreference - Delete a preference from the program file
*----------------------------------------------------------------------------
* Returns:
*   SUCCESS:    1
*   FAIL:       0
*****************************************************************************/
short
INI_DeletePreference(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option     /* INPUT:   Name of preference option       */
    );

#endif
