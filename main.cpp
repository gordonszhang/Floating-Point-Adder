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

void internalRep(unsigned int hide, unsigned int frac, unsigned int gr, int exp,
                 string label) {
  cout << "internal rep of " << left << setw(15) << label << hide << "."
  << toBinary(frac, 4) << " " << toBinary(gr, 2) << " x 2^(";
  if(exp > 0) cout << "+";
  cout << exp << ")" << endl;
}

int main() {
  // Container variable declarations
  // All but exponent and value are unsigned to simplify comparison logic
  // Ex: 0b1000 > 0b0111
  unsigned int x, y, s = 256;
  unsigned int x_sign, x_hide, x_frac, x_gr;
  unsigned int y_sign, y_hide, y_frac, y_gr;
  unsigned int s_sign, s_hide, s_frac, s_gr;
  int x_exp, y_exp, s_exp;
  float x_val, y_val, s_val;

  while(x < 0 || x > 255 || y < 0 || y > 255) {
    cout << "Input two integers (0-255): " << endl;
    cin >> x >> y;
  }

  cout << endl << "first value: " << x << endl;
  cout << "second value: " << y << endl << endl;
  cout << "encoding of first value: " << setw(14) << toBinary(x, 8) << endl;
  cout << "encoding of second value: " << setw(13) << toBinary(y, 8) << endl << endl;

  // Assign variables
  x_sign = (x >> 7) & 1;
  x_hide = 1;
  x_exp = ((x >> 4) & 0x7) - 4;
  x_frac = x & 0xf;
  x_gr = 0;

  y_sign = (y >> 7) & 1;
  y_hide = 1;
  y_exp = ((y >> 4) & 0x7) - 4;
  y_frac = y & 0xf;
  y_gr = 0;


  internalRep(x_hide, x_frac, x_gr, x_exp, "first value:");
  internalRep(y_hide, y_frac, y_gr, y_exp, "second value:");

  cout << "hidden bit---------------------^ ffff gr" << endl;
  cout << "4-bit fraction-------------------^^^^" << endl << endl;

  x_val = (x_hide + x_frac / 16.0) * pow(2, x_exp);
  cout << "first value is: " << setw(2) << (!x_sign ? "+" : "-") << setw(6)
  << setfill('0') << left << x_hide + x_frac / 16.0 << " x 2^("
  << (((x_exp - 4) > 0) ? "+" : "") << x_exp << ") = " << x_val << endl;

  y_val = (y_hide + y_frac / 16.0) * pow(2, y_exp);
  cout << "second value is: " << setw(1) << (!y_sign ? "+" : "-") << setw(6)
  << left << y_hide + y_frac / 16.0 << " x 2^("
  << (((y_exp - 4) > 0) ? "+" : "") << y_exp << ") = " << y_val
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

    internalRep(x_hide, x_frac, x_gr, x_exp, "first value");
    internalRep(y_hide, y_frac, y_gr, y_exp, "second value");
  }

  while (x_exp != y_exp) {
    cout << "second operand shifted to equalize exponents" << endl;
    y_gr = ((y_frac & 1) << 1) | (y_gr>>1);
    y_frac = (y_frac >> 1) | (y_hide << 3);
    y_hide = 0;
    y_exp++;
    internalRep(y_hide, y_frac, y_gr, y_exp, "second value");
  }

  if (x_sign == 1) {
    x_frac = ~x_frac;
    x_hide = ~x_hide & 0x3;
    cout << "x_hide is " << toBinary(x_hide, 2);
    x_gr = (~x_gr & 0x3) + 1;
    if(x_gr > 0x3) {
      x_frac = x_frac + 1;
      x_gr = x_gr & 0x3;
    }
    x_frac = x_frac & 0xf;
    cout << "first operand negated" << endl;
      internalRep(x_hide & 0x1, x_frac, x_gr, x_exp, "first value");

  }

  if (y_sign == 1) {
    y_frac = ~y_frac;
    y_hide = ~y_hide & 0x3;
    cout << "y_hide is " << toBinary(y_hide, 2);
    y_gr = (~y_gr & 0x3) + 1;

    if(y_gr > 0x3) {
      y_frac = y_frac + 1;
      y_gr = y_gr & 0x3;
    }

    y_frac = y_frac & 0xf;

    cout << "second operand negated" << endl;
        internalRep(y_hide & 0x1, y_frac, y_gr, y_exp, "second value");
  }

  cout << "addition takes place" << endl;
  internalRep(x_hide & 0x1, x_frac, x_gr, x_exp, "first value");
  internalRep(y_hide & 0x1, y_frac, y_gr, y_exp, "second value");

  bool overflow = false;
  bool carry = false;
  s_exp = x_exp;
  s_gr = x_gr + y_gr;

  s_frac = x_frac + y_frac;

  if(s_gr > 0x3) {
    cout << "no";
    s_frac = s_frac + 0x1;
    s_gr = s_gr & 0x3;
  }

  s_hide = x_hide + y_hide;
  if(s_frac > 0xf) {
    s_hide = s_hide + 0x1;
    s_frac = s_frac & 0xf;
  }

  if(s_hide > 0x3) {
        cout << "carry set to true" << endl;
    carry = true;
    s_hide = s_hide & 0x3;
  }

  if(s_hide > 0x1) {
    cout << "overflow set to true" << endl;
    overflow = true;
  }

  s_sign = (s_hide >> 1) & 0x1;

  cout << "internal rep of sum: " << setw(10) << toBinary(s_hide,2) << "." << toBinary(s_frac, 4)
  << " " << toBinary(s_gr, 2) << " x 2^(";
  if(s_exp > 0) cout << "+";
  cout << s_exp << ")" << endl;


  if(carry && overflow) {
    cout << "sum negated" << endl;
    s_frac = ~s_frac & 0xf;
    s_hide = ~s_hide & 0x3;
    s_gr = (~s_gr & 0x3) + 1;
    if(s_gr > 0x3) {
      s_frac = s_frac + 1;
      s_gr = s_gr & 0x3;
    }
    cout << "s_frac is " << toBinary(s_frac, 5) << endl;
    cout << "s_sign is " << toBinary(s_sign, 2) << endl;
    if(s_frac > 0xf) {
      s_hide = (s_hide + 1) & 0x3;
    }
    s_frac = s_frac & 0xf;
    cout << "internal rep of sum: " << setw(10) << toBinary(s_hide,2) << "." << toBinary(s_frac, 4)
    << " " << toBinary(s_gr, 2) << " x 2^(";
    if(s_exp > 0) cout << "+";
    cout << s_exp << ")" << endl;

  }

  if(!carry && overflow) {
    cout << "sum normalized" << endl;

    while ((s_hide & 0x1) < 0x1) {
      s_gr = ((s_frac & 1) << 1) | (s_gr>>1);
      s_frac = (s_frac >> 1) | (s_hide & 0x1);
      s_hide = (s_hide >> 1) & 0x3;
      s_exp++;
    }

    cout << "internal rep of sum: " << setw(10) << toBinary(s_hide,2) << "." << toBinary(s_frac, 4)
    << " " << toBinary(s_gr, 2) << " x 2^(";
    if(s_exp  > 0) cout << "+";
    cout << s_exp << ")" << endl;

    s_sign = 0;
  }

  else {
    cout << "sum normalized" << endl;
    while (s_hide < 0x1) {
      s_hide = ((s_hide << 1) & 0x1) | ((s_frac >> 3) & 1);
      s_frac = ((s_frac << 1) & 0xf) | ((s_gr >> 1) & 1);
      s_gr = (s_gr << 1) & 0x3;
      s_exp--;
    }
    cout << "internal rep of sum: " << setw(10) << s_sign << s_hide << "." << toBinary(s_frac, 4)
    << " " << toBinary(s_gr, 2) << " x 2^(";
    if(s_exp  > 0) cout << "+";
    cout << s_exp << ")" << endl;
  }

  if(((s_gr >> 1) & 0x1) == 1) {


  if((s_frac & 0x1) == 1) {
    cout << "sum rounded" << endl;
    s_frac = s_frac + 1;
    if(s_gr > 0x3) {
      s_frac = s_frac + 0x1;
      s_gr = s_gr & 0x3;
    }
    s_hide = x_hide + y_hide;
    if(s_frac > 0xf) {
      s_hide = s_hide + 0x1;
      s_frac = s_frac & 0xf;
    }
    s_sign = x_sign + y_sign;

    if(s_hide > 0x1) {
      s_sign = s_sign + 0x1;
      s_hide = s_hide & 1;
    }

    s_sign = s_sign & 0x1;

  }
  }

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
    if(s_exp > 0) cout << "+";
    cout << s_exp << ")" << endl;
  }

  cout << "hidden bit---------------------^ ffff gr" << endl;
  cout << "4-bit fraction-------------------^^^^" << endl << endl;
  cout << "s_sign is " << toBinary(s_sign, 2) << endl;
  s_exp += 4;
  s = s_sign;
    cout << "s is " << toBinary(s, 1) << endl;
  s = (s << 3) + (s_exp);
    cout << "s is " << toBinary(s, 5) << endl;
  s = (s << 4) + s_frac;
    cout << "s is " << toBinary(s, 7) << endl;

  cout << "encoding of returned value:" << setw(11) << toBinary(s, 8) << endl << endl;


  return 0;
}
