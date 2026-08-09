#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "analysis.h"
#include "fft.h"


#define MAX_RMS      30000.0
#define MAX_BAR      30

#define DFT_SIZE     1024
#define SAMPLE_RATE  44100.0

#define NUM_BARS     32
#define MAX_HEIGHT   20


#define RESET     "\033[0m"
#define BAR_COLOR "\033[38;5;208m"
// ============================================================
// STEREO RMS METER
// ============================================================

void stereoaudio_sampling(unsigned char buf[], size_t buffer_size)
{
    static float display_L = 0.0f;
    static float display_R = 0.0f;

    int16_t *samples = (int16_t *)buf;

    int frames = buffer_size / (2 * sizeof(int16_t));

    int64_t RMS_L = 0;
    int64_t RMS_R = 0;


    // --------------------------------------------------------
    // Calculate RMS
    // --------------------------------------------------------

    for (int i = 0; i < frames; i++) {

        int16_t L = samples[i * 2];
        int16_t R = samples[i * 2 + 1];

        RMS_L += (int64_t)L * L;
        RMS_R += (int64_t)R * R;
    }


    double rms_L = sqrt((double)RMS_L / frames);
    double rms_R = sqrt((double)RMS_R / frames);


    // --------------------------------------------------------
    // Logarithmic normalization
    // --------------------------------------------------------

    double norm_L =
        log10(1.0 + rms_L) /
        log10(1.0 + MAX_RMS);

    double norm_R =
        log10(1.0 + rms_R) /
        log10(1.0 + MAX_RMS);


    if (norm_L > 1.0)
        norm_L = 1.0;

    if (norm_R > 1.0)
        norm_R = 1.0;


    // --------------------------------------------------------
    // Fast attack / slow decay
    // --------------------------------------------------------

    if (norm_L > display_L)
        display_L = norm_L;
    else
        display_L *= 0.90f;


    if (norm_R > display_R)
        display_R = norm_R;
    else
        display_R *= 0.90f;


    int bar_L = (int)(display_L * MAX_BAR);
    int bar_R = (int)(display_R * MAX_BAR);


    // --------------------------------------------------------
    // Terminal output
    // --------------------------------------------------------

    printf("\033[H");

    printf("L: ");

    for (int i = 0; i < bar_L; i++)
        printf("▆ ");

    printf("\n");


    printf("R: ");

    for (int i = 0; i < bar_R; i++)
        printf("▆ ");

    printf("\n");


    fflush(stdout);
}



// ============================================================
// DFT SPECTRUM ANALYZER
// ============================================================

void dft_analyze(unsigned char buf[], size_t buffer_size)
{
    int16_t *samples = (int16_t *)buf;


    double input[DFT_SIZE];
    Complex output[DFT_SIZE];


    int frames =
        buffer_size / (2 * sizeof(int16_t));


    if (frames < DFT_SIZE)
        return;


    // ========================================================
    // STEREO -> MONO
    // ========================================================

    for (int i = 0; i < DFT_SIZE; i++) {

        int16_t L = samples[i * 2];
        int16_t R = samples[i * 2 + 1];

        input[i] =
            ((double)L + (double)R) / 2.0;
    }


    // ========================================================
    // REMOVE DC OFFSET
    // ========================================================

    double mean = 0.0;


    for (int i = 0; i < DFT_SIZE; i++)
        mean += input[i];


    mean /= DFT_SIZE;


    for (int i = 0; i < DFT_SIZE; i++)
        input[i] -= mean;


    // ========================================================
    // DFT
    // ========================================================

    dft(input, output, DFT_SIZE);


    // ========================================================
    // CREATE FREQUENCY BARS
    // ========================================================

    double bar_magnitude[NUM_BARS] = {0};


    for (int k = 1; k < DFT_SIZE / 2; k++) {

        double frequency =
            (double)k *
            SAMPLE_RATE /
            DFT_SIZE;


        // Don't display above 16 kHz
        if (frequency > 16000.0)
            break;


        // ----------------------------------------------------
        // Magnitude
        // ----------------------------------------------------

        double magnitude =
            sqrt(
                output[k].real * output[k].real +
                output[k].imag * output[k].imag
            ) / DFT_SIZE;


        // ----------------------------------------------------
        // Logarithmic frequency mapping
        //
        // 40 Hz -> first bar
        // 16 kHz -> last bar
        // ----------------------------------------------------

        double min_freq = 100.0;
        double max_freq = 16000.0;


        int bar =
            (int)(
                log(frequency / min_freq) /
                log(max_freq / min_freq) *
                NUM_BARS
            );


        if (bar >= 0 && bar < NUM_BARS) {

            if (magnitude > bar_magnitude[bar])
                bar_magnitude[bar] = magnitude;
        }
    }


    // ========================================================
    // FIND MAXIMUM MAGNITUDE
    // ========================================================

    double max_magnitude = 0.0;


    for (int i = 0; i < NUM_BARS; i++) {

        if (bar_magnitude[i] > max_magnitude)
            max_magnitude =
                bar_magnitude[i];
    }


    if (max_magnitude < 1e-9)
        max_magnitude = 1.0;


    // ========================================================
    // CONVERT MAGNITUDE -> BAR HEIGHT
    // ========================================================

    int heights[NUM_BARS];


    for (int i = 0; i < NUM_BARS; i++) {

        double normalized =
            bar_magnitude[i] /
            max_magnitude;


        heights[i] =
            (int)(normalized * MAX_HEIGHT);


        if (heights[i] > MAX_HEIGHT)
            heights[i] = MAX_HEIGHT;


        if (heights[i] < 0)
            heights[i] = 0;
    }


    // ========================================================
    // CLEAR TERMINAL
    // ========================================================

    printf("\033[H");
    printf("\033[J");


    // Hide cursor
    printf("\033[?25l");



    // ========================================================
    // DRAW VERTICAL SPECTRUM
    // ========================================================

    for (int row = MAX_HEIGHT; row >= 1; row--) {

        for (int bar = 0; bar < NUM_BARS; bar++) {

            
        if (heights[bar] >= row)
            printf("%s█ %s", BAR_COLOR, RESET);
        else
            printf("  ");
    
        }

        printf("\n");
    }


    // ========================================================
    // BASE LINE
    // ========================================================

    for (int i = 0; i < NUM_BARS; i++)
        printf("──");

    printf("\n");


    // ========================================================
    // FREQUENCY LABELS
    // ========================================================

    printf("100Hz");

    for (int i = 0; i < NUM_BARS - 1; i++)
        printf("  ");

    printf("16kHz\n");


    fflush(stdout);
}