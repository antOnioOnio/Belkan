#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
  
};

struct Nodo{
  estado state;
  int g;
  int h;
  int total;
  int indicePadre;
  Action accion;

};



class ComportamientoJugador : public Comportamiento {
private:

  void printSensores(const Sensores &sensores);

  public:


    
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);

    bool sePuedeIr(int fila, int columna);

    int interact(Action accion, int valor);

    void VisualizaPlan(const estado &st, const list<Action> &plan);

    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

    //int encontrarMinimo(const vector<Nodo> &abiertos);

    void generaEstado(Nodo &node ,Action accion, const estado &destino);

    void meterEnAbiertos(vector<Nodo> &abiertos,vector<Nodo>&cerrados,  Nodo &node);

    void generaPlanAldeano(estado &actual, int posAldeano);

    void actualizaPosicion(Action accion);

    void inicializaMapaRepulsion();

    bool compruebaNivel3();

    Action llenaPlanRepulsion(estado &actual, Sensores &sensores);

    void actualizaPosRepulsion(estado &actual, Action accion);

    void actualizaTerreno(estado &actual, Sensores &sensores);
        private:
    // Declarar Variables de Estado
    const int filasMatriz = 200;
    const int columnasMatriz = 200;
    int fil;
    int col;
    int filaCtual;
    int colActual;
    int brujula=0;
    int filaRe ;
    int coluRe ;
    bool primera = true;
    int ** mapaRepulsion = new int*[filasMatriz];
    estado destino, origen;
    list<Action> plan;
    list<Action> planAldeano ;
    list<Action> planMinima;
    vector<estado> vecinos;
    int contador = 0;
    bool primeraLVL = true;
    bool primeraRepu = true;
    bool pri= true;
    bool yaConozco = false;
    bool hayPlan = false;
    bool pathFinding(const estado &origen, const estado &destino, list<Action> &plan);
    void PintaPlan(list<Action> plan);
    bool encontrado = false;
    bool BuscaDestino = true;
    bool conozcoK = false;
    bool recalcular = false;
};



#endif
