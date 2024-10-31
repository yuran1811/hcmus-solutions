#include <iostream>

using namespace std;

void experiment1() {
  float a = 4.245f;
  int b = a;
  float c = b;

  cout << "float -> int -> float:\n";
  cout << "Before: " << a << ", After: " << c << " => "
       << (a == c ? "Ket qua nhu ban dau" : "Ket qua khong nhu ban dau")
       << "\n\n";
}

void experiment2() {
  int a = 6;
  float b = a;
  int c = b;

  cout << "int -> float -> int:\n";
  cout << "Before: " << a << ", After: " << c << " => "
       << (a == c ? "Ket qua nhu ban dau" : "Ket qua khong nhu ban dau")
       << "\n\n";
}

void experiment3() {
  float a = 0.1f;
  float b = 0.1f;
  float c = 0.1f;

  cout << "(a + b) + c = " << (a + b) + c << "\na + (b + c) = " << (a + (b + c))
       << "\n=> phep cong so cham dong "
       << (((a + b) + c) == (a + (b + c)) ? "co tinh ket hop"
                                          : "khong co tinh ket hop")
       << "\n\n";
}

void experiment4() {
  float f = 18.11f;
  int i = (int)(3.14159 * f);
  cout << "*) f = " << f;
  cout << "\n*) 3.14159 * f = 56.8941949\n=> i = 56\n";

  f = f + (float)i;
  cout << "*) f = f + i = 18.11 + 56 = 74.11\n\n";

  cout << "i == (int)((float)i): " << (i == (int)((float)i) ? "true" : "false")
       << "\n";
  cout << "i == (int)((double)i): "
       << (i == (int)((double)i) ? "true" : "false") << "\n";
  cout << "| i == (int)((float)i) == (int)((double)i): true\n| because "
          "(float)i = (double)i = 56.0 then (int)56 = 56 = i\n\n";

  cout << "f == (float)((int)f): " << (f == (float)((int)f) ? "true" : "false")
       << "\n";
  cout << "f == (double)((int)f): "
       << (f == (double)((int)f) ? "true" : "false") << "\n";
  cout << "| f == (float)((int)f) == (double)((int)f): false\n| because "
          "(float)((int)f) = (double)((int)f) = 74.0 then 74.0 != 74.11 = f\n";
}

int main() {
  experiment1();
  experiment2();
  experiment3();
  experiment4();

  return 0;
}