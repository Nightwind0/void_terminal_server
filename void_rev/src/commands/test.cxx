#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "../void_util.h"
#include <math.h>

using namespace std;

double g_rand();

double g_random(int missiles)
{
    double total = 0;
    for(int i=0;i<missiles;i++)
    {
	total += g_rand() *1.75   + 2;
    }

    return total;
}

double g_rand()
{    
    double x1, x2, w, y1, y2;
    
    do {
	x1 = 2.0 * ((double)random() / (double)RAND_MAX) - 1.0;
	x2 = 2.0 * ((double)random() / (double)RAND_MAX) - 1.0;
	w = x1 * x1 + x2 * x2;

    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    
    
    return y1;
}

int main()
{

    srandom(time(NULL));

    ostringstream os;
    os.fill(' ');

    unsigned int attack;

    while(cin >> attack)
    {
	double g = g_rand() * 5 + (attack *2);
	int dam = (int)trunc(g);

	cout << dam << endl;
	cout << trunc(g_random(attack)) << endl;
    }

#if 0

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

#endif
}
