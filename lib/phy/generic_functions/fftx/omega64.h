
#pragma once
#include <math.h>
#include <stdlib.h>

#pragma GCC diagnostic push
#ifdef __clang__

#else // __clang__
#pragma GCC diagnostic ignored "-Wall"

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Waggressive-loop-optimizations"
#endif // __clang__

#define PI 3.14159265358979323846

typedef struct {
  double r, i;
} complex_t;

static complex_t COMPLEX(double r, double i)
{
  complex_t cplx;
  cplx.r = r;
  cplx.i = i;
  return cplx;
}

static complex_t omega(int N, int k)
{
  complex_t w;
  w.r = cos(2 * PI * k / N);
  w.i = sin(2 * PI * k / N);
  return w;
}

#define creal(c) ((c).r)
#define cimag(c) ((c).i)

/* More accurate cosine */
static double cospi(double a)
{ /*return cos(PI*a); */
  double sign = +1.0;
  while (a >= 2.0)
    a -= 2.0;
  while (a < 0.0)
    a += 2.0;
  if (a >= 1.0) {
    a -= 1.0;
    sign = -1.0;
  }

  if (a == 0.0)
    return sign;
  else if (a == 0.25)
    return sign * 0.70710678118654752440;
  else if (a == 0.5)
    return 0.0;

  if (a > 0.5) {
    a -= 0.5;
    return -sign * sin(PI * a);
  } else {
    return sign * cos(PI * a);
  }
}

static double sinpi(double a)
{ /*return sin(PI*a); */
  return cospi(0.5 - a);
}

static int powmod(int phi, int g, int exp, int N)
{
  int retVal, i;

  retVal = phi % N;
  for (i = 0; i < exp; i++)
    retVal = (retVal * g) % N;
  return (retVal);
}

static double fr(int m, int i, double r)
{
  if ((i % 2) == 0)
    return (r + 2 * ((int)i / 2)) / m;
  else
    return (2 - r + 2 * ((int)i / 2)) / m;
}

#define RE_CPX(x) ((x).r)
#define IM_CPX(x) ((x).i)
#define CPX complex_t

#define MAX_INT_INT(a, b)                                                                                              \
  ({                                                                                                                   \
    __typeof__(a) _a = (a);                                                                                            \
    __typeof__(b) _b = (b);                                                                                            \
    _a > _b ? _a : _b;                                                                                                 \
  })
#define MIN_INT_INT(a, b)                                                                                              \
  ({                                                                                                                   \
    __typeof__(a) _a = (a);                                                                                            \
    __typeof__(b) _b = (b);                                                                                            \
    _a < _b ? _a : _b;                                                                                                 \
  })

#define MAX_FLT_FLT MAX_INT_INT
#define MIN_FLT_FLT MIN_INT_INT

//#pragma GCC diagnostic pop
