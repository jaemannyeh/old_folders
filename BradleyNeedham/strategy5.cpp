
#include <iostream>

struct Robot {
    struct Search {
        virtual void apply(Robot& r) = 0;
        virtual ~Search() = default;
    };
    struct Attack {
        virtual void apply(Robot& r) = 0;
        virtual ~Attack() = default;
    };
    struct Defend {
        virtual void apply(Robot& r) = 0;
        virtual ~Defend() = default;
    };
    Robot(Search* search, Attack* attack, Defend* defend) :
    m_search(search), m_attack(attack), m_defend(defend) {}
    void search() { m_search->apply(*this); }
    void attack() { m_attack->apply(*this); }
    void defend() { m_defend->apply(*this); }
private:
    Search* m_search;
    Attack* m_attack;
    Defend* m_defend;
};

struct Linear : Robot::Search {
    virtual void apply(Robot& r) { std::cout << "Linear\n"; }
};

struct Spiral : Robot::Search {
    virtual void apply(Robot& r) { std::cout << "Spiral\n"; }
};

struct Pinch : Robot::Attack {
    virtual void apply(Robot& r) { std::cout << "Pinch\n"; }
};

struct Punch : Robot::Attack {
    virtual void apply(Robot& r) { std::cout << "Punch\n"; }
};

struct Run : Robot::Defend {
    virtual void apply(Robot& r) { std::cout << "Run\n"; }
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
    
    Robot r1(&linear,&pinch,&run);
    Robot r2(&spiral,&punch,&run);

    go(r1);
    std::cout << '\n';
    go(r2);
    
    return 0;
}

// g++ -std=c++11 strategy5.cpp