#pragma once

#include <iostream>
#include <vector>
#include "Graph.h"
#include "graphAlgorithmen.h"
#include "WarteschlangeBranchAndBound.h"

//=======================
// Subgradientenverfahren
//=======================

/* Verfahrensbeschreibung
--------------------------
Eine untere Schranke U des MCDPP kann mit Hilfe einer Lagrange-Relaxation ermittelt
werden. Der Parameter der Lagrange Relaxation ist lambda. Die untere Schranke U(lambda) ist abhängig
vom Parameter lambda. Im Subgradientenverfahren werden unterschiedliche lambda-Werte gebildet,
die zu unterschiedlichen unteren Schranken führen. Aus max_iter Iterationen wird dann das größte U(lambda) bestimmt.
Um neue lambda-Werte zu erhalten,  wird eine Folge von lambda-Werten gebildet mit:

lambda_{i+1} = lambda_{i} + alpha_{i} * subgradient_{i}

Der Subgradient subgradient_{i} wird bestimmt aus der Differenz von tatsächlichen Fluss und maximal erlaubten Fluss
in jedem Knoten. alpha ist die Schrittweite und wird hier mit der newtonischen Schrittweite berechnet:

alpha_{i} = theta_{i} (l_{upper} -  U(lambda)) / (|| subgradient_{i} ||^2)

l_{upper} ist eine obere Schranke mit der Eigenschaft:

l_{upper} > Gesamtkosten optimaler Lösung

theta_{i} ist ein Parameter, der sich alle n_{iter_half} Iterationen halbiert, wenn innerhalb der
n_{iter_half} Iterationen keine neue größere untere Schranke erhalten wird.

Dieses Subgradientenverfahren wird in subGradOpt durchgeführt. Zusätzlich kann man sich je nach
LambdaZustand unterschiedliche lambda-Werte die während des Subgradientenverfahrens entstanden
sind zurückgeben lassen. Weiterhin wird der Zustand der Instanz zurückgegeben (siehe WarteschlangeBranchAndBound)

Weiterhin können zulässige Lösungen während des Subgradientenverfahrens entstehen. Die gefundene
zulässige Lösung mit den kleinsten Gesamtkosten wird ebenfalls zurückgegeben zusammen mit ihren
Gesamtkosten.

In der Funktion dijkstra_lambda wird der Dijkstra Algorithmus mit den um lambda_{edge} angepassten Kosten
durchgeführt. lambda_{edge} ordnet jeder Kanten (i,j) zwischen den Knoten i und j einen Wert lambda_{edge}(i,j) zu:

lambda_{edge}(i,j) = lambda[i] + lambda[j]

lambda_{edge} wird in der Funktion lambdaEdgeInitialisieren gebildet.
*/



// Daten-Structs fuer Speichern der Ergebnisse ------------------------------ 

// Dijkstra-Objekt
// speichert Distanzen, Vorgaenger, Iterationen aus Dijkstra-Algorithmus
struct DijkstraObj
{
	// Liste der kuerzesten Abstaende vom Wurzelknoten mit Index = Knoten
	vector<double> dist;
	// Liste der Vorgaenger im Kuerzeste-Wege-Baum mit Index = Knoten
	vector<int> vorg;
	// Zaehlung der While Iterationen in dijkstra()
	unsigned int iter_zahl = 0;
};

// Subgradienten-Objekt
// Speichert untere Schranke, Zustand der unteren Schranke, lambda, zulässige Lösung und deren Gesamtkosten
struct SubgradObj
{
	//Untere Schranke
	double untere_schranke = 0;
	//Zustand untere Schranke
	Zustand zustand = KANDIDAT;
	//Lambda
	vector<double> lambda;
	// Kosten gefundener zulässiger Lösung mit kleinsten Gesamtkosten
	double gesamtkosten_zulaessig = UNENDLICH;
	// Wegkanten der zulässigen Lösung zu gesamtkosten_zulaessig
	vector<size_t> zulaessige_loesung;
};


// Welches Lambda wird im Subgradientenverfahren gespeichert
enum LambdaZustand
{
	LAMBDA_NULL,		// lambda_node wird immer als 0 Vektor gespeichert
	LAMBDA_MAX,			// lambda_node von der maximalen unteren Schranke des Subgradientenverfahrens wird gespeichert
	LAMBDA_KONFLIKT		// lambda_node mit kleinster Zahl an Konflikten des Subgradientenverfahrens wird gespeichert
};



// Dijkstra-Algorithmus mit angepassten lambda-Kosten --------------------------------------
DijkstraObj dijkstra_lambda(const Graph& graph, size_t s, size_t t, const vector<double>& lambda, double l_upper);

// Subgradientenverfahren -----------------------------------------------------------------------
SubgradObj subGradOpt(const Graph& base_graph, const vector<pair<size_t, size_t>>& alle_demands, const vector<size_t>& geloeschte_knoten, const vector<double>& lambda_node_start,
	const vector<size_t>& terminals_grad, size_t max_iter, double obere_schranke, size_t n_iter_half, double l_upper, const LambdaZustand& lambda_option);

// Lambdas für Subgradientenverfahren initialisieren
void lambdaEdgeInitialisieren(const Graph& base_graph, vector<double>& lambda_edge,
	vector<double>& lambda_edge_neu, const vector<double>& lambda_node, const vector<size_t>& geloeschte_knoten,
	double l_upper);
