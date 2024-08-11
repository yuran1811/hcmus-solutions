#include "bmp.hpp"

int main(int argc, char *argv[]) {
  BMPApp app;
  FILE *f = app.bmp.openFile(argv[1]);
  if (!f) return 1;

  if (app.isMissingArg(argc)) {
    fprintf(stderr, "Unsuitable arguments!\n");
    fprintf(stderr, "Usage: %s <filename> -h <hParts> -w <wParts>\n", "cutbmp");
    fprintf(stderr, "Example: %s ./img.bmp -h 2\n", "cutbmp");
    fprintf(stderr, "Example: %s ./img.bmp -h 2 -w 2\n", "cutbmp");
    return 1;
  }

  int hParts = 1, wParts = 1;
  for (char c = 0; (c = getopt(argc, argv, "-h:-w:")) != -1;) {
    if (!optarg) continue;

    switch (c) {
      case 'h':
        hParts = atoi(optarg);
        break;
      case 'w':
        wParts = atoi(optarg);
        break;
    }
  }

  app.cutBitmap(f, hParts, wParts);

  app.blacknwhite(f, true);
  app.equalization(f, true);
  app.grayScaleTozero(f, true);
  app.invertColor(f, true);

  app.scale(f, 2, true);
  app.downResolution(f, 20, true);

  app.flipHorizontal(f, true);
  app.flipVertical(f, true);

  fclose(f);

  return 0;
}
