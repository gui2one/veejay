#include "fft.h"


FFT::FFT()
{
	samples = fftw_alloc_real(samples_count);
	if (!samples)
		errx(3, "cannot allocate input");
	output = fftw_alloc_complex(samples_count);
	if (!output)
		errx(3, "cannot allocate output");	
	prepare_fftw();
}

void FFT::prepare_fftw()
{
	unsigned int a;



	for (a = 0; a < samples_count; a++) {
		
		samples[a] = 0;
		
		fftw_complex temp;
		temp[0] = 0.0;
		temp[1] = 0.0;
		memcpy(output[a], temp, sizeof( fftw_complex));
		//~ output[a]  = 0;
	}

	plan = fftw_plan_dft_r2c_1d(
								samples_count,
								samples, 
								output,
								0 /*FFTW_PATIENT*/
							);
	if (!plan)
		errx(3, "plan not created");
}

fftw_complex * FFT::execute_plan(double * _samples)
{
	//~ double temp[512];
	//~ for (size_t a = 0; a < samples_count; a++) {
		
		//~ temp[a] = _samples[(a*2)];

	//~ }	
	

	

	memcpy(samples, _samples, sizeof(double) * samples_count);
	
	fftw_execute(plan);
	
	return output;
}

