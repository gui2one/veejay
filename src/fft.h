#ifndef FFT_H
#define FFT_H
#include <err.h>
#include "pch.h"
#include <complex.h>
#include <fftw3.h>
#include <random>
class FFT
{
	
public:

	FFT();
	void prepare_fftw();
	fftw_complex * execute_plan(double * _samples);
	
	
	fftw_complex *output;
	fftw_plan plan;

	int height, width;

	unsigned int samples_count = 512;
	

	double * samples;

	float max;
	
private:
		/* add your private declarations */
};

#endif /* FFT_H */ 
