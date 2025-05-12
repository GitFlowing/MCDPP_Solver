#pragma once
#include <iostream>
#include <vector>
#include "Graph.h"

//========================================
// Algorithmen auf der Datenstruktur Graph
//========================================


// Daten-Structs fuer Speichern der Ergebnisse ------------------------------ 

// Speichert Weg
struct Weg
{
	//Knoten des Weges
	vector<size_t> weg_knoten;
	//Kanten des Weges
	vector<size_t> weg_kanten;
};


// Graph-Untersuchungen ---------------------------------------------------------------------
Weg wegFinden(const Graph& g, const vector<int>& vorg, size_t start, size_t ende);
double minKantenGewicht(const Graph& g);

// Obere Schranke MCDPP -----------------------------------------------------------------------
double kantenSumme(const Graph& base_graph);
double kantenSummeUpgrade(const Graph& base_graph, const  Graph& demand_graph);