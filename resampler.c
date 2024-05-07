#include <stdlib.h>
#include <stdint.h>

// NOTE(peter): To get the sample rate on the Amiga, use this equation:
//              3546895 / PERiod  which you can find if you resource the
//              demo you are remaking where they set the
//              AUDxPER : DFF0A6 : DFF0B6 : DFF0C6 : DFF0D6

/*
 * NOTE(peter): Memory returned should be freed by the caller!
 */
static int16_t *resample_audio(const int8_t *input_audio, size_t input_size, uint32_t period, uint32_t *outsize) {
	float input_sample_rate = 3546895.0f / (float)period;								// Calculate the input sample rate from the period

	float ratio = (float)48000.f / input_sample_rate;									// Calculate the resampling ratio

	size_t output_size = (size_t)((float)input_size * ratio);						// Calculate the size of the resampled audio
	*outsize = output_size;

	int16_t *output_audio = (int16_t*)malloc(output_size * sizeof(int16_t));	// Allocate memory for the resampled audio

	if(output_audio) {
		// Perform linear interpolation for resampling
		for (size_t i = 0; i < output_size; ++i) {
			float index = (float)i / ratio;
			uint32_t idx_floor = (uint32_t)index;
			uint32_t idx_ceil = idx_floor + 1;

			// Perform linear interpolation between adjacent samples
			float frac = index - idx_floor;
			float sample1 = (float)input_audio[idx_floor] / 128.0f;				// Convert input to normalized float
			float sample2 = (float)input_audio[idx_ceil] / 128.0f;				// Convert input to normalized float
			float interpolated_sample = (1.0f - frac) * sample1 + frac * sample2;

			// Scale the interpolated sample to the 16-bit range
			output_audio[i] = (int16_t)(interpolated_sample * 32767.0f);		// Convert back to 16-bit signed integer
		}
		return output_audio;
	}
	return 0;
}

#if 0
int main(int argc, char *argv[]) {
	// Check if the correct number of command-line arguments is provided
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <period> <input_file> <output_file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Extract command-line arguments
	uint32_t period = atoi(argv[1]);
	const char* input_file = argv[2];
	const char* output_file = argv[3];

	// Open input file for reading
	FILE* input_fp = fopen(input_file, "rb");

	fseek(input_fp, 0, SEEK_END);
	size_t input_size = ftell(input_fp);
	fseek(input_fp, 0, SEEK_SET);

	int8_t* input_audio = (int8_t*)malloc(input_size);
	size_t x = fread(input_audio, sizeof(int8_t), input_size, input_fp);
	fclose(input_fp);

	// Perform resampling
	uint32_t outsize = 0;
	int16_t* output_audio = resample_audio(input_audio, input_size, period, &outsize);
	if (output_audio == NULL) {
		fprintf(stderr, "Error: Resampling failed\n");
		free(input_audio);
		return EXIT_FAILURE;
	}

	FILE* output_fp = fopen(output_file, "wb");

	fwrite(output_audio, sizeof(int16_t), outsize, output_fp);

	// Close output file
	fclose(output_fp);

	// Free allocated memory
	free(input_audio);
	free(output_audio);

	return EXIT_SUCCESS;
}
#endif


