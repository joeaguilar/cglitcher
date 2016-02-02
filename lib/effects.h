#ifndef G_EFFECT_H
#define G_EFFECT_H


#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <cmath>
#include <ctime>


#define LSBUINT32COLOR(x,y,z) ( (0xFF << 24) | ((z) << 16) | ((y) << 8) | (x) )
#define MSBUINT32COLOR(x,y,z) ( ((x) << 24) | ((y) << 16) | ((z) << 8) | 0xFF )
#define UNPACKLSBUINT32COLOR(x,y,z,u32c) { (x) = (u32c & 0xFF);      (y) = (u32c >>  8) &0xFF; (z) = (u32c >> 16) &0xFF;  }
#define UNPACKMSBUINT32COLOR(x,y,z,u32c) { (x) = (u32c >> 24)& 0xFF; (y) = (u32c >> 16) &0xFF; (z) = (u32c >>  8) &0xFF;  }

const uint_fast8_t MAX8  = 0xFF;
const uint_fast16_t MAX16 = 0xFFFF;
const uint_fast32_t MAX32 = 0xFFFFFFFF;


typedef struct Options {
  char effect;
  size_t order;
  size_t height;
  size_t width;
  size_t length;
  uint_least8_t channel;
  uint_least8_t threshold;
  double depth;
  uint_least8_t xspace;
  uint_least8_t yspace;
  uint_least8_t stroke;
  uint_least8_t weight;
  uint_least32_t color;
} Options;
  



int isLSB() 
{
  union {
    long int l;
    char c[sizeof (long int)];
  } u;

  u.l = 1;

  if (u.c[sizeof(long int)-1] == 1) {
    return 0;
  } else {
    return 1;
  }
}


// void readImageDataFromOpenCv (std::string filename, uint_least8_t *cvpixels, Options *opts);
// void TestArrays(cv::Mat& I);
void setOptions(const cv::Mat & m, Options *opts);
void A8Bit2BcopyBuffer(uint_fast8_t *chunk, uint_fast32_t *dupBuffer, Options *opts);
void A2B8BitcopyBuffer(uint_fast32_t *chunk, uint_fast8_t *dupBuffer, Options *opts);
void A2BcopyBuffer(uint_fast32_t *chunk, uint_fast32_t *dupBuffer, Options *opts);
int channelSelector32LSB(uint_least32_t *chunk, uint_least32_t *dupBuffer, Options *opts);

// noop
void noop  (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// poisson blends
void source_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void source_poisson_subtract (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void sample_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void sample_poisson_subtract (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

void source_poisson_subtract_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void source_poisson_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void sample_poisson_subtract_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void sample_poisson_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

void poisson_blend_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void poisson_blend_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// pixelsorts
void pixelsort (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// displacements
void deplace  (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// reverbs
void wet_reverb (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// zooms
void zoomth (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void zoom_b (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void zoom_r (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

void crunch (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
// colorswap
void rgb_2_bgr (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void rgb_2_gbr (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void rgb_2_grb (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void rgb_2_brg (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
// shapes

void pixelate (uint_fast32_t *chunk, uint_fast32_t *dupBuffer, Options *opts);

void outtashape (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void outtacircle (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void outtatriangle (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void outtasquare (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// copy shapes - these can be used for selections
void copyBlocks (uint_fast32_t *pixels, uint_fast32_t *clone, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh);
void copyCircles(uint_fast32_t *pixels, uint_fast32_t *clone, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh, size_t radius);
void copyTriangles(uint_fast32_t *pixels, uint_fast32_t *clone, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh);

// Data moshes
void smosh (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void smosht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void posht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void fosht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void losht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// gradient blends
void gradient_blend_add_sample(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void gradient_blend_add_source(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void gradient_blend_subtract_sample(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void gradient_blend_subtract_source(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// test
void pssht(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// void pixelswap (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
// void convertCVDataTo32bit(uint_least8_t *cvPixels, uint_fast32_t* rgbPixels , uint_fast32_t *dupBuffer,size_t size);
void convertCVDataTo32bit(uint_least8_t *cvPixels, uint_least32_t* rgbPixels , size_t size);
void convert32bitToCVData(uint_least8_t *cvPixels, uint_least32_t* rgbPixels , size_t size, size_t length);

void funnelEffect(std::uint_fast32_t *pixels, std::uint_fast32_t *clone, Options *opts );



void getParametersFromNode(v8::Local<v8::Array>, Options*);


#endif
