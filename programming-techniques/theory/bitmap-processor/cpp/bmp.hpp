#ifndef BMP_HPP
#define BMP_HPP

#include "global.hpp"

struct RGBColor {
  uint8_t b;
  uint8_t g;
  uint8_t r;

  RGBColor() : r(0), g(0), b(0) {}
  RGBColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

  void print();
};

namespace UtilFunctions {
uint32_t getPadding(uint32_t w);
uint32_t getPixelArraySize(uint32_t w, uint32_t h);
};  // namespace UtilFunctions

struct PixelArray {
  uint32_t size;
  uint32_t w, h;
  uint32_t padding;
  uint8_t *rawData = NULL;
  RGBColor **data = NULL;

  PixelArray();
  PixelArray(uint32_t size, uint32_t w, uint32_t h);
  ~PixelArray();

  void print();
  void toMatrix();
  void flatMatrix();
};

struct BMPSignature {
  uint8_t data[2];

  BMPSignature();
};

struct BMPHeaderInfo {
  uint32_t fileSize;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t pixelArrayOffsetBytes;
};

struct BMP_HEADER_RAW {
  BMPSignature signature;
  BMPHeaderInfo info;
};

struct BMPHeader : BMP_HEADER_RAW {
  void read(FILE *f);
  void print();
};

struct BMP_DIB_RAW {
  uint32_t size;
  uint32_t imgWidth;
  uint32_t imgHeight;
  uint16_t colorPlanes;
  uint16_t colorDepth;
  uint32_t compressionAlgo;
  uint32_t pixelArraySize;
  uint32_t xPixelsPerMeter;
  uint32_t yPixelsPerMeter;
  uint32_t colorTableSize;
  uint32_t colorTableImportant;
};

struct BMPDIB : BMP_DIB_RAW {
  void read(FILE *f);
  void print();
};

struct BMPFile {
  BMPHeader header;
  BMPDIB dib;
  PixelArray *pixels = nullptr;

  FILE *openFile(const char *filename);

  bool is24Bit();
  bool isBMP();

  void readFileInfo(FILE *f);
  void readPixels(FILE *f);

  bool fetchBMPData(FILE *f);
  void writeBMPData(FILE *f, BMPFile &bmp);

  void printInfo(bool printPixels);
};

struct BMPApp {
  BMPFile bmp;

  bool isMissingArg(int argc);

  void cutBitmap(FILE *f, int hParts, int wParts);

  void flipHorizontal(FILE *f, bool exportFile);
  void flipVertical(FILE *f, bool exportFile);

  void scale(FILE *f, int n, bool exportFile);
  void downResolution(FILE *f, int intensity, bool exportFile);

  void blacknwhite(FILE *f, bool exportFile);
  void invertColor(FILE *f, bool exportFile);
  void equalization(FILE *f, bool exportFile);
  void grayScaleTozero(FILE *f, bool exportFile);
};

#endif