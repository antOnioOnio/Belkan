#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <stack>
#include <vector>

void ComportamientoJugador::PintaPlan(list<Action> plan) {
    auto it = plan.begin();
    while (it != plan.end()) {
        if (*it == actFORWARD) {
            cout << "A ";
        } else if (*it == actTURN_R) {
            cout << "D ";
        } else if (*it == actTURN_L) {
            cout << "I ";
        } else {
            cout << "- ";
        }
        it++;
    }
    cout << endl;
}

void AnularMatriz(vector<vector<unsigned char>> &m) {
    for (int i = 0; i < m[0].size(); i++) {
        for (int j = 0; j < m.size(); j++) {
            m[i][j] = 0;
        }
    }
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan) {
    AnularMatriz(mapaConPlan);
    estado cst = st;

    auto it = plan.begin();
    while (it != plan.end()) {
        if (*it == actFORWARD) {
            switch (cst.orientacion) {
                case 0:
                    cst.fila--;
                    break;
                case 1:
                    cst.columna++;
                    break;
                case 2:
                    cst.fila++;
                    break;
                case 3:
                    cst.columna--;
                    break;
            }
            mapaConPlan[cst.fila][cst.columna] = 1;
        } else if (*it == actTURN_R) {
            cst.orientacion = (cst.orientacion + 1) % 4;
        } else {
            cst.orientacion = (cst.orientacion + 3) % 4;
        }
        it++;
    }
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void CalcularCostoH(Nodo &node, const estado &destino) {

    node.h = abs(node.state.fila - destino.fila) + abs(node.state.columna - destino.columna);
    node.g += 10;
    node.total = node.h + node.g;
}

void ComportamientoJugador::generaEstado(Nodo &node, Action accion, const estado &destino) {

    if (accion == actFORWARD) {
        node.accion = actFORWARD;

        if (node.state.orientacion == 0) {
            node.state.fila -= 1;
        } else if (node.state.orientacion == 1) {
            node.state.columna += 1;
        } else if (node.state.orientacion == 2) {
            node.state.fila += 1;
        } else if (node.state.orientacion == 3) {
            node.state.columna -= 1;
        }
    } else if (accion == actTURN_L) {
        node.accion = actTURN_L;

        if (node.state.orientacion == 0) {
            node.state.orientacion = 3;
        } else if (node.state.orientacion == 1) {
            node.state.orientacion = 0;
        } else if (node.state.orientacion == 2) {
            node.state.orientacion = 1;
        } else if (node.state.orientacion == 3) {
            node.state.orientacion = 2;
        }
    } else if (accion == actTURN_R) {
        node.accion = actTURN_R;

        if (node.state.orientacion == 0) {
            node.state.orientacion = 1;
        } else if (node.state.orientacion == 1) {
            node.state.orientacion = 2;
        } else if (node.state.orientacion == 2) {
            node.state.orientacion = 3;
        } else if (node.state.orientacion == 3) {
            node.state.orientacion = 0;
        }
    }
    CalcularCostoH(node, destino);
}

void ComportamientoJugador::meterEnAbiertos(vector<Nodo> &abiertos, vector<Nodo> &cerrados, Nodo &node) {

    bool estaEncerrados = false;
    bool estaEnabiertos = false;

    for (int i = 0; i < cerrados.size() && !estaEncerrados; i++) {
        if (node.state.fila == cerrados[i].state.fila &&
            node.state.columna == cerrados[i].state.columna &&
            node.state.orientacion == cerrados[i].state.orientacion) {
            estaEncerrados = true;

        }
    }

    if (!estaEncerrados) {

        for (int i = 0; i < abiertos.size() && !estaEnabiertos; i++) {
            if (node.state.fila == abiertos[i].state.fila &&
                node.state.columna == abiertos[i].state.columna &&
                node.state.orientacion == abiertos[i].state.orientacion) {
                if (node.total <= abiertos[i].total) {
                    abiertos[i].total = node.total;
                }
                estaEnabiertos = true;
            }
        }
        if (!estaEnabiertos) {
            if (sePuedeIr(node.state.fila, node.state.columna)) {
                abiertos.push_back(node);
            }
        }
    }
}

void ordenaVector(vector<Nodo> &abiertos) {
    Nodo aux;

    int minimo;
    if (abiertos.size() > 1) {
        for (int i = 0; i < (abiertos.size() - 1); ++i) {
            minimo = i;
            for (int j = i + 1; j < abiertos.size(); ++j) {
                if (abiertos[minimo].total < abiertos[j].total) {
                    minimo = j;
                }
                aux = abiertos[minimo];
                abiertos[minimo] = abiertos[i];
                abiertos[i] = aux;
            }
        }
    }
}

bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {

    plan.clear();

    vector<Nodo> cerrados;
    vector<Nodo> abiertos;
    Nodo primero;
    stack<Action> acciones;
    int indiceCerrados = 0;
    Nodo actual, siguienteForward, siguienteLeft, siguienteRight;

    if ((origen.fila == destino.fila) && (origen.columna == destino.columna)) {

        return true;
    } else {
        primero.state = origen;
        primero.indicePadre = -1;
        primero.accion = actIDLE;
        meterEnAbiertos(abiertos, cerrados, primero);


        actual = abiertos[0];
        int contador = 0;
        while (!abiertos.empty()) {
            if (actual.state.fila == destino.fila && actual.state.columna == destino.columna) {

                int indice_seguir = actual.indicePadre;
                acciones.push(actual.accion);
                while (indice_seguir != -1) {
                    acciones.push(cerrados[indice_seguir].accion);

                    indice_seguir = cerrados[indice_seguir].indicePadre;
                }
                while (!acciones.empty()) {
                    plan.push_back(acciones.top());
                    acciones.pop();
                }
                return true;
            } else {
                cerrados.push_back(actual);
                contador++;
                abiertos.pop_back();


                siguienteForward = actual;
                siguienteLeft = actual;
                siguienteRight = actual;

                generaEstado(siguienteForward, actFORWARD, destino);
                siguienteForward.indicePadre = indiceCerrados;
                meterEnAbiertos(abiertos, cerrados, siguienteForward);

                generaEstado(siguienteLeft, actTURN_L, destino);
                siguienteLeft.indicePadre = indiceCerrados;
                meterEnAbiertos(abiertos, cerrados, siguienteLeft);

                generaEstado(siguienteRight, actTURN_R, destino);
                siguienteRight.indicePadre = indiceCerrados;
                meterEnAbiertos(abiertos, cerrados, siguienteRight);

                ordenaVector(abiertos);
                actual = abiertos[abiertos.size() - 1];
                indiceCerrados++;
            }
        }

        return false;
    }


}

bool ComportamientoJugador::sePuedeIr(int fila, int columna) {

    if (fila >= 0 && fila <= mapaResultado.size()-1) {
        if (columna >= 0 && columna <= mapaResultado.size()-1) {
            if (mapaResultado[fila][columna] == 'K' ||
                mapaResultado[fila][columna] == 'S' ||
                mapaResultado[fila][columna] == 'T' ||
                mapaResultado[fila][columna] == '?') {
                return true;
            }
        }
    } else {
        return false;
    }

}

void ComportamientoJugador::generaPlanAldeano(estado &actual, int posAldeano) {
    plan.clear();
    planAldeano.clear();
    estado air, destinop;
    air.fila = actual.fila;
    air.columna = actual.columna;
    if (posAldeano == 1 || posAldeano == 2 || posAldeano == 6) {
        if (actual.orientacion == 0) {
            if (sePuedeIr(air.fila + 1, air.columna))
                air.fila++;
            if (sePuedeIr(air.fila, air.columna + 1))
                air.columna++;
            while (!sePuedeIr(air.fila, air.columna)) {
                if (sePuedeIr(air.fila + 1, air.columna))
                    air.fila++;
                if (sePuedeIr(air.fila, air.columna + 1))
                    air.columna++;
            }
        } else if (actual.orientacion == 1) {
            if (sePuedeIr(air.fila + 1, air.columna))
                air.fila++;
            if (sePuedeIr(air.fila, air.columna - 1))
                air.columna--;
            while (!sePuedeIr(air.fila, air.columna)) {
                if (sePuedeIr(air.fila + 1, air.columna))
                    air.fila++;
                if (sePuedeIr(air.fila, air.columna - 1))
                    air.columna--;
            }
        } else if (actual.orientacion == 2) {
            if (sePuedeIr(air.fila - 1, air.columna))
                air.fila--;
            if (sePuedeIr(air.fila, air.columna - 1))
                air.columna--;
            while (!sePuedeIr(air.fila, air.columna)) {
                if (sePuedeIr(air.fila - 1, air.columna))
                    air.fila--;
                if (sePuedeIr(air.fila, air.columna - 1))
                    air.columna--;
            }
        } else if (actual.orientacion == 3) {
            if (sePuedeIr(air.fila - 1, air.columna))
                air.fila--;
            if (sePuedeIr(air.fila, air.columna + 1))
                air.columna++;
            while (!sePuedeIr(air.fila, air.columna)) {
                if (sePuedeIr(air.fila - 1, air.columna))
                    air.fila--;
                if (sePuedeIr(air.fila, air.columna + 1))
                    air.columna++;
            }
        }
    } else if (posAldeano == 3) {

        if (actual.orientacion == 0) {
            if (sePuedeIr(air.fila + 1, air.columna))
                air.fila++;
            if (sePuedeIr(air.fila, air.columna - 1))
                air.columna--;
            while (!sePuedeIr(air.fila, air.columna)) {
                if (sePuedeIr(air.fila + 1, air.columna))
                    air.fila++;
                if (sePuedeIr(air.fila, air.columna - 1))
                    air.columna--;
            }
        } else if (actual.orientacion == 1) {
            if (sePuedeIr(air.fila - 1, air.columna))
                air.fila--;
            if (sePuedeIr(air.fila, air.columna - 1))
                air.columna--;
            while (!sePuedeIr(air.fila, air.columna)) {
                if (sePuedeIr(air.fila - 1, air.columna))
                    air.fila--;
                if (sePuedeIr(air.fila, air.columna - 1))
                    air.columna--;
            }
        } else if (actual.orientacion == 2) {
            if (sePuedeIr(air.fila - 1, air.columna))
                air.fila--;
            if (sePuedeIr(air.fila, air.columna + 1))
                air.columna++;
            while (!sePuedeIr(air.fila, air.columna)) {
                if (sePuedeIr(air.fila - 1, air.columna))
                    air.fila--;
                if (sePuedeIr(air.fila, air.columna + 1))
                    air.columna++;
            }
        } else if (actual.orientacion == 3) {
            if (sePuedeIr(air.fila + 1, air.columna))
                air.fila++;
            if (sePuedeIr(air.fila, air.columna + 1))
                air.columna++;
            while (!sePuedeIr(air.fila, air.columna)) {
                if (sePuedeIr(air.fila + 1, air.columna))
                    air.fila++;
                if (sePuedeIr(air.fila, air.columna + 1))
                    air.columna++;
            }
        }
    }

    destinop.fila = air.fila;
    destinop.columna = air.columna;


    if (!pathFinding(actual, destinop, planAldeano)) {
        cout << " pathfinding del aldeano devuelve false " << endl;
    }


}

void ComportamientoJugador::actualizaPosicion(Action accion) {

    if (accion == actTURN_L) {
        brujula--;
        if (brujula == -1) {
            brujula = 3;
        }
    }
    if (accion == actTURN_R) {
        brujula++;
        if (brujula == 4) {
            brujula = 0;
        }
    }
    if (accion == actFORWARD) {
        if (brujula == 0) {
            filaCtual--;
        } else if (brujula == 1) {
            colActual++;
        } else if (brujula == 2) {
            filaCtual++;
        } else if (brujula == 3) {
            colActual--;
        }

    }
}

void ComportamientoJugador::inicializaMapaRepulsion() {

    for (int i = 0; i < filasMatriz; i++) {
        mapaRepulsion[i] = new int[columnasMatriz];
    }

    for (int i = 0; i < filasMatriz; ++i) {
        for (int j = 0; j < columnasMatriz; ++j) {
            mapaRepulsion[i][j] = 0;
        }
    }

}

bool ComportamientoJugador::compruebaNivel3() {

    for (int i = 0; i < mapaResultado.size(); ++i) {
        for (int j = 0; j < mapaResultado[0].size(); ++j) {
            if (mapaResultado[i][j] == '?') {
                return true;
            }
        }
    }
    return false;
}


Action ComportamientoJugador::llenaPlanRepulsion(estado &actual, Sensores &sensores) {

    Action accion;
    mapaRepulsion[filaRe][coluRe]++;
    if (primeraRepu) {
        if (sensores.terreno[2] == 'K' || sensores.terreno[2] == 'S' || sensores.terreno[2] == 'T') {

            accion = actFORWARD;
            //planMinima.push_back(actFORWARD);
            primeraRepu = false;
            return accion;
        }
    } else {

        int izquierda, derecha, frontal;
        izquierda = derecha = frontal = 0;

        switch (actual.orientacion) {
            case 0:
                frontal = mapaRepulsion[filaRe - 1][coluRe];
                izquierda = mapaRepulsion[filaRe][coluRe - 1];
                derecha = mapaRepulsion[filaRe][coluRe + 1];
                break;
            case 1:
                frontal = mapaRepulsion[filaRe][coluRe + 1];
                izquierda = mapaRepulsion[filaRe - 1][coluRe];
                derecha = mapaRepulsion[filaRe + 1][coluRe];
                break;
            case 2:
                frontal = mapaRepulsion[filaRe + 1][coluRe];
                izquierda = mapaRepulsion[filaRe][coluRe + 1];
                derecha = mapaRepulsion[filaRe][coluRe - 1];
                break;
            case 3:
                frontal = mapaRepulsion[filaRe][coluRe - 1];
                izquierda = mapaRepulsion[filaRe + 1][coluRe];
                derecha = mapaRepulsion[filaRe - 1][coluRe];
                break;

        }

        if (frontal <= derecha && frontal <= izquierda) {
            if (sensores.terreno[2] == 'K' || sensores.terreno[2] == 'S' || sensores.terreno[2] == 'T') {
                accion = actFORWARD;

            } else {
                switch (actual.orientacion) {
                    case 0:
                        mapaRepulsion[filaRe - 1][coluRe] += 1000;
                        break;
                    case 1:
                        mapaRepulsion[filaRe][coluRe + 1] += 1000;
                        break;
                    case 2:
                        mapaRepulsion[filaRe + 1][coluRe] += 1000;
                        break;
                    case 3:
                        mapaRepulsion[filaRe][coluRe - 1] += 1000;
                        break;
                }
            }
            return accion;
        } else {
            if (derecha < izquierda) {
                accion = actTURN_R;
                return accion;

            } else {
                accion = actTURN_L;
                return accion;

            }
        }
    }

}


void ComportamientoJugador::actualizaPosRepulsion(estado &actual, Action accion) {


    if (accion == actTURN_L) {
        brujula--;
        if (brujula == -1) {
            brujula = 3;
        }
    }
    if (accion == actTURN_R) {
        brujula++;
        if (brujula == 4) {
            brujula = 0;
        }
    }
    if (accion == actFORWARD) {
        if (brujula == 0) {
            filaRe--;
        } else if (brujula == 1) {
            coluRe++;
        } else if (brujula == 2) {
            filaRe++;
        } else if (brujula == 3) {
            coluRe--;
        }

    }

}

void ComportamientoJugador::actualizaTerreno(estado &actual, Sensores &sensores) {

    int filP = actual.fila;
    int colP = actual.columna;

    mapaResultado[filP][colP] = sensores.terreno[0];
    switch (actual.orientacion) {
        case 0:
            mapaResultado[filP - 1][colP - 1] = sensores.terreno[1];
            mapaResultado[filP - 1][colP] = sensores.terreno[2];
            mapaResultado[filP - 1][colP + 1] = sensores.terreno[3];
            mapaResultado[filP - 2][colP - 2] = sensores.terreno[4];
            mapaResultado[filP - 2][colP - 1] = sensores.terreno[5];
            mapaResultado[filP - 2][colP] = sensores.terreno[6];
            mapaResultado[filP - 2][colP + 1] = sensores.terreno[7];
            mapaResultado[filP - 2][colP + 2] = sensores.terreno[8];
            mapaResultado[filP - 3][colP - 3] = sensores.terreno[9];
            mapaResultado[filP - 3][colP - 2] = sensores.terreno[10];
            mapaResultado[filP - 3][colP - 1] = sensores.terreno[11];
            mapaResultado[filP - 3][colP] = sensores.terreno[12];
            mapaResultado[filP - 3][colP + 1] = sensores.terreno[13];
            mapaResultado[filP - 3][colP + 2] = sensores.terreno[14];
            mapaResultado[filP - 3][colP + 3] = sensores.terreno[15];
            break;
        case 1:
            mapaResultado[filP - 1][colP + 1] = sensores.terreno[1];
            mapaResultado[filP][colP + 1] = sensores.terreno[2];
            mapaResultado[filP + 1][colP + 1] = sensores.terreno[3];
            mapaResultado[filP - 2][colP + 2] = sensores.terreno[4];
            mapaResultado[filP - 1][colP + 2] = sensores.terreno[5];
            mapaResultado[filP][colP + 2] = sensores.terreno[6];
            mapaResultado[filP + 1][colP + 2] = sensores.terreno[7];
            mapaResultado[filP + 2][colP + 2] = sensores.terreno[8];
            mapaResultado[filP - 3][colP + 3] = sensores.terreno[9];
            mapaResultado[filP - 2][colP + 3] = sensores.terreno[10];
            mapaResultado[filP - 1][colP + 3] = sensores.terreno[11];
            mapaResultado[filP][colP + 3] = sensores.terreno[12];
            mapaResultado[filP + 1][colP + 3] = sensores.terreno[13];
            mapaResultado[filP + 2][colP + 3] = sensores.terreno[14];
            mapaResultado[filP + 3][colP + 3] = sensores.terreno[15];


            break;
        case 3:
            mapaResultado[filP + 1][colP - 1] = sensores.terreno[1];
            mapaResultado[filP][colP - 1] = sensores.terreno[2];
            mapaResultado[filP - 1][colP - 1] = sensores.terreno[3];
            mapaResultado[filP + 2][colP - 2] = sensores.terreno[4];
            mapaResultado[filP + 1][colP - 2] = sensores.terreno[5];
            mapaResultado[filP][colP - 2] = sensores.terreno[6];
            mapaResultado[filP - 1][colP - 2] = sensores.terreno[7];
            mapaResultado[filP - 2][colP - 2] = sensores.terreno[8];
            mapaResultado[filP + 3][colP - 3] = sensores.terreno[9];
            mapaResultado[filP + 2][colP - 3] = sensores.terreno[10];
            mapaResultado[filP + 1][colP - 3] = sensores.terreno[11];
            mapaResultado[filP][colP - 3] = sensores.terreno[12];
            mapaResultado[filP - 1][colP - 3] = sensores.terreno[13];
            mapaResultado[filP - 2][colP - 3] = sensores.terreno[14];
            mapaResultado[filP - 3][colP - 3] = sensores.terreno[15];


            break;
        case 2:
            mapaResultado[filP + 1][colP + 1] = sensores.terreno[1];
            mapaResultado[filP + 1][colP] = sensores.terreno[2];
            mapaResultado[filP + 1][colP - 1] = sensores.terreno[3];
            mapaResultado[filP + 2][colP + 2] = sensores.terreno[4];
            mapaResultado[filP + 2][colP + 1] = sensores.terreno[5];
            mapaResultado[filP + 2][colP] = sensores.terreno[6];
            mapaResultado[filP + 2][colP - 1] = sensores.terreno[7];
            mapaResultado[filP + 2][colP - 2] = sensores.terreno[8];
            mapaResultado[filP + 3][colP + 3] = sensores.terreno[9];
            mapaResultado[filP + 3][colP + 2] = sensores.terreno[10];
            mapaResultado[filP + 3][colP + 1] = sensores.terreno[11];
            mapaResultado[filP + 3][colP] = sensores.terreno[12];
            mapaResultado[filP + 3][colP - 1] = sensores.terreno[13];
            mapaResultado[filP + 3][colP - 2] = sensores.terreno[14];
            mapaResultado[filP + 3][colP - 3] = sensores.terreno[15];


            break;
    }


}

Action ComportamientoJugador::think(Sensores sensores) {
    Action accion;


    if (compruebaNivel3())     // si estamos en el nivel 3
    {

        if (sensores.vida == 1999) {
            filaRe = coluRe = brujula = 0;
            inicializaMapaRepulsion();
        }


        if ((sensores.mensajeF != -1 && sensores.mensajeC != -1) || yaConozco) //  conozco mi posicion
        {
            yaConozco = true;

            if(sensores.mensajeF != -1 && sensores.mensajeC != -1){

                conozcoK = true;
                //He llegado a un K, me guardo posicion y ejecuto pathFinding.
                estado a;
                filaCtual = sensores.mensajeF;
                colActual = sensores.mensajeC;
                a.fila = filaCtual;
                a.columna= colActual;
                a.orientacion = brujula;

                destino.fila = sensores.destinoF;
                destino.columna = sensores.destinoC;
                actualizaTerreno(a,sensores);


                if ( ! pathFinding(a,destino,plan)){
                    cout << " devuelve false " << endl;
                }

                //return actIDLE;
            }

            // si es la primera vez que entramos actualizamos parametros

                origen.fila = filaCtual;
                origen.columna = colActual;
                origen.orientacion = brujula;

                actualizaTerreno(origen, sensores);

                //  comprobamos si hay aldeano
                bool hayAldeano = false;
                int contadorPeligro = 0;
                for (auto it = sensores.superficie.begin(); it != sensores.superficie.end() && !hayAldeano; it++) {
                    if (*it == 'a') {
                        hayAldeano = true;
                    }
                    contadorPeligro++;
                }
                contadorPeligro--;
                if (hayAldeano && contadorPeligro == 2) {

                    return actIDLE;
                }

                if (plan.size() > 0) {
                    cout <<"el plan no esta vacio" << endl;
                    accion = *(plan.begin());
                    estado b;
                    b.fila = filaCtual;
                    b.columna = colActual;
                    if (accion == actFORWARD) {
                        switch (brujula) {
                            case 0:
                                if (mapaResultado[filaCtual - 1][colActual] == 'K' ||
                                    mapaResultado[filaCtual - 1][colActual] == 'S' ||
                                    mapaResultado[filaCtual - 1][colActual] == 'T') {
                                    actualizaPosicion(accion);
                                    plan.erase(plan.begin());

                                    return accion;
                                } else {
                                    recalcular = true;
                                    plan.clear();
                                };
                                break;
                            case 1:
                                if (mapaResultado[filaCtual][colActual + 1] == 'K' ||
                                    mapaResultado[filaCtual][colActual + 1] == 'S' ||
                                    mapaResultado[filaCtual][colActual + 1] == 'T') {
                                    actualizaPosicion(accion);

                                    actualizaTerreno(b,sensores);
                                    plan.erase(plan.begin());

                                    return accion;
                                } else {
                                    recalcular = true;
                                    plan.clear();
                                };
                                break;
                            case 2:
                                if (mapaResultado[filaCtual + 1][colActual] == 'K' ||
                                    mapaResultado[filaCtual + 1][colActual] == 'S' ||
                                    mapaResultado[filaCtual + 1][colActual] == 'T') {
                                    actualizaPosicion(accion);
                                    actualizaTerreno(b,sensores);
                                    plan.erase(plan.begin());

                                    return accion;
                                } else {
                                    recalcular = true;
                                    plan.clear();
                                };
                                break;
                            case 3:
                                if (mapaResultado[filaCtual][colActual - 1] == 'K' ||
                                    mapaResultado[filaCtual][colActual - 1] == 'S' ||
                                    mapaResultado[filaCtual][colActual - 1] == 'T') {
                                    actualizaPosicion(accion);
                                    actualizaTerreno(b,sensores);
                                    plan.erase(plan.begin());

                                    return accion;
                                } else {
                                    recalcular = true;
                                    plan.clear();
                                };
                                break;

                        }
                    } else {

                        actualizaPosicion(accion);
                        actualizaTerreno(b,sensores);
                        plan.erase(plan.begin());
                        return accion;
                    }

                } else if(recalcular){
                    if ( sensores.destinoF != -1 && sensores.destinoC != -1){
                        destino.fila = sensores.destinoF;
                        destino.columna = sensores.destinoC;
                    }
                    cout << "Ejecuto pathfinding abajo con" << destino.fila << " " << destino.columna << endl;
                    origen.fila = filaCtual;
                    origen.columna = colActual;
                    origen.orientacion = brujula;
                    //actualizaTerreno(origen, sensores);
                    if (!pathFinding(origen, destino, plan)) {
                        cout << " pathfinding devuelve false " << endl;
                        return actIDLE;
                    }
                    recalcular = false;
                    accion = *(plan.begin());
                    actualizaPosicion(accion);
                    actualizaTerreno(origen,sensores);
                    plan.erase(plan.begin());
                    return accion;

                }


                if ( filaCtual == destino.fila && colActual == destino.columna){
                    yaConozco = false;
                }

        } else { // no conozco mi posicion, ir a casilla menor repulsion

            estado actual;
            if (pri) {
                brujula = 0;
                filaRe = 100;
                coluRe = 100;
                pri = false;
            }
            actual.fila = filaRe;
            actual.columna = coluRe;
            actual.orientacion = brujula;


            Action accion;
            accion = llenaPlanRepulsion(actual, sensores);

            // accion = planMinima.front();
            actualizaPosRepulsion(actual, accion);
            //planMinima.pop_front();

            return accion;
        }

    } else {// no es el nivel 3



        // si es la primera vez que entramos actualizamos parametros
        if (sensores.vida == 499 || primera) {
            destino.fila = sensores.destinoF;
            destino.columna = sensores.destinoC;
            origen.fila = sensores.mensajeF;
            origen.columna = sensores.mensajeC;
            filaCtual = origen.fila;
            colActual = origen.columna;
            origen.orientacion = 0;

            if (!pathFinding(origen, destino, plan)) {
                cout << " devuelve false " << endl;
            }
            accion = *(plan.begin());
            actualizaPosicion(accion);
            plan.erase(plan.begin());
            primera = false;

            return accion;

        } else {
            // si hemos llegado al final
            if (filaCtual == destino.fila && colActual == destino.columna) {
                return actIDLE;
            }
            // si el plan del aldeano no esta vacio, ejecutamos accion contra aldeano
            if (!planAldeano.empty()) {
                accion = *(planAldeano.begin());
                actualizaPosicion(accion);
                planAldeano.erase(planAldeano.begin());
                return accion;

            } else {
                // si el plan contra el aldeano esta vacio, comprobamos si hay aldeano
                bool hayAldeano = false;
                int contadorPeligro = 0;
                for (auto it = sensores.superficie.begin(); it != sensores.superficie.end() && !hayAldeano; it++) {
                    if (*it == 'a') {
                        hayAldeano = true;
                    }
                    contadorPeligro++;
                }
                contadorPeligro--;
                if (hayAldeano &&
                    (contadorPeligro == 1 || contadorPeligro == 2 || contadorPeligro == 3 || contadorPeligro == 6)) {
                    contador++;
                    origen.fila = filaCtual;
                    origen.columna = colActual;
                    origen.orientacion = brujula;
                    if (contador > 5) {
                        return actIDLE;
                    }
                    generaPlanAldeano(origen, contadorPeligro);
                    accion = *(planAldeano.begin());
                    actualizaPosicion(accion);
                    planAldeano.erase(planAldeano.begin());
                    return accion;
                }
            }
            if (plan.empty()) {

                origen.fila = filaCtual;
                origen.columna = colActual;
                origen.orientacion = brujula;

                if (!pathFinding(origen, destino, plan)) {
                    cout << " pathfinding devuelve false " << endl;
                    return actIDLE;
                }

                accion = *(plan.begin());
                actualizaPosicion(accion);
                plan.erase(plan.begin());
                return accion;

            } else {
                accion = *(plan.begin());
                actualizaPosicion(accion);
                plan.erase(plan.begin());
                return accion;
            }


        }

    }
}

void ComportamientoJugador::printSensores(const Sensores &sensores) {

    /*
     cout<< endl;
    cout<< "----------------PERCEPCION--------------\n";
    cout<<"VIDA: "<<sensores.vida<<endl;
    cout<<"COLISION: "<<sensores.colision<<endl;
    cout<<"reset "<<sensores.reset<<endl;
    cout<<"(mensajeF,mensajeC): "<<sensores.mensajeF << " , "<<sensores.mensajeC <<endl;
    cout<< "(fil, col): "<< sensores.fil <<" , "<<sensores.col <<endl ;
    cout<< "tiempo: "<<sensores.tiempo<<endl;
    cout<< "(destinoF, destinoC): "<<sensores.destinoF <<" , "<<sensores.destinoC <<endl;
    cout<< "terreno: "<< endl;

    for (auto it=sensores.terreno.begin(); it!= sensores.terreno.end(); it++){
          cout<<" "<<*it;
     }
     cout<<"\nSuperficie: ";


     for (auto it=sensores.superficie.begin(); it!= sensores.superficie.end(); it++){
          cout<<" "<<*it;
     }

 
    */
}

int ComportamientoJugador::interact(Action accion, int valor) {
    return false;
}
