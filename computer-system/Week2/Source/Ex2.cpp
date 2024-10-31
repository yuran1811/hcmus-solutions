#include <iostream>
#include <cstdio>
#include <cstring>

#define BIT(x, i) ((x >> i) & 1)
#define MASK(x, i) (x << i)

using namespace std;

void forceFloat(float *p, char *s) {
  int x = 0;
  for (int i = 0; i < min((int)strlen(s), 32); i++)
    x |= MASK((i < (int)strlen(s) ? s[i] - '0' : 0),
              (min((int)strlen(s), 32) - 1 - i));
  *p = *(float *)&x;
}

int main() {
  cout << "Nhap day nhi phan: ";
  char *s = new char[32];
  scanf("%s", s);

  float p(0.0);
  forceFloat(&p, s);
  cout << "So cham dong tuong ung la: ";
  cout << p << "\n";

  delete[] s;

  return 0;
}

/*
01111111110000000000000000000000: NaN
01111111100000000000000000000000: inf
00111101110011001100110011001101: 0.1
10100011001101011000000000000000: -9.83913471531 × 10-18
01000100001101100001000000000000: 728.25

00000000000000000000000000000101: 7.00649e-45
                             101: 7.00649e-45
*/