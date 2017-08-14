
#include <iostream>
#include <string.h>

using namespace std;

class Thousand;
class Hundred;
class Ten;
class One;

class RNInterpreter {
public:    
    RNInterpreter(); // ctor for client
    RNInterpreter(int) {} // ctor for subclasses, avoids infinite loop
    int interpret(char*); // interpret() for client
    virtual void interpret(char *input, int &total) {
    }
protected:
    // cannot be pure virtual because client asks for instance
    virtual const char one() {}
    virtual const char *four() {}
    virtual const char five() {}
    virtual const char *nine() {}
    virtual const int multiplier() {}    
private:
    RNInterpreter *thousands;
    RNInterpreter *hundreds;
    RNInterpreter *tens;
    RNInterpreter *ones;
};

class Thousand: public RNInterpreter {
public:
    Thousand(int): RNInterpreter(1) {}
};

class Hundred: public RNInterpreter {
public:
    Hundred(int): RNInterpreter(1) {}
};

class Ten: public RNInterpreter {
public:
    Ten(int): RNInterpreter(1) {}
};

class One: public RNInterpreter {
public:
    One(int): RNInterpreter(1) {}
protected:    
    const char one() { return 'I'; }
    const char *four() { return "IV"; }
    const char five() { return 'V'; }
    const char *nine() { return "IX"; }
    const int multiplier() { return 1; }    
};

RNInterpreter::RNInterpreter() {
    // use 1-arg ctor to avoid infinite loop
    thousands = new Thousand(1);
    hundreds = new Hundred(1);
    tens = new Ten(1);
    ones = new One(1);
}

int RNInterpreter::interpret(char *input) {
    int total;
    thousands->interpret(input, total);
    hundreds->interpret(input, total);
    tens->interpret(input, total);
    ones->interpret(input, total);
    if (strcmp(input, ""))  // if input was invalid, return 0
        return 0;
    return total;
}

int main() {
  RNInterpreter interpreter;
  char input[20];
  cout << "Enter Roman Numeral: ";
  while (cin >> input) {
    cout << "   interpretation is " << interpreter.interpret(input) << endl;
    cout << "Enter Roman Numeral: ";
  }
}

