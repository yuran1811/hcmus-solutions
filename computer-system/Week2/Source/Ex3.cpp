#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>

#define BIT(x, i) ((x >> i) & 1)
#define MASK(x, i) (x << i)

using namespace std;

void printBin(int n) {
  for (int i = 31; i >= 0; i--) {
    if (i == 31)
      cout << "Sign:\t\t";
    else if (i == 30)
      cout << "Exponent:\t";
    else if (i == 22)
      cout << "Significand:\t";

    cout << (n >> i & 1) << "\0\n"[i == 31 || i == 23];
  }
}

void dumpFloat(float *p) {
  int *x = (int *)p;
  printBin(*x);
}

void forceFloat(float *p, char *s) {
  int x = 0;
  for (int i = 0; i < min((int)strlen(s), 32); i++)
    x |= MASK((i < (int)strlen(s) ? s[i] - '0' : 0),
              (min((int)strlen(s), 32) - 1 - i));
  *p = *(float *)&x;
}

int main() {
  cout << "- Bieu dien nhi phan cua 1.3e+20:\n";
  dumpFloat((float *)new float(1.3e+20));

  cout << "\n\n- So float nho nhat > 0 la:\n";
  int smallest = 1;
  float smallestFloat = *(float *)&smallest;
  cout << smallestFloat << '\n';
  cout << "- Bieu dien nhi phan cua " << smallestFloat << ":\n";
  dumpFloat(&smallestFloat);

  float p(0.0);
  forceFloat(&p, (char *)"01111111100000000000000000000000");
  cout << "\n\nSo vo cung (inf): " << p
       << " (0 11111111 00000000000000000000000)\n";

  forceFloat(&p, (char *)"01111111110000000000000000000000");
  cout << "So bao loi (nan): " << p
       << " (0 11111111 10000000000000000000000)\n";

  cout << "\nNhung truong hop tao ra cac so dac biet tren:\n";
  forceFloat(&p, (char *)"01111111100000000000000000000000");
  cout << "inf + inf = " << p + p << '\n';

  forceFloat(&p, (char *)"01111111110000000000000000000000");
  cout << "nan + nan = " << p + p << '\n';

  forceFloat(&p, (char *)"01111111100000000000000000000000");
  cout << "inf - inf = " << p - p << '\n';

  forceFloat(&p, (char *)"01111111100000000000000000000000");
  cout << "1 - inf = " << 1 - p << '\n';

  forceFloat(&p, (char *)"01111111110000000000000000000000");
  cout << "nan - nan = " << p - p << '\n';

  forceFloat(&p, (char *)"01111111100000000000000000000000");
  cout << "inf * inf = " << p * p << '\n';

  forceFloat(&p, (char *)"01111111110000000000000000000000");
  cout << "nan * nan = " << p * p << '\n';

  forceFloat(&p, (char *)"01111111100000000000000000000000");
  cout << "inf / inf = " << p / p << '\n';

  forceFloat(&p, (char *)"01111111110000000000000000000000");
  cout << "nan / nan = " << p / p << '\n';

  forceFloat(&p, (char *)"10101010000000000000000000000000");
  cout << "sqrt(p) (p < 0) = " << sqrt(p) << '\n';

  forceFloat(&p, (char *)"00001000010000000000000000000000");
  cout << "p / 0 = " << p / 0 << '\n';

  forceFloat(&p, (char *)"00000000000000000000000000000000");
  cout << "0 / 0 = " << p / p << '\n';

  return 0;
}