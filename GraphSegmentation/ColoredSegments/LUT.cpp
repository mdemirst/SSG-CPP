#include "LUT.h"

 int LUT::relationLUT[26][26] = {0};

void LUT::setArray()
{
	for (int t = 0; t <26; t++){
		relationLUT[t][t] = 1;}

	relationLUT[24][25] = 1;//mx white & white
	relationLUT[25][24] = 1;//white & mx white 
	relationLUT[23][18] = 1;//black & dark gray
	relationLUT[18][23] = 1;
	relationLUT[18][22] = 1;//dark gray & mx black
	relationLUT[22][18] = 1;
	relationLUT[8][9] = 1; // light blue & dark blue  
	relationLUT[21][3] = 1;//yellow & brown
	relationLUT[3][20] = 1;

}