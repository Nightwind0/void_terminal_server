#include <iostream>
#include <sstream>
#include "../void_util.h"

using namespace std;

int main()
{


    ostringstream os;
    os.fill(' ');


    os << Color()->get(BLUE);
    os.width(20);
    os << left<< "Turns Per Sector";

    os << Color()->get(LIGHTBLUE);
    os.width(4);
    os << right << 12;
    os << ' ';
    os.width(20);
    os << left << "Cost";
    os.width(7);
    os  << right<< 50000 << ' ';
    os.width(20);
    os << left << "Blahblah";
    os.width(6);
    os << right << 123456 ;

    cout << os.str() << endl;


}
