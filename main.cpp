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
  cout << "internal rep of " << left << setfill(' ') << setw(14) << label
       << toBinary(hide, 2) << "." << toBinary(frac, 4) << " "
       << toBinary(gr, 2) << " x 2^(";
  if(exp > 0) cout << "+";
  cout << exp << ")" << endl;
}

void floatingRep(unsigned int sign, unsigned int hide, unsigned int frac,
                 int exp, float val, string label) {
  cout << right << setfill(' ') << setw(16) << label << (!sign ? "+" : "-")
       << left <<setw(6) << setfill('0') << hide + frac / 16.0 << " x 2^("
       << ((exp > 0) ? "+" : "") << exp << ") = " << val << endl;
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

  // Prompt terminal for input of two unsigned integers in the range 0-255
  while(x < 0 || x > 255 || y < 0 || y > 255) {
    cout << "Input two integers (0-255): " << endl;
    cin >> x >> y;
  }

  // Preliminary printouts; display binary encoding
  cout << endl << "first value: " << x << endl;
  cout << "second value: " << y << endl << endl;
  cout << "encoding of first value: " << setw(14) << toBinary(x, 8) << endl;
  cout << "encoding of second value: " << setw(13) << toBinary(y, 8) << endl << endl;

  // Assign variables
  x_sign = (x >> 7) & 1;          // sign = sign     -> 1 bit
  x_hide = 1;                     // hide = hidden   -> 1 bit (2 for sum)
  x_exp = ((x >> 4) & 0x7) - 4;   // exp = exponent  -> 3 bits
  x_frac = x & 0xf;               // frac = fraction -> 4 bits
  x_gr = 0;

  y_sign = (y >> 7) & 1;
  y_hide = 1;
  y_exp = ((y >> 4) & 0x7) - 4;
  y_frac = y & 0xf;
  y_gr = 0;

  // Display internal representations of x and y
  internalRep(x_hide, x_frac, x_gr, x_exp, "first value:");
  internalRep(y_hide, y_frac, y_gr, y_exp, "second value:");

  cout << "hidden bit---------------------^ ffff gr" << endl;
  cout << "4-bit fraction-------------------^^^^" << endl << endl;

  // Display floating point values of x and y
  x_val = (x_hide + x_frac / 16.0) * pow(2, x_exp);
  floatingRep(x_sign, x_hide, x_frac, x_exp, x_val, "first value is:");

  y_val = (y_hide + y_frac / 16.0) * pow(2, y_exp);
  floatingRep(y_sign, y_hide, y_frac, y_exp, y_val, "second value is:");

  /* Begin floating-point addition */

  cout << endl << "addition of the values: " << endl;
  // If x's exponent is greater than y's, swap all variables
  if (y_exp > x_exp) {
    cout << "operands are swapped" << endl;
    int t_sign, t_frac, t_exp;

    t_sign = x_sign;  // Store temp = x
    t_frac = x_frac;
    t_exp  = x_exp;

    x_sign = y_sign;  // Load x = y
    x_frac = y_frac;
    x_exp  = y_exp;

    y_sign = t_sign;  // Load y = temp
    y_frac = t_frac;
    y_exp  = t_exp;

    internalRep(x_hide, x_frac, x_gr, x_exp, "first value:");
    internalRep(y_hide, y_frac, y_gr, y_exp, "second value:");
  }

  // Left shift the signficand of y until x's and y's exponents are equal
  while (x_exp != y_exp) {
    cout << "second operand shifted to equalize exponents" << endl;
    y_gr = ((y_frac & 1) << 1) | (y_gr>>1);
    y_frac = (y_frac >> 1) | (y_hide << 3);
    y_hide = 0;
    y_exp++;

    internalRep(y_hide, y_frac, y_gr, y_exp, "second value:");
  }

  // If x's sign bit is set, perform two's complement
  if (x_sign == 1) {
    // Invert bits and add 1 to the least significant bit
    x_frac = ~x_frac & 0xf;
    x_hide = ~x_hide & 0x3;
    x_gr = (~x_gr & 0x3) + 1;

    // Carry if overflow occurs
    if(x_gr > 0x3) {
      x_frac = x_frac + 1;
      x_gr = x_gr & 0x3;
      if(x_frac > 0xf) x_hide = (x_hide + 1) & 0x3;
    }

    cout << "first operand negated" << endl;
    internalRep(x_hide, x_frac, x_gr, x_exp, "first value:");
  }

  // If y's sign bit is set, perform two's complement
  if (y_sign == 1) {
    // Invert bits and add 1 to the least significant bit
    y_frac = ~y_frac & 0xf;
    y_hide = ~y_hide & 0x3;
    y_gr = (~y_gr & 0x3) + 1;

    // Carry if overflow occurs
    if(y_gr > 0x3) {
      y_frac = y_frac + 1;
      y_gr = y_gr & 0x3;
      if(y_frac > 0xf) y_hide = (y_hide + 1) & 0x3;
    }

    cout << "second operand negated" << endl;
    internalRep(y_hide, y_frac, y_gr, y_exp, "second value:");
  }

  // Add significands of x and y
  cout << "addition takes place" << endl;
  internalRep(x_hide, x_frac, x_gr, x_exp, "first value:");
  internalRep(y_hide, y_frac, y_gr, y_exp, "second value:");

  // Flags for overflow and carry
  bool overflow = false;      // True if the second bit of s_hide is set
  bool carry = false;         // True if a bit is carried out of s_hide

  s_exp = x_exp;              // Exponent of sum is same as addends
  s_gr = x_gr + y_gr;         // Add guard bits
  s_frac = x_frac + y_frac;   // Add fraction bits

  if(s_gr > 0x3) {            // If a bit is carried out of s_gr
    s_frac = s_frac + 0x1;    // Add 1 to least significant bit of s_frac
    s_gr = s_gr & 0x3;        // Mask s_gr back to the range 0b00-0b11
  }

  s_hide = x_hide + y_hide;   // Add hidden bits

  if(s_frac > 0xf) {          // If a bit is carried out of s_frac
    s_hide = s_hide + 0x1;    // Add 1 to least significant bit of s_hide
    s_frac = s_frac & 0xf;    // Mask s_frac back to the range 0b0000-0b1111
  }

  // Set overflow flag if s_hide > 0b01
  if(s_hide > 0x1) {
    cout << "overflow set to true" << endl;
    overflow = true;
  }

  // Set carry flag if s_hide > 0b11
  if(s_hide > 0x3) {
    cout << "carry set to true" << endl;
    carry = true;
    s_hide = s_hide & 0x3;
  }

  // Tentatively set sign bit of sum to most significant bit of s_hide
  s_sign = (s_hide >> 1) & 0x1;

  internalRep(s_hide, s_frac, s_gr, s_exp, "sum:");

  if(carry && overflow) {
    cout << "sum negated" << endl;
    s_frac = ~s_frac & 0xf;
    s_hide = ~s_hide & 0x3;
    s_gr = (~s_gr & 0x3) + 1;

    if(s_gr > 0x3) {
      s_frac = s_frac + 1;
      s_gr = s_gr & 0x3;
    }

    if(s_frac > 0xf) {
      s_hide = (s_hide + 1) & 0x3;
    }

    s_frac = s_frac & 0xf;

    internalRep(s_hide, s_frac, s_gr, s_exp, "sum:");
  }

  if(!carry && overflow) {
    cout << "sum normalized" << endl;

    while ((s_hide & 0x1) < 0x1) {
      s_gr = ((s_frac & 1) << 1) | (s_gr>>1);
      s_frac = (s_frac >> 1) | (s_hide & 0x1);
      s_hide = (s_hide >> 1) & 0x3;
      s_exp++;
    }

    internalRep(s_hide, s_frac, s_gr, s_exp, "sum:");

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
    internalRep(s_hide, s_frac, s_gr, s_exp, "sum:");
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
    internalRep(s_hide, s_frac, s_gr, s_exp, "sum:");
  }

  cout << "hidden bit---------------------^ ffff gr" << endl;
  cout << "4-bit fraction-------------------^^^^" << endl << endl;
  cout << "s_sign is " << toBinary(s_sign, 2) << endl;

  s_val = ((s_hide & 0x1) + (s_frac & 0xf) / 16.0) * pow(2, s_exp);

  s_exp += 4;
  s = s_sign;
    cout << "s is " << toBinary(s, 1) << endl;
  s = (s << 3) + (s_exp);
    cout << "s is " << toBinary(s, 5) << endl;
  s = (s << 4) + s_frac;
    cout << "s is " << toBinary(s, 7) << endl;
  s_val = (s_hide + s_frac / 16.0) * pow(2, s_exp);
  cout << "encoding of returned value:" << setw(11) << toBinary(s, 8) << endl << endl;

  floatingRep(x_sign, x_hide, x_frac, x_exp, x_val, "");
  floatingRep(y_sign, y_hide, y_frac, y_exp, y_val, "added to ");
  floatingRep(s_sign, s_hide, s_frac, s_exp - 4, s_val, "equals ");

  return 0;
}
