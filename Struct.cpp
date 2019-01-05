//Changer la taille du network
#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // get/set directory
#include <math.h>
#include "Struct.hpp"
#include <fstream> //write and read fichier

using namespace std;

vector<string> split(string text , char element)
{
    vector<string> res;
    string t = "";
    for(ulong i = 0; i < text.length(); i++)
    {
        if (text[i] == element)
        {
            res.push_back(t);
            t = "";
        }
        else
        {
            t+= text[i];
        }
    }
    return res;
}

Neural::Neural()
//constructeur de la classe Neural, val de biais random[-20;20], (data,data_save,error) set 0
{
    this->data = 0;
    this->data_save = 0;
    this->error = 0;
    this->biais = (random()%20)-10;
}

Neural::Neural(int nb)
//constructeur de la classe Neural, val de biais random[-20;20], (data,data_save,error) set 0 / nb represente le nombre de poid
{
    this->data = 0;
    this->data_save = 0;
    this->error = 0;
    this->biais = (random()%20)-10;
    int r ;
    for(int i = 0; i<nb; i++) 
    {
        r= (random()%40)-20;
        this->poid.push_back(r);
    } 
}

Neural::Neural(double biais, vector<double> poid)
//constructeur de la classe Neural avec valeur connu
{
    this->biais = biais;
    this->poid = poid;
}

Layer::Layer(int nb)
//Constructeur de la classe Layer avec nb le nombre de neuronne contenu dans cette couche
{
    for(int i = 0; i<nb; i++)
    {
        this->neurone.push_back(Neural());
    }   
}

Layer::Layer(int nb, int nb2) //utiliser celui la
//Constructeur de la classe Layer avec nb le nombre de neuronne contenu dans cette couche
{
    for(int i = 0; i<nb; i++)
    {
        this->neurone.push_back(Neural(nb2));
    }   
}

Layer::Layer(int nb, vector<double> biais, vector<vector<double>> poid)
//Constructeur de la classe Layer avec valeur connu
{
    for(int i = 0; i<nb; i++)
    {
        this->neurone.push_back(Neural(biais[i],poid[i]));
    }   
}

Network::Network(int nb_entrer, int nb_layers, int taille_layer, int nb_sortie)
//Constructeur de la classe, nb_entrer represente le nombre de neuronne d'entrer, nb_layer le nombre de couche hinden
// taille_layer est le nombre de neurone par couche dans les couches interrieur et nb_sortie le nombre de neurone de sortie
{
    this->layer.push_back(Layer(nb_entrer,taille_layer));
    for(int i = 0; i < nb_layers-1;i++)
    {
        this->layer.push_back(Layer(taille_layer,taille_layer));
    }
    if(taille_layer > 0)
    {
        this->layer.push_back(Layer(taille_layer,nb_sortie));
    }
    this->layer.push_back(Layer(nb_sortie));
}

Network::Network(vector<int> l)
//Constructeur de la classe Network avec une list indiquant le nombre de neurone par couche
{
    for(unsigned int i = 0; i < l.size()-1; i++)
    {
        this->layer.push_back(Layer(l[i],l[i+1]));
    }
    if(l.size() > 0)
    {
        this->layer.push_back(Layer(l[l.size()-1]));
    }
}

Network::Network(vector<int> l, vector<vector<double>> biais, vector<vector<vector<double>>> poid) //constructeur de la serialisation
{
    for(unsigned int i = 0; i < l.size(); i++)
    {
        this->layer.push_back(Layer(l[i],biais[i],poid[i]));
    }
}

///////////////////////////////

void putEnter(Network &reseau, vector<double> l)
//Set les entrers du reseau
{
    for(unsigned long i = 0; i < reseau.layer[0].neurone.size(); i++)
    {
        reseau.layer[0].neurone[i].data = l[i];
    }
}

void save(Layer &n)
//sauvegard des valeur des neurone avant d appliquer la sigmoid
{
  for(unsigned int i = 0; i < n.neurone.size();i++)
  {
    n.neurone[i].data_save = n.neurone[i].data ;
    }
}

void propagation_avant(Layer a, Layer &b)
//propagation des donner, 1) multiplication du layer d'avant par les poid puis ajout du biais, 2) sauvage puis application de sigmoid
{
    for(unsigned int i =0; i < b.neurone.size() ;i++)
    {
        for(unsigned int j = 0; j < a.neurone.size(); j ++)
        {
	        b.neurone[i].data += (a.neurone[j].data * a.neurone[j].poid[i]);
        }       
        b.neurone[i].data += b.neurone[i].biais;
    }
    save(b);
    sig_apply(b);
}

void forward_network(Network &res)
//lance une propagation entre chaque couche du reseau
{
    if(res.layer.size() > 0)
    {
        for(unsigned long i = 0; i < res.layer.size()-1;i++)
        {
            propagation_avant(res.layer[i], res.layer[i+1]);
        }
    }
}

///////////////////////////////////////////////////////////////

double sigmoid(double x)
//fonction sigmoid
{
  return 1 / (1 + exp(-x));
}

void sig_apply(Layer &a)
//Application de la fonction sigmoid sur tout les neurone d un layer
{
    for(unsigned long i = 0; i<a.neurone.size(); i++)
    {
        a.neurone[i].data = sigmoid(a.neurone[i].data);
    }
}
 double deriver_sigmoid(double x)
 //deriver de la fonction
{
    return sigmoid(x) * (1 - sigmoid(x));
}
//////////////////////////////////////////////////////////////

void puterror(Network &res, vector<double>  sortie)
//set les erreur pour un reseau perceptron
{
    for(unsigned long i = 0; i < res.layer[res.layer.size()-1].neurone.size() ; i++)
    {
        res.layer[res.layer.size()-1].neurone[i].error = sortie[i] - res.layer[res.layer.size()-1].neurone[i].data  ;
    }
}

void corr(Network &res)
{
    double deriv;
    double valErr;
    unsigned long size = res.layer.size();
    for(unsigned long i =0; i< size - 1 ; i++) // pour chaque couche 
    {
        for(unsigned int x =0; x< res.layer[size - 2 - i].neurone.size(); x++) // neurone x d'une couche i
            {
                double s = 0;

                for(unsigned int h =0; h < res.layer[size - 1 - i].neurone.size(); h++) // neurone h de la couche i+1
                {
                     s+= res.layer[size-1-i].neurone[h].error * res.layer[size-2-i].neurone[x].poid[h];
                }
                deriv = deriver_sigmoid(res.layer[size-2-i].neurone[x].data_save);
                res.layer[size-2-i].neurone[x].error= deriv * s;
                res.layer[size-2-i].neurone[x].biais += res.layer[size-2-i].neurone[x].error;
                
            }
        for(unsigned int x =0; x< res.layer[size - 2 - i].neurone.size(); x++) 
        {
            for(unsigned int j =0; j< res.layer[size-1-i].neurone.size() ; j++)
                {
                    valErr = res.layer[size-1-i].neurone[j].error;
                    res.layer[size-2-i].neurone[x].poid[j]+= res.layer[size-2-i].neurone[x].data * valErr;
                }
        }
    }
}

///////////////////////////////////////////////////////////////

double errtaux(Network reseau)
{
    long size = reseau.layer.size();
    double res= 0;
    unsigned int i;
    for(i = 0; i < reseau.layer[size-1].neurone.size() ; i++)
    {
        res += abs(reseau.layer[size-1].neurone[i].error);
    }
    return res / (double)(i);
}

double neural_network_training (Network &res, vector<double> entrer, vector<double> sortie)
{
  putEnter(res, entrer);
  forward_network(res);
  puterror(res, sortie);
  double terr = errtaux(res);
  corr(res);
  return terr;
}

///////////////////////////////////////////////////////////////
void start(Network res, vector<double> entrer)
{
    putEnter(res, entrer);
    forward_network(res);  
    affich_start(res,entrer) ;
}

int training(Network &res, vector<vector<double>> entrer, vector<vector<double>> sortie)
{
  double s= 0;
  double i = 0;
  double val_abs;
  int r;
  while(i < 100)
  {
    r = random()%(entrer.size());
    val_abs = neural_network_training(res, entrer[r] ,sortie[r]);
    if(val_abs< 0.005 )
    {
        s+=1;
    } 
    i++;
  }
  return s/((double)(i))*100 ;
}

///////////////////////////////////////////////////////////////
void printP(Layer a)
{
    for(unsigned long i = 0; i < a.neurone.size(); i++)
    {
        cout << a.neurone[i].data  << "["<< a.neurone[i].error<<"]| " ;
    }
    cout << endl;
    for(unsigned long i = 0; i < a.neurone.size(); i++) //2
    {
        if(a.neurone[i].poid.size() != 0)
        {
            cout << "n" << i << " -> ";
        }
        for(unsigned long j = 0; j < a.neurone[i].poid.size(); j++) //3
        {
            cout << a.neurone[i].poid[j] << " | " ;
        }
                if(a.neurone[i].poid.size() != 0)
        {
            cout << endl;
        }
    }
}

void pprintP(Network reseau)
{
    cout << "######################## verif ####################### " << endl;
    for(unsigned long i = 0; i < reseau.layer.size() ; i++)
    {
        printP(reseau.layer[i]);
    }
    cout << "###################################################### " << endl;
}

void print_data(Layer a)
{
    for(unsigned long i = 0; i < a.neurone.size(); i++)
    {
        cout << a.neurone[i].data << " | " ;
    }
    cout << endl;
}

void pprint_data(Network reseau)
{
    cout << " print data " << endl;
    for(unsigned long i = 0; i < reseau.layer.size() ; i++)
    {
        print_data(reseau.layer[i]);
    }
}

void print_error(Layer a)
{
    for(unsigned long i = 0; i < a.neurone.size(); i++)
    {
        cout << a.neurone[i].error << " | " ;
    }
    cout << endl;
}

void pprint_error(Network reseau)
{
    cout << "print error " << endl;
    for(unsigned long i = 0; i < reseau.layer.size() ; i++)
    {
        print_error(reseau.layer[i]);
    }
}

void print_biais(Layer a)
{
    for(unsigned long i = 0; i < a.neurone.size(); i++)
    {
        cout << a.neurone[i].biais << " | " ;
    }
    cout << endl;
}

void pprint_biais(Network reseau)
{
    cout << "print biais " << endl;
    for(unsigned long i = 0; i < reseau.layer.size() ; i++)
    {
        print_biais(reseau.layer[i]);
    }
}

void affichage(Network res, vector<vector<double>> entrer, vector<vector<double>> sortie)
{
    cout << "################################RESULTAT###############################"<<endl;
    cout << "entrer                    obtenu                      attendu"<<endl;
    for(unsigned int i = 0; i < entrer.size(); i++)
    {
        
        for(unsigned int j = 0; j < entrer[i].size()-1; j++)
        {
            cout << entrer[i][j] << "   ";
        }
        cout << entrer[i][entrer[i].size() -1]<< "                   ";
        putEnter(res,entrer[i]);
        forward_network(res);
        for(unsigned int j = 0; j < sortie[i].size(); j++)
        {
            cout << res.layer[res.layer.size() - 1].neurone[j].data << "                   ";
        }
        for(unsigned int j = 0; j < sortie[i].size() ; j++)
        {
            cout << double(sortie[i][j]) << " ";
        }
        cout << endl;
    }
    cout << "#######################################################################"<<endl;
}

void affich_start(Network res, vector<double> entrer)
{
    cout << "################################RESULTAT###############################"<<endl;
    cout << "entrer                    obtenu                      "<<endl;
    for(unsigned int i = 0; i < entrer.size(); i++)
    {
        cout << entrer[i] << "   ";
        for(unsigned int j = 0; j < res.layer[res.layer.size()-1].neurone.size(); j++)
        {
            cout << res.layer[res.layer.size() - 1].neurone[j].data << "                   ";
        }
        cout << endl;
    }
    cout << "#######################################################################"<<endl;
}
///////////////////////////////////////////////////////////////SERIALIZATION
string get_data(string path_source)
{
    ifstream fichier(path_source,ios::in);
    string result;
    if(fichier)
    {
        string line;
        while(getline(fichier, line))
        {
            result = result +"\n"+ line;
        }
        fichier.close();
    }
    else
    {
        cerr << "Can't open the file" << endl;
    }
    return result;
}
void put_data(string text,string path_destination)
{
    ofstream w_fichier(path_destination,ios::out | ios::trunc);

    w_fichier << text << endl;
    w_fichier.close();
}

string get_current()
{
    return get_current_dir_name();
}
void set_current(string path)
{
    const char* p = path.c_str();
    chdir(p);
}

void savenetwork(Network res)
{
    string text = "";
    text +=to_string(res.layer.size()) + "\n";

    for(uint i = 0; i < res.layer.size(); i++)
    {
        text += to_string(res.layer[i].neurone.size()) + "\n";

        for(uint j = 0; j < res.layer[i].neurone.size(); j++)
        {
            text += to_string(res.layer[i].neurone[j].biais) + "\n";

            for(uint h = 0; h < res.layer[i].neurone[j].poid.size() ; h++)
            {
                text += to_string(res.layer[i].neurone[j].poid[h]) + "|";
            }
            text+="\n";
        }
    }
    put_data(text, get_current() +"/save");
}

Network get_save()
{
    string data = get_data(get_current()+"/save");
    vector<string> datalist = split(data , '\n');
    vector<int> l;
    int size = stoi(datalist[0]);
    for(uint i = 0; i < size; i++)
    {
        
    }
}