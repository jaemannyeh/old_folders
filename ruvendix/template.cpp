
// g++ template.cpp 2>&1 | more
// http://ruvendix.blog.me/220950210666

#include <stdio.h>
#include <iostream>

//////// //////// //////// ////////

template <class DataType>
DataType add_010(DataType Arg1, DataType Arg2);

int main_010(void) {
    std::cout << __FUNCTION__ << std::endl;
    printf("%d\n",add_010(__LINE__,__LINE__));
    printf("%f\n",add_010(0.1,0.2));
    return 0;
}

template <class DataType>
DataType add_010(DataType Arg1, DataType Arg2) {
    return (Arg1+Arg2);
}

//////// //////// //////// ////////

template <typename T>
T add_020(T Arg1, T Arg2) {
    return (Arg1+Arg2);
}

template char add_020(char,char);
template int add_020<>(int,int);
template float add_020<float>(float,float);

int main_020(void) {
    std::cout << __FUNCTION__ << std::endl;
    printf("%d\n",add_020(__LINE__,__LINE__));
    printf("%f\n",add_020(0.1,0.2));
    printf("%f\n",add_020<double>((double)0.3,(double)0.4));
    return 0;
}

//////// //////// //////// ////////

template <typename T>
void print_data_030(const T &rData) {
    std::cout << rData << std::endl;    
}

class CHouse {
    friend std::ostream & operator << (std::ostream &rCout, const CHouse &rHouse);
private:    
    int x;
    int y;
public:    
    CHouse(void) : x(__LINE__), y(__LINE__) {}
    CHouse(int ix, int iy) : x(ix), y(iy) {}
    virtual ~CHouse() {}
};

std::ostream & operator << (std::ostream &rCout, const CHouse &rHouse) {
    std::cout << rHouse.x << " " << rHouse.y;
    return rCout;
}

int main_030(void) {
    std::cout << __FUNCTION__ << std::endl;
    print_data_030(__LINE__);
    print_data_030(0.5);
    print_data_030<double>(0.6);
    CHouse MyHouse(__LINE__,__LINE__);
    print_data_030(MyHouse);
    return 0;
}

//////// //////// //////// ////////

template <typename T> void print_data_040(const T &rData) {
    std::cout << rData << std::endl;    
}

class CLot {
    friend void print_data_040(const CLot &rLot);
private:
    int x;
    int y;
public:
    CLot(void) : x(__LINE__), y(__LINE__) {}
    CLot(int ix,int iy) : x(ix), y(iy) {}
    virtual ~CLot() {};
};

void print_data_040(const CLot &rLot) {
    std::cout << rLot.x << " " << rLot.y << std::endl;
}

int main_040(void) {
    std::cout << __FUNCTION__ << std::endl;
    print_data_040(__LINE__);
    print_data_040(0.5);
    print_data_040<double>(0.6);
    CLot MyLot(__LINE__,__LINE__);
    print_data_040(MyLot);
    return 0;
}

//////// //////// //////// ////////

int main(void) {
    main_010();
    main_020();
    main_030();
    main_040();    
}
