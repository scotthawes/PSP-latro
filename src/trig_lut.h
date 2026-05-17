#pragma once

#include <stdint.h>

#ifndef TRIG_LUT_SIZE
#define TRIG_LUT_SIZE 512
#endif

float trig_cos_lut(float angle);
float trig_sin_lut(float angle);
