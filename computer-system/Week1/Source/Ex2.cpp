#include <iostream>

#define BIT(x, i) (((x) >> (i)) & 1)
#define MASK(x, i) ((x) << (i))

using namespace std;

namespace utils {
void printBin(int n, int maxBit = 8) {
  for (int i = maxBit - 1; i >= 0; i--) cout << BIT(n, i) << "\0 "[i % 4 == 0];
}

void printBoothBin(int n) {
  for (int i = 16; i >= 0; i--)
    cout << BIT(n, i) << "\0 "[i == 13 || i == 9 || i == 5 || i == 1];
}

void modBit(int &n, int i, bool x) { n ^= (-x ^ n) & MASK(1, i); }

const int bintoi(const string &a, int maxBit = 8) {
  const bool isNegative = a[0] == '1';

  int res = !isNegative ? 0 : -1;
  for (int i = 0; i < maxBit; i++) {
    const bool x = a[i] - '0';
    const int mask = MASK(x, maxBit - 1 - i);

    if (isNegative)
      modBit(res, maxBit - 1 - i, x);
    else
      res |= mask;
  }

  return res;
}

const int bintoi_raw(const string &a, int maxBit = 8) {
  int res = 0;
  for (int i = 0; i < maxBit; i++) {
    const bool x = a[i] - '0';
    const int mask = MASK(x, maxBit - 1 - i);
    res |= mask;
  }

  return res;
}

const string itobin(const int &n, int maxBit = 8) {
  string res = "";
  for (int i = maxBit - 1; i >= 0; i--) res += BIT(n, i) ? "1" : "0";
  return res;
}

const string invert(const string &a, int maxBit = 8) {
  string res = "";
  for (int i = 0; i < maxBit; i++) res += a[i] - '0' ? "0" : "1";
  return res;
}
}  // namespace utils

int sum(const string &a, const string &b, int maxBit = 8) {
  int res = 0;
  bool carry = 0;

  for (int i = maxBit - 1; i >= 0; i--) {
    const bool x = a[i] - '0';
    const bool y = b[i] - '0';

    res |= MASK(x ^ y ^ carry, maxBit - 1 - i);
    carry = (x & y) | (carry & x) | (carry & y);
  }

  return utils::bintoi(utils::itobin(res, maxBit), maxBit);
}

void sumInDiv(int &pattern, const int &m) {
  int tmp = 0;
  for (int c = 0, i = 0; i < 8; i++) {
    const bool x = BIT(pattern, i + 9);
    const bool y = BIT(m, i);

    tmp |= MASK(x ^ y ^ c, i);

    c = (x & y) | (c & x) | (c & y);
  }

  for (int i = 0; i < 8; i++) {
    utils::modBit(pattern, i + 9, 0);
    pattern |= MASK(BIT(tmp, i), i + 9);
  }
}

int sub(const string &a, const string &b) {
  return sum(a, utils::itobin(sum(utils::invert(b), utils::itobin(1))));
}

int multi(const string &a, const string &b) {
  const int m =
      utils::bintoi_raw(utils::itobin(utils::bintoi(a), 32).substr(24));
  const int m_comp = sum(utils::invert(utils::itobin(m)), utils::itobin(1));
  const int q =
      utils::bintoi_raw(utils::itobin(utils::bintoi(b), 32).substr(24));

  int pattern = q << 1;

  for (int loopCount = 8; loopCount-- > 0;) {
    const bool q1 = BIT(pattern, 1);
    const bool q0 = BIT(pattern, 0);

    if (q1 ^ q0) sumInDiv(pattern, (q1 == 1 && q0 == 0) ? m_comp : m);

    const bool isNegavite = BIT(pattern, 16);
    pattern >>= 1;
    pattern |= MASK(isNegavite, 16);
  }

  return utils::bintoi(utils::itobin(pattern, 32).substr(15, 16), 8 * 2);
}

int div(const string &a, const string &b) {
  const int m =
      utils::bintoi_raw(utils::itobin(utils::bintoi(b), 32).substr(24));
  const int m_comp = sum(utils::invert(utils::itobin(m)), utils::itobin(1));
  const int q =
      utils::bintoi_raw(utils::itobin(utils::bintoi(a), 32).substr(24));
  const int q_comp = sum(utils::invert(utils::itobin(q)), utils::itobin(1));

  const bool qNegative = BIT(q, 7);
  const bool mNegative = BIT(m, 7);

  int pattern = (qNegative ? q_comp : q) << 1;
  // if (qNegative)
  //   for (int i = 16; i > 8; i--) {
  //     pattern |= MASK(1, i);
  //   }

  for (int loopCount = 8; loopCount-- > 0;) {
    pattern <<= 1;

    sumInDiv(pattern, mNegative ? m : m_comp);

    if (BIT(pattern, 16)) {
      utils::modBit(pattern, 1, 0);
      sumInDiv(pattern, mNegative ? m_comp : m);
    } else
      utils::modBit(pattern, 1, 1);
  }

  cout << "r="
       << ((qNegative) ? -1 : 1) *
              utils::bintoi(utils::itobin(pattern, 32).substr(15, 8), 8)
       << ", q=";
  return ((qNegative ^ mNegative) ? -1 : 1) *
         utils::bintoi(utils::itobin(pattern, 32).substr(23, 8), 8);
}

int main() {
  string a, b;

  cout << "Nhap a: ";
  cin >> a;
  cout << "Nhap b: ";
  cin >> b;

  if (a.length() != 8 || b.length() != 8)
    return cout << "Nhap day nhi phan 8 bit!", 0;

  cout << "a: " << utils::bintoi(a) << '\n';
  cout << "b: " << utils::bintoi(b) << "\n\n";

  cout << "a+b: " << sum(a, b) << '\n';
  cout << "a-b: " << sub(a, b) << '\n';
  cout << "a*b: " << multi(a, b) << '\n';
  cout << "a/b: " << div(a, b) << '\n';

  return 0;
}

/*
88, 21
01011000
00010101

-88, 21
10101000
00010101

-88, -21
10101000
11101011

88, -21
01011000
11101011

7, -3
00000111
11111101

7, 3
00000111
00000011

*/