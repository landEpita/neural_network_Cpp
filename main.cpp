#include <iostream>
#include "Struct.hpp"

int main()
{
    Network res = Network(2,1,4,1);
    vector<vector<double>> enter;
    for(int i = 0; i < 4 ; i++)
    {
        vector<double> a (2);
        enter.push_back(a);
    }                                       // 0 et 0 -> 0
    enter[1][1] = 1;                        // 0 et 1 -> 1
    enter[2][0] = 1;                        // 1 et 0 -> 1
    enter[3][0] = 1;                        
    enter[3][1] = 1;                        // 1 et 1 -> 1

    vector<vector<double>> sortie;
    for(int i = 0; i < 4 ; i++)
    {
        vector<double> a;
        a.push_back(0);
        sortie.push_back(a);
    }
    sortie[0][0] = 0;
    sortie[1][0] = 1;
    sortie[2][0] = 1;
    sortie[3][0] = 0;

    double note = 0;
    long v = 0;
    while(note < 90)
    {
        note = training(res,enter,sortie);
        cout << note << endl;
        v++;
    }
    cout << v*100 << endl;
    affichage(res, enter , sortie);
    savenetwork(res);
    return 1;
}