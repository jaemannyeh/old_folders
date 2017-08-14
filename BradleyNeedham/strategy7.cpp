
#include <iostream>
#include <functional>

struct Robot {
    using Search = std::function<void(Robot&)>;
    using Attack = std::function<void(Robot&)>;
    using Defend = std::function<void(Robot&)>;
    Robot(Search search, Attack attack, Defend defend) :
    m_search(search), m_attack(attack), m_defend(defend) {}
    void search() { m_search(*this); }
    void attack() { m_attack(*this); }
    void defend() { m_defend(*this); }
private:
    Search m_search;
    Attack m_attack;
    Defend m_defend;
};

struct Linear : Robot::Search {
    void operator()(Robot& r) { std::cout << "Linear\n"; }
};

struct Spiral : Robot::Search {
    void operator()(Robot& r) { std::cout << "Spiral\n"; }
};

struct Pinch : Robot::Attack {
    void operator()(Robot& r) { std::cout << "Pinch\n"; }
};

struct Punch : Robot::Attack {
    void operator()(Robot& r) { std::cout << "Punch\n"; }
};

struct Run : Robot::Defend {
    void operator()(Robot& r) { std::cout << "Run\n"; }
};

void go(Robot& r) {
    r.search();
    r.attack();
    r.defend();
}

int main() {
    Linear linear;
    Spiral spiral;
    Pinch pinch;
    Punch punch;
    Run run;
    
    Robot r1(linear,pinch,[](Robot& r) { std::cout << "Hide\n"; });
    Robot r2(spiral,punch,run);

    go(r1);
    std::cout << '\n';
    go(r2);
    
    return 0;
}

// g++ -std=c++11 strategy5.cpp