#include <stdint.h>
#include <stdlib.h>

#include <cmath>
#include <cstring>
#include <iostream>

using namespace std;

const double M_PI = 3.14159265358979323846;

const char *COMPRESSION_ALGO[] = {"No", "RLE", "Huffman", "JPEG", "PNG"};

struct RGBColor {
  uint8_t b;
  uint8_t g;
  uint8_t r;

  RGBColor() : r(0), g(0), b(0) {}
  RGBColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

  void print() {
    cout << "RGB(" << (unsigned int)r << ", " << (unsigned int)g << ", "
         << (unsigned int)b << ")" << endl;
  }
};

namespace UtilFunctions {
uint32_t getPadding(uint32_t w) { return (4 - (w * sizeof(RGBColor)) % 4) % 4; }
};  // namespace UtilFunctions

struct PixelArray {
  uint32_t size;
  uint32_t w, h;
  uint32_t padding;
  uint8_t *rawData = NULL;
  RGBColor **data = NULL;

  PixelArray() : size(0), w(0), h(0), padding(0) {}

  PixelArray(uint32_t size, uint32_t w, uint32_t h)
      : size(size), w(w), h(h), padding(UtilFunctions::getPadding(w)) {
    data = (RGBColor **)malloc(h * sizeof(RGBColor *));

    for (int i = 0; i < h; i++)
      data[i] = (RGBColor *)malloc(w * sizeof(RGBColor));
  }

  ~PixelArray() {
    free(rawData);

    for (int i = 0; i < h; i++) free(data[i]);
    free(data);
  }

  void print() {
    cout << "==== PIXEL ARRAY INFO ====" << endl;
    cout << "+ Size  : " << size << " byte(s)" << endl;
    cout << "+ Width : " << w << " pixel(s)" << endl;
    cout << "+ Height: " << h << " pixel(s)" << endl;
    cout << "+ Pixel Array:\n";

    for (int i = 0; i < size; i++) {
      cout << (unsigned int)rawData[i] << " ";
    }
    cout << endl;

    for (int i = 0; i < h; i++)
      for (int j = 0; j < w; j++) {
        data[i][j].print();
      }
  }

  void toMatrix() {
    int dataIdx = 0;
    for (int rowIdx = 0; rowIdx < h; rowIdx++) {
      for (int colIdx = 0; colIdx < w * sizeof(RGBColor) + padding;) {
        int idx = colIdx + rowIdx * (w * sizeof(RGBColor) + padding);

        if (colIdx < w * sizeof(RGBColor)) {
          unsigned int _b = rawData[idx];
          unsigned int _g = rawData[idx + 1];
          unsigned int _r = rawData[idx + 2];

          RGBColor color(_r, _g, _b);

          data[rowIdx][colIdx / sizeof(RGBColor)] = color;

          colIdx += sizeof(RGBColor);
        } else
          colIdx += padding;
      }
    }
  }

  void flatMatrix() {
    free(rawData);
    rawData = (uint8_t *)malloc(size * sizeof(uint8_t));

    int rawDataIdx = 0;
    for (int i = 0; i < h; i++) {
      for (int j = 0; j < w; j++) {
        rawData[rawDataIdx] = data[i][j].b;
        rawData[rawDataIdx + 1] = data[i][j].g;
        rawData[rawDataIdx + 2] = data[i][j].r;
        rawDataIdx += 3;
      }

      for (int j = 0; j < padding; j++) {
        rawData[rawDataIdx] = 0;
        rawDataIdx++;
      }
    }
  }
};

struct BMPSignature {
  unsigned char data[2];

  BMPSignature() { data[0] = data[1] = 0; }
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
  void read(FILE *f) {
    if (!f) return;

    fseek(f, 0, SEEK_SET);

    fread(&signature, sizeof(BMPSignature), 1, f);
    fread(&info, sizeof(BMPHeaderInfo), 1, f);
  }

  void print() {
    cout << "==== BMP HEADER ====" << endl;
    cout << "+ Signature  : " << signature.data[0] << signature.data[1] << endl;
    cout << "+ File Size  : " << info.fileSize << " byte(s)" << endl;
    cout << "+ Reserved1  : " << info.reserved1 << endl;
    cout << "+ Reserved2  : " << info.reserved2 << endl;
    cout << "+ Data Offset: " << info.pixelArrayOffsetBytes << " byte(s)"
         << endl;
  }
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
  void read(FILE *f) {
    if (!f) return;

    fseek(f, sizeof(BMPSignature) + sizeof(BMPHeaderInfo), SEEK_SET);
    fread(this, sizeof(BMPDIB), 1, f);
  }

  void print() {
    cout << "==== BMP DIB ====" << endl;
    cout << "+ DIB Size  : " << size << endl;
    cout << "+ Img Width : " << imgWidth << " pixel(s)" << endl;
    cout << "+ Img Height: " << imgHeight << " pixel(s)" << endl;
    cout << "+ Color Plan: " << colorPlanes << endl;
    cout << "+ Color Depth: " << colorDepth << " bit(s)" << endl;
    cout << "+ Compression: " << COMPRESSION_ALGO[compressionAlgo] << endl;
    cout << "+ Pixel Size: " << pixelArraySize << " byte(s)" << endl;
    cout << "+ X Pixels/m: " << xPixelsPerMeter << endl;
    cout << "+ Y Pixels/m: " << yPixelsPerMeter << endl;
  }
};

struct BMPFile {
  BMPHeader header;
  BMPDIB dib;
  PixelArray *pixels;

  bool is24Bit() { return dib.colorDepth == 24; }

  bool isBMP() {
    return header.signature.data[0] == 'B' && header.signature.data[1] == 'M';
  }

  void printInfo(bool printPixels = false) {
    header.print();
    dib.print();

    if (printPixels) pixels->print();
  }

  FILE *openFile(const char *filename) {
    FILE *f = fopen(filename, "rb");

    if (!f) {
      fprintf(stderr, "Could not open file\n");
      return NULL;
    }

    return f;
  }

  void readFileInfo(FILE *f) {
    header.read(f);
    dib.read(f);
  }

  void readPixels(FILE *f) {
    pixels = new PixelArray(dib.pixelArraySize, dib.imgWidth, dib.imgHeight);

    uint8_t *buf = (uint8_t *)malloc(pixels->size * sizeof(uint8_t));
    fseek(f, header.info.pixelArrayOffsetBytes, SEEK_SET);
    fread(buf, sizeof(uint8_t) * pixels->size, 1, f);
    pixels->rawData = buf;
    pixels->toMatrix();
  }

  bool fetchBMPData(FILE *f) {
    if (!f) return false;

    readFileInfo(f);

    if (!isBMP() || !is24Bit()) {
      fprintf(stderr, "Invalid BMP 24-bit file\n");
      return false;
    }

    readPixels(f);

    return true;
  }
};

struct BMPApp {
  BMPFile bmp;

  bool isMissingArg(int argc, int l, int r) { return (argc < l || r < argc); }

  void scale(FILE *f, int n, bool exportFile = false) {
    if (!bmp.fetchBMPData(f)) return;

    int newWidth = bmp.pixels->w * n;
    int newHeight = bmp.pixels->h * n;

    PixelArray *newPixels = new PixelArray(
        newWidth * newHeight * sizeof(RGBColor) + newWidth * newHeight,
        newWidth, newHeight);

    for (int i = 0; i < newHeight; i++) {
      for (int j = 0; j < newWidth; j++) {
        newPixels->data[i][j] = bmp.pixels->data[i / n][j / n];
      }
    }

    newPixels->flatMatrix();

    char *filename = (char *)malloc(100 * sizeof(char));
    sprintf(filename, "./dist/output.scale-%d.bmp", n);

    FILE *out = fopen(filename, "wb");
    bmp.header.info.fileSize = sizeof(BMP_HEADER_RAW) + sizeof(BMPDIB) +
                               newPixels->size + sizeof(BMPSignature);
    fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
    fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);

    bmp.dib.pixelArraySize = newPixels->size;
    bmp.dib.imgWidth = newWidth;
    bmp.dib.imgHeight = newHeight;
    fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);

    fwrite(newPixels->rawData, newPixels->size, 1, out);
    fclose(out);

    free(filename);
    delete newPixels;
  }

  void downResolution(FILE *f, int intensity = 2, bool exportFile = false) {
    if (!bmp.fetchBMPData(f)) return;

    for (int i = 0; i < bmp.pixels->h; i++) {
      for (int j = 0; j < bmp.pixels->w; j++) {
        bmp.pixels->data[i][j] =
            bmp.pixels->data[i - i % intensity][j - j % intensity];
      }
    }

    bmp.pixels->flatMatrix();

    char *filename = (char *)malloc(100 * sizeof(char));
    sprintf(filename, "./dist/output.censor.bmp");

    FILE *out = fopen(filename, "wb");
    fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
    fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);
    fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);
    fwrite(bmp.pixels->rawData, bmp.pixels->size, 1, out);
    fclose(out);

    free(filename);
  }

  void blacknwhite(FILE *f, bool exportFile = false) {
    if (!bmp.fetchBMPData(f)) return;

    for (int i = 0; i < bmp.pixels->h; i++) {
      for (int j = 0; j < bmp.pixels->w; j++) {
        const int r = bmp.pixels->data[i][j].r;
        const int g = bmp.pixels->data[i][j].g;
        const int b = bmp.pixels->data[i][j].b;
        const int average = (r + g + b) / 3;

        bmp.pixels->data[i][j] = RGBColor(average, average, average);
      }
    }

    bmp.pixels->flatMatrix();

    char *filename = (char *)malloc(100 * sizeof(char));
    sprintf(filename, "./dist/output.blacknwhite.bmp");

    FILE *out = fopen(filename, "wb");
    fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
    fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);
    fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);
    fwrite(bmp.pixels->rawData, bmp.pixels->size, 1, out);
    fclose(out);

    free(filename);
  }

  void cutBitmap(FILE *f, int hParts, int wParts) {
    if (!bmp.fetchBMPData(f)) return;

    wParts = wParts < 0 ? 1 : wParts;

    int hPartSize = bmp.pixels->h / hParts;
    int wPartSize = bmp.pixels->w / wParts;

    for (int i = 0; i < hParts; i++) {
      for (int j = 0; j < wParts; j++) {
        PixelArray *part = new PixelArray(
            hPartSize * wPartSize * sizeof(RGBColor) + hPartSize * wPartSize,
            wPartSize, hPartSize);

        for (int k = 0; k < hPartSize; k++)
          for (int l = 0; l < wPartSize; l++) {
            part->data[k][l] =
                bmp.pixels->data[i * hPartSize + k][j * wPartSize + l];
          }

        part->flatMatrix();

        char *filename = (char *)malloc(100 * sizeof(char));
        sprintf(filename, "./dist/part-%d-%d.bmp", i, j);

        FILE *out = fopen(filename, "wb");
        bmp.header.info.fileSize = sizeof(BMP_HEADER_RAW) + sizeof(BMPDIB) +
                                   part->size + sizeof(BMPSignature);
        fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
        fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);

        bmp.dib.pixelArraySize = part->size;
        bmp.dib.imgWidth = wPartSize;
        bmp.dib.imgHeight = hPartSize;
        fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);

        fwrite(part->rawData, part->size, 1, out);
        fclose(out);

        free(filename);
        delete part;
      }
    }
  }

  void flipHorizontal(FILE *f, bool exportFile = false) {
    if (!bmp.fetchBMPData(f)) return;

    for (int i = 0; i < bmp.pixels->h; i++) {
      for (int j = 0; j < bmp.pixels->w / 2; j++) {
        RGBColor temp = bmp.pixels->data[i][j];
        bmp.pixels->data[i][j] = bmp.pixels->data[i][bmp.pixels->w - j - 1];
        bmp.pixels->data[i][bmp.pixels->w - j - 1] = temp;
      }
    }

    bmp.pixels->flatMatrix();

    char *filename = (char *)malloc(100 * sizeof(char));
    sprintf(filename, "./dist/output.flip-horizontal.bmp");

    FILE *out = fopen(filename, "wb");
    fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
    fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);
    fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);
    fwrite(bmp.pixels->rawData, bmp.pixels->size, 1, out);
    fclose(out);

    free(filename);
  }

  void flipVertical(FILE *f, bool exportFile = false) {
    if (!bmp.fetchBMPData(f)) return;

    for (int i = 0; i < bmp.pixels->h / 2; i++) {
      for (int j = 0; j < bmp.pixels->w; j++) {
        RGBColor temp = bmp.pixels->data[i][j];
        bmp.pixels->data[i][j] = bmp.pixels->data[bmp.pixels->h - i - 1][j];
        bmp.pixels->data[bmp.pixels->h - i - 1][j] = temp;
      }
    }

    bmp.pixels->flatMatrix();

    char *filename = (char *)malloc(100 * sizeof(char));
    sprintf(filename, "./dist/output.flip-vertical.bmp");

    FILE *out = fopen(filename, "wb");
    fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
    fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);
    fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);
    fwrite(bmp.pixels->rawData, bmp.pixels->size, 1, out);
    fclose(out);

    free(filename);
  }

  void invertColor(FILE *f, bool exportFile = false) {
    if (!bmp.fetchBMPData(f)) return;

    for (int i = 0; i < bmp.pixels->h; i++) {
      for (int j = 0; j < bmp.pixels->w; j++) {
        bmp.pixels->data[i][j].r = 255 - bmp.pixels->data[i][j].r;
        bmp.pixels->data[i][j].g = 255 - bmp.pixels->data[i][j].g;
        bmp.pixels->data[i][j].b = 255 - bmp.pixels->data[i][j].b;
      }
    }

    bmp.pixels->flatMatrix();

    char *filename = (char *)malloc(100 * sizeof(char));
    sprintf(filename, "./dist/output.invert.bmp");

    FILE *out = fopen(filename, "wb");
    fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
    fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);
    fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);
    fwrite(bmp.pixels->rawData, bmp.pixels->size, 1, out);
    fclose(out);

    free(filename);
  }

  void grayScaleTozero(FILE *f, bool exportFile = false) {
    if (!bmp.fetchBMPData(f)) return;

    blacknwhite(f);

    for (int i = 0; i < bmp.pixels->h; i++) {
      for (int j = 0; j < bmp.pixels->w; j++) {
        if (bmp.pixels->data[i][j].r > 80) continue;

        bmp.pixels->data[i][j].r = 0;
        bmp.pixels->data[i][j].g = 0;
        bmp.pixels->data[i][j].b = 0;
      }
    }

    bmp.pixels->flatMatrix();

    char *filename = (char *)malloc(100 * sizeof(char));
    sprintf(filename, "./dist/output.grayScaleTozero.bmp");

    FILE *out = fopen(filename, "wb");
    fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
    fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);
    fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);
    fwrite(bmp.pixels->rawData, bmp.pixels->size, 1, out);
    fclose(out);

    free(filename);
  }

  void equalization(FILE *f, bool exportFile = false) {
    if (!bmp.fetchBMPData(f)) return;

    int hist[256] = {0};
    int cdf[256] = {0};
    int n = bmp.pixels->w * bmp.pixels->h;

    for (int i = 0; i < bmp.pixels->h; i++) {
      for (int j = 0; j < bmp.pixels->w; j++) {
        hist[bmp.pixels->data[i][j].r]++;
      }
    }

    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
      cdf[i] = cdf[i - 1] + hist[i];
    }

    for (int i = 0; i < bmp.pixels->h; i++) {
      for (int j = 0; j < bmp.pixels->w; j++) {
        bmp.pixels->data[i][j].r = 255 * cdf[bmp.pixels->data[i][j].r] / n;
        bmp.pixels->data[i][j].g = 255 * cdf[bmp.pixels->data[i][j].g] / n;
        bmp.pixels->data[i][j].b = 255 * cdf[bmp.pixels->data[i][j].b] / n;
      }
    }

    bmp.pixels->flatMatrix();

    char *filename = (char *)malloc(100 * sizeof(char));
    sprintf(filename, "./dist/output.equalization.bmp");

    FILE *out = fopen(filename, "wb");
    fwrite(&bmp.header.signature, sizeof(BMPSignature), 1, out);
    fwrite(&bmp.header.info, sizeof(BMPHeaderInfo), 1, out);
    fwrite(&bmp.dib, sizeof(BMP_DIB_RAW), 1, out);
    fwrite(bmp.pixels->rawData, bmp.pixels->size, 1, out);
    fclose(out);

    free(filename);
  }
};

int main(int argc, char *argv[]) {
  BMPApp app;
  FILE *f = app.bmp.openFile(argv[1]);

  if (!f) return 0;
  if (app.isMissingArg(argc, 3, 4)) {
    fprintf(stderr, "Unsuitable arguments!\n");
    fprintf(stderr, "Usage: %s <filename> <hParts> <wParts>\n", "cutbmp");
    fprintf(stderr, "Example: %s ./img.bmp 2 2\n", "cutbmp");
    return 0;
  }

  int hParts = atoi(argv[2]);
  int wParts = argc == 4 ? atoi(argv[3]) : -1;

  app.cutBitmap(f, hParts, wParts);

  app.blacknwhite(f, true);
  app.equalization(f, true);
  app.grayScaleTozero(f, true);
  app.invertColor(f, true);

  app.downResolution(f, 8, true);
  app.scale(f, 3, true);

  app.flipHorizontal(f, true);
  app.flipVertical(f, true);

  fclose(f);

  // TEST
  // cout << "\n========= TEST =========\n";
  // f = fopen("./lena-1.bmp", "rb");
  // BMPFile bmp;
  // bmp.fetchBMPData(f);
  // bmp.printInfo();
  // fclose(f);

  // cout << "\n========= TEST =========\n";
  // f = fopen("./part-0-0.bmp", "rb");
  // bmp.fetchBMPData(f);
  // bmp.printInfo();
  // fclose(f);

  return 0;
}
