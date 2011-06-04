
#line 1 "lexer.rl"
// vim: filetype=ragel

#include "crawlc.hpp"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>


#line 223 "lexer.rl"



#line 17 "lexer.cpp"
static const char _crawlc_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	31, 1, 32, 1, 33, 1, 34, 1, 
	35, 1, 36, 1, 37, 1, 38, 1, 
	39, 1, 40, 1, 41, 1, 42, 1, 
	43, 1, 44, 1, 45, 1, 46, 1, 
	47, 1, 48, 1, 49, 1, 50, 1, 
	51, 1, 52, 1, 53, 1, 54, 1, 
	55, 1, 56, 1, 57, 1, 58, 1, 
	59, 1, 60, 1, 62, 1, 63, 1, 
	64, 1, 66, 1, 67, 1, 68, 1, 
	69, 1, 70, 1, 71, 1, 72, 1, 
	73, 1, 74, 1, 75, 1, 76, 1, 
	77, 1, 78, 1, 79, 1, 80, 1, 
	81, 1, 82, 1, 83, 1, 84, 1, 
	85, 1, 86, 1, 87, 1, 88, 1, 
	89, 1, 90, 1, 96, 1, 97, 1, 
	98, 1, 99, 2, 0, 91, 2, 1, 
	92, 2, 2, 93, 2, 3, 94, 2, 
	4, 95, 2, 5, 65, 2, 7, 60, 
	2, 7, 61, 2, 10, 11, 2, 10, 
	12, 2, 10, 13, 2, 10, 14, 2, 
	10, 15, 2, 10, 16, 2, 10, 17, 
	2, 10, 18, 2, 10, 19, 2, 10, 
	20, 2, 10, 21, 2, 10, 22, 2, 
	10, 23, 2, 10, 24, 2, 10, 25, 
	2, 10, 26, 2, 10, 27, 2, 10, 
	28, 2, 10, 29, 2, 10, 30
};

static const short _crawlc_key_offsets[] = {
	0, 0, 3, 4, 19, 21, 23, 29, 
	35, 41, 47, 53, 59, 65, 71, 77, 
	83, 89, 95, 101, 107, 108, 112, 114, 
	116, 119, 120, 125, 131, 133, 135, 137, 
	143, 149, 155, 161, 167, 173, 179, 185, 
	191, 197, 203, 209, 215, 221, 269, 270, 
	271, 274, 275, 276, 278, 280, 283, 287, 
	289, 292, 300, 305, 312, 318, 323, 324, 
	326, 327, 328, 330, 331, 338, 339, 347, 
	355, 363, 371, 380, 388, 396, 404, 413, 
	421, 429, 437, 445, 453, 461, 469, 477, 
	485, 493, 501, 509, 517, 525, 535, 543, 
	551, 559, 567, 575, 583, 591, 599, 607, 
	615, 623, 631, 640, 648, 656, 664, 672, 
	680, 688, 696, 704, 712, 721, 729, 737, 
	745, 753, 761, 769, 777, 785, 793, 801, 
	809, 817, 825, 833, 841, 849, 857, 859, 
	862
};

static const char _crawlc_trans_keys[] = {
	10, 39, 92, 39, 34, 39, 85, 92, 
	102, 110, 114, 117, 120, 48, 55, 97, 
	98, 116, 118, 48, 55, 48, 55, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 48, 57, 65, 70, 97, 
	102, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 48, 57, 65, 70, 97, 
	102, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 48, 57, 65, 70, 97, 
	102, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 46, 43, 45, 48, 57, 
	48, 57, 10, 42, 10, 42, 47, 10, 
	46, 69, 101, 48, 57, 48, 57, 65, 
	70, 97, 102, 10, 96, 48, 55, 48, 
	55, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 48, 57, 65, 70, 97, 
	102, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 48, 57, 65, 70, 97, 
	102, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 48, 57, 65, 70, 97, 
	102, 48, 57, 65, 70, 97, 102, 48, 
	57, 65, 70, 97, 102, 10, 33, 34, 
	37, 38, 39, 40, 41, 42, 43, 44, 
	45, 46, 47, 48, 58, 59, 60, 61, 
	62, 91, 92, 93, 94, 96, 98, 99, 
	100, 101, 102, 105, 114, 115, 116, 117, 
	118, 123, 124, 125, 126, 35, 36, 49, 
	57, 63, 64, 65, 122, 61, 61, 38, 
	61, 94, 61, 61, 43, 61, 45, 61, 
	46, 48, 57, 69, 101, 48, 57, 48, 
	57, 42, 47, 61, 46, 69, 101, 120, 
	48, 55, 56, 57, 40, 69, 101, 48, 
	57, 46, 69, 101, 48, 55, 56, 57, 
	48, 57, 65, 70, 97, 102, 46, 69, 
	101, 48, 57, 61, 60, 61, 61, 61, 
	61, 62, 61, 95, 48, 57, 65, 90, 
	97, 122, 61, 95, 114, 48, 57, 65, 
	90, 97, 122, 95, 101, 48, 57, 65, 
	90, 97, 122, 95, 97, 48, 57, 65, 
	90, 98, 122, 95, 107, 48, 57, 65, 
	90, 97, 122, 95, 97, 111, 48, 57, 
	65, 90, 98, 122, 95, 115, 48, 57, 
	65, 90, 97, 122, 95, 101, 48, 57, 
	65, 90, 97, 122, 95, 110, 48, 57, 
	65, 90, 97, 122, 95, 115, 116, 48, 
	57, 65, 90, 97, 122, 95, 116, 48, 
	57, 65, 90, 97, 122, 95, 105, 48, 
	57, 65, 90, 97, 122, 95, 110, 48, 
	57, 65, 90, 97, 122, 95, 117, 48, 
	57, 65, 90, 97, 122, 95, 101, 48, 
	57, 65, 90, 97, 122, 95, 101, 48, 
	57, 65, 90, 97, 122, 95, 102, 48, 
	57, 65, 90, 97, 122, 95, 97, 48, 
	57, 65, 90, 98, 122, 95, 117, 48, 
	57, 65, 90, 97, 122, 95, 108, 48, 
	57, 65, 90, 97, 122, 95, 116, 48, 
	57, 65, 90, 97, 122, 95, 108, 48, 
	57, 65, 90, 97, 122, 95, 115, 48, 
	57, 65, 90, 97, 122, 95, 101, 48, 
	57, 65, 90, 97, 122, 95, 97, 111, 
	117, 48, 57, 65, 90, 98, 122, 95, 
	108, 48, 57, 65, 90, 97, 122, 95, 
	108, 48, 57, 65, 90, 97, 122, 95, 
	116, 48, 57, 65, 90, 97, 122, 95, 
	104, 48, 57, 65, 90, 97, 122, 95, 
	114, 48, 57, 65, 90, 97, 122, 95, 
	111, 48, 57, 65, 90, 97, 122, 95, 
	117, 48, 57, 65, 90, 97, 122, 95, 
	103, 48, 57, 65, 90, 97, 122, 95, 
	104, 48, 57, 65, 90, 97, 122, 95, 
	114, 48, 57, 65, 90, 97, 122, 95, 
	110, 48, 57, 65, 90, 97, 122, 95, 
	99, 48, 57, 65, 90, 97, 122, 95, 
	102, 109, 48, 57, 65, 90, 97, 122, 
	95, 112, 48, 57, 65, 90, 97, 122, 
	95, 111, 48, 57, 65, 90, 97, 122, 
	95, 114, 48, 57, 65, 90, 97, 122, 
	95, 116, 48, 57, 65, 90, 97, 122, 
	95, 101, 48, 57, 65, 90, 97, 122, 
	95, 116, 48, 57, 65, 90, 97, 122, 
	95, 117, 48, 57, 65, 90, 97, 122, 
	95, 114, 48, 57, 65, 90, 97, 122, 
	95, 110, 48, 57, 65, 90, 97, 122, 
	95, 116, 119, 48, 57, 65, 90, 97, 
	122, 95, 114, 48, 57, 65, 90, 97, 
	122, 95, 117, 48, 57, 65, 90, 97, 
	122, 95, 99, 48, 57, 65, 90, 97, 
	122, 95, 116, 48, 57, 65, 90, 97, 
	122, 95, 105, 48, 57, 65, 90, 97, 
	122, 95, 116, 48, 57, 65, 90, 97, 
	122, 95, 99, 48, 57, 65, 90, 97, 
	122, 95, 104, 48, 57, 65, 90, 97, 
	122, 95, 121, 48, 57, 65, 90, 97, 
	122, 95, 112, 48, 57, 65, 90, 97, 
	122, 95, 101, 48, 57, 65, 90, 97, 
	122, 95, 110, 48, 57, 65, 90, 97, 
	122, 95, 105, 48, 57, 65, 90, 97, 
	122, 95, 111, 48, 57, 65, 90, 97, 
	122, 95, 110, 48, 57, 65, 90, 97, 
	122, 95, 97, 48, 57, 65, 90, 98, 
	122, 95, 114, 48, 57, 65, 90, 97, 
	122, 61, 124, 10, 34, 92, 34, 39, 
	85, 92, 102, 110, 114, 117, 120, 48, 
	55, 97, 98, 116, 118, 0
};

static const char _crawlc_single_lengths[] = {
	0, 3, 1, 9, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 2, 0, 2, 
	3, 1, 3, 0, 2, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 40, 1, 1, 
	3, 1, 1, 2, 2, 1, 2, 0, 
	3, 4, 3, 3, 0, 3, 1, 2, 
	1, 1, 2, 1, 1, 1, 2, 2, 
	2, 2, 3, 2, 2, 2, 3, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 4, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	2, 2, 3, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 3, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 3, 
	9
};

static const char _crawlc_range_lengths[] = {
	0, 0, 0, 3, 1, 1, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 0, 1, 1, 0, 
	0, 0, 1, 3, 0, 1, 1, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 4, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 1, 
	0, 2, 1, 2, 3, 1, 0, 0, 
	0, 0, 0, 0, 3, 0, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 0, 0, 
	3
};

static const short _crawlc_index_offsets[] = {
	0, 0, 4, 6, 19, 21, 23, 27, 
	31, 35, 39, 43, 47, 51, 55, 59, 
	63, 67, 71, 75, 79, 81, 85, 87, 
	90, 94, 96, 101, 105, 108, 110, 112, 
	116, 120, 124, 128, 132, 136, 140, 144, 
	148, 152, 156, 160, 164, 168, 213, 215, 
	217, 221, 223, 225, 228, 231, 234, 238, 
	240, 244, 251, 256, 262, 266, 271, 273, 
	276, 278, 280, 283, 285, 290, 292, 298, 
	304, 310, 316, 323, 329, 335, 341, 348, 
	354, 360, 366, 372, 378, 384, 390, 396, 
	402, 408, 414, 420, 426, 432, 440, 446, 
	452, 458, 464, 470, 476, 482, 488, 494, 
	500, 506, 512, 519, 525, 531, 537, 543, 
	549, 555, 561, 567, 573, 580, 586, 592, 
	598, 604, 610, 616, 622, 628, 634, 640, 
	646, 652, 658, 664, 670, 676, 682, 685, 
	689
};

static const unsigned char _crawlc_indicies[] = {
	1, 1, 2, 0, 3, 1, 4, 4, 
	6, 4, 4, 4, 4, 7, 8, 5, 
	4, 4, 1, 9, 1, 10, 1, 11, 
	11, 11, 1, 12, 12, 12, 1, 13, 
	13, 13, 1, 14, 14, 14, 1, 15, 
	15, 15, 1, 16, 16, 16, 1, 17, 
	17, 17, 1, 18, 18, 18, 1, 19, 
	19, 19, 1, 20, 20, 20, 1, 21, 
	21, 21, 1, 22, 22, 22, 1, 23, 
	23, 23, 1, 24, 24, 24, 1, 26, 
	25, 28, 28, 29, 27, 29, 27, 32, 
	33, 31, 32, 33, 34, 31, 36, 35, 
	38, 40, 40, 39, 37, 41, 41, 41, 
	37, 43, 44, 42, 46, 45, 47, 45, 
	48, 48, 48, 45, 49, 49, 49, 45, 
	50, 50, 50, 45, 51, 51, 51, 45, 
	52, 52, 52, 45, 53, 53, 53, 45, 
	54, 54, 54, 45, 55, 55, 55, 45, 
	56, 56, 56, 45, 57, 57, 57, 45, 
	58, 58, 58, 45, 59, 59, 59, 45, 
	60, 60, 60, 45, 61, 61, 61, 45, 
	63, 64, 65, 66, 67, 68, 69, 70, 
	71, 72, 73, 74, 75, 76, 77, 79, 
	80, 81, 82, 83, 85, 1, 86, 87, 
	42, 88, 89, 90, 91, 92, 93, 94, 
	95, 96, 97, 98, 99, 100, 101, 1, 
	1, 78, 1, 84, 62, 103, 102, 105, 
	104, 107, 108, 109, 106, 111, 110, 113, 
	112, 115, 116, 114, 118, 119, 117, 121, 
	122, 120, 40, 40, 122, 123, 29, 123, 
	31, 35, 125, 124, 38, 40, 40, 128, 
	127, 39, 126, 129, 40, 40, 122, 123, 
	38, 40, 40, 127, 39, 126, 41, 41, 
	41, 126, 38, 40, 40, 78, 126, 131, 
	130, 133, 134, 132, 136, 135, 138, 137, 
	140, 141, 139, 143, 142, 84, 84, 84, 
	84, 27, 145, 144, 84, 147, 84, 84, 
	84, 146, 84, 148, 84, 84, 84, 146, 
	84, 149, 84, 84, 84, 146, 84, 150, 
	84, 84, 84, 146, 84, 151, 152, 84, 
	84, 84, 146, 84, 153, 84, 84, 84, 
	146, 84, 154, 84, 84, 84, 146, 84, 
	155, 84, 84, 84, 146, 84, 156, 157, 
	84, 84, 84, 146, 84, 158, 84, 84, 
	84, 146, 84, 159, 84, 84, 84, 146, 
	84, 160, 84, 84, 84, 146, 84, 161, 
	84, 84, 84, 146, 84, 162, 84, 84, 
	84, 146, 84, 163, 84, 84, 84, 146, 
	84, 164, 84, 84, 84, 146, 84, 165, 
	84, 84, 84, 146, 84, 166, 84, 84, 
	84, 146, 84, 167, 84, 84, 84, 146, 
	84, 168, 84, 84, 84, 146, 84, 169, 
	84, 84, 84, 146, 84, 170, 84, 84, 
	84, 146, 84, 171, 84, 84, 84, 146, 
	84, 172, 173, 174, 84, 84, 84, 146, 
	84, 175, 84, 84, 84, 146, 84, 176, 
	84, 84, 84, 146, 84, 177, 84, 84, 
	84, 146, 84, 178, 84, 84, 84, 146, 
	84, 179, 84, 84, 84, 146, 84, 180, 
	84, 84, 84, 146, 84, 181, 84, 84, 
	84, 146, 84, 182, 84, 84, 84, 146, 
	84, 183, 84, 84, 84, 146, 84, 184, 
	84, 84, 84, 146, 84, 185, 84, 84, 
	84, 146, 84, 186, 84, 84, 84, 146, 
	84, 187, 188, 84, 84, 84, 146, 84, 
	189, 84, 84, 84, 146, 84, 190, 84, 
	84, 84, 146, 84, 191, 84, 84, 84, 
	146, 84, 192, 84, 84, 84, 146, 84, 
	193, 84, 84, 84, 146, 84, 194, 84, 
	84, 84, 146, 84, 195, 84, 84, 84, 
	146, 84, 196, 84, 84, 84, 146, 84, 
	197, 84, 84, 84, 146, 84, 198, 199, 
	84, 84, 84, 146, 84, 200, 84, 84, 
	84, 146, 84, 201, 84, 84, 84, 146, 
	84, 202, 84, 84, 84, 146, 84, 203, 
	84, 84, 84, 146, 84, 204, 84, 84, 
	84, 146, 84, 205, 84, 84, 84, 146, 
	84, 206, 84, 84, 84, 146, 84, 207, 
	84, 84, 84, 146, 84, 208, 84, 84, 
	84, 146, 84, 209, 84, 84, 84, 146, 
	84, 210, 84, 84, 84, 146, 84, 211, 
	84, 84, 84, 146, 84, 212, 84, 84, 
	84, 146, 84, 213, 84, 84, 84, 146, 
	84, 214, 84, 84, 84, 146, 84, 215, 
	84, 84, 84, 146, 84, 216, 84, 84, 
	84, 146, 218, 219, 217, 1, 221, 222, 
	220, 224, 224, 226, 224, 224, 224, 224, 
	227, 228, 225, 224, 224, 223, 0
};

static const unsigned char _crawlc_trans_targs[] = {
	2, 0, 3, 45, 2, 4, 6, 14, 
	18, 5, 2, 7, 8, 9, 10, 11, 
	12, 13, 2, 15, 16, 17, 2, 19, 
	2, 45, 45, 45, 22, 55, 45, 23, 
	23, 24, 45, 25, 45, 45, 58, 26, 
	21, 60, 28, 28, 45, 135, 30, 135, 
	32, 33, 34, 35, 36, 37, 38, 135, 
	40, 41, 42, 135, 44, 135, 45, 45, 
	46, 45, 47, 48, 1, 45, 45, 50, 
	51, 45, 52, 53, 56, 57, 61, 62, 
	45, 63, 65, 66, 68, 45, 45, 69, 
	70, 74, 84, 90, 93, 106, 111, 116, 
	125, 128, 132, 45, 134, 45, 45, 45, 
	45, 45, 45, 45, 45, 49, 45, 45, 
	45, 45, 45, 45, 45, 45, 45, 45, 
	45, 20, 54, 45, 45, 45, 45, 59, 
	27, 45, 45, 45, 45, 64, 45, 45, 
	45, 45, 45, 45, 45, 67, 45, 45, 
	45, 45, 45, 71, 72, 73, 68, 75, 
	77, 76, 68, 78, 79, 80, 68, 81, 
	82, 83, 68, 85, 86, 87, 88, 89, 
	68, 91, 92, 68, 94, 103, 104, 95, 
	96, 97, 98, 99, 100, 101, 102, 68, 
	68, 105, 68, 68, 107, 108, 109, 110, 
	68, 112, 113, 114, 115, 68, 117, 121, 
	118, 119, 120, 68, 122, 123, 124, 68, 
	126, 127, 68, 129, 130, 131, 68, 133, 
	68, 45, 45, 45, 135, 135, 136, 135, 
	135, 29, 31, 39, 43
};

static const unsigned char _crawlc_trans_actions[] = {
	13, 0, 0, 89, 9, 0, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 7, 0, 0, 0, 5, 0, 
	3, 131, 23, 137, 0, 0, 133, 0, 
	15, 0, 83, 0, 168, 135, 225, 0, 
	0, 0, 0, 15, 87, 145, 0, 147, 
	0, 0, 0, 0, 0, 0, 0, 156, 
	0, 0, 0, 153, 0, 150, 81, 165, 
	0, 162, 0, 0, 11, 39, 41, 0, 
	0, 45, 0, 21, 21, 222, 222, 0, 
	43, 0, 0, 0, 228, 35, 37, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 31, 0, 33, 95, 71, 
	103, 51, 109, 77, 57, 0, 111, 59, 
	99, 47, 113, 25, 61, 115, 27, 63, 
	93, 0, 225, 127, 101, 49, 125, 222, 
	0, 85, 91, 29, 121, 0, 73, 105, 
	53, 97, 69, 123, 75, 0, 107, 55, 
	119, 67, 129, 0, 0, 0, 192, 0, 
	0, 0, 195, 0, 0, 0, 210, 0, 
	0, 0, 186, 0, 0, 0, 0, 0, 
	198, 0, 0, 174, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 189, 
	177, 0, 201, 171, 0, 0, 0, 0, 
	207, 0, 0, 0, 0, 183, 0, 0, 
	0, 0, 0, 216, 0, 0, 0, 180, 
	0, 0, 204, 0, 0, 0, 219, 0, 
	213, 117, 65, 79, 139, 141, 21, 143, 
	159, 0, 0, 0, 0
};

static const unsigned char _crawlc_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 17, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 17, 
	0
};

static const unsigned char _crawlc_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 19, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 19, 
	0
};

static const short _crawlc_eof_trans[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 26, 28, 28, 31, 
	31, 31, 38, 38, 0, 46, 46, 46, 
	46, 46, 46, 46, 46, 46, 46, 46, 
	46, 46, 46, 46, 46, 0, 103, 105, 
	107, 111, 113, 115, 118, 121, 124, 124, 
	125, 127, 124, 127, 127, 127, 131, 133, 
	136, 138, 140, 143, 28, 145, 147, 147, 
	147, 147, 147, 147, 147, 147, 147, 147, 
	147, 147, 147, 147, 147, 147, 147, 147, 
	147, 147, 147, 147, 147, 147, 147, 147, 
	147, 147, 147, 147, 147, 147, 147, 147, 
	147, 147, 147, 147, 147, 147, 147, 147, 
	147, 147, 147, 147, 147, 147, 147, 147, 
	147, 147, 147, 147, 147, 147, 147, 147, 
	147, 147, 147, 147, 147, 147, 218, 0, 
	224
};

static const int crawlc_start = 45;
static const int crawlc_first_final = 45;
static const int crawlc_error = 0;

static const int crawlc_en_main = 45;
static const int crawlc_en_string_interp = 135;


#line 226 "lexer.rl"

static uint32_t parse_hex(const char *p, size_t len)
{
	uint32_t out = 0;
	while (len--) {
		char c = *p;
		unsigned char n = 0;
		if (c >= '0' && c <= '9')
			n = c - '0';
		else if (c >= 'a' && c <= 'f')
			n = 10 + (c - 'a');
		else if (c >= 'A' && c <= 'F')
			n = 10 + (c - 'A');
		out = (out << 4) | n;
		p++;
	}
	return out;
}

static uint32_t parse_oct(const char *p)
{
	uint32_t a, b, c;
	a = p[0] - '0';
	b = p[1] - '0';
	c = p[2] - '0';
	return (a << 6) + (b << 3) + c;
}

//------------------------------------------------------------------------------

void lexer_t::set_input(source_file_t *f)
{
	beg       = 0;
	end       = 0;
	file      = f;
	p         = &file->data.front();
	pe        = &file->data.back();
	eof       = pe;

	
#line 514 "lexer.cpp"
	{
	cs = crawlc_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 266 "lexer.rl"
}

token_t *lexer_t::next_token()
{
	token_t *n = 0;
	if (!next.empty()) {
		n = next.back();
		next.pop_back();
		return n;
	}

	
#line 535 "lexer.cpp"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_acts = _crawlc_actions + _crawlc_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 9:
#line 1 "lexer.rl"
	{ts = p;}
	break;
#line 556 "lexer.cpp"
		}
	}

	_keys = _crawlc_trans_keys + _crawlc_key_offsets[cs];
	_trans = _crawlc_index_offsets[cs];

	_klen = _crawlc_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _crawlc_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _crawlc_indicies[_trans];
_eof_trans:
	cs = _crawlc_trans_targs[_trans];

	if ( _crawlc_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _crawlc_actions + _crawlc_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 12 "lexer.rl"
	{ append_oct(p-2); }
	break;
	case 1:
#line 13 "lexer.rl"
	{ append_hex(p-1, 2); }
	break;
	case 2:
#line 14 "lexer.rl"
	{ append_hex(p-3, 4); }
	break;
	case 3:
#line 15 "lexer.rl"
	{ append_hex(p-7, 8); }
	break;
	case 4:
#line 16 "lexer.rl"
	{ append_escaped((*p)); }
	break;
	case 5:
#line 17 "lexer.rl"
	{ interp.clear(); }
	break;
	case 6:
#line 18 "lexer.rl"
	{ interp.append(1, (*p)); }
	break;
	case 7:
#line 20 "lexer.rl"
	{
		if (file)
			file->add_line((p+1) - &file->data.front());
	}
	break;
	case 10:
#line 1 "lexer.rl"
	{te = p+1;}
	break;
	case 11:
#line 56 "lexer.rl"
	{act = 1;}
	break;
	case 12:
#line 57 "lexer.rl"
	{act = 2;}
	break;
	case 13:
#line 58 "lexer.rl"
	{act = 3;}
	break;
	case 14:
#line 59 "lexer.rl"
	{act = 4;}
	break;
	case 15:
#line 60 "lexer.rl"
	{act = 5;}
	break;
	case 16:
#line 61 "lexer.rl"
	{act = 6;}
	break;
	case 17:
#line 62 "lexer.rl"
	{act = 7;}
	break;
	case 18:
#line 63 "lexer.rl"
	{act = 8;}
	break;
	case 19:
#line 64 "lexer.rl"
	{act = 9;}
	break;
	case 20:
#line 65 "lexer.rl"
	{act = 10;}
	break;
	case 21:
#line 67 "lexer.rl"
	{act = 11;}
	break;
	case 22:
#line 68 "lexer.rl"
	{act = 12;}
	break;
	case 23:
#line 69 "lexer.rl"
	{act = 13;}
	break;
	case 24:
#line 70 "lexer.rl"
	{act = 14;}
	break;
	case 25:
#line 71 "lexer.rl"
	{act = 15;}
	break;
	case 26:
#line 72 "lexer.rl"
	{act = 16;}
	break;
	case 27:
#line 73 "lexer.rl"
	{act = 17;}
	break;
	case 28:
#line 152 "lexer.rl"
	{act = 67;}
	break;
	case 29:
#line 158 "lexer.rl"
	{act = 68;}
	break;
	case 30:
#line 194 "lexer.rl"
	{act = 73;}
	break;
	case 31:
#line 79 "lexer.rl"
	{te = p+1;{ tok_op(TOK_ELLIPSIS);  {p++; goto _out; } }}
	break;
	case 32:
#line 81 "lexer.rl"
	{te = p+1;{ tok_op(TOK_INC);       {p++; goto _out; } }}
	break;
	case 33:
#line 82 "lexer.rl"
	{te = p+1;{ tok_op(TOK_DEC);       {p++; goto _out; } }}
	break;
	case 34:
#line 83 "lexer.rl"
	{te = p+1;{ tok_op(TOK_DECLARIZE); {p++; goto _out; } }}
	break;
	case 35:
#line 85 "lexer.rl"
	{te = p+1;{ tok_op(TOK_LCURLY);    {p++; goto _out; } }}
	break;
	case 36:
#line 86 "lexer.rl"
	{te = p+1;{ tok_op(TOK_RCURLY);    {p++; goto _out; } }}
	break;
	case 37:
#line 87 "lexer.rl"
	{te = p+1;{ tok_op(TOK_LSB);       {p++; goto _out; } }}
	break;
	case 38:
#line 88 "lexer.rl"
	{te = p+1;{ tok_op(TOK_RSB);       {p++; goto _out; } }}
	break;
	case 39:
#line 89 "lexer.rl"
	{te = p+1;{ tok_op(TOK_LPAREN);    {p++; goto _out; } }}
	break;
	case 40:
#line 90 "lexer.rl"
	{te = p+1;{ tok_op(TOK_RPAREN);    {p++; goto _out; } }}
	break;
	case 41:
#line 91 "lexer.rl"
	{te = p+1;{ tok_op(TOK_SEMICOLON); {p++; goto _out; } }}
	break;
	case 42:
#line 94 "lexer.rl"
	{te = p+1;{ tok_op(TOK_COMMA);     {p++; goto _out; } }}
	break;
	case 43:
#line 111 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_TIMES);   {p++; goto _out; } }}
	break;
	case 44:
#line 112 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_DIVIDE);  {p++; goto _out; } }}
	break;
	case 45:
#line 113 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_MOD);     {p++; goto _out; } }}
	break;
	case 46:
#line 114 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_SHIFTL);  {p++; goto _out; } }}
	break;
	case 47:
#line 115 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_SHIFTR);  {p++; goto _out; } }}
	break;
	case 48:
#line 116 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_AND);     {p++; goto _out; } }}
	break;
	case 49:
#line 117 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_ANDNOT);  {p++; goto _out; } }}
	break;
	case 50:
#line 119 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_PLUS);    {p++; goto _out; } }}
	break;
	case 51:
#line 120 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_MINUS);   {p++; goto _out; } }}
	break;
	case 52:
#line 121 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_OR);      {p++; goto _out; } }}
	break;
	case 53:
#line 122 "lexer.rl"
	{te = p+1;{ tok_op(TOK_A_XOR);     {p++; goto _out; } }}
	break;
	case 54:
#line 124 "lexer.rl"
	{te = p+1;{ tok_op(TOK_EQ);        {p++; goto _out; } }}
	break;
	case 55:
#line 125 "lexer.rl"
	{te = p+1;{ tok_op(TOK_NEQ);       {p++; goto _out; } }}
	break;
	case 56:
#line 126 "lexer.rl"
	{te = p+1;{ tok_op(TOK_LE);        {p++; goto _out; } }}
	break;
	case 57:
#line 127 "lexer.rl"
	{te = p+1;{ tok_op(TOK_GE);        {p++; goto _out; } }}
	break;
	case 58:
#line 131 "lexer.rl"
	{te = p+1;{ tok_op(TOK_ANDAND);    {p++; goto _out; } }}
	break;
	case 59:
#line 133 "lexer.rl"
	{te = p+1;{ tok_op(TOK_OROR);      {p++; goto _out; } }}
	break;
	case 60:
#line 140 "lexer.rl"
	{te = p+1;}
	break;
	case 61:
#line 143 "lexer.rl"
	{te = p+1;}
	break;
	case 62:
#line 146 "lexer.rl"
	{te = p+1;}
	break;
	case 63:
#line 161 "lexer.rl"
	{te = p+1;{ 
			tok_int(ts, te-2);
			tok_op_pos(TOK_DOT, te-2, te-1);
			tok_op_pos(TOK_LPAREN, te-1, te);
			{p++; goto _out; }
		}}
	break;
	case 64:
#line 173 "lexer.rl"
	{te = p+1;{
			beg = ts; end = te;
			interp.assign(ts+1, (te-ts)-2);
			tok_string(TOK_STRING);
			{p++; goto _out; }
		}}
	break;
	case 65:
#line 181 "lexer.rl"
	{te = p+1;{ beg = p; {cs = 135; goto _again;} }}
	break;
	case 66:
#line 184 "lexer.rl"
	{te = p+1;{
			beg = ts; end = te;
			tok_string(TOK_CHAR);
			{p++; goto _out; }
		}}
	break;
	case 67:
#line 92 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_COLON);     {p++; goto _out; } }}
	break;
	case 68:
#line 93 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_DOT);       {p++; goto _out; } }}
	break;
	case 69:
#line 95 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_NOT);       {p++; goto _out; } }}
	break;
	case 70:
#line 96 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_ASSIGN);    {p++; goto _out; } }}
	break;
	case 71:
#line 98 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_TIMES);     {p++; goto _out; } }}
	break;
	case 72:
#line 99 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_DIVIDE);    {p++; goto _out; } }}
	break;
	case 73:
#line 100 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_MOD);       {p++; goto _out; } }}
	break;
	case 74:
#line 101 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_SHIFTL);    {p++; goto _out; } }}
	break;
	case 75:
#line 102 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_SHIFTR);    {p++; goto _out; } }}
	break;
	case 76:
#line 103 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_AND);       {p++; goto _out; } }}
	break;
	case 77:
#line 104 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_ANDNOT);    {p++; goto _out; } }}
	break;
	case 78:
#line 106 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_PLUS);      {p++; goto _out; } }}
	break;
	case 79:
#line 107 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_MINUS);     {p++; goto _out; } }}
	break;
	case 80:
#line 108 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_OR);        {p++; goto _out; } }}
	break;
	case 81:
#line 109 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_XOR);       {p++; goto _out; } }}
	break;
	case 82:
#line 128 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_LT);        {p++; goto _out; } }}
	break;
	case 83:
#line 129 "lexer.rl"
	{te = p;p--;{ tok_op(TOK_GT);        {p++; goto _out; } }}
	break;
	case 84:
#line 152 "lexer.rl"
	{te = p;p--;{ tok_int(ts, te); {p++; goto _out; } }}
	break;
	case 85:
#line 158 "lexer.rl"
	{te = p;p--;{ tok_float(ts, te); {p++; goto _out; } }}
	break;
	case 86:
#line 194 "lexer.rl"
	{te = p;p--;{ 
			beg = ts; end = te;
			interp.assign(ts, te-ts);
			tok_string(TOK_IDENT);
			{p++; goto _out; }
		}}
	break;
	case 87:
#line 93 "lexer.rl"
	{{p = ((te))-1;}{ tok_op(TOK_DOT);       {p++; goto _out; } }}
	break;
	case 88:
#line 99 "lexer.rl"
	{{p = ((te))-1;}{ tok_op(TOK_DIVIDE);    {p++; goto _out; } }}
	break;
	case 89:
#line 152 "lexer.rl"
	{{p = ((te))-1;}{ tok_int(ts, te); {p++; goto _out; } }}
	break;
	case 90:
#line 1 "lexer.rl"
	{	switch( act ) {
	case 1:
	{{p = ((te))-1;} tok_op(TOK_IF);          {p++; goto _out; } }
	break;
	case 2:
	{{p = ((te))-1;} tok_op(TOK_ELSE);        {p++; goto _out; } }
	break;
	case 3:
	{{p = ((te))-1;} tok_op(TOK_FOR);         {p++; goto _out; } }
	break;
	case 4:
	{{p = ((te))-1;} tok_op(TOK_SWITCH);      {p++; goto _out; } }
	break;
	case 5:
	{{p = ((te))-1;} tok_op(TOK_RETURN);      {p++; goto _out; } }
	break;
	case 6:
	{{p = ((te))-1;} tok_op(TOK_CONTINUE);    {p++; goto _out; } }
	break;
	case 7:
	{{p = ((te))-1;} tok_op(TOK_FALLTHROUGH); {p++; goto _out; } }
	break;
	case 8:
	{{p = ((te))-1;} tok_op(TOK_BREAK);       {p++; goto _out; } }
	break;
	case 9:
	{{p = ((te))-1;} tok_op(TOK_CASE);        {p++; goto _out; } }
	break;
	case 10:
	{{p = ((te))-1;} tok_op(TOK_DEFAULT);     {p++; goto _out; } }
	break;
	case 11:
	{{p = ((te))-1;} tok_op(TOK_FUNC);        {p++; goto _out; } }
	break;
	case 12:
	{{p = ((te))-1;} tok_op(TOK_TYPE);        {p++; goto _out; } }
	break;
	case 13:
	{{p = ((te))-1;} tok_op(TOK_IMPORT);      {p++; goto _out; } }
	break;
	case 14:
	{{p = ((te))-1;} tok_op(TOK_CONST);       {p++; goto _out; } }
	break;
	case 15:
	{{p = ((te))-1;} tok_op(TOK_VAR);         {p++; goto _out; } }
	break;
	case 16:
	{{p = ((te))-1;} tok_op(TOK_STRUCT);      {p++; goto _out; } }
	break;
	case 17:
	{{p = ((te))-1;} tok_op(TOK_UNION);       {p++; goto _out; } }
	break;
	case 67:
	{{p = ((te))-1;} tok_int(ts, te); {p++; goto _out; } }
	break;
	case 68:
	{{p = ((te))-1;} tok_float(ts, te); {p++; goto _out; } }
	break;
	case 73:
	{{p = ((te))-1;} 
			beg = ts; end = te;
			interp.assign(ts, te-ts);
			tok_string(TOK_IDENT);
			{p++; goto _out; }
		}
	break;
	}
	}
	break;
	case 91:
#line 210 "lexer.rl"
	{te = p+1;}
	break;
	case 92:
#line 211 "lexer.rl"
	{te = p+1;}
	break;
	case 93:
#line 212 "lexer.rl"
	{te = p+1;}
	break;
	case 94:
#line 213 "lexer.rl"
	{te = p+1;}
	break;
	case 95:
#line 214 "lexer.rl"
	{te = p+1;}
	break;
	case 96:
#line 18 "lexer.rl"
	{te = p+1;{ interp.append(1, (*p)); }}
	break;
	case 97:
#line 216 "lexer.rl"
	{te = p+1;{
			end = te;
			tok_string(TOK_STRING);
			cs = 45;
			{p++; goto _out; }
		}}
	break;
	case 98:
#line 18 "lexer.rl"
	{te = p;p--;{ interp.append(1, (*p)); }}
	break;
	case 99:
#line 18 "lexer.rl"
	{{p = ((te))-1;}{ interp.append(1, (*p)); }}
	break;
#line 1108 "lexer.cpp"
		}
	}

_again:
	_acts = _crawlc_actions + _crawlc_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 8:
#line 1 "lexer.rl"
	{ts = 0;}
	break;
#line 1121 "lexer.cpp"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	if ( _crawlc_eof_trans[cs] > 0 ) {
		_trans = _crawlc_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	_out: {}
	}

#line 278 "lexer.rl"

	if (!next.empty()) {
		n = next.back();
		next.pop_back();
	}
	return n;
}

// interp functions
void lexer_t::append_hex(const char *p, size_t len)
{
	char utfbuf[8];
	uint32_t hex = parse_hex(p, len);
	len = unicode_to_utf8(utfbuf, hex);
	interp.append(utfbuf, len);
}

void lexer_t::append_oct(const char *p)
{
	char utfbuf[8];
	uint32_t oct = parse_oct(p);
	size_t len = unicode_to_utf8(utfbuf, oct);
	interp.append(utfbuf, len);
}

void lexer_t::append_escaped(const char c)
{
	switch (c) {
	case '"':  interp.append(1, '"');  break;
	case 'a':  interp.append(1, '\a'); break;
	case 'b':  interp.append(1, '\b'); break;
	case 'f':  interp.append(1, '\f'); break;
	case 'n':  interp.append(1, '\n'); break;
	case 'r':  interp.append(1, '\r'); break;
	case 't':  interp.append(1, '\t'); break;
	case 'v':  interp.append(1, '\v'); break;
	case '\\': interp.append(1, '\\'); break;
	case '\'': interp.append(1, '\''); break;
	}
}

void lexer_t::tok_int(char *b, char *e)
{
	beg = b; end = e;
	interp.assign(b, e-b);
	token_t *n = new token_t(interp.c_str(), TOK_INT,
				 loc(), beg, end);
	next.insert(next.begin(), n);
}

void lexer_t::tok_float(char *b, char *e)
{
	beg = b; end = e;
	interp.assign(b, e-b);
	token_t *n = new token_t(interp.c_str(), TOK_FLOAT,
				 loc(), beg, end);
	next.insert(next.begin(), n);
}

void lexer_t::tok_string(int type)
{
	token_t *n = new token_t(interp.c_str(), type,
				 loc(), beg, end);
	next.insert(next.begin(), n);
}

void lexer_t::tok_op(int optype)
{
	beg = ts; end = te;
	token_t *n = new token_t(0, optype, loc(), beg, end);
	next.insert(next.begin(), n);
}

void lexer_t::tok_op_pos(int optype, char *b, char *e)
{
	beg = b; end = e;
	token_t *n = new token_t(0, optype, loc(), beg, end);
	next.insert(next.begin(), n);
}

source_loc_t lexer_t::loc()
{
	source_loc_t loc = beg - &file->data.front();
	if (file)
		loc += file->offset;
	return loc;
}
