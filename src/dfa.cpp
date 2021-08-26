#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "tripodes.h"

float mean(float *tab, size_t tab_size)
{
	float result = 0;

	for (size_t i = 0; i < tab_size; i++)
	{
		result += tab[i];
	}
	return (result / tab_size);
}

float ft_abs(float x)
{
	return (x < 0 ? -x : x);
}

float variance(float *tab, size_t tab_size)
{
	float mean_r = mean(tab, tab_size);
	float result = 0;

	for (size_t i = 0; i < tab_size; i++)
	{
		result += ft_abs(tab[i] - mean_r) * ft_abs(tab[i] - mean_r);
	}
	return (result);
}

// float sqrtf(float n){
// 	// Max and min are used to take into account numbers less than 1
// 	float lo = 1 < n ? 1 : n;// min(1, n);
// 	float hi = 1 > n ? 1 : n;// max(1, n);
// 	float mid;

//   // Update the bounds to be off the target by a factor of 10
// 	while (100 * lo * lo < n)
// 	{
// 		lo *= 10;
// 	}
// 	while (100 * hi * hi > n)
// 	{
// 		hi *= 0.1;
// 	}
// 	for (int i = 0 ; i < 100 ; i++)
// 	{
// 		mid = (lo+hi)/2;
// 		if (mid*mid == n)
// 		{
// 			return mid;
// 		}
// 		if (mid*mid > n) 
// 		{
// 			hi = mid;
// 		}
// 		else
// 		{
// 			lo = mid;
// 		}
// 	}
// 	return mid;
// }

float std2(float *tab, size_t tab_size)
{
	float variance_r = variance(tab, tab_size);
	return (sqrtf(variance_r / tab_size));
}

float *cumsum(float *tab, size_t tab_size, float offset)
{
	float	*result;
	float	gap = 0;

	result = (float*)malloc(sizeof(float) * tab_size);

	for (size_t i = 0; i < tab_size; i++)
	{
		result[i] = tab[i] + gap + offset;
		gap = tab[i] + gap + offset;
	}
	return (result);
}

void printTab(float *tab, size_t tab_size)
{
	printf("(");
	for (size_t i = 0; i < tab_size; i++)
	{
		printf("%.2f, ", tab[i]);
	}
	printf(")\n");
}

void printIntegerTab(int32_t *tab, size_t tab_size)
{
	printf("(");
	for (size_t i = 0; i < tab_size; i++)
	{
		printf("%d, ", tab[i]);
	}
	printf(")\n");
}


void printfloatTab(float **tab, size_t y_size, size_t x_size)
{
	for (size_t y = 0; y < y_size; y++)
	{
		printf("[");
		for (size_t x = 0; x < x_size; x++)
		{
			printf("%.2f, ", tab[y][x]);
		}
		printf("]\n");
	}
	printf("\n");
}

float **create_strided_tab(size_t shape_y, size_t shape_x, float *data)
{
	float	**tab = 0;
	size_t	i = 0;
	if ((tab = (float**)malloc(sizeof(float*) * shape_y)) == 0)
	{
		exit(0);
	}
	for (size_t i = 0; i < shape_y; i++)
	{
		tab[i] = (float*)malloc(sizeof(float) * shape_x);
	}
	for (size_t y = 0; y < shape_y; y++)
	{
		for (size_t x = 0; x < shape_x; x++)
		{
			tab[y][x] = data[i++];
		}
	}
	return (tab);
}

float *arrange(float x)
{
	float *array = 0;
	if ((array = (float*)malloc(sizeof(float) * x)) == 0)
	{
		exit(0);
	}
	for (int32_t i = 0; i < x; i++)
	{
		array[i] = (float)i;
	}
	return (array);
}

float	*initialize_float_tab(size_t size, float value)
{
	float	*tab = 0;
	if ((tab = (float*)malloc(sizeof(float) * size)) == 0)
	{
		exit(0);
	}
	for (size_t i = 0; i < size; i++)
	{
		tab[i] = value;
	}
	return (tab);
}
float Ln(float x) {
  int negatif = 0;
  float fois = 1;
  float ajout = 0;
  if(x<=0.0) return 0;
  if(x<1.0) {
	negatif = 1;
	x = 1.0/x;
  }
  while(x >= 10.0) {
	x /= 10.0;
	ajout += 2.302585092994046;
  };
 
  while(x>=1.1) {
	x = sqrtf(x);
	fois *= 2;
  };
  x--;
  float savx = x;
  float i = 2;
  float xp = x*x;
  float quotient = (xp/i);
  float dl = x-quotient;
  while (1.0E-15<quotient) {
	i++;
	xp *= x;
	dl += (xp/i);
	i++;
	xp *= x;
	quotient = (xp/i);
	dl -= quotient;
  }
 
  dl *= fois;
  dl += ajout;
  if(negatif) dl = -dl;
  return dl;
}

float	*polyfit(float *x, float *y, size_t n)
{
	float	*coeff;

	coeff = (float*)malloc(sizeof(float) * 2);
	float meanX = mean(x, n);
	float meanY = mean(y, n);
	float sum1 = 0, sum2 = 0;
	for (int j = 0; j < n; j++)
	{
		sum1 += (x[j] - meanX) * (y[j] - meanY);
		sum2 += (x[j] - meanX) * (x[j] - meanX);
	}
	coeff[0] = sum1 / sum2;
	coeff[1] = meanY - coeff[0] * meanX;
	return (coeff);
}
float	polyfit2(float *x, float *y, size_t n)
{
	float meanX = mean(x, n);
	float meanY = mean(y, n);
	float sum1 = 0, sum2 = 0;
	for (int j = 0; j < n; j++)
	{
		sum1 += (x[j] - meanX) * (y[j] - meanY);
		sum2 += (x[j] - meanX) * (x[j] - meanX);
	}
	return (sum1 / sum2);
}


float	mean_square(float *x, float *y, size_t n)
{
	float sum1 = 0;
	for (int j = 0; j < n; j++)
		sum1 += (x[j] - y[j]) * (x[j] - y[j]);
	return (sqrtf(sum1 / n));
}

float	mean_square2(float *x, size_t n)
{
	float sum1 = 0;
	for (int j = 0; j < n; j++)
		sum1 += x[j] * x[j];
	return (sqrtf(sum1 / n));
}

float	*polyval(float *coeff, float *x, size_t n)
{
	float	*y;

	y = (float*)malloc(sizeof(float) * n);
	for (int j = 0; j < n; j++)
	{
		y[j] = coeff[0] * x[j] + coeff[1];
	}
	return (y);
}

// float *arrange(float min, float max, float step, size_t *n)
// {
// 	*n = 0;
// 	float	*ret;
// 	while (min + *n * step < max)
// 	{
// 		(*n)++;
// 	}
// 	ret = (float*)malloc(sizeof(float) * (*n));
// 	for (int i = 0; i < (*n); i++)
// 	{
// 		ret[i] = min + i * step;
// 	}
// 	return (ret);
// }

// float ft_pow(float x, float n)
// {
// 	printf("X : %f\n", x);
// 	printf("N : %f\n", n);

// 	float ret = 1;
// 	if (n == 0)
// 	{
// 		return (1);
// 	}
// 	if (n < 0)
// 	{
// 		return (-1);
// 	}
// 	while (n-- > 0)
// 	{
// 		if (n < 1)
// 		{
// 			ret = ret * x * (1 + n);
// 		}
// 		else
// 		ret = ret * x;
// 	}
// 	printf("ret : %f\n\n", ret);

// 	return (ret);
// }

float *arrange_scales(float min, float max, float step, size_t *n)
{
	*n = 0;
	float	*ret;
	while (min + *n * step < max)
	{
		(*n)++;
	}
	ret = (float*)malloc(sizeof(float) * (*n));
	for (int i = 0; i < (*n); i++)
	{
		// ret[i] = (int)((min + i * step) * (min + i * step));
		ret[i] = (int)pow(2, (min + i * step));
	}
	return (ret);
}

float *calc_rms(float *x, size_t size_x, float scale)
{
	// printf("IN RMS :");
	// printTab(x, size_x);
	// printf(", %f\n", scale);
	float shape[2];

	shape[0] = (int)((size_x) / scale);
	shape[1] = scale;
	// printf("shape (%f, %f)\n", shape[0], shape[1]);
	float **x_strides = create_strided_tab(shape[0], shape[1], x);
	// printf("x_strides : ");
	// printfloatTab(x_strides, shape[0], shape[1]);

	float *scale_ax = arrange(scale);
	// printf("scale_ax : ");
	// printTab(scale_ax, scale);
	float *rms = initialize_float_tab(shape[0], 0.0);
	// printf("rms : ");
	// printTab(rms, shape[0]);
	for (size_t y = 0; y < shape[0]; y++)
	{
		float *coeff = polyfit(scale_ax, x_strides[y], shape[1]);
		// printf("Coeff");
		// printTab(coeff, 2);
		float *xfit = polyval(coeff, scale_ax, shape[1]);
		// printf("xfit");
		// printTab(xfit, shape[1]);
		rms[y] = mean_square(x_strides[y], xfit, shape[1]);
		// printf("rms[w], %f\n", rms[y]);
		// printf("\n");
		free(coeff);
		free(xfit);
	}
	for (size_t i = 0; i < shape[0]; i++)
	{
		free(x_strides[i]);
	}
	free(x_strides);
	free(scale_ax);
	return (rms);
}
//
float dfa(float *x, size_t size_x, float min_scale, float max_scale, float scale_dens)
{
	float *y = cumsum(x, size_x, -mean(x, size_x));
	// printf("y :");
	// printTab(y, size_x);
	// printf("mean : %f\n", mean(x, size_x));


	size_t scales_size = 0;
	float *scales = arrange_scales(min_scale, max_scale, scale_dens, &scales_size);
	// printf("Scales :");
	// printTab(scales, scales_size);
	float *fluct = initialize_float_tab(scales_size, 0.0);
	// printf("Fluct :");
	// printTab(fluct, scales_size);
	float prevalue = 1;
	for (size_t i = 0; i < scales_size; i++)
	{
		float *rms = calc_rms(y, size_x, scales[i]);
		fluct[i] = mean_square2(rms, (int)((size_x) / scales[i]));
		if (fluct[i] == fluct[i])
		{
			prevalue = fluct[i];
		}
		else
		{
			fluct[i] = prevalue;
		}
		free(rms);
	}
	// printf("fluct ");
	// printTab(fluct, scales_size);

	for (int j = 0; j < scales_size; j++)
	{
		scales[j] = Ln(scales[j]) / Ln(2);
		fluct[j] = Ln(fluct[j]) / Ln(2);
	}

	float alpha = polyfit2(scales, fluct, scales_size);
	free(scales);
	free(fluct);
	free(y);
	return (alpha);
}

// int	main(void)
// {
// 	float x[] = {8, 10, 6, 9, 7, 5, 5, 11, 11, 8, 6, 7, 9, 10, 7, 9};

// 	printf("Dfa : %f\n", dfa(x, sizeof(x) / 4, 2, 4.5, 0.5));
// 	return (0);
// }