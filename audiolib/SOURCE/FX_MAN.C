/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/**********************************************************************
   module: FX_MAN.C

   author: James R. Dose
   date:   March 17, 1994

   Device independant sound effect routines.

   (c) Copyright 1994 James R. Dose.  All Rights Reserved.
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "sndcards.h"
#include "multivoc.h"
#include "blaster.h"
#include "pas16.h"
#include "sndscape.h"
#include "guswave.h"
#include "sndsrc.h"
// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
#include "adlibfx.h"
#include "pcfx.h"
#endif
#include "ll_man.h"
#include "user.h"
#include "fx_man.h"
#if (LIBVER_ASSREV < 19960116L) // *** VERSIONS RESTORATION ***
#include "memcheck.h"
#endif

#define TRUE  ( 1 == 1 )
#define FALSE ( !TRUE )

static unsigned FX_MixRate;

int FX_SoundDevice = -1;
int FX_ErrorCode = FX_Ok;
// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
int FX_Installed = FALSE;
#endif

void TextMode( void );
#pragma aux TextMode =  \
    "mov    ax, 0003h", \
    "int    10h"        \
    modify [ ax ];

#define FX_SetErrorCode( status ) \
   FX_ErrorCode = ( status );

/*---------------------------------------------------------------------
   Function: FX_ErrorString

   Returns a pointer to the error message associated with an error
   number.  A -1 returns a pointer the current error.
---------------------------------------------------------------------*/

char *FX_ErrorString
   (
   int ErrorNumber
   )

   {
   char *ErrorString;

   switch( ErrorNumber )
      {
      case FX_Warning :
      case FX_Error :
         ErrorString = FX_ErrorString( FX_ErrorCode );
         break;

      case FX_Ok :
         ErrorString = "Fx ok.";
         break;

      case FX_ASSVersion :
         ErrorString = "Apogee Sound System Version " ASS_VERSION_STRING "  "
            // *** VERSIONS RESTORATION ***
            // Note that chances are a non-Unicode char was originally used directly
#if (LIBVER_ASSREV < 19960510L)
            "Programmed by Jim Dos\x82\n"
            "Copyright 1995 Apogee Software, Ltd.\n";
#else
            "Programmed by Jim Dose\n"
            "(c) Copyright 1995 James R. Dose.  All Rights Reserved.\n";
#endif
         break;

      case FX_BlasterError :
         ErrorString = BLASTER_ErrorString( BLASTER_Error );
         break;

      case FX_SoundCardError :
         switch( FX_SoundDevice )
         {
            case SoundBlaster :
            case Awe32 :
               ErrorString = BLASTER_ErrorString( BLASTER_Error );
               break;

            case ProAudioSpectrum :
            case SoundMan16 :
               ErrorString = PAS_ErrorString( PAS_Error );
               break;

            // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
            case Adlib :
               ErrorString = ADLIBFX_ErrorString( ADLIBFX_Error );
               break;

            case PC :
               ErrorString = PCFX_ErrorString( PCFX_Error );
               break;
#endif

            case SoundScape :
               ErrorString = SOUNDSCAPE_ErrorString( SOUNDSCAPE_Error );
               break;

            case UltraSound :
               ErrorString = GUSWAVE_ErrorString( GUSWAVE_Error );
               break;

            case SoundSource :
            case TandySoundSource :
               ErrorString = SS_ErrorString( SS_Error );
               break;
            }
         break;

      case FX_InvalidCard :
         ErrorString = "Invalid Sound Fx device.";
         break;

      case FX_MultiVocError :
         ErrorString = MV_ErrorString( MV_Error );
         break;

      case FX_DPMI_Error :
         ErrorString = "DPMI Error in FX_MAN.";
         break;

      default :
         ErrorString = "Unknown Fx error code.";
         break;
      }

   return( ErrorString );
   }


/*---------------------------------------------------------------------
   Function: FX_SetupCard

   Sets the configuration of a sound device.
---------------------------------------------------------------------*/

int FX_SetupCard
   (
   int SoundCard,
   fx_device *device
   )

   {
   int status;
   int DeviceStatus;
   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   BLASTER_CONFIG Blaster;
#endif

   if ( USER_CheckParameter( "ASSVER" ) )
      {
      FX_SetErrorCode( FX_ASSVersion );
      return( FX_Error );
      }

   FX_SoundDevice = SoundCard;

   status = FX_Ok;
   FX_SetErrorCode( FX_Ok );

   switch( SoundCard )
      {
      case SoundBlaster :
      case Awe32 :
      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
         if ( BLASTER_GetEnv( &Blaster ) != BLASTER_Ok )
#else
         DeviceStatus = BLASTER_Init();
         if ( DeviceStatus != BLASTER_Ok )
#endif
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Error;
            break;
            }

         // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
         BLASTER_SetCardSettings( Blaster );

         DeviceStatus = BLASTER_Init();
         if ( DeviceStatus != BLASTER_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            return( FX_Error );
            }
#endif
         device->MaxVoices = 32;
#if (LIBVER_ASSREV < 19960510L)
         BLASTER_GetCardInfo( &device->MaxSampleBits, &device->MaxChannels );
#endif
         break;

      case ProAudioSpectrum :
      case SoundMan16 :
         DeviceStatus = PAS_Init();
         if ( DeviceStatus != PAS_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Error;
            break;
            }

         device->MaxVoices = 32;
         PAS_GetCardInfo( &device->MaxSampleBits, &device->MaxChannels );
         break;

      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
      case Adlib :
      case PC :
         device->MaxVoices     = 1;
         device->MaxSampleBits = status; // FIXME HACK, close but still not there
         device->MaxChannels   = 1;
         break;
#endif

      case GenMidi :
      case SoundCanvas :
      case WaveBlaster :
         device->MaxVoices     = 0;
         device->MaxSampleBits = 0;
         device->MaxChannels   = 0;
         break;

      case SoundScape :
         device->MaxVoices = 32;
         DeviceStatus = SOUNDSCAPE_GetCardInfo( &device->MaxSampleBits,
            &device->MaxChannels );
         if ( DeviceStatus != SOUNDSCAPE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Error;
            }
         break;

      case UltraSound :
         if ( GUSWAVE_Init( 8 ) != GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Error;
            break;
            }

         device->MaxVoices     = 8;
         device->MaxSampleBits = 0;
         device->MaxChannels   = 0;
         break;

      case SoundSource :
      case TandySoundSource :
         DeviceStatus = SS_Init( SoundCard );
         if ( DeviceStatus != SS_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Error;
            break;
            }
         SS_Shutdown();
         device->MaxVoices     = 32;
         device->MaxSampleBits = 8;
         device->MaxChannels   = 1;
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Error;
      }

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_GetBlasterSettings

   Returns the current BLASTER environment variable settings.
---------------------------------------------------------------------*/

int FX_GetBlasterSettings
   (
   fx_blaster_config *blaster
   )

   {
   int status;
   BLASTER_CONFIG Blaster;

   FX_SetErrorCode( FX_Ok );

   status = BLASTER_GetEnv( &Blaster );
   if ( status != BLASTER_Ok )
      {
      FX_SetErrorCode( FX_BlasterError );
      return( FX_Error );
      }

   blaster->Type      = Blaster.Type;
   blaster->Address   = Blaster.Address;
   blaster->Interrupt = Blaster.Interrupt;
   blaster->Dma8      = Blaster.Dma8;
   blaster->Dma16     = Blaster.Dma16;
   blaster->Midi      = Blaster.Midi;
   blaster->Emu       = Blaster.Emu;

   return( FX_Ok );
   }


/*---------------------------------------------------------------------
   Function: FX_SetupSoundBlaster

   Handles manual setup of the Sound Blaster information.
---------------------------------------------------------------------*/

int FX_SetupSoundBlaster
   (
   fx_blaster_config blaster,
   int *MaxVoices,
   int *MaxSampleBits,
   int *MaxChannels
   )

   {
   int DeviceStatus;
   BLASTER_CONFIG Blaster;

   FX_SetErrorCode( FX_Ok );

   FX_SoundDevice = SoundBlaster;

   Blaster.Type      = blaster.Type;
   Blaster.Address   = blaster.Address;
   Blaster.Interrupt = blaster.Interrupt;
   Blaster.Dma8      = blaster.Dma8;
   Blaster.Dma16     = blaster.Dma16;
   Blaster.Midi      = blaster.Midi;
   Blaster.Emu       = blaster.Emu;

   BLASTER_SetCardSettings( Blaster );

   DeviceStatus = BLASTER_Init();
   if ( DeviceStatus != BLASTER_Ok )
      {
      FX_SetErrorCode( FX_SoundCardError );
      return( FX_Error );
      }

   *MaxVoices = 8;
   BLASTER_GetCardInfo( MaxSampleBits, MaxChannels );

   return( FX_Ok );
   }


/*---------------------------------------------------------------------
   Function: FX_Init

   Selects which sound device to use.
---------------------------------------------------------------------*/

int FX_Init
   (
   int SoundCard,
   int numvoices,
   int numchannels,
   int samplebits,
   unsigned mixrate
   )

   {
   int status;
   int devicestatus;

   // *** VERSIONS RESTORATION ***
   // FIXME - Can't be a vanilla bug?
#if (LIBVER_ASSREV < 19950821L)
   if ( status = FX_ErrorCode )
      {
      return( FX_Error );
      }
#else
   if ( FX_Installed )
      {
      FX_Shutdown();
      }
#endif

   if ( USER_CheckParameter( "ASSVER" ) )
      {
      FX_SetErrorCode( FX_ASSVersion );
      return( FX_Error );
      }

   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19950821L)
   if ( LL_LockMemory() != LL_Ok )
#else
   status = LL_LockMemory();
   if ( status != LL_Ok )
#endif
      {
      FX_SetErrorCode( FX_DPMI_Error );
      return( FX_Error );
      }

   FX_MixRate = mixrate;

   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
   status = FX_Ok;
#endif
   FX_SoundDevice = SoundCard;
   switch( SoundCard )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
      case SoundSource :
      case TandySoundSource :
      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
      case UltraSound :
#endif
         devicestatus = MV_Init( SoundCard, FX_MixRate, numvoices,
            numchannels, samplebits );
         if ( devicestatus != MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            status = FX_Error;
            }
         break;

      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
#if (LIBVER_ASSREV < 19950821L)
      case UltraSound :
         if ( GUSWAVE_Init( numvoices ) != GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Error;
            }
         break;

#endif
      case Adlib :
         if ( ADLIBFX_Init() != ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Error;
            }
         break;

      case PC :
         if ( PCFX_Init() != PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Error;
            }
         break;

#endif // LIBVER_ASSREV
      default :
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Error;
      }

   if ( status != FX_Ok )
      {
      LL_UnlockMemory();
      }
   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
   else
      {
      FX_Installed = TRUE;
      }
#endif

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_Shutdown

   Terminates use of sound device.
---------------------------------------------------------------------*/

int FX_Shutdown
   (
   void
   )

   {
   int status;

   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
   if ( !FX_Installed )
      {
      return( FX_Ok );
      }
#endif

   status = FX_Ok;
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
      case SoundSource :
      case TandySoundSource :
      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
      case UltraSound :
#endif
         status = MV_Shutdown();
         if ( status != MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            status = FX_Error;
            }
         break;

      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
#if (LIBVER_ASSREV < 19950821L)
      case UltraSound :
         GUSWAVE_Shutdown();
         break;

#endif
      case Adlib :
         ADLIBFX_Shutdown();
         break;

      case PC :
         PCFX_Shutdown();
         break;

#endif // LIBVER_ASSREV
      default :
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Error;
      }

   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
   FX_Installed = FALSE;
#endif
   LL_UnlockMemory();

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_SetCallback

   Sets the function to call when a voice is done.
---------------------------------------------------------------------*/

int FX_SetCallBack
   (
   void ( *function )( unsigned long )
   )

   {
   int status;

   status = FX_Ok;

   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
      case SoundSource :
      case TandySoundSource :
      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
      case UltraSound :
#endif
         MV_SetCallBack( function );
         break;

      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
#if (LIBVER_ASSREV < 19950821L)
      case UltraSound :
         GUSWAVE_SetCallBack( function );
         break;

#endif
      case Adlib :
         ADLIBFX_SetCallBack( function );
         break;

      case PC :
         PCFX_SetCallBack( function );
         break;

#endif // LIBVER_ASSREV
      default :
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Error;
      }

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_SetVolume

   Sets the volume of the current sound device.
---------------------------------------------------------------------*/

void FX_SetVolume
   (
   int volume
   )

   {
   int status;

   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
         if ( BLASTER_CardHasMixer() )
            {
            BLASTER_SetVoiceVolume( volume );
            }
         else
            {
            MV_SetVolume( volume );
            }
         break;

      case ProAudioSpectrum :
      case SoundMan16 :
         status = PAS_SetPCMVolume( volume );
         if ( status != PAS_Ok )
            {
            MV_SetVolume( volume );
            }
         break;

      case GenMidi :
      case SoundCanvas :
      case WaveBlaster :
         break;

      case SoundScape :
      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
      case SoundSource :
      case TandySoundSource :
#endif
         MV_SetVolume( volume );
         break;

      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
      case Adlib :
         ADLIBFX_SetTotalVolume( volume );
         break;

#endif
      case UltraSound :
         GUSWAVE_SetVolume( volume );
         break;

      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
      case PC :
         PCFX_SetTotalVolume( volume );
         break;
#else
      case SoundSource :
      case TandySoundSource :
         MV_SetVolume( volume );
         break;
#endif
      }
   }


/*---------------------------------------------------------------------
   Function: FX_GetVolume

   Returns the volume of the current sound device.
---------------------------------------------------------------------*/

int FX_GetVolume
   (
   void
   )

   {
   int volume;

   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
         if ( BLASTER_CardHasMixer() )
            {
            volume = BLASTER_GetVoiceVolume();
            }
         else
            {
            volume = MV_GetVolume();
            }
         break;

      case ProAudioSpectrum :
      case SoundMan16 :
         volume = PAS_GetPCMVolume();
         if ( volume == PAS_Error )
            {
            volume = MV_GetVolume();
            }
         break;

      case GenMidi :
      case SoundCanvas :
      case WaveBlaster :
         volume = 255;
         break;

      case SoundScape :
         volume = MV_GetVolume();
         break;

      case UltraSound :
         volume = GUSWAVE_GetVolume();
         break;

      // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
      case Adlib :
         volume = ADLIBFX_GetTotalVolume();
         break;

      case PC :
         volume = PCFX_GetTotalVolume();
         break;

#endif
      case SoundSource :
      case TandySoundSource :
         volume = MV_GetVolume();
         break;

      default :
         volume = 0;
      }
   return( volume );
   }


/*---------------------------------------------------------------------
   Function: FX_SetReverseStereo

   Set the orientation of the left and right channels.
---------------------------------------------------------------------*/

void FX_SetReverseStereo
   (
   int setting
   )

   {
   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         MV_SetReverseStereo( setting );
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         GUSWAVE_SetReverseStereo( setting );
         break;
#endif
      }
#else
   MV_SetReverseStereo( setting );
#endif
   }


/*---------------------------------------------------------------------
   Function: FX_GetReverseStereo

   Returns the orientation of the left and right channels.
---------------------------------------------------------------------*/

int FX_GetReverseStereo
   (
   void
   )

   {
   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         return MV_GetReverseStereo();

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         return GUSWAVE_GetReverseStereo();
#endif

      default:
         return( FALSE );
      }
#else
   return MV_GetReverseStereo();
#endif
   }


/*---------------------------------------------------------------------
   Function: FX_SetReverb

   Sets the reverb level.
---------------------------------------------------------------------*/

void FX_SetReverb
   (
   int reverb
   )

   {
   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         MV_SetReverb( reverb );
         break;
      }
#else
   MV_SetReverb( reverb );
#endif
   }


/*---------------------------------------------------------------------
   Function: FX_SetFastReverb

   Sets the reverb level.
---------------------------------------------------------------------*/

void FX_SetFastReverb
   (
   int reverb
   )

   {
   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         MV_SetFastReverb( reverb );
         break;
      }
#else
   MV_SetFastReverb( reverb );
#endif
   }


// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19950821L)
/*---------------------------------------------------------------------
   Function: FX_GetMaxReverbDelay

   Returns the maximum delay time for reverb.
---------------------------------------------------------------------*/

int FX_GetMaxReverbDelay
   (
   void
   )

   {
   return MV_GetMaxReverbDelay();
   }


/*---------------------------------------------------------------------
   Function: FX_GetReverbDelay

   Returns the current delay time for reverb.
---------------------------------------------------------------------*/

int FX_GetReverbDelay
   (
   void
   )

   {
   return MV_GetReverbDelay();
   }


/*---------------------------------------------------------------------
   Function: FX_SetReverbDelay

   Sets the delay level of reverb to add to mix.
---------------------------------------------------------------------*/

void FX_SetReverbDelay
   (
   int delay
   )

   {
   MV_SetReverbDelay( delay );
   }
#endif // LIBVER_ASSREV


/*---------------------------------------------------------------------
   Function: FX_VoiceAvailable

   Checks if a voice can be play at the specified priority.
---------------------------------------------------------------------*/

int FX_VoiceAvailable
   (
   int priority
   )

   {
   // *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   int status = FALSE;
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         status = MV_VoiceAvailable( priority );
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         status = GUSWAVE_VoiceAvailable( priority );
         break;

#endif
      case Adlib :
         status = ADLIBFX_VoiceAvailable( priority );
         break;

      case PC :
         status = PCFX_VoiceAvailable( priority );
         break;
      }
   return( status );
#else
   return MV_VoiceAvailable( priority );
#endif
   }

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV >= 19960510L)
/*---------------------------------------------------------------------
   Function: FX_EndLooping

   Stops the voice associated with the specified handle from looping
   without stoping the sound.
---------------------------------------------------------------------*/

int FX_EndLooping
   (
   int handle
   )

   {
   int status;

   status = MV_EndLooping( handle );
   if ( status == MV_Error )
      {
      FX_SetErrorCode( FX_MultiVocError );
      status = FX_Warning;
      }

   return( status );
   }
#endif

/*---------------------------------------------------------------------
   Function: FX_SetPan

   Sets the stereo and mono volume level of the voice associated
   with the specified handle.
---------------------------------------------------------------------*/

int FX_SetPan
   (
   int handle,
   int vol,
   int left,
   int right
   )

   {
// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   int status = FX_Ok;

   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         status = MV_SetPan( handle, vol, left, right );
         if ( status == MV_Error )
            {
            FX_SetErrorCode( FX_MultiVocError );
            status = FX_Warning;
            }
         break;

      case Adlib :
         status = ADLIBFX_SetVolume( handle, vol );
         if ( status == ADLIBFX_Error )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Warning;
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case PC :
         break;

      default:
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Error;
      }
#else // LIBVER_ASSREV
   int status;

   status = MV_SetPan( handle, vol, left, right );
   if ( status == MV_Error )
      {
      FX_SetErrorCode( FX_MultiVocError );
      status = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_SetPitch

   Sets the pitch of the voice associated with the specified handle.
---------------------------------------------------------------------*/

int FX_SetPitch
   (
   int handle,
   int pitchoffset
   )

   {
// *** VERSIONS RESTORATION ***
// FIXME - Verify error codes come from the right enums, in ALL files
#if (LIBVER_ASSREV < 19960116L)
   int status = FX_Ok;

   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         status = MV_SetPitch( handle, pitchoffset );
         if ( status == MV_Error )
            {
            FX_SetErrorCode( FX_MultiVocError );
            status = FX_Warning;
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         status = GUSWAVE_SetPitch( handle, pitchoffset );
         // VERSIONS RESTORATION - A vanilla bug
         if ( handle == GUSWAVE_Error )
            {
            FX_SetErrorCode( FX_SoundCardError );
            }
         break;

#endif
      case PC :
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Error;
      }
#else // LIBVER_ASSREV
   int status;

   status = MV_SetPitch( handle, pitchoffset );
   if ( status == MV_Error )
      {
      FX_SetErrorCode( FX_MultiVocError );
      status = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_SetFrequency

   Sets the frequency of the voice associated with the specified handle.
---------------------------------------------------------------------*/

int FX_SetFrequency
   (
   int handle,
   int frequency
   )

   {
   int status;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         status = MV_SetFrequency( handle, frequency );
         if ( status == MV_Error )
            {
            FX_SetErrorCode( FX_MultiVocError );
            status = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Error;
      }
#else // LIBVER_ASSREV
   status = MV_SetFrequency( handle, frequency );
   if ( status == MV_Error )
      {
      FX_SetErrorCode( FX_MultiVocError );
      status = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_PlayVOC

   Begin playback of sound data with the given volume and priority.
---------------------------------------------------------------------*/

int FX_PlayVOC
   (
   char *ptr,
   int pitchoffset,
   int vol,
   int left,
   int right,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_PlayVOC( ptr, pitchoffset, vol, left, right,
            priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         handle = GUSWAVE_PlayVOC( ptr, pitchoffset, 0, vol,
            priority, callbackval );
         if ( handle < GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

#endif
      case Adlib :
         // FIXME (RESTORATION) - Might be a vanilla bug here (swapped args)
         handle = ADLIBFX_Play( ptr, priority, vol, callbackval );
         if ( handle < ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      case PC :
         handle = PCFX_Play( ptr, priority, callbackval );
         if ( handle < PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Error;
      }
#else // LIBVER_ASSREV
   handle = MV_PlayVOC( ptr, pitchoffset, vol, left, right,
      priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_PlayLoopedVOC

   Begin playback of sound data with the given volume and priority.
---------------------------------------------------------------------*/

int FX_PlayLoopedVOC
   (
   char *ptr,
   long loopstart,
   long loopend,
   int pitchoffset,
   int vol,
   int left,
   int right,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_PlayLoopedVOC( ptr, loopstart, loopend, pitchoffset,
            vol, left, right, priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

      case Adlib :
         // FIXME (RESTORATION) - Might be a vanilla bug here (swapped args)
         handle = ADLIBFX_Play( ptr, priority, vol, callbackval );
         if ( handle < ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      case PC :
         handle = PCFX_Play( ptr, priority, callbackval );
         if ( handle < PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Error;
      }
#else // LIBVER_ASSREV
   handle = MV_PlayLoopedVOC( ptr, loopstart, loopend, pitchoffset,
      vol, left, right, priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_PlayWAV

   Begin playback of sound data with the given volume and priority.
---------------------------------------------------------------------*/

int FX_PlayWAV
   (
   char *ptr,
   int pitchoffset,
   int vol,
   int left,
   int right,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_PlayWAV( ptr, pitchoffset, vol, left, right,
            priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         handle = GUSWAVE_PlayWAV( ptr, pitchoffset, 0, vol,
            priority, callbackval );
         if ( handle < GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

#endif
      case Adlib :
         // FIXME (RESTORATION) - Might be a vanilla bug here (swapped args)
         handle = ADLIBFX_Play( ptr, priority, vol, callbackval );
         if ( handle < ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      case PC :
         handle = PCFX_Play( ptr, priority, callbackval );
         if ( handle < PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Error;
      }
#else // LIBVER_ASSREV
   handle = MV_PlayWAV( ptr, pitchoffset, vol, left, right,
      priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_PlayWAV

   Begin playback of sound data with the given volume and priority.
---------------------------------------------------------------------*/

int FX_PlayLoopedWAV
   (
   char *ptr,
   long loopstart,
   long loopend,
   int pitchoffset,
   int vol,
   int left,
   int right,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_PlayLoopedWAV( ptr, loopstart, loopend, pitchoffset,
            vol, left, right, priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

      case Adlib :
         // FIXME (RESTORATION) - Might be a vanilla bug here (swapped args)
         handle = ADLIBFX_Play( ptr, priority, vol, callbackval );
         if ( handle < ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      case PC :
         handle = PCFX_Play( ptr, priority, callbackval );
         if ( handle < PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Error;
      }
#else // LIBVER_ASSREV
   handle = MV_PlayLoopedWAV( ptr, loopstart, loopend,
      pitchoffset, vol, left, right, priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_PlayVOC3D

   Begin playback of sound data at specified angle and distance
   from listener.
---------------------------------------------------------------------*/

int FX_PlayVOC3D
   (
   char *ptr,
   int pitchoffset,
   int angle,
   int distance,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_PlayVOC3D( ptr, pitchoffset, angle, distance,
            priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         handle = GUSWAVE_PlayVOC( ptr, pitchoffset, angle, 255-distance,
            priority, callbackval );
         if ( handle < GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

#endif
      case Adlib :
         handle = ADLIBFX_Play( ptr, 255-distance, priority, callbackval );
         if ( handle < ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      case PC :
         handle = PCFX_Play( ptr, priority, callbackval );
         if ( handle < PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Error;
      }
#else // LIBVER_ASSREV
   handle = MV_PlayVOC3D( ptr, pitchoffset, angle, distance,
      priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_PlayWAV3D

   Begin playback of sound data at specified angle and distance
   from listener.
---------------------------------------------------------------------*/

int FX_PlayWAV3D
   (
   char *ptr,
   int pitchoffset,
   int angle,
   int distance,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_PlayWAV3D( ptr, pitchoffset, angle, distance,
            priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         handle = GUSWAVE_PlayWAV( ptr, pitchoffset, angle, 255-distance,
            priority, callbackval );
         if ( handle < GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

#endif
      case Adlib :
         handle = ADLIBFX_Play( ptr, 255-distance, priority, callbackval );
         if ( handle < ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      case PC :
         handle = PCFX_Play( ptr, priority, callbackval );
         if ( handle < PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Error;
      }
#else // LIBVER_ASSREV
   handle = MV_PlayWAV3D( ptr, pitchoffset, angle, distance,
      priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_PlayRaw

   Begin playback of raw sound data with the given volume and priority.
---------------------------------------------------------------------*/

int FX_PlayRaw
   (
   char *ptr,
   unsigned long length,
   unsigned rate,
   int pitchoffset,
   int vol,
   int left,
   int right,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_PlayRaw( ptr, length, rate, pitchoffset,
            vol, left, right, priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

      case Adlib :
         // FIXME (RESTORATION) - Might be a vanilla bug here (swapped args)
         handle = ADLIBFX_Play( ptr, priority, vol, callbackval );
         if ( handle < ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      case PC :
         handle = PCFX_Play( ptr, priority, callbackval );
         if ( handle < PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Error;
      }
#else // LIBVER_ASSREV
   handle = MV_PlayRaw( ptr, length, rate, pitchoffset,
      vol, left, right, priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_PlayLoopedRaw

   Begin playback of raw sound data with the given volume and priority.
---------------------------------------------------------------------*/

int FX_PlayLoopedRaw
   (
   char *ptr,
   unsigned long length,
   char *loopstart,
   char *loopend,
   unsigned rate,
   int pitchoffset,
   int vol,
   int left,
   int right,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_PlayLoopedRaw( ptr, length, loopstart, loopend,
            rate, pitchoffset, vol, left, right, priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

      case Adlib :
         // FIXME (RESTORATION) - Might be a vanilla bug here (swapped args)
         handle = ADLIBFX_Play( ptr, priority, vol, callbackval );
         if ( handle < ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      case PC :
         handle = PCFX_Play( ptr, priority, callbackval );
         if ( handle < PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Error;
      }
#else // LIBVER_ASSREV
   handle = MV_PlayLoopedRaw( ptr, length, loopstart, loopend,
      rate, pitchoffset, vol, left, right, priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_Pan3D

   Set the angle and distance from the listener of the voice associated
   with the specified handle.
---------------------------------------------------------------------*/

int FX_Pan3D
   (
   int handle,
   int angle,
   int distance
   )

   {
// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   int status = FX_Ok;

   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         status = MV_Pan3D( handle, angle, distance );
         if ( status != MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            status = FX_Warning;
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         status = GUSWAVE_SetPan3D( handle, angle, distance );
         break;

#endif
      case Adlib :
         status = ADLIBFX_SetVolume( handle, 255-distance );
         if ( status != ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            status = FX_Warning;
            }
         break;

      case PC :
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Error;
      }
#else // LIBVER_ASSREV
   int status;

   status = MV_Pan3D( handle, angle, distance );
   if ( status != MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      status = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_SoundActive

   Tests if the specified sound is currently playing.
---------------------------------------------------------------------*/

int FX_SoundActive
   (
   int handle
   )

   {
// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundScape :
      case SoundSource :
      case TandySoundSource :
         return( MV_VoicePlaying( handle ) );

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         return( GUSWAVE_VoicePlaying( handle ) );

#endif
      case Adlib :
         return( ADLIBFX_SoundPlaying( handle ) );

      case PC :
         return( PCFX_SoundPlaying( handle ) );
      }
   return( FALSE );
#else // LIBVER_ASSREV
   return( MV_VoicePlaying( handle ) );
#endif // LIBVER_ASSREV
   }


/*---------------------------------------------------------------------
   Function: FX_SoundsPlaying

   Reports the number of voices playing.
---------------------------------------------------------------------*/

int FX_SoundsPlaying
   (
   void
   )

   {
// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         return( MV_VoicesPlaying() );
#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION

      case UltraSound :
         return( GUSWAVE_VoicesPlaying() );
#endif
      }
   return( FALSE );
#else // LIBVER_ASSREV
   return( MV_VoicesPlaying() );
#endif // LIBVER_ASSREV
   }


/*---------------------------------------------------------------------
   Function: FX_StopSound

   Halts playback of a specific voice
---------------------------------------------------------------------*/

int FX_StopSound
   (
   int handle
   )

   {
   int status;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         status = MV_Kill( handle );
         if ( status != MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            return( FX_Warning );
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         status = GUSWAVE_Kill( handle );
         if ( status != GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            return( FX_Warning );
            }
         break;

#endif
      case Adlib :
         status = ADLIBFX_Stop( handle );
         if ( status != ADLIBFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            return( FX_Warning );
            }
         break;

      case PC :
         status = PCFX_Stop( handle );
         if ( status != PCFX_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            return( FX_Warning );
            }
         break;

      default:
         FX_SetErrorCode( FX_InvalidCard );
       }
#else // LIBVER_ASSREV
   status = MV_Kill( handle );
   if ( status != MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      return( FX_Warning );
      }
#endif // LIBVER_ASSREV

   return( FX_Ok );
   }


/*---------------------------------------------------------------------
   Function: FX_StopAllSounds

   Halts playback of all sounds.
---------------------------------------------------------------------*/

int FX_StopAllSounds
   (
   void
   )

   {
   int status;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         status = MV_KillAllVoices();
         if ( status != MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            return( FX_Warning );
            }
         break;

#if (LIBVER_ASSREV < 19950821L) // VERSIONS RESTORATION
      case UltraSound :
         status = GUSWAVE_KillAllVoices();
         if ( status != GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            return( FX_Warning );
            }
         break;

#endif
      case Adlib :
      case PC :
         break;

      default:
         FX_SetErrorCode( FX_InvalidCard );
      }
#else // LIBVER_ASSREV
   status = MV_KillAllVoices();
   if ( status != MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      return( FX_Warning );
      }
#endif // LIBVER_ASSREV

   return( FX_Ok );
   }


/*---------------------------------------------------------------------
   Function: FX_StartDemandFeedPlayback

   Plays a digitized sound from a user controlled buffering system.
---------------------------------------------------------------------*/

int FX_StartDemandFeedPlayback
   (
   void ( *function )( char **ptr, unsigned long *length ),
   int rate,
   int pitchoffset,
   int vol,
   int left,
   int right,
   int priority,
   unsigned long callbackval
   )

   {
   int handle;

// *** VERSIONS RESTORATION ***
#if (LIBVER_ASSREV < 19960116L)
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
      case SoundScape :
#if (LIBVER_ASSREV >= 19950821L) // VERSIONS RESTORATION
      case UltraSound :
#endif
      case SoundSource :
      case TandySoundSource :
         handle = MV_StartDemandFeedPlayback( function, rate,
            pitchoffset, vol, left, right, priority, callbackval );
         if ( handle < MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            handle = FX_Warning;
            }
         break;

#if (LIBVER_ASSREV < 19950821L)
      case UltraSound :
         handle = GUSWAVE_StartDemandFeedPlayback( function, rate,
            pitchoffset, 0, vol, priority, callbackval );
         if ( handle < GUSWAVE_Ok )
            {
            FX_SetErrorCode( FX_SoundCardError );
            handle = FX_Warning;
            }
         break;

#endif
      default:
         FX_SetErrorCode( FX_InvalidCard );
         handle = FX_Warning;
      }
#else // LIBVER_ASSREV
   handle = MV_StartDemandFeedPlayback( function, rate,
      pitchoffset, vol, left, right, priority, callbackval );
   if ( handle < MV_Ok )
      {
      FX_SetErrorCode( FX_MultiVocError );
      handle = FX_Warning;
      }
#endif // LIBVER_ASSREV

   return( handle );
   }


/*---------------------------------------------------------------------
   Function: FX_StartRecording

   Starts the sound recording engine.
---------------------------------------------------------------------*/

int FX_StartRecording
   (
   int MixRate,
   void ( *function )( char *ptr, int length )
   )

   {
   int status;

   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
         status = MV_StartRecording( MixRate, function );
         if ( status != MV_Ok )
            {
            FX_SetErrorCode( FX_MultiVocError );
            status = FX_Warning;
            }
         else
            {
            status = FX_Ok;
            }
         break;

      default :
         FX_SetErrorCode( FX_InvalidCard );
         status = FX_Warning;
         break;
      }

   return( status );
   }


/*---------------------------------------------------------------------
   Function: FX_StopRecord

   Stops the sound record engine.
---------------------------------------------------------------------*/

void FX_StopRecord
   (
   void
   )

   {
   // Stop sound playback
   switch( FX_SoundDevice )
      {
      case SoundBlaster :
      case Awe32 :
      case ProAudioSpectrum :
      case SoundMan16 :
         MV_StopRecord();
         break;
      }
   }
