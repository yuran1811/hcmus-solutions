#include <iostream>

using std::cin;
using std::cout;

void printBin(int n) {
  cout << "Day nhi phan cua " << n << " la:\n";
  for (int i = 31; i >= 0; i--) cout << (n >> i & 1) << "\0 "[i % 4 == 0];
  cout << '\n';
}

int handleBinPattern(char *a) {
  int x = 0;
  for (int i = 31; i >= 0; i--) x |= (a[i] - '0') << (31 - i);
  return x;
}

void section1() {
  cout << "Nhap so nguyen (32-bit): ";
  int n;
  cin >> n;

  printBin(n);
  cout << '\n';
}

void section2() {
  cout << "Nhap day nhi phan (32-bit): ";
  char *a = new char[32];
  scanf("%s", a);

  cout << "So nguyen tuong ung la: ";
  cout << handleBinPattern(a) << '\n';

  delete[] a;
}

int main() {
  section1();
  section2();

  return 0;
}

/*
12345
11111111111111110100110110010010
*/