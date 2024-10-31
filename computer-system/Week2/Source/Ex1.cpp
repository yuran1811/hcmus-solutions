#include <iostream>

using std::cin;
using std::cout;

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

int main() {
  cout << "Nhap so cham dong (32-bit): ";
  float x(0.0);
  cin >> x;

  dumpFloat(&x);

  return 0;
}