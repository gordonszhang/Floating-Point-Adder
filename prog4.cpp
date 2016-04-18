/*
 *  Gordon Zhang (gordonz) John McElvenny (jlmcelv)
 *  CPSC 2310 Section 01
 *  Program 4
 *  Due 4/14/16
 *
 *  "prog4.cpp"
 *  Given two integer encodings of 8-bit floating point numbers (between 0-255)
 *  through command line input, shows the steps required to add the floating
 *  point values. In addition, prints the values of the 8-bit floating point
 *  addends as well as their sum.
 */

#include <iostream>  // C++ input/ouput library
#include <iomanip>   // C++ output formatting library
#include <string>    // C++ string library

using namespace std;

/*  string toBinary(int, int)
 *  Description: Converts an integer to its binary represntation in a
 *  user-defined number of bits and returns it as a C++ string
 */
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

/*  double toFloat(unsign int, bool)
 *  Description: Converts a significand stored in an unsigned integer to its
 *  floating point value. Returns that value as a double
 */
double toFloat(unsigned int sig, bool negative) {
  if(sig == 0) return 0.0;
  return (((sig >> 2) & 0xF) / 16.0 + 1) * (negative ? -1 : 1);
}

/*  void invert(unsigned int, bool)
 *  Description: Performs two's complement on a significand (stored in unsigned
 *  integer) passed by reference. In addition, flips the negative flag
 *  associated with the significand.
 */
void invert(unsigned int &sig, bool& negative) {
  sig = ((~sig) & 0xFF) + 1;  // Two's complement is saved over original var
  negative = !negative;
}

/*  void internalRep(unsigned int, int, bool, string)
 *  Description: Extracts the 8-bit floating point internal representation
 *  from a passed significand and prints it out with formatting to cout
 */
void internalRep(unsigned int sig, int exp, bool extra, string type) {
  unsigned int hide = (sig >> 6) & 0x3;
  unsigned int frac = (sig >> 2) & 0xF;
  unsigned int gr = sig & 0x3;

  cout << "internal rep of " << left << showpos << setfill(' ')
       << setw(14 - extra) << type << toBinary(hide, (extra ? 2 : 1)) << "."
       << toBinary(frac, 4) << " " << toBinary(gr, 2) << " x 2^(" << exp << ")"
       << endl;
}

/*  void roundEven(unsigned int, int)
 *  Description: If the guard bit of a passed signficand is set, rounds up
 *  (when fraction is odd) or down (when fraction is already even). Otherwise,
 *  does nothing
 */
void roundEven(unsigned int &sig, int &exp) {
  if((sig & 0x3) >= 0x2) {  // Mask for guard bits and compare > 0b10
    if((sig >> 2) & 0x1) sig = sig + 0x4; // If fraction is odd, increment it
    cout << "sum rounded" << endl;
    internalRep(sig, exp, true, "sum:"); // Print resultant representation
  }
}

/*  void normalize(unsigned int, int)
 *  Description: If the significand's hidden bit is not 1, shifts left or right
 *  depending on whether it is too small or too large, respectively
 */
void normalize(unsigned int &sig, int &exp) {
  bool normalized = false;
  while (sig >= 0x80) {       // Check significand > 0b10000000 (too large)
    sig = (sig & 0xFF) >> 1;  // Shift bits right
    exp++;                    // Increment exponent
    normalized = true;
  }
  while (sig < 0x40) {        // Check significand < 0b1000000 (too small)
    sig = sig << 1;           // Shift bits left
    exp--;                    // Decrement exponent
    normalized = true;
  }
  if (normalized) {
    cout << "sum normalized" << endl;
    internalRep(sig, exp, true, "sum:");
  }
}

/*  bool shift(unsigned int, int, int)
 *  Description: Shifts exponent of a significand to match another significand's
 *  exponent. Precondition is that target's exponent is larger than sig's
 */
bool shift(unsigned int &sig, int &exp, int target) {
  if (exp == target) return true;
  cout << "second operand shifted to equalize exponents" << endl;
  exp++;
  sig = sig >> 1;
  internalRep(sig, exp, false, "second value:");
  return false;
}

/*  void encoding(unsigned int, string)
 *  Description: Prints out the binary encoding of a passed integer, with
 *  formatting
 */
void encoding(unsigned int i, string type) {
  cout << "encoding of " << type << right << setw(10) << toBinary(i, 8) << endl;
}

/*  void floatingRep(double, int, string)
 *  Description: Prints out a decimal significand in scientific notation
 *  along with its exponent
 */
void floatingRep(double val, int exp, string type) {
  double fin = val;
  if(exp > 0) for(int i = 0; i < exp; i++) fin = fin * 2;
  else for (int i = 0; i > exp; i--) fin = fin / 2;
  cout << right << setfill(' ') << setw(16) << type << " " << showpos
       << left << setw(7) << setfill('0') << val << " x 2^("
       << exp << ") = " << fin << endl;
}

/*  void extract(unsigned int, bool, int, unsigned int)
 *  Description: Extracts the appropriate bits for a binary significand from
 *  an 8-bit binary encoding and stores them into a corresponding signficand
 */
void extract(unsigned int value, bool &negative, int &exp, unsigned int &sig) {
  negative = (value >> 7) & 0x1;        // sign      -> 1 bit
  sig = ((value & 0xF) | 0x10) << 2;    // hidden    -> 1 bit (2 for sum)
                                        // fraction  -> 4 bits
  exp = ((value >> 4) & 0x7) - 4;       // exponent  -> 3 bits
                                        // also, unbias exponent (subract 4)
  if (exp == -4) sig = 0;   // Set significand to 0, if biased exponent = 0
}

int main() {
  // Container variable declarations
  // All but exponent and value are unsigned to simplify comparison logic
  // Ex: 0b1000 > 0b0111
  unsigned int x, y, s = 256;         // Inputted integers, 0-255
  unsigned int x_sig, y_sig, s_sig;   // Floating-point significands
  bool x_neg, y_neg, s_neg;           // Negative flags, if negative then true
  int x_exp, y_exp, x_oexp, y_oexp, s_exp, s_oexp;  // Exponents
  double x_val, y_val, s_val;         // Decimal values

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
  extract(x, x_neg, x_exp, x_sig);
  extract(y, y_neg, y_exp, y_sig);

  // Calculate floating point values of x and y
  x_val = toFloat(x_sig, x_neg);
  y_val = toFloat(y_sig, y_neg);

  // If original exponent was 0, set value and exponent to 0
  if(x_exp == -4) {
    x_val = 0;
    x_exp = 0;
    x_neg = false;
  }
  if(y_exp == -4) {
    y_val = 0;
    y_exp = 0;
    y_neg = false;
  }

  // Save original exponents for future output
  x_oexp = x_exp;
  y_oexp = y_exp;

  // Display internal representations of x and y
  internalRep(x_sig, x_exp, false, "first value:");
  internalRep(y_sig, y_exp, false, "second value:");

  cout << "hidden bit---------------------^ ffff gr" << endl;
  cout << "4-bit fraction-------------------^^^^" << endl << endl;

  /* Begin floating-point addition */
  cout << endl << "addition of the values: " << endl;

  // If x's exponent is greater than y's, swap all variables
  if (y_exp > x_exp) {
    cout << "operands are swapped" << endl;
    unsigned int t_sig;
    int t_exp;
    bool t_neg;

    t_sig = x_sig;  // Store temp = x
    t_exp  = x_exp;
    t_neg = x_neg;

    x_sig = y_sig;  // Load x = y
    x_neg = y_neg;
    x_exp  = y_exp;

    y_sig = t_sig;  // Load y = temp
    y_neg = t_neg;
    y_exp  = t_exp;

    internalRep(x_sig, x_exp, false, "first value:");
    internalRep(y_sig, y_exp, false, "second value:");
  }

  // Left shift the signficand of y until x's and y's exponents are equal
  while (!shift(y_sig, y_exp, x_exp));


  bool negated = false;

  if (x_neg != y_neg) {
    // If x's sign bit is set, perform two's complement
    if (x_neg) {
      invert(x_sig, x_neg);
      cout << "first operand negated" << endl;
      internalRep(x_sig, x_exp, false, "first value:");
      negated = true;
    }
    // If y's sign bit is set, perform two's complement
    if (y_neg) {
      invert(y_sig, y_neg);
      cout << "second operand negated" << endl;
      internalRep(y_sig, y_exp, false, "second value:");
      negated = true;
    }
    s_neg = false;
  }
  // If both signs are equal, sum shares that sign
  else s_neg = y_neg;

  // Add significands of x and y
  cout << "addition takes place" << endl;
  internalRep(x_sig, x_exp, false, "first value:");
  internalRep(y_sig, y_exp, false, "second value:");

  s_sig = (x_sig + y_sig) & 0xFF;         // Add significands
  s_exp = y_exp;                          // Sum's exponent is same as addends'
  s_oexp = s_exp;                         // Save sum's exponent
  internalRep(s_sig, s_exp, true, "sum:");

  // If one of the addends was negated and addition results in overflow,
  // negate the sum
  if(negated && s_sig >= 0x80) {
    cout << "sum negated" << endl;
    invert(s_sig, s_neg);
    internalRep(y_sig, y_exp, true, "sum:");
  }

  if(s_sig != 0) {           // Ignore if sum exponent = 0
  normalize(s_sig, s_exp);   // Normalize the sum (shift), as needed
  roundEven(s_sig, s_exp);   // Round if guard bit is set
  normalize(s_sig, s_exp);   // Normalize to ensure significand is 01.XXXX
  s_oexp = s_exp;            // Save sum's original exponent
  s_exp += 4;                // Re-bias exponent
  }

  s_val = toFloat(s_sig, s_neg);  // Calculate the floating point value of sum
  cout << "hidden bit---------------------^ ffff gr" << endl;
  cout << "4-bit fraction-------------------^^^^" << endl << endl;

  // Assemble the binary encoding of sum
  s = ((s_sig >> 2) & 0xF) + (((s_exp) & 0x7) << 4) + ((s_neg ? 1 : 0) << 7);
  encoding(s, "returned value:");

  // Display the floating point addends and the resultant sum
  floatingRep(x_val, x_oexp, "");
  floatingRep(y_val, y_oexp, "added to");
  floatingRep(s_val, s_oexp, "equals");

  return 0;
}
