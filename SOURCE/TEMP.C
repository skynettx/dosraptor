               PTR_DrawCursor ( FALSE );
               GFX_FadeOut ( 0, 0, 0, 64 );
               memset ( displaybuffer, 0, 64000 );
               GFX_MarkUpdate ( 0, 0, 320, 200 );
               GFX_DisplayUpdate();
               GFX_SetPalette ( palette, 0 );
               SND_PlaySong ( SONG11_MUS );
               INTRO_PlayMain();
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
               GFX_FadeIn ( palette, 64 );
               PTR_DrawCursor ( TRUE );
