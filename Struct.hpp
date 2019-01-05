# ifndef __STRUCT_HPP_
# define __STRUCT_HPP_

#include <string>
#include <iostream>
#include <vector>


using namespace std;

class Neural
{
public:
    double data;
    double data_save;
    double error;
    double biais;
    vector<double> poid;
    Neural();
    Neural(int nb);
    Neural(double biais, vector<double> poid); //Constructeur pour la serialisation
};

class Layer
{
public:
    Layer(int nb);
    Layer(int nb, int nb2);
    Layer(int nb, vector<double> biais, vector<vector<double>> poid);//Constructeur pour la serialisation
    vector<Neural> neurone;
};

class Network
{
public:
    vector<Layer> layer;
    Network(vector<int> l);
    Network(int nb_entrer, int nb_layers, int taille_layer, int nb_sortie);
    Network(vector<int> l, vector<vector<double>> biais, vector<vector<vector<double>>> poid); //constructeur de la serialisation
};

//////////////////////////////
void putEnter(Network &reseau, vector<double> l);
void propagation_avant(Layer a, Layer &b);
void forward_network(Network &res);
void save(Layer &n);
///////////////////////////////
double sigmoid(double x);
void sig_apply(Layer &a);
double deriver_sigmoid(double x);
//////////////////////////////////////////////////////////////
void puterror(Network &res, vector<double>  sortie);
void corr(Network &res);
///////////////////////////////////////////////////////////////
double errtaux(Network reseau);
double neural_network_training (Network &res, vector<double> entrer, vector<double> sortie);
////////////////////////////////
void start(Network res, vector<double> entrer);
int training(Network &res, vector<vector<double>> entrer, vector<vector<double>> sortie);
////////////////////////////////
void printP(Layer a);
void pprintP(Network reseau);
void print(Layer a);
void pprint_data(Network reseau);
void print_data(Layer a);
void print_error(Layer a);
void pprint_error(Network reseau);
void print_biais(Layer a);
void pprint_biais(Network reseau);
void affichage(Network res, vector<vector<double>> entrer, vector<vector<double>> sortie);
void affich_start(Network res, vector<double> entrer);
////////////////////////////////
string get_data(string path_source);
void put_data(string text,string path_destination);
string get_current();
void set_current(string path);
void savenetwork(Network res);

#endif
