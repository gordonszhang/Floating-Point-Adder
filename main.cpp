#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

string toBinary(int value, int length) {
  string output = "";
  for(int i = length - 1; i >= 0; i--) {
    if((value >> i) & 1)
    output += "1";
    else
    output += "0";
  }
  return output;
}

int main() {
  int x, y, s = -1;
  int x_sign, x_hide, x_exp, x_frac, x_gr;
  int y_sign, y_hide, y_exp, y_frac, y_gr;
  int s_sign, s_hide, s_exp, s_frac, s_gr;
  float x_val, y_val;

  while(x < 0 || x > 255 || y < 0 || y > 255) {
    cout << "Input two integers (0-255): " << endl;
    cin >> x >> y;
  }

  cout << endl << "first value: " << x << endl;
  cout << "second value: " << y << endl << endl;
  cout << "encoding of first value: " << setw(14) << toBinary(x, 8) << endl;
  cout << "encoding of second value: " << setw(13) << toBinary(y, 8) << endl << endl;

  x_sign = (x >> 7) & 1;
  x_hide = 1;
  x_exp = (x >> 4) & 0x7;
  x_frac = x & 0xf;
  x_gr = 0;

  y_sign = (y >> 7) & 1;
  y_hide = 1;
  y_exp = (y >> 4) & 0x7;
  y_frac = y & 0xf;
  y_gr = 0;


  cout << "internal rep of first value: " << setw(3) << x_hide << "." << toBinary(x_frac, 4)
  << " " << toBinary(x_gr, 2) << " x 2^(";
  if((x_exp - 4) > 0) cout << "+";
  cout << x_exp - 4 << ")" << endl;

  cout << "internal rep of second value: " << setw(2) << y_hide << "." << toBinary(y_frac, 4)
  << " " << toBinary(y_gr, 2) << " x 2^(";
  if((y_exp - 4) > 0) cout << "+";
  cout << y_exp - 4 << ")" << endl;

  cout << "hidden bit---------------------^ ffff gr" << endl;
  cout << "4-bit fraction-------------------^^^^" << endl << endl;

  x_val = (x_hide + x_frac / 16.0) * pow(2, x_exp - 4);
  cout << "first value is: " << setw(2) << (!x_sign ? "+" : "-") << setw(6)
  << setfill('0') << left << x_hide + x_frac / 16.0 << " x 2^("
  << (((x_exp - 4) > 0) ? "+" : "") << x_exp - 4 << ") = " << x_val << endl;

  y_val = (y_hide + y_frac / 16.0) * pow(2, y_exp - 4);
  cout << "second value is: " << setw(1) << (!y_sign ? "+" : "-") << setw(6)
  << left << y_hide + y_frac / 16.0 << " x 2^("
  << (((y_exp - 4) > 0) ? "+" : "") << y_exp - 4 << ") = " << y_val
  << setfill(' ') << right << endl;



  cout << endl << "addition of the values: " << endl;
  if (y_exp > x_exp) {
    cout << "operands are swapped" << endl;
    int t_sign, t_frac, t_exp;
    //store temp = x
    t_sign = x_sign;
    t_frac = x_frac;
    t_exp  = x_exp;
    //load x = y
    x_sign = y_sign;
    x_frac = y_frac;
    x_exp  = y_exp;
    //load y = temp
    y_sign = t_sign;
    y_frac = t_frac;
    y_exp  = t_exp;
    cout << "internal rep of first value: " << setw(3) << x_hide << "." << toBinary(x_frac, 4)
    << " " << toBinary(x_gr, 2) << " x 2^(";
    if((x_exp - 4) > 0) cout << "+";
    cout << x_exp - 4 << ")" << endl;

    cout << "internal rep of second value: " << setw(2) << y_hide << "." << toBinary(y_frac, 4)
    << " " << toBinary(y_gr, 2) << " x 2^(";
    if((y_exp - 4) > 0) cout << "+";
    cout << y_exp - 4 << ")" << endl;
  }

  while (x_exp != y_exp) {
    cout << "second operand shifted to equalize exponents" << endl;
    y_gr = ((y_frac & 1) << 1) | (y_gr>>1);
    y_frac = (y_frac >> 1) | (y_hide << 3);
    y_hide = 0;
    y_exp++;
    cout << "internal rep of second value: " << setw(2) << y_hide << "." << toBinary(y_frac, 4)
    << " " << toBinary(y_gr, 2) << " x 2^(";
    if((y_exp - 4) > 0) cout << "+";
    cout << y_exp - 4 << ")" << endl;
  }

  if (x_sign == 1) {
    x_sign = 0;
    x_frac = ~x_frac + 1;
    x_hide = x_hide ^ 1;
    cout << "first operand negated" << endl;
    cout << "internal rep of first value: " << setw(3) << x_hide << "." << toBinary(x_frac, 4)
    << " " << toBinary(x_gr, 2) << " x 2^(";
    if((x_exp - 4) > 0) cout << "+";
    cout << x_exp - 4 << ")" << endl;

  }

  if (y_sign == 1) {
    y_sign = 0;
    y_frac = ~y_frac + 1;
    y_hide = y_hide ^ 1;
    cout << "second operand negated" << endl;
    cout << "internal rep of second value: " << setw(2) << y_hide << "." << toBinary(y_frac, 4)
    << " " << toBinary(y_gr, 2) << " x 2^(";
    if((y_exp - 4) > 0) cout << "+";
    cout << y_exp - 4 << ")" << endl;
  }

  cout << "addition takes place" << endl;
  cout << "internal rep of first value: " << setw(3) << x_hide << "." << toBinary(x_frac, 4)
  << " " << toBinary(x_gr, 2) << " x 2^(";
  if((x_exp - 4) > 0) cout << "+";
  cout << x_exp - 4 << ")" << endl;

  cout << "internal rep of second value: " << setw(2) << y_hide << "." << toBinary(y_frac, 4)
  << " " << toBinary(y_gr, 2) << " x 2^(";
  if((y_exp - 4) > 0) cout << "+";
  cout << y_exp - 4 << ")" << endl;


  s_exp = x_exp;
  s_gr = x_gr ^ y_gr;
  s_frac = x_frac ^ y_frac;
  if(s_gr > 0x3) {
    s_frac = s_frac ^ ((s_gr & (0x4)) >> 1);
    s_gr = s_gr & 0x3;
  }

  s_hide = x_hide ^ y_hide;
    cout<< "s_hide is " << s_hide;
  if(s_frac > 0xf) {
    s_hide = s_hide ^ ((s_frac & (0x10)) >> 4);
    s_frac = s_frac & 0xf;
  }
  s_sign = x_sign ^ y_sign;
  cout<< "s_hide is " << s_hide;
  if(s_hide > 0x1) {
    s_sign = s_sign ^ ((s_hide & (0x2)) >> 1);
    s_hide = s_hide & 1;
  }
  cout<< "s_sign is " << s_sign;
  s_sign = s_sign & 0x1;

  cout << "internal rep of sum: " << setw(10) << s_sign << s_hide << "." << toBinary(s_frac, 4)
  << " " << toBinary(s_gr, 2) << " x 2^(";
  if((s_exp - 4) > 0) cout << "+";
  cout << s_exp - 4 << ")" << endl;

  if(s_hide < 0x1) {
    cout << "sum normalized" << endl;
    while (s_hide < 0x1) {
      s_hide = ((s_hide << 1) & 0x1) | ((s_frac >> 3) & 1);
      s_frac = ((s_frac << 1) & 0xf) | ((s_gr >> 1) & 1);
      s_gr = (s_gr << 1) & 0x3;
      s_exp--;
    }
    cout << "internal rep of sum: " << setw(10) << s_sign << s_hide << "." << toBinary(s_frac, 4)
    << " " << toBinary(s_gr, 2) << " x 2^(";
    if((s_exp - 4) > 0) cout << "+";
    cout << s_exp - 4 << ")" << endl;
  }

  cout << "hidden bit---------------------^ ffff gr" << endl;
  cout << "4-bit fraction-------------------^^^^" << endl << endl;

  s = s_sign;
  s = (s << 4) | ((s_exp) << 4);
  s = s | s_frac;

  cout << "encoding of returned value:" << setw(11) << toBinary(s, 8) << endl << endl;


  return 0;
}
