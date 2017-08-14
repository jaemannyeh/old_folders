
#include <iostream>

struct Robot {
    virtual void search() = 0;
    virtual void attack() = 0;
    virtual void defend() = 0;
    virtual ~Robot() = default;
};

struct Linear : Robot {
    virtual void search() { std::cout << "Linear\n"; }
};

struct LinearPinch : Linear {
    virtual void attack() { std::cout << "Pinch\n"; }
};

struct LinearPunch : Linear {
    virtual void attack() { std::cout << "Punch\n"; }
};

struct LinearPinchRun : LinearPinch {
    virtual void defend() { std::cout << "Run\n"; }
};

struct LinearPunchRun : LinearPunch {
    virtual void defend() { std::cout << "Run\n"; }
};

struct Spiral : Robot {
    virtual void search() { std::cout << "Spiral\n"; }
};

struct SpiralPinch : Spiral {
    virtual void attack() { std::cout << "Pinch\n"; }
};

struct SpiralPinchRun : SpiralPinch {
    virtual void defend() { std::cout << "Run\n"; }
};

void go(Robot& r) {
    r.search();
    r.attack();
    r.defend();
}

int main() {
    //LinearPinchRun r1;
    LinearPunchRun r1;
    SpiralPinchRun r2;
    
    go(r1);
    std::cout << '\n';
    go(r2);
    
    return 0;
}

// g++ -std=c++11 strategy2.cpp