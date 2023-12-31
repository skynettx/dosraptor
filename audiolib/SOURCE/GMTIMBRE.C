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
#if (LIBVER_ASSREV < 19950821L) // *** VERSIONS RESTORATION ***
#include "memcheck.h"
#endif
typedef struct
   {
   unsigned char SAVEK[ 2 ];
   unsigned char Level[ 2 ];
   unsigned char Env1[ 2 ];
   unsigned char Env2[ 2 ];
   unsigned char Wave[ 2 ];
   unsigned char Feedback;
#if (LIBVER_ASSREV < 19950821L) // *** VERSIONS RESTORATION ***
   unsigned char Transpose;
#else
   signed   char Transpose;
   signed   char Velocity;
#endif
   } TIMBRE;

TIMBRE ADLIB_TimbreBank[ 256 ] =
   {
      { { 33, 33 }, { 143, 6 }, { 242, 242 }, { 69, 118 }, { 0, 0 }, 8, 0 },
      { { 49, 33 }, { 75, 0 }, { 242, 242 }, { 84, 86 }, { 0, 0 }, 8, 0 },
      { { 49, 33 }, { 73, 0 }, { 242, 242 }, { 85, 118 }, { 0, 0 }, 8, 0 },
      { { 177, 97 }, { 14, 0 }, { 242, 243 }, { 59, 11 }, { 0, 0 }, 6, 0 },
      { { 1, 33 }, { 87, 0 }, { 241, 241 }, { 56, 40 }, { 0, 0 }, 0, 0 },
      { { 1, 33 }, { 147, 0 }, { 241, 241 }, { 56, 40 }, { 0, 0 }, 0, 0 },
      { { 33, 54 }, { 128, 14 }, { 162, 241 }, { 1, 213 }, { 0, 0 }, 8, 0 },
      { { 1, 1 }, { 146, 0 }, { 194, 194 }, { 168, 88 }, { 0, 0 }, 10, 0 },
      { { 12, 129 }, { 92, 0 }, { 246, 243 }, { 84, 181 }, { 0, 0 }, 0, 0 },
      { { 7, 17 }, { 151, 128 }, { 246, 245 }, { 50, 17 }, { 0, 0 }, 2, 0 },
      { { 23, 1 }, { 33, 0 }, { 86, 246 }, { 4, 4 }, { 0, 0 }, 2, 0 },
      { { 24, 129 }, { 98, 0 }, { 243, 242 }, { 230, 246 }, { 0, 0 }, 0, 0 },
      { { 24, 33 }, { 35, 0 }, { 247, 229 }, { 85, 216 }, { 0, 0 }, 0, 0 },
      { { 21, 1 }, { 145, 0 }, { 246, 246 }, { 166, 230 }, { 0, 0 }, 4, 0 },
      { { 69, 129 }, { 89, 128 }, { 211, 163 }, { 130, 227 }, { 0, 0 }, 12, 0 },
      { { 3, 129 }, { 73, 128 }, { 116, 179 }, { 85, 5 }, { 1, 0 }, 4, 0 },
      { { 113, 49 }, { 146, 0 }, { 246, 241 }, { 20, 7 }, { 0, 0 }, 2, 0 },
      { { 114, 48 }, { 20, 0 }, { 199, 199 }, { 88, 8 }, { 0, 0 }, 2, 0 },
      { { 112, 177 }, { 68, 0 }, { 170, 138 }, { 24, 8 }, { 0, 0 }, 4, 0 },
      { { 35, 177 }, { 147, 0 }, { 151, 85 }, { 35, 20 }, { 1, 0 }, 4, 0 },
      { { 97, 177 }, { 19, 128 }, { 151, 85 }, { 4, 4 }, { 1, 0 }, 0, 0 },
      { { 36, 177 }, { 72, 0 }, { 152, 70 }, { 42, 26 }, { 1, 0 }, 12, 0 },
      { { 97, 33 }, { 19, 0 }, { 145, 97 }, { 6, 7 }, { 1, 0 }, 10, 0 },
      { { 33, 161 }, { 19, 137 }, { 113, 97 }, { 6, 7 }, { 0, 0 }, 6, 0 },
      { { 2, 65 }, { 156, 128 }, { 243, 243 }, { 148, 200 }, { 1, 0 }, 12, 0 },
      { { 3, 17 }, { 84, 0 }, { 243, 241 }, { 154, 231 }, { 1, 0 }, 12, 0 },
      { { 35, 33 }, { 95, 0 }, { 241, 242 }, { 58, 248 }, { 0, 0 }, 0, 0 },
      { { 3, 33 }, { 135, 128 }, { 246, 243 }, { 34, 243 }, { 1, 0 }, 6, 0 },
      { { 3, 33 }, { 71, 0 }, { 249, 246 }, { 84, 58 }, { 0, 0 }, 0, 0 },
      { { 35, 33 }, { 72, 0 }, { 149, 132 }, { 25, 25 }, { 1, 0 }, 8, 0 },
      { { 35, 33 }, { 74, 0 }, { 149, 148 }, { 25, 25 }, { 1, 0 }, 8, 0 },
      { { 9, 132 }, { 161, 128 }, { 32, 209 }, { 79, 248 }, { 0, 0 }, 8, 0 },
      { { 33, 162 }, { 30, 0 }, { 148, 195 }, { 6, 166 }, { 0, 0 }, 2, 0 },
      { { 49, 49 }, { 18, 0 }, { 241, 241 }, { 40, 24 }, { 0, 0 }, 10, 0 },
      { { 49, 49 }, { 141, 0 }, { 241, 241 }, { 232, 120 }, { 0, 0 }, 10, 0 },
      { { 49, 50 }, { 91, 0 }, { 81, 113 }, { 40, 72 }, { 0, 0 }, 12, 0 },
      { { 1, 33 }, { 139, 64 }, { 161, 242 }, { 154, 223 }, { 0, 0 }, 8, 0 },
      { { 1, 33 }, { 137, 64 }, { 161, 242 }, { 154, 223 }, { 0, 0 }, 8, 0 },
      { { 49, 49 }, { 139, 0 }, { 244, 241 }, { 232, 120 }, { 0, 0 }, 10, 0 },
      { { 49, 49 }, { 18, 0 }, { 241, 241 }, { 40, 24 }, { 0, 0 }, 10, 0 },
      { { 49, 33 }, { 21, 0 }, { 221, 86 }, { 19, 38 }, { 1, 0 }, 8, 0 },
      { { 49, 33 }, { 22, 0 }, { 221, 102 }, { 19, 6 }, { 1, 0 }, 8, 0 },
      { { 113, 49 }, { 73, 0 }, { 209, 97 }, { 28, 12 }, { 1, 0 }, 8, 0 },
      { { 33, 35 }, { 77, 128 }, { 113, 114 }, { 18, 6 }, { 1, 0 }, 2, 0 },
      { { 241, 225 }, { 64, 0 }, { 241, 111 }, { 33, 22 }, { 1, 0 }, 2, 0 },
      { { 2, 1 }, { 26, 128 }, { 245, 133 }, { 117, 53 }, { 1, 0 }, 0, 0 },
      { { 2, 1 }, { 29, 128 }, { 245, 243 }, { 117, 244 }, { 1, 0 }, 0, 0 },
      { { 16, 17 }, { 65, 0 }, { 245, 242 }, { 5, 195 }, { 1, 0 }, 2, 0 },
      { { 33, 162 }, { 155, 1 }, { 177, 114 }, { 37, 8 }, { 1, 0 }, 14, 0 },
      { { 161, 33 }, { 152, 0 }, { 127, 63 }, { 3, 7 }, { 1, 1 }, 0, 0 },
      { { 161, 97 }, { 147, 0 }, { 193, 79 }, { 18, 5 }, { 0, 0 }, 10, 0 },
      { { 33, 97 }, { 24, 0 }, { 193, 79 }, { 34, 5 }, { 0, 0 }, 12, 0 },
      { { 49, 114 }, { 91, 131 }, { 244, 138 }, { 21, 5 }, { 0, 0 }, 0, 0 },
      { { 161, 97 }, { 144, 0 }, { 116, 113 }, { 57, 103 }, { 0, 0 }, 0, 0 },
      { { 113, 114 }, { 87, 0 }, { 84, 122 }, { 5, 5 }, { 0, 0 }, 12, 0 },
      { { 144, 65 }, { 0, 0 }, { 84, 165 }, { 99, 69 }, { 0, 0 }, 8, 0 },
      { { 33, 33 }, { 146, 1 }, { 133, 143 }, { 23, 9 }, { 0, 0 }, 12, 0 },
      { { 33, 33 }, { 148, 5 }, { 117, 143 }, { 23, 9 }, { 0, 0 }, 12, 0 },
      { { 33, 97 }, { 148, 0 }, { 118, 130 }, { 21, 55 }, { 0, 0 }, 12, 0 },
      { { 49, 33 }, { 67, 0 }, { 158, 98 }, { 23, 44 }, { 1, 1 }, 2, 0 },
      { { 33, 33 }, { 155, 0 }, { 97, 127 }, { 106, 10 }, { 0, 0 }, 2, 0 },
      { { 97, 34 }, { 138, 6 }, { 117, 116 }, { 31, 15 }, { 0, 0 }, 8, 0 },
      { { 161, 33 }, { 134, 13 }, { 114, 113 }, { 85, 24 }, { 1, 0 }, 0, 0 },
      { { 33, 33 }, { 77, 0 }, { 84, 166 }, { 60, 28 }, { 0, 0 }, 8, 0 },
      { { 49, 97 }, { 143, 0 }, { 147, 114 }, { 2, 11 }, { 1, 0 }, 8, 0 },
      { { 49, 97 }, { 142, 0 }, { 147, 114 }, { 3, 9 }, { 1, 0 }, 8, 0 },
      { { 49, 97 }, { 145, 0 }, { 147, 130 }, { 3, 9 }, { 1, 0 }, 10, 0 },
      { { 49, 97 }, { 142, 0 }, { 147, 114 }, { 15, 15 }, { 1, 0 }, 10, 0 },
      { { 33, 33 }, { 75, 0 }, { 170, 143 }, { 22, 10 }, { 1, 0 }, 8, 0 },
      { { 49, 33 }, { 144, 0 }, { 126, 139 }, { 23, 12 }, { 1, 1 }, 6, 0 },
      { { 49, 50 }, { 129, 0 }, { 117, 97 }, { 25, 25 }, { 1, 0 }, 0, 0 },
      { { 50, 33 }, { 144, 0 }, { 155, 114 }, { 33, 23 }, { 0, 0 }, 4, 0 },
      { { 225, 225 }, { 31, 0 }, { 133, 101 }, { 95, 26 }, { 0, 0 }, 0, 0 },
      { { 225, 225 }, { 70, 0 }, { 136, 101 }, { 95, 26 }, { 0, 0 }, 0, 0 },
      { { 161, 33 }, { 156, 0 }, { 117, 117 }, { 31, 10 }, { 0, 0 }, 2, 0 },
      { { 49, 33 }, { 139, 0 }, { 132, 101 }, { 88, 26 }, { 0, 0 }, 0, 0 },
      { { 225, 161 }, { 76, 0 }, { 102, 101 }, { 86, 38 }, { 0, 0 }, 0, 0 },
      { { 98, 161 }, { 203, 0 }, { 118, 85 }, { 70, 54 }, { 0, 0 }, 0, 0 },
      { { 98, 161 }, { 153, 0 }, { 87, 86 }, { 7, 7 }, { 0, 0 }, 11, 0 },
      { { 98, 161 }, { 147, 0 }, { 119, 118 }, { 7, 7 }, { 0, 0 }, 11, 0 },
      { { 34, 33 }, { 89, 0 }, { 255, 255 }, { 3, 15 }, { 2, 0 }, 0, 0 },
      { { 33, 33 }, { 14, 0 }, { 255, 255 }, { 15, 15 }, { 1, 1 }, 0, 0 },
      { { 34, 33 }, { 70, 128 }, { 134, 100 }, { 85, 24 }, { 0, 0 }, 0, 0 },
      { { 33, 161 }, { 69, 0 }, { 102, 150 }, { 18, 10 }, { 0, 0 }, 0, 0 },
      { { 33, 34 }, { 139, 0 }, { 146, 145 }, { 42, 42 }, { 1, 0 }, 0, 0 },
      { { 162, 97 }, { 158, 64 }, { 223, 111 }, { 5, 7 }, { 0, 0 }, 2, 0 },
      { { 32, 96 }, { 26, 0 }, { 239, 143 }, { 1, 6 }, { 0, 2 }, 0, 0 },
      { { 33, 33 }, { 143, 128 }, { 241, 244 }, { 41, 9 }, { 0, 0 }, 10, 0 },
      { { 119, 161 }, { 165, 0 }, { 83, 160 }, { 148, 5 }, { 0, 0 }, 2, 0 },
      { { 97, 177 }, { 31, 128 }, { 168, 37 }, { 17, 3 }, { 0, 0 }, 10, 0 },
      { { 97, 97 }, { 23, 0 }, { 145, 85 }, { 52, 22 }, { 0, 0 }, 12, 0 },
      { { 113, 114 }, { 93, 0 }, { 84, 106 }, { 1, 3 }, { 0, 0 }, 0, 0 },
      { { 33, 162 }, { 151, 0 }, { 33, 66 }, { 67, 53 }, { 0, 0 }, 8, 0 },
      { { 161, 33 }, { 28, 0 }, { 161, 49 }, { 119, 71 }, { 1, 1 }, 0, 0 },
      { { 33, 97 }, { 137, 3 }, { 17, 66 }, { 51, 37 }, { 0, 0 }, 10, 0 },
      { { 161, 33 }, { 21, 0 }, { 17, 207 }, { 71, 7 }, { 1, 0 }, 0, 0 },
      { { 58, 81 }, { 206, 0 }, { 248, 134 }, { 246, 2 }, { 0, 0 }, 2, 0 },
      { { 33, 33 }, { 21, 0 }, { 33, 65 }, { 35, 19 }, { 1, 0 }, 0, 0 },
      { { 6, 1 }, { 91, 0 }, { 116, 165 }, { 149, 114 }, { 0, 0 }, 0, 0 },
      { { 34, 97 }, { 146, 131 }, { 177, 242 }, { 129, 38 }, { 0, 0 }, 12, 0 },
      { { 65, 66 }, { 77, 0 }, { 241, 242 }, { 81, 245 }, { 1, 0 }, 0, 0 },
      { { 97, 163 }, { 148, 128 }, { 17, 17 }, { 81, 19 }, { 1, 0 }, 6, 0 },
      { { 97, 161 }, { 140, 128 }, { 17, 29 }, { 49, 3 }, { 0, 0 }, 6, 0 },
      { { 164, 97 }, { 76, 0 }, { 243, 129 }, { 115, 35 }, { 1, 0 }, 4, 0 },
      { { 2, 7 }, { 133, 3 }, { 210, 242 }, { 83, 246 }, { 0, 1 }, 0, 0 },
      { { 17, 19 }, { 12, 128 }, { 163, 162 }, { 17, 229 }, { 1, 0 }, 0, 0 },
      { { 17, 17 }, { 6, 0 }, { 246, 242 }, { 65, 230 }, { 1, 2 }, 4, 0 },
      { { 147, 145 }, { 145, 0 }, { 212, 235 }, { 50, 17 }, { 0, 1 }, 8, 0 },
      { { 4, 1 }, { 79, 0 }, { 250, 194 }, { 86, 5 }, { 0, 0 }, 12, 0 },
      { { 33, 34 }, { 73, 0 }, { 124, 111 }, { 32, 12 }, { 0, 1 }, 6, 0 },
      { { 49, 33 }, { 133, 0 }, { 221, 86 }, { 51, 22 }, { 1, 0 }, 10, 0 },
      { { 32, 33 }, { 4, 129 }, { 218, 143 }, { 5, 11 }, { 2, 0 }, 6, 0 },
      { { 5, 3 }, { 106, 128 }, { 241, 195 }, { 229, 229 }, { 0, 0 }, 6, 0 },
      { { 7, 2 }, { 21, 0 }, { 236, 248 }, { 38, 22 }, { 0, 0 }, 10, 0 },
      { { 5, 1 }, { 157, 0 }, { 103, 223 }, { 53, 5 }, { 0, 0 }, 8, 0 },
      { { 24, 18 }, { 150, 0 }, { 250, 248 }, { 40, 229 }, { 0, 0 }, 10, 0 },
      { { 16, 0 }, { 134, 3 }, { 168, 250 }, { 7, 3 }, { 0, 0 }, 6, 0 },
      { { 17, 16 }, { 65, 3 }, { 248, 243 }, { 71, 3 }, { 2, 0 }, 4, 0 },
      { { 1, 16 }, { 142, 0 }, { 241, 243 }, { 6, 2 }, { 2, 0 }, 14, 0 },
      { { 14, 192 }, { 0, 0 }, { 31, 31 }, { 0, 255 }, { 0, 3 }, 14, 0 },
      { { 6, 3 }, { 128, 136 }, { 248, 86 }, { 36, 132 }, { 0, 2 }, 14, 0 },
      { { 14, 208 }, { 0, 5 }, { 248, 52 }, { 0, 4 }, { 0, 3 }, 14, 0 },
      { { 14, 192 }, { 0, 0 }, { 246, 31 }, { 0, 2 }, { 0, 3 }, 14, 0 },
      { { 213, 218 }, { 149, 64 }, { 55, 86 }, { 163, 55 }, { 0, 0 }, 0, 0 },
      { { 53, 20 }, { 92, 8 }, { 178, 244 }, { 97, 21 }, { 2, 0 }, 10, 0 },
      { { 14, 208 }, { 0, 0 }, { 246, 79 }, { 0, 245 }, { 0, 3 }, 14, 0 },
      { { 38, 228 }, { 0, 0 }, { 255, 18 }, { 1, 22 }, { 0, 1 }, 14, 0 },
      { { 0, 0 }, { 0, 0 }, { 243, 246 }, { 240, 201 }, { 0, 2 }, 14, 0 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 0, 0 }, { 0, 0 }, { 252, 250 }, { 5, 23 }, { 2, 0 }, 14, 52 },
      { { 0, 1 }, { 2, 0 }, { 255, 255 }, { 7, 8 }, { 0, 0 }, 0, 48 },
      { { 0, 0 }, { 0, 0 }, { 252, 250 }, { 5, 23 }, { 2, 0 }, 14, 58 },
      { { 0, 0 }, { 0, 0 }, { 246, 246 }, { 12, 6 }, { 0, 0 }, 4, 60 },
      { { 12, 18 }, { 0, 0 }, { 246, 251 }, { 8, 71 }, { 0, 2 }, 10, 47 },
      { { 0, 0 }, { 3, 0 }, { 248, 246 }, { 42, 69 }, { 0, 1 }, 4, 43 },
      { { 12, 18 }, { 0, 0 }, { 246, 251 }, { 8, 71 }, { 0, 2 }, 10, 49 },
      { { 0, 0 }, { 3, 0 }, { 248, 246 }, { 42, 69 }, { 0, 1 }, 4, 43 },
      { { 12, 18 }, { 0, 0 }, { 246, 251 }, { 8, 71 }, { 0, 2 }, 10, 51 },
      { { 0, 0 }, { 3, 0 }, { 248, 246 }, { 42, 69 }, { 0, 1 }, 4, 43 },
      { { 12, 18 }, { 0, 0 }, { 246, 251 }, { 8, 71 }, { 0, 2 }, 10, 54 },
      { { 12, 18 }, { 0, 0 }, { 246, 251 }, { 8, 71 }, { 0, 2 }, 10, 57 },
      { { 0, 0 }, { 3, 0 }, { 248, 246 }, { 42, 69 }, { 0, 1 }, 4, 72 },
      { { 12, 18 }, { 0, 0 }, { 246, 251 }, { 8, 71 }, { 0, 2 }, 10, 60 },
      { { 14, 208 }, { 0, 10 }, { 245, 159 }, { 48, 2 }, { 0, 0 }, 14, 76 },
      { { 14, 7 }, { 10, 93 }, { 228, 245 }, { 228, 229 }, { 3, 1 }, 6, 84 },
      { { 2, 5 }, { 3, 10 }, { 180, 151 }, { 4, 247 }, { 0, 0 }, 14, 36 },
      { { 78, 158 }, { 0, 0 }, { 246, 159 }, { 0, 2 }, { 0, 3 }, 14, 76 },
      { { 17, 16 }, { 69, 8 }, { 248, 243 }, { 55, 5 }, { 2, 0 }, 8, 84 },
      { { 14, 208 }, { 0, 0 }, { 246, 159 }, { 0, 2 }, { 0, 3 }, 14, 83 },
      { { 128, 16 }, { 0, 13 }, { 255, 255 }, { 3, 20 }, { 3, 0 }, 12, 84 },
      { { 14, 7 }, { 8, 81 }, { 248, 244 }, { 66, 228 }, { 0, 3 }, 14, 24 },
      { { 14, 208 }, { 0, 10 }, { 245, 159 }, { 48, 2 }, { 0, 0 }, 14, 77 },
      { { 1, 2 }, { 0, 0 }, { 250, 200 }, { 191, 151 }, { 0, 0 }, 7, 60 },
      { { 1, 1 }, { 81, 0 }, { 250, 250 }, { 135, 183 }, { 0, 0 }, 6, 65 },
      { { 1, 2 }, { 84, 0 }, { 250, 248 }, { 141, 184 }, { 0, 0 }, 6, 59 },
      { { 1, 2 }, { 89, 0 }, { 250, 248 }, { 136, 182 }, { 0, 0 }, 6, 51 },
      { { 1, 0 }, { 0, 0 }, { 249, 250 }, { 10, 6 }, { 3, 0 }, 14, 45 },
      { { 0, 0 }, { 128, 0 }, { 249, 246 }, { 137, 108 }, { 3, 0 }, 14, 71 },
      { { 3, 12 }, { 128, 8 }, { 248, 246 }, { 136, 182 }, { 3, 0 }, 15, 60 },
      { { 3, 12 }, { 133, 0 }, { 248, 246 }, { 136, 182 }, { 3, 0 }, 15, 58 },
      { { 14, 0 }, { 64, 8 }, { 118, 119 }, { 79, 24 }, { 0, 2 }, 14, 53 },
      { { 14, 3 }, { 64, 0 }, { 200, 155 }, { 73, 105 }, { 0, 2 }, 14, 64 },
      { { 215, 199 }, { 220, 0 }, { 173, 141 }, { 5, 5 }, { 3, 0 }, 14, 71 },
      { { 215, 199 }, { 220, 0 }, { 168, 136 }, { 4, 4 }, { 3, 0 }, 14, 61 },
      { { 128, 17 }, { 0, 0 }, { 246, 103 }, { 6, 23 }, { 3, 3 }, 14, 61 },
      { { 128, 17 }, { 0, 9 }, { 245, 70 }, { 5, 22 }, { 2, 3 }, 14, 48 },
      { { 6, 21 }, { 63, 0 }, { 0, 247 }, { 244, 245 }, { 0, 0 }, 1, 48 },
      { { 6, 18 }, { 63, 0 }, { 0, 247 }, { 244, 245 }, { 3, 0 }, 0, 69 },
      { { 6, 18 }, { 63, 0 }, { 0, 247 }, { 244, 245 }, { 0, 0 }, 1, 68 },
      { { 1, 2 }, { 88, 0 }, { 103, 117 }, { 231, 7 }, { 0, 0 }, 0, 63 },
      { { 65, 66 }, { 69, 8 }, { 248, 117 }, { 72, 5 }, { 0, 0 }, 0, 74 },
      { { 10, 30 }, { 64, 78 }, { 224, 255 }, { 240, 5 }, { 3, 0 }, 8, 60 },
      { { 10, 30 }, { 124, 82 }, { 224, 255 }, { 240, 2 }, { 3, 0 }, 8, 80 },
      { { 14, 0 }, { 64, 8 }, { 122, 123 }, { 74, 27 }, { 0, 2 }, 14, 64 },
      { { 14, 7 }, { 10, 64 }, { 228, 85 }, { 228, 57 }, { 3, 1 }, 6, 69 },
      { { 5, 4 }, { 5, 64 }, { 249, 214 }, { 50, 165 }, { 3, 0 }, 14, 73 },
      { { 2, 21 }, { 63, 0 }, { 0, 247 }, { 243, 245 }, { 3, 0 }, 8, 75 },
      { { 1, 2 }, { 79, 0 }, { 250, 248 }, { 141, 181 }, { 0, 0 }, 7, 68 },
      { { 0, 0 }, { 0, 0 }, { 246, 246 }, { 12, 6 }, { 0, 0 }, 4, 48 },
      { { 33, 17 }, { 17, 0 }, { 163, 196 }, { 67, 34 }, { 2, 0 }, 13, 53 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 },
      { { 16, 17 }, { 68, 0 }, { 248, 243 }, { 119, 6 }, { 2, 0 }, 8, 35 }
   };
