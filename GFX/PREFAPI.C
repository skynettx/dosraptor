#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>

#include "prefapi.h"

//  #define UNITEST

static char ProfilePath[260];

     
/*---------------------------------------------------------------------------
* GetPrivateProfileString
*----------------------------------------------------------------------------
* Returns:
*   1:   Found
*   0:  Not Found
*--------------------------------------------------------------------------*/
static short
GetPrivateProfileString( 
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    char            *def,       /* INPUT:   Default value for preference    */
    char            *buf,       /* OUTPUT:  buffer to place preference      */
    int             buflen,     /* INPUT:   Maximum size of buffer          */
    char            *file       /* INPUT:   File to retrieve data from      */
    )
{
    char            buffer[128];
    char            *token;
    FILE            *fptr;
    short           found = 0;
    short           len;
    
    if( ( fptr = fopen( file, "r" ) ) != NULL )
    {
        len = strlen( section );        
        while( !found && fgets( buffer, sizeof( buffer ), fptr ) )
        {
            if( buffer[0] == '[' )
            {
                for( len = 0; buffer[len] != '\0' && buffer[len] != ']'; ++len )
                    ;
                if( buffer[len] != ']' )
                    continue;
                buffer[len] = '\0';    
                if( stricmp( buffer+1, section ) == 0 )
                    found = 1;
            }       
        }
        
        while( found && fgets( buffer, sizeof( buffer ), fptr ) )
        {
            if( buffer[0] == '[' )
                found = 0;
            else if( ( token = strtok( buffer, "=\n" ) ) != NULL )
            {
                if( stricmp( token, option ) == 0 )
                {
                    fclose( fptr );
                    token = strtok( NULL, "=\n" );
                    if( token )
                    {
                        strncpy( buf, token, buflen );
                        buf[buflen-1] = '\0';
                    }    
                    else
                        buf[0] = '\0';   
                    return 1;
                }    
            }
        }
        fclose( fptr );
    }    
    
    if( def )
        strncpy( buf, def, buflen );
     else
        buf[0] = '\0';   
    return 0;
}
    
/*---------------------------------------------------------------------------
* WritePrivateProfileString
*----------------------------------------------------------------------------
* Returns:
*   1:  Success
*   0:  Failure
*---------------------------------------------------------------------------*/
static short
WritePrivateProfileString( 
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    char            *buf,       /* OUTPUT:  preference to write             */
    char            *file       /* INPUT:   File to retrieve data from      */
    )
{
    char            buffer[128];
    char            *token;
    FILE            *fptr;
    short           found = 0;
    short           len;
    long            pos1, pos2, pos3;
    long            exist_len, new_len, delta;
    long            size;
    size_t          cnt;
    
    if( section == NULL || *section == '\0' )
        return 0;
        
    if( ( fptr = fopen( file, "rb+" ) ) == NULL )
    {
        if( ( fptr = fopen( file, "wb+" ) ) == NULL )
            return 0;
//        fprintf( fptr, "[default]\r\n", section );
        fseek( fptr, 0, SEEK_SET );
    }       
    
        
    // Search for section
    pos3 = 0;
    while( !found && fgets( buffer, sizeof( buffer ), fptr ) )
    {
        if( buffer[0] == '\r' || buffer[0] == '\n' )
            continue;
        if( buffer[0] == '[' )
        {
            for( len = 0; buffer[len] != '\0' && buffer[len] != ']'; ++len )
                ;
            if( buffer[len] != ']' )
                continue;
            buffer[len] = '\0';    
            if( stricmp( buffer+1, section ) == 0 )
            {
                found = 1;
                break;
            }    
         }       
         pos3 = ftell( fptr );    
    }
    // POS3 = Beginning of section
    
    if( !found )        // Create the new section
    {
        fseek( fptr, 0, SEEK_END );
        if( option && *option != '\0' )
            fprintf( fptr, "\r\n[%s]\r\n", section );
        else
        {
            fclose( fptr );
            return 1;   // Delete section that doesn't exist
        }    
            
        found = 0;
        pos1 = pos2 = ftell( fptr );
    }
    else
    {   // Search section for option
        pos1 = ftell( fptr );
        while( fgets( buffer, sizeof( buffer ), fptr ) )
        {
            pos2 = ftell( fptr );
            if( buffer[0] == '[' )   // Option not found
            {
                found = 0;
                break;
            }
            else if( ( token = strtok( buffer, "=\r\n" ) ) != NULL )
            {
                if( stricmp( token, option ) == 0 ) // Option found
                {
                    if( stricmp( buf, strtok( NULL, "=\r\n" ) ) == 0 )
                    {
                        fclose( fptr );
                        return 1;    // Option value is the same
                    }    
                    found = 1;
                    break;
                }    
                pos1 = pos2;
            }    
        }
    }
    // POS3 = Beginning of section
    // POS2 = Current Position after last line read
    // POS1 = Point before current line, the existing option if found
    
    if( !found )            // Option doesn't exist
    {
        if( pos1 == pos2 )  // At end of file, append and exit
        {
            if( option && *option != '\0' && buf && *buf != '\0' )
            {
                fseek( fptr, 0, SEEK_END );
                fprintf( fptr, "%s=%s\r\n", option, buf );
            }
            fclose( fptr );
            return 1;
        }
        if( option && *option != '\0' )
            exist_len = 0;
        else 
        {                   // Deleting the section
            exist_len = pos1 - pos3;    
            pos2 = pos1;    // Last line read was next section, don't delete
        }    
    }
    else
        exist_len = pos2 - pos1;

    if( option && *option != '\0' && buf && *buf != '\0' )
    {
        sprintf( buffer, "%s=%s\r\n", option, buf );
        new_len = strlen( buffer );
    }
    else
        new_len = 0;    
    delta = new_len - exist_len;
    if( option && *option != '\0' )
        pos3 = pos1;            // Remember where the new line goes
        
    // POS3 = Beginning of text to remove
    // POS2 = Current Position after text to remove
    // POS1 = Point before current line, the existing option if found
        
    if( delta < 0 )         // Shrink file
    {
        while( 1 )
        {
            fseek( fptr, pos2, SEEK_SET );
            if( ( cnt = fread( buffer, sizeof( char ), sizeof( buffer ), fptr ) ) == 0 )
                break;
            pos1 = ftell( fptr );
            fseek( fptr, pos2+delta, SEEK_SET );
            fwrite( buffer, sizeof( char ), cnt, fptr );
            pos2 = pos1;
        }
        fseek( fptr, delta, SEEK_END );
        pos1 = ftell( fptr );
        chsize( fileno( fptr ), pos1 );
    }
    else if( delta > 0 )    // Expand file, starting at the end
    {                       // ...working backwards
        size = sizeof( buffer );
        fseek( fptr, 0, SEEK_END );
        pos2 = pos1 = ftell( fptr );
        while( 1 )
        {
            pos2 = pos2 - size;
            if( pos2 < pos3 )   // Moved to left of line that changed
            {
                size = pos1 - pos3;  // Calculate what is left
                if( size <= 0 )
                    break;
                pos2 = pos3;
            }
            fseek( fptr, pos2, SEEK_SET );
            pos1 = pos2;
            if( ( cnt = fread( buffer, sizeof( char ), (size_t) size, fptr ) ) == 0 )
                break;
            fseek( fptr, pos2+delta, SEEK_SET );
            fwrite( buffer, sizeof( char ), cnt, fptr );
        }
    }

    if( option && *option != '\0' && buf && *buf != '\0' )
    {            
        fseek( fptr, pos3, SEEK_SET );
        fprintf( fptr, "%s=%s\r\n", option, buf );
    }    
    
    fclose( fptr );
    return 1;
}


/****************************************************************************
* INI_InitPreference - Specify the preference program file
*----------------------------------------------------------------------------
* Returns:
*   1:   File found
*   0:  File not found
*****************************************************************************/
short
INI_InitPreference(
    char            *profile    /* INPUT:  Profile buffer                   */
    )
{
    if( profile )
        strcpy( ProfilePath, profile );
        
    if( access( ProfilePath, 04 ) == 0 )
        return 1;
        
    return 0;
}

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
    )
{
    char            buffer[20];
    char            Def[20];
   
    ltoa( def, Def, 10 );
    INI_GetPreference( section, option, buffer, sizeof( buffer ), Def );
    return( atol( buffer ) );
}

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
    )
{
    char            buffer[32];
    char            Def[32];
   
    ltoa( def, Def, 10 );
    INI_GetPreference( section, option, buffer, sizeof( buffer ), Def );
    sscanf( buffer, "%x", &def );
    return ( def );
}

/****************************************************************************
* INI_GetPreferenceBool - Retrieve a boolean value from the program file
*----------------------------------------------------------------------------
* Returns:
*   1 for 1
*   0 for 0
*****************************************************************************/
short
INI_GetPreferenceBool(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    short           def         /* INPUT:   Default value for preference    */
    )
{
    char            buffer[10];
   
    if( def )
        INI_GetPreference( section, option, buffer, sizeof( buffer ), "TRUE" );
    else   
        INI_GetPreference( section, option, buffer, sizeof( buffer ), "FALSE" );
    if( buffer[0] == '1' ||
        stricmp( buffer, "TRUE" ) == 0
      )
        return 1;
    if( buffer[0] == '0' || 
        stricmp( buffer, "FALSE" ) == 0
        
      )
        return 0;   
    return( atoi( buffer ) );
}

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
    )
{
    if( section == NULL || option == NULL || buf == NULL )
    {
        buf[0] = '\0';
        return NULL;
    }
   
    GetPrivateProfileString( section, option, def, buf, buflen, ProfilePath );

    return buf;
}   
   

/****************************************************************************
* INI_PutPreferenceLong - Write a long integer value to the program file
*----------------------------------------------------------------------------
* Returns:
*   SUCCESS:    1
*   FAIL:       0
*****************************************************************************/
short
INI_PutPreferenceLong(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    long            val         /* INPUT:   Value to write                  */
    )
{
   char buffer[32];
   
   if ( val == -1 )
   {
      INI_DeletePreference ( section, option );
      return(0);
   }

   ltoa( val, buffer, 10 );
   return( INI_PutPreference( section, option, buffer ) );
}
 

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
    )
{
   char buffer[32];
   
   if ( val == -1 )
   {
      INI_DeletePreference ( section, option );
      return(0);
   }

   sprintf ( buffer, "%x", val );

   return( INI_PutPreference( section, option, buffer ) );
} 

/****************************************************************************
* INI_PutPreferenceBool - Write a Boolean value to the program file
*----------------------------------------------------------------------------
* Returns:
*   SUCCESS:    1
*   FAIL:       0
*****************************************************************************/
short
INI_PutPreferenceBool(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    short           val         /* INPUT:   Value to write                  */
    )
{
      return( INI_PutPreference( section, option, (val)?"TRUE":"FALSE" ) );
}

/****************************************************************************
* INI_PutPreference - Write a string value to the program file
*----------------------------------------------------------------------------
* Returns:
*   SUCCESS:    1
*   FAIL:       0
*****************************************************************************/
short
INI_PutPreference(
    char            *section,   /* INPUT:   Name of section                 */
    char            *option,    /* INPUT:   Name of preference option       */
    char            *val        /* INPUT:   Value to write                  */
    )
{
    if( section == NULL || option == NULL || val == NULL )
        return 0;
        
    return( WritePrivateProfileString( section, option, val, ProfilePath ) );
}

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
    )
{
    if( section == NULL )       // If option is NULL, entire section deleted */
        return 0;
        
    return( WritePrivateProfileString( section, option, NULL, ProfilePath ) );
}


