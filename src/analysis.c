#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "analysis.h"

#define MAX_RMS  10000
#define MAX_BAR  30

void stereoaudio_sampling(unsigned char buf[], size_t buffer_size){

	static float display_L = 0.0f;
    static float display_R = 0.0f;

	int16_t *samples = (int16_t *)buf;
	int frames = buffer_size/(2*2);
	int64_t RMS_L = 0;
	int64_t RMS_R = 0;
	for(int i =0; i<frames; i++){
		int16_t L = samples[i*2];
		int16_t R = samples[i*2 + 1];

		RMS_L += (int64_t)L*L;
		RMS_R += (int64_t)R*R;
		//printf("Frame = %d  L = %d  R = %d\n", i, L, R);
	}
	double rms_L = sqrt((double)RMS_L / frames);
	double rms_R = sqrt((double)RMS_R / frames);
	double norm_L =log10(1.0 + rms_L) / log10(1.0 + MAX_RMS);
 
	double norm_R =log10(1.0 + rms_R) / log10(1.0 + MAX_RMS);

	if (norm_L > 1.0) norm_L = 1.0;
	if (norm_R > 1.0) norm_R = 1.0;

	// Fast attack
	if (norm_L > display_L)
    	display_L = norm_L;
	else
    	display_L *= 0.90f;

// Fast attack
	if (norm_R > display_R)
    	display_R = norm_R;
	else
    	display_R *= 0.90f;

	int bar_L = (int)(display_L * MAX_BAR);
	int bar_R = (int)(display_R * MAX_BAR);

	printf("\033[1H"); 
	printf("L: "); 
	for (int i = 0; i < bar_L; i++) { printf("▆"); } 
	printf("\n"); 
	printf("R: "); 
	for (int i = 0; i < bar_R; i++) { printf("▆"); } 
	printf("\n"); 
	// Make sure output appears immediately 
	fflush(stdout); 
}
