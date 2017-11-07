/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#ifndef DEMO_CLIPS_H
#define DEMO_CLIPS_H
#include "note_macros.h"

/* Melodies */

static const unsigned char melody_1[] = 
{
0,  0,  72,
2,  0,  72,

4,  0,  72,
6,  0,  70,
7,  0,  72,
8,  0,  PREV_NOTE,
9,  0,  PREV_NOTE,
10,  0,  PREV_NOTE,

14, 0, 67, 
15, 0, 70, 

16, 0, 72,
18, 0, 72,
20, 0, 72,
22, 0, 75,
23, 0, 72,
24, 0, PREV_NOTE,
25, 0, PREV_NOTE,
26, 0, PREV_NOTE,
27, 0, PREV_NOTE,
28, 0, PREV_NOTE,
};

static const unsigned char melody_2[] =
{
0,  0,  75,
2,  0,  75,

4,  0,  75,
6,  0,  72,
7,  0,  75,
8,  0,  PREV_NOTE,
9,  0,  PREV_NOTE,
10,  0,  PREV_NOTE,

14, 0, 70, 
15, 0, 72, 

16, 0, 75,
18, 0, 75,
20, 0, 75,
22, 0, 77,
23, 0, 75,
24, 0, PREV_NOTE,
25, 0, PREV_NOTE,
26, 0, PREV_NOTE,
27, 0, PREV_NOTE,
28, 0, PREV_NOTE,
};

static const unsigned char melody_3[] =
{
4, 0, 79,
5, 0, 77,
6, 0, 75,
7, 0, 72,
8, 0, PREV_NOTE,
9, 0, PREV_NOTE,
10, 0, 70,
11, 0, PREV_NOTE,

16, 0, 74,
17, 0, PREV_NOTE,

19, 0, 70,
20, 0, PREV_NOTE,

22, 0, 72,
23, 0, PREV_NOTE,
24, 0, PREV_NOTE,
25, 0, PREV_NOTE,
};

static const unsigned char melody_4[] =
{
0,  0,  67,         1,
1,  0,  PREV_NOTE,  1,
2,  0,  70,         2,
3,  0,  PREV_NOTE,  2,
4,  0,  PREV_NOTE,  2,

6,  0,  63,         1,
7,  0,  PREV_NOTE,  1,
8,  0,  67,         2,
9,  0,  PREV_NOTE,  2,
10,  0,  PREV_NOTE,  2,

12,  0,  60,  1,
13,  0,  PREV_NOTE,  1,
14,  0,  63,  2,
15,  0,  PREV_NOTE,  2,

16,  0,  PREV_NOTE,  2,
17,  0,  PREV_NOTE,  2,
18,  0,  65,  2,
19,  0,  PREV_NOTE,  2,
20,  0,  PREV_NOTE,  2,
21,  0,  PREV_NOTE,  2,
22,  0,  67,  2,
23,  0,  PREV_NOTE,  2,
24,  0,  PREV_NOTE,  2,
25,  0,  PREV_NOTE,  2,


};

static const unsigned char melody_5[] =
{
2, 0, 67,
4, 0, 72,
6, 0, 79,
7, 0, PREV_NOTE,
8, 0, PREV_NOTE,
9, 0, PREV_NOTE,
10, 0, PREV_NOTE,
11, 0, PREV_NOTE,

14, 0, 77,
15, 0, 75,

16, 0, 74,
17, 0, PREV_NOTE,
18, 0, PREV_NOTE,

20, 0, 75,
21, 0, PREV_NOTE,
22, 0, 74,
23, 0, PREV_NOTE,
24, 0, PREV_NOTE,

26, 0, 70,
27, 0, PREV_NOTE,
28, 0, PREV_NOTE,
29, 0, PREV_NOTE,

};

/* Drum Loops */

static const unsigned char drum_loop_1[] =
{
0,   0,     60,
1,   0,     PREV_NOTE,
2,   0,     PREV_NOTE,
3,   0,     60,

4,   0,     PREV_NOTE,
5,   0,     PREV_NOTE,
6,   0,     60,
7,   0,     PREV_NOTE,

8,   0,     PREV_NOTE,
9,   0,     PREV_NOTE,
10,  0,     60,
11,  0,     PREV_NOTE,

12,  0,     60,
13,  0,     PREV_NOTE,
14,  0,     PREV_NOTE,
15,  0,     PREV_NOTE,
/*   snare  */
4,   1,     61,
5,   1,     PREV_NOTE,
6,   1,     PREV_NOTE,
7,   1,     PREV_NOTE,
12,  1,     61,
13,  1,     PREV_NOTE,
14,  1,     PREV_NOTE,
15,  1,     PREV_NOTE,
/*   hihat  */
0,   2,     62,
1,   2,     PREV_NOTE,
2,   2,     62,
3,   2,     PREV_NOTE,
4,   2,     62,
5,   2,     PREV_NOTE,
6,   2,     62,
7,   2,     PREV_NOTE,
8,   2,     62,
9,   2,     PREV_NOTE,
10,  2,     62,
11,  2,     PREV_NOTE,
12,  2,     62,
13,  2,     PREV_NOTE,
14,  2,     62,
15,  2,     62,

};

static const unsigned char drum_loop_2[] =
{
/* kick */
/* +--- ---- --+- -+-- */
0,   0,  60,
1,   0,  PREV_NOTE,
10,  0,  60,
11,  0,  PREV_NOTE,
13,  0,  60,
14,  0,  PREV_NOTE,

/* snare */
/* ---- +--+ ---- +--- */
4,   1,  61,
5,   1,  PREV_NOTE,
6,   1,  PREV_NOTE,

7,  1,  61,
8,  1,  PREV_NOTE,
9,  1,  PREV_NOTE,
10,  1,  PREV_NOTE,

12,  1,  61,
13,  1,  PREV_NOTE,
14,  1,  PREV_NOTE,
15,  1,  PREV_NOTE,

/* hihat */
/* +-++ +-++ ++++ -+++ */
0,   2,  62,
2,   2,  62,
3,   2,  62,

4,   2,  62,
6,   2,  62,
7,   2,  62,

8,   2,  62,
9,   2,  62,
10,  2,  62,
11,  2,  62,

13,  2,  62,
14,  2,  62,
15,  2,  62,

};

static const unsigned char drum_loop_3[] =
{
/* kick */
/* +--- +--- +--- +--- */
0,   0,  60,
1,   0,  PREV_NOTE,
4,   0,  60,
5,   0,  PREV_NOTE,
8,   0,  60,
9,   0,  PREV_NOTE,
12,  0,  60,
13,  0,  PREV_NOTE,

/* snare */
/* ---- +--- ---- +--- */
4,   1,  61,
5,   1,  PREV_NOTE,
12,  1,  61,
13,  1,  PREV_NOTE,

/* hihat */
/* --+- --+- --+- --+- */
2, 2, 62,
6, 2, 62,
10, 2, 62,
14, 2, 62,

};

static const unsigned char drum_loop_4[] =
{
/* kick */
/* +--- +--- +--- +--- */
0,   0,  60,
1,   0,  PREV_NOTE,
4,   0,  60,
5,   0,  PREV_NOTE,
8,   0,  60,
9,   0,  PREV_NOTE,
12,  0,  60,
13,  0,  PREV_NOTE,

/* ---- +--+ --+- --- */
4,   1,  61,
5,   1,  PREV_NOTE,
7,   1,  61,
8,   1,  PREV_NOTE,
10,  1,  61,
11,  1,  PREV_NOTE,

/* hihat */
/* --+- --++ --+- --++ */
2,   2,  62,
6,   2,  62,
7,   2,  62,

10,  2,  62,

14,  2,  62,
15,  2,  62,
};

static const unsigned char drum_loop_5[] =
{
/* kick */
/* +--- +--- +--- +--- */
0,   0,  60,
1,   0,  PREV_NOTE,
4,   0,  60,
5,   0,  PREV_NOTE,
8,   0,  60,
9,   0,  PREV_NOTE,
12,  0,  60,
13,  0,  PREV_NOTE,


/* hihat */
/* --+- --++ --+- --++ */
2,   2,  62,
6,   2,  62,
7,   2,  62,

10,  2,  62,

14,  2,  62,
15,  2,  62,
};

/* Bass loops */

static unsigned char bass_loop_1[] = 
{
0,   0,  48,         1,  0,
1,   0,  PREV_NOTE,  1,  0,
3,   0,  43,         1,  0,
4,   0,  PREV_NOTE,  1,  0,
6,   0,  39,         1,  0,
7,   0,  PREV_NOTE,  1,  0,
8,   0,  PREV_NOTE,  1,  0,
9,   0,  PREV_NOTE,  1,  0,
12,  0,  46,         1,  0,
14,  0,  51,         1,  0,
16,  0,  48,         1,  0,
17,  0,  PREV_NOTE,  1,  0,
19,  0,  43,         1,  0,
20,  0,  PREV_NOTE,  1,  0,
22,  0,  39,         1,  0,
23,  0,  PREV_NOTE,  1,  0,
24,  0,  PREV_NOTE,  1,  0,
25,  0,  PREV_NOTE,  1,  0,
28,  0,  46,         1,  0,
30,  0,  51,         1,  0,
};

static unsigned char bass_loop_2[] = 
{
0,   0,  41,         1,  0,
1,   0,  PREV_NOTE,  1,  0,
3,   0,  41,         1,  0,
4,   0,  PREV_NOTE,  1,  0,
6,   0,  39,         1,  0,
7,   0,  PREV_NOTE,  1,  0,
8,   0,  PREV_NOTE,  1,  0,
9,   0,  PREV_NOTE,  1,  0,
14,  0,  41,         1,  2,
15,  0,  42,         1,  2,
16,  0,  43,         1,  0,
17,  0,  PREV_NOTE,  1,  0,
19,  0,  43,         1,  0,
20,  0,  PREV_NOTE,  1,  0,
22,  0,  36,         1,  0,
23,  0,  PREV_NOTE,  1,  0,
24,  0,  PREV_NOTE,  1,  0,
25,  0,  PREV_NOTE,  1,  0,
26,  0,  PREV_NOTE,  1,  0,
};

static unsigned char bass_loop_3[] = 
{
0,   0,  44,  1,  0,
1,   0,  PREV_NOTE,  1,  0,
2,   0,  PREV_NOTE,  1,  0,
3,   0,  PREV_NOTE,  1,  0,

6,   0,  41,  1,  0,
7,   0,  PREV_NOTE,  1,  0,
8,   0,  PREV_NOTE,  1,  0,
9,   0,  PREV_NOTE,  1,  0,
10,   0,  PREV_NOTE,  1,  0,
11,   0,  PREV_NOTE,  1,  0,

16, 0, 48, 1, 0,
19, 0, 51, 1, 0,
22, 0, 48, 1, 0,
23, 0, PREV_NOTE, 1, 0,
24, 0, PREV_NOTE, 1, 0,
25, 0, PREV_NOTE, 1, 0,
26, 0, PREV_NOTE, 1, 0,


};

static unsigned char bass_loop_4[] = 
{
0,   0,  44,  1,  0,
1,   0,  PREV_NOTE,  1,  0,
3,   0,  56,  1,  0,

6,   0,  43,  1,  0,
7,   0,  PREV_NOTE,  1,  0,
8,   0,  PREV_NOTE,  1,  0,
9,   0,  PREV_NOTE,  1,  0,

14,   0,  46,  1,  2,
15,   0,  47,  1,  2,

16, 0, 48, 1, 0,
17, 0, PREV_NOTE, 1, 0,
19, 0, 48, 1, 0,

22, 0, 41, 1, 0,
23, 0, PREV_NOTE, 1, 0,
24, 0, PREV_NOTE, 1, 0,
25, 0, PREV_NOTE, 1, 0,
};

static unsigned char bass_loop_5[] = 
{
0,   0,  48,  1,  0,
1,   0,  PREV_NOTE,  1,  0,
3,   0,  48,  1,  0,

6,   0,  41,  1,  0,
7,   0,  PREV_NOTE,  1,  0,
8,   0,  PREV_NOTE,  1,  0,
9,   0,  PREV_NOTE,  1,  0,

16, 0, 39, 1, 0,
17, 0, PREV_NOTE, 1, 0,
19, 0, 39, 1, 0,

22, 0, 46, 1, 0,
23, 0, PREV_NOTE, 1, 0,
24, 0, PREV_NOTE, 1, 0,
25, 0, PREV_NOTE, 1, 0,
};

/* Chord Progressions */

static unsigned char chord_loop_1[] = 
{
/* Bar 1 */
0,   0,  60,
1,   0,  PREV_NOTE,
2,   0,  PREV_NOTE,
3,   0,  PREV_NOTE,

4,   0,  PREV_NOTE,

6,   0,  63,
7,   0,  PREV_NOTE,

8,   0,  PREV_NOTE,
9,   0,  PREV_NOTE,
10,  0,  PREV_NOTE,
11,  0,  PREV_NOTE,
12,  0,  PREV_NOTE,
13,  0,  PREV_NOTE,
14,  0,  PREV_NOTE,

0,   1,  67,
1,   1,  PREV_NOTE,
2,   1,  PREV_NOTE,
3,   1,  PREV_NOTE,
4,   1,  PREV_NOTE,
5,   1,  PREV_NOTE,
6,   1,  PREV_NOTE,
7,   1,  PREV_NOTE,
8,   1,  PREV_NOTE,
9,   1,  PREV_NOTE,
10,  1,  PREV_NOTE,
11,  1,  PREV_NOTE,
12,  1,  PREV_NOTE,
13,  1,  PREV_NOTE,
14,  1,  PREV_NOTE,

0,   2,  70,
1,   2,  PREV_NOTE,
2,   2,  PREV_NOTE,
3,   2,  PREV_NOTE,
4,   2,  PREV_NOTE,
5,   2,  PREV_NOTE,
6,   2,  PREV_NOTE,
7,   2,  PREV_NOTE,
8,   2,  PREV_NOTE,
9,   2,  PREV_NOTE,
10,  2,  PREV_NOTE,
11,  2,  PREV_NOTE,
12,  2,  PREV_NOTE,
13,  2,  PREV_NOTE,
14,  2,  PREV_NOTE,

0,   3,  75,
1,   3,  PREV_NOTE,
2,   3,  PREV_NOTE,
3,   3,  PREV_NOTE,

4,   3,  PREV_NOTE,
6,   3,  74,
7,   3,  PREV_NOTE,

8,   3,  PREV_NOTE,
9,   3,  PREV_NOTE,
10,  3,  PREV_NOTE,
11,  3,  PREV_NOTE,

12,  3,  PREV_NOTE,
13,  3,  PREV_NOTE,
14,  3,  PREV_NOTE,

/* Bar 2 */

16,   0,  60,
17,   0,  PREV_NOTE,
18,   0,  PREV_NOTE,
19,   0,  PREV_NOTE,

20,   0,  PREV_NOTE,

22,   0,  63,
23,   0,  PREV_NOTE,

24,   0,  PREV_NOTE,
25,   0,  PREV_NOTE,
26,  0,  PREV_NOTE,
27,  0,  PREV_NOTE,
28,  0,  PREV_NOTE,
29,  0,  PREV_NOTE,
30,  0,  PREV_NOTE,

16,   1,  67,
17,   1,  PREV_NOTE,
18,   1,  PREV_NOTE,
19,   1,  PREV_NOTE,
20,   1,  PREV_NOTE,
21,   1,  PREV_NOTE,
22,   1,  PREV_NOTE,
23,   1,  PREV_NOTE,
24,   1,  PREV_NOTE,
25,   1,  PREV_NOTE,
26,  1,  PREV_NOTE,
27,  1,  PREV_NOTE,
28,  1,  PREV_NOTE,
29,  1,  PREV_NOTE,
30,  1,  PREV_NOTE,

16,   2,  70,
17,   2,  PREV_NOTE,
18,   2,  PREV_NOTE,
19,   2,  PREV_NOTE,
20,   2,  PREV_NOTE,
21,   2,  PREV_NOTE,
22,   2,  PREV_NOTE,
23,   2,  PREV_NOTE,
24,   2,  PREV_NOTE,
25,   2,  PREV_NOTE,
26,  2,  PREV_NOTE,
27,  2,  PREV_NOTE,
28,  2,  PREV_NOTE,
29,  2,  PREV_NOTE,
30,  2,  PREV_NOTE,

16,   3,  75,
17,   3,  PREV_NOTE,
18,   3,  PREV_NOTE,
19,   3,  PREV_NOTE,

20,   3,  PREV_NOTE,
22,   3,  74,
23,   3,  PREV_NOTE,

24,   3,  PREV_NOTE,
25,   3,  PREV_NOTE,
26,  3,  PREV_NOTE,
27,  3,  PREV_NOTE,

28,  3,  PREV_NOTE,
29,  3,  PREV_NOTE,
30,  3,  PREV_NOTE,

};

static unsigned char chord_loop_2[] = 
{
/* 
Bar 1: 
F Ab C Eb
Eb G Bb D
*/
0,   0,  65,
1,   0,  PREV_NOTE,
2,   0,  PREV_NOTE,
3,   0,  PREV_NOTE,
4,   0,  PREV_NOTE,

6,   0,  63,
7,   0,  PREV_NOTE,
8,   0,  PREV_NOTE,
9,   0,  PREV_NOTE,
10,  0,  PREV_NOTE,
11,  0,  PREV_NOTE,
12,  0,  PREV_NOTE,
13,  0,  PREV_NOTE,
14,  0,  PREV_NOTE,

0,   1,  68,
1,   1,  PREV_NOTE,
2,   1,  PREV_NOTE,
3,   1,  PREV_NOTE,
4,   1,  PREV_NOTE,

6,   1,  67,
7,   1,  PREV_NOTE,
8,   1,  PREV_NOTE,
9,   1,  PREV_NOTE,
10,  1,  PREV_NOTE,
11,  1,  PREV_NOTE,
12,  1,  PREV_NOTE,
13,  1,  PREV_NOTE,
14,  1,  PREV_NOTE,

0,   2,  72,
1,   2,  PREV_NOTE,
2,   2,  PREV_NOTE,
3,   2,  PREV_NOTE,
4,   2,  PREV_NOTE,

6,   2,  70,
7,   2,  PREV_NOTE,
8,   2,  PREV_NOTE,
9,   2,  PREV_NOTE,
10,  2,  PREV_NOTE,
11,  2,  PREV_NOTE,
12,  2,  PREV_NOTE,
13,  2,  PREV_NOTE,
14,  2,  PREV_NOTE,

0,   3,  75,
1,   3,  PREV_NOTE,
2,   3,  PREV_NOTE,
3,   3,  PREV_NOTE,
4,   3,  PREV_NOTE,

6,   3,  74,
7,   3,  PREV_NOTE,
8,   3,  PREV_NOTE,
9,   3,  PREV_NOTE,
10,  3,  PREV_NOTE,
11,  3,  PREV_NOTE,
12,  3,  PREV_NOTE,
13,  3,  PREV_NOTE,
14,  3,  PREV_NOTE,

/* 
Bar 2
F G Bb D
Eb G Bb C
*/

16,  0,  65,
17,  0,  PREV_NOTE,
18,  0,  PREV_NOTE,
19,  0,  PREV_NOTE,
20,  0,  PREV_NOTE,
22,  0,  63,
23,  0,  PREV_NOTE,
24,  0,  PREV_NOTE,
25,  0,  PREV_NOTE,
26,  0,  PREV_NOTE,
27,  0,  PREV_NOTE,
28,  0,  PREV_NOTE,
29,  0,  PREV_NOTE,
30,  0,  PREV_NOTE,

16,  1,  67,
17,  1,  PREV_NOTE,
18,  1,  PREV_NOTE,
19,  1,  PREV_NOTE,
20,  1,  PREV_NOTE,
22,  1,  67,
23,  1,  PREV_NOTE,
24,  1,  PREV_NOTE,
25,  1,  PREV_NOTE,
26,  1,  PREV_NOTE,
27,  1,  PREV_NOTE,
28,  1,  PREV_NOTE,
29,  1,  PREV_NOTE,
30,  1,  PREV_NOTE,

16,  2,  70,
17,  2,  PREV_NOTE,
18,  2,  PREV_NOTE,
19,  2,  PREV_NOTE,
20,  2,  PREV_NOTE,
22,  2,  70,
23,  2,  PREV_NOTE,
24,  2,  PREV_NOTE,
25,  2,  PREV_NOTE,
26,  2,  PREV_NOTE,
27,  2,  PREV_NOTE,
28,  2,  PREV_NOTE,
29,  2,  PREV_NOTE,
30,  2,  PREV_NOTE,

16,  3,  74,
17,  3,  PREV_NOTE,
18,  3,  PREV_NOTE,
19,  3,  PREV_NOTE,
20,  3,  PREV_NOTE,
22,  3,  72,
23,  3,  PREV_NOTE,
24,  3,  PREV_NOTE,
25,  3,  PREV_NOTE,
26,  3,  PREV_NOTE,
27,  3,  PREV_NOTE,
28,  3,  PREV_NOTE,
29,  3,  PREV_NOTE,
30,  3,  PREV_NOTE,
};

static unsigned char chord_loop_3[] = 
{
/* 
Bar 1 
Ab F G C 
F Ab Bb  Eb
*/
0,   0,  56,
1,   0,  PREV_NOTE,
2,   0,  PREV_NOTE,
3,   0,  PREV_NOTE,
4,   0,  PREV_NOTE,

6,   0,  52,
7,   0,  PREV_NOTE,
8,   0,  PREV_NOTE,
9,   0,  PREV_NOTE,
10,  0,  PREV_NOTE,
11,  0,  PREV_NOTE,
12,  0,  PREV_NOTE,
13,  0,  PREV_NOTE,
14,  0,  PREV_NOTE,

0,   1,  65,
1,   1,  PREV_NOTE,
2,   1,  PREV_NOTE,
3,   1,  PREV_NOTE,
4,   1,  PREV_NOTE,

6,   1,  68,
7,   1,  PREV_NOTE,
8,   1,  PREV_NOTE,
9,   1,  PREV_NOTE,
10,  1,  PREV_NOTE,
11,  1,  PREV_NOTE,
12,  1,  PREV_NOTE,
13,  1,  PREV_NOTE,
14,  1,  PREV_NOTE,

0,   2,  67,
1,   2,  PREV_NOTE,
2,   2,  PREV_NOTE,
3,   2,  PREV_NOTE,
4,   2,  PREV_NOTE,

6,   2,  70,
7,   2,  PREV_NOTE,
8,   2,  PREV_NOTE,
9,   2,  PREV_NOTE,
10,  2,  PREV_NOTE,
11,  2,  PREV_NOTE,
12,  2,  PREV_NOTE,
13,  2,  PREV_NOTE,
14,  2,  PREV_NOTE,

0,   3,  72,
1,   3,  PREV_NOTE,
2,   3,  PREV_NOTE,
3,   3,  PREV_NOTE,
4,   3,  PREV_NOTE,

6,   3,  75,
7,   3,  PREV_NOTE,
8,   3,  PREV_NOTE,
9,   3,  PREV_NOTE,
10,  3,  PREV_NOTE,
11,  3,  PREV_NOTE,
12,  3,  PREV_NOTE,
13,  3,  PREV_NOTE,
14,  3,  PREV_NOTE,

/* 
Bar 2
Eb C D G
C Eb F Bb 
Eb C D G
*/

16,  0,  51,
17,  0,  PREV_NOTE,
19,  0,  48,

20,  0,  PREV_NOTE,

22,  0,  51,
23,  0,  PREV_NOTE,
24,  0,  PREV_NOTE,
25,  0,  PREV_NOTE,
26,  0,  PREV_NOTE,
27,  0,  PREV_NOTE,
28,  0,  PREV_NOTE,
29,  0,  PREV_NOTE,
30,  0,  PREV_NOTE,

16,  1,  72,
17,  1,  PREV_NOTE,
19,  1,  75,

20,  1,  PREV_NOTE,
22,  1,  72,
23,  1,  PREV_NOTE,
24,  1,  PREV_NOTE,
25,  1,  PREV_NOTE,
26,  1,  PREV_NOTE,
27,  1,  PREV_NOTE,
28,  1,  PREV_NOTE,
29,  1,  PREV_NOTE,
30,  1,  PREV_NOTE,

16,  2,  74,
17,  2,  PREV_NOTE,

19,  2,  77,
20,  2,  PREV_NOTE,

22,  2,  74,
23,  2,  PREV_NOTE,
24,  2,  PREV_NOTE,
25,  2,  PREV_NOTE,
26,  2,  PREV_NOTE,
27,  2,  PREV_NOTE,
28,  2,  PREV_NOTE,
29,  2,  PREV_NOTE,
30,  2,  PREV_NOTE,

16,  3,  79,
17,  3,  PREV_NOTE,

19,  3,  82,
20,  3,  PREV_NOTE,
22,  3,  79,
23,  3,  PREV_NOTE,
24,  3,  PREV_NOTE,
25,  3,  PREV_NOTE,
26,  3,  PREV_NOTE,
27,  3,  PREV_NOTE,
28,  3,  PREV_NOTE,
29,  3,  PREV_NOTE,
30,  3,  PREV_NOTE,
};

static unsigned char chord_loop_4[] = 
{
/* 
chord 1
Ab G C Eb 
G F Bb D
*/
0,   0,  56,
1,   0,  PREV_NOTE,
2,   0,  PREV_NOTE,
3,   0,  PREV_NOTE,
4,   0,  PREV_NOTE,

6,   0,  55,
7,   0,  PREV_NOTE,
8,   0,  PREV_NOTE,
9,   0,  PREV_NOTE,
10,  0,  PREV_NOTE,
11,  0,  PREV_NOTE,
12,  0,  PREV_NOTE,
13,  0,  PREV_NOTE,
14,  0,  PREV_NOTE,

0,   1,  67,
1,   1,  PREV_NOTE,
2,   1,  PREV_NOTE,
3,   1,  PREV_NOTE,
4,   1,  PREV_NOTE,

6,   1,  65,
7,   1,  PREV_NOTE,
8,   1,  PREV_NOTE,
9,   1,  PREV_NOTE,
10,  1,  PREV_NOTE,
11,  1,  PREV_NOTE,
12,  1,  PREV_NOTE,
13,  1,  PREV_NOTE,
14,  1,  PREV_NOTE,

0,   2,  72,
1,   2,  PREV_NOTE,
2,   2,  PREV_NOTE,
3,   2,  PREV_NOTE,
4,   2,  PREV_NOTE,

6,   2,  70,
7,   2,  PREV_NOTE,
8,   2,  PREV_NOTE,
9,   2,  PREV_NOTE,
10,  2,  PREV_NOTE,
11,  2,  PREV_NOTE,
12,  2,  PREV_NOTE,
13,  2,  PREV_NOTE,
14,  2,  PREV_NOTE,

0,   3,  75,
1,   3,  PREV_NOTE,
2,   3,  PREV_NOTE,
3,   3,  PREV_NOTE,
4,   3,  PREV_NOTE,

6,   3,  74,
7,   3,  PREV_NOTE,
8,   3,  PREV_NOTE,
9,   3,  PREV_NOTE,
10,  3,  PREV_NOTE,
11,  3,  PREV_NOTE,
12,  3,  PREV_NOTE,
13,  3,  PREV_NOTE,
14,  3,  PREV_NOTE,
/* 
chord 2
C G Bb Eb
F Ab C Eb
*/

16,  0,  48,
17,  0,  PREV_NOTE,
19,  0,  48,

20,  0,  PREV_NOTE,

22,  0,  53,
23,  0,  PREV_NOTE,
24,  0,  PREV_NOTE,
25,  0,  PREV_NOTE,
26,  0,  PREV_NOTE,
27,  0,  PREV_NOTE,
28,  0,  PREV_NOTE,
29,  0,  PREV_NOTE,
30,  0,  PREV_NOTE,

16,  1,  67,
17,  1,  PREV_NOTE,
19,  1,  67,

20,  1,  PREV_NOTE,
22,  1,  68,
23,  1,  PREV_NOTE,
24,  1,  PREV_NOTE,
25,  1,  PREV_NOTE,
26,  1,  PREV_NOTE,
27,  1,  PREV_NOTE,
28,  1,  PREV_NOTE,
29,  1,  PREV_NOTE,
30,  1,  PREV_NOTE,

16,  2,  70,
17,  2,  PREV_NOTE,

19,  2,  70,
20,  2,  PREV_NOTE,

22,  2,  72,
23,  2,  PREV_NOTE,
24,  2,  PREV_NOTE,
25,  2,  PREV_NOTE,
26,  2,  PREV_NOTE,
27,  2,  PREV_NOTE,
28,  2,  PREV_NOTE,
29,  2,  PREV_NOTE,
30,  2,  PREV_NOTE,

16,  3,  75,
17,  3,  PREV_NOTE,

19,  3,  75,
20,  3,  PREV_NOTE,
22,  3,  75,
23,  3,  PREV_NOTE,
24,  3,  PREV_NOTE,
25,  3,  PREV_NOTE,
26,  3,  PREV_NOTE,
27,  3,  PREV_NOTE,
28,  3,  PREV_NOTE,
29,  3,  PREV_NOTE,
30,  3,  PREV_NOTE,
};

static unsigned char chord_loop_5[] = 
{
/* 
Bar 1
C G Bb Eb
F Ab C Eb
*/
0,   0,  48,
1,   0,  PREV_NOTE,
2,   0,  PREV_NOTE,
3,   0,  PREV_NOTE,
4,   0,  PREV_NOTE,

6,   0,  41,
7,   0,  PREV_NOTE,
8,   0,  PREV_NOTE,
9,   0,  PREV_NOTE,
10,  0,  PREV_NOTE,
11,  0,  PREV_NOTE,
12,  0,  PREV_NOTE,
13,  0,  PREV_NOTE,
14,  0,  PREV_NOTE,

0,   1,  67,
1,   1,  PREV_NOTE,
2,   1,  PREV_NOTE,
3,   1,  PREV_NOTE,
4,   1,  PREV_NOTE,

6,   1,  68,
7,   1,  PREV_NOTE,
8,   1,  PREV_NOTE,
9,   1,  PREV_NOTE,
10,  1,  PREV_NOTE,
11,  1,  PREV_NOTE,
12,  1,  PREV_NOTE,
13,  1,  PREV_NOTE,
14,  1,  PREV_NOTE,

0,   2,  70,
1,   2,  PREV_NOTE,
2,   2,  PREV_NOTE,
3,   2,  PREV_NOTE,
4,   2,  PREV_NOTE,

6,   2,  72,
7,   2,  PREV_NOTE,
8,   2,  PREV_NOTE,
9,   2,  PREV_NOTE,
10,  2,  PREV_NOTE,
11,  2,  PREV_NOTE,
12,  2,  PREV_NOTE,
13,  2,  PREV_NOTE,
14,  2,  PREV_NOTE,

0,   3,  75,
1,   3,  PREV_NOTE,
2,   3,  PREV_NOTE,
3,   3,  PREV_NOTE,
4,   3,  PREV_NOTE,

6,   3,  75,
7,   3,  PREV_NOTE,
8,   3,  PREV_NOTE,
9,   3,  PREV_NOTE,
10,  3,  PREV_NOTE,
11,  3,  PREV_NOTE,
12,  3,  PREV_NOTE,
13,  3,  PREV_NOTE,
14,  3,  PREV_NOTE,
/* 
Bar 2
Eb G Bb D
Bb F Bb D
*/

16,  0,  39,
17,  0,  PREV_NOTE,
19,  0,  39,

20,  0,  PREV_NOTE,

22,  0,  46,
23,  0,  PREV_NOTE,
24,  0,  PREV_NOTE,
25,  0,  PREV_NOTE,
26,  0,  PREV_NOTE,
27,  0,  PREV_NOTE,
28,  0,  PREV_NOTE,
29,  0,  PREV_NOTE,
30,  0,  PREV_NOTE,

16,  1,  67,
17,  1,  PREV_NOTE,
19,  1,  67,

20,  1,  PREV_NOTE,
22,  1,  65,
23,  1,  PREV_NOTE,
24,  1,  PREV_NOTE,
25,  1,  PREV_NOTE,
26,  1,  PREV_NOTE,
27,  1,  PREV_NOTE,
28,  1,  PREV_NOTE,
29,  1,  PREV_NOTE,
30,  1,  PREV_NOTE,

16,  2,  70,
17,  2,  PREV_NOTE,

19,  2,  70,
20,  2,  PREV_NOTE,

22,  2,  70,
23,  2,  PREV_NOTE,
24,  2,  PREV_NOTE,
25,  2,  PREV_NOTE,
26,  2,  PREV_NOTE,
27,  2,  PREV_NOTE,
28,  2,  PREV_NOTE,
29,  2,  PREV_NOTE,
30,  2,  PREV_NOTE,

16,  3,  75,
17,  3,  PREV_NOTE,

19,  3,  75,
20,  3,  PREV_NOTE,
22,  3,  74,
23,  3,  PREV_NOTE,
24,  3,  PREV_NOTE,
25,  3,  PREV_NOTE,
26,  3,  PREV_NOTE,
27,  3,  PREV_NOTE,
28,  3,  PREV_NOTE,
29,  3,  PREV_NOTE,
30,  3,  PREV_NOTE,
};

static const unsigned char altmelody_1[] = 
{
0,  0,  72,
1,  0,  PREV_NOTE,
2,  0,  PREV_NOTE,
};

static const unsigned char altmelody_2[] = 
{
0,  0,  72,
1,  0,  PREV_NOTE,
2,  0,  PREV_NOTE,

22, 0, 75, 
23, 0, PREV_NOTE,
24, 0, PREV_NOTE,
28, 0, 77, 
29, 0, PREV_NOTE,
30, 0, PREV_NOTE,
};

static const unsigned char altmelody_3[] = 
{
0,  0,  79,
1,  0,  PREV_NOTE,
2,  0,  PREV_NOTE,

28,  0,  82,
29,  0,  PREV_NOTE,
30,  0,  PREV_NOTE,
};

static const unsigned char altmelody_4[] = 
{
0,  0,  79,
1,  0,  PREV_NOTE,
2,  0,  PREV_NOTE,
6,  0,  77,
7,  0,  PREV_NOTE,
8,  0,  PREV_NOTE,
};

static const unsigned char altmelody_5[] = 
{
0,  0,  82,
1,  0,  PREV_NOTE,
2,  0,  PREV_NOTE,
12,  0,  79,
13,  0,  PREV_NOTE,
14,  0,  PREV_NOTE,
};
#endif
