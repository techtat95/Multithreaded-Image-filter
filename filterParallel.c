#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "filterParallel.h"

void applyThread(BMPImage * imageIn, BMPImage * imageOut, BoxFilter f, int tid, int numThreads) {
	int area = (imageOut -> norm_height + numThreads - 1)/numThreads;
	int part1 = area * tid;
	int sum = part1 + area;

	if(sum > imageOut -> norm_height) sum = imageOut -> norm_height;

	int b;
	int a;
	int r;
	int g;

	for(int p = part1; p < sum; ++p){
	 for(int q = 0; q < imageIn -> header.width_px; ++q){
	 b = 0;
	 a = 0;
	 r = 0;
	 g = 0;

	for(int z = 0; z < 3; ++z){
	 for(int w = 0; w < 3; ++w){
	  if((p-1+z < 0)||(p-1+z >= imageOut -> norm_height)||(q-1+w < 0)||(q-1+w >= imageOut -> header.width_px)){
	b = b + f.filter[z][w] * imageIn -> pixels[p][q].blue;
	a = a + f.filter[z][w] * imageIn -> pixels[p][q].alpha;
	r = r + f.filter[z][w] * imageIn -> pixels[p][q].red;
	g = g + f.filter[z][w] * imageIn -> pixels[p][q].green;
	}
	else{
	b = b + f.filter[z][w] * imageIn -> pixels[p-1+z][q-1+w].blue;
	a = a + f.filter[z][w] * imageIn -> pixels[p-1+z][q-1+w].alpha;
	r = r + f.filter[z][w] * imageIn -> pixels[p-1+z][q-1+w].red;
	g = g + f.filter[z][w] * imageIn -> pixels[p-1+z][q-1+w].green;
	}
      }
    }
	b = b / (int)f.norm;
	a = a / (int)f.norm;
	r = r / (int)f.norm;
	g = g / (int)f.norm;

	if(b > 255)
	b = 255;
	else if(b<0)
	b = 0;
	imageOut -> pixels[p][q].blue = (uint8_t)b;

	if(a > 255)
	a = 255;
	else if(a<0)
	a = 0;
	imageOut -> pixels[p][q].alpha = (uint8_t)a;

	if(r > 255)
	r = 255;
	else if(r<0)
	r = 0;
	imageOut -> pixels[p][q].red = (uint8_t)r;

	if(g > 255)
	g = 255;
	else if(g<0)
	g = 0;
	imageOut -> pixels[p][q].green = (uint8_t)g;
	}
      }

	return;
}

void * applyThreadWrapper(void * args) {
	ApplyArgs * applyArgs = (ApplyArgs *) args;
	fprintf(stdout, "calling thread %d\n", applyArgs->tid);
	
	applyThread(applyArgs->in, applyArgs->out, applyArgs->filter, applyArgs->tid, applyArgs->numThreads);
	
	fprintf(stdout, "finished with thread %d\n", applyArgs->tid);
	
	return NULL;
}

void apply(BMPImage * imageIn, BMPImage * imageOut, BoxFilter f, int numThreads) {

	pthread_t * t1 = malloc(sizeof(pthread_t) * numThreads);
	ApplyArgs * spacer = malloc(sizeof(ApplyArgs) * numThreads);

	for(int m = 0; m < numThreads; ++m)
	{
	spacer[m].tid = m;
	spacer[m].numThreads = numThreads;
	spacer[m].in = imageIn;
	spacer[m].out = imageOut;
	spacer[m].filter = f;
	}

	for(int m = 0; m < numThreads; ++m){
	pthread_create(&t1[m], NULL, applyThreadWrapper, &spacer[m]);
	}

	for(int m = 0; m < numThreads; ++m){
	pthread_join(t1[m], NULL);
	}

	free(t1);
	free(spacer);

	return;	
}


