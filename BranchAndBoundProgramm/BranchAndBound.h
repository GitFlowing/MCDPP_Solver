#pragma once
#include "Graph.h"
#include "WarteschlangeBranchAndBound.h"
#include "subgradient.h"

// ==================================
// Branch & Bound Verfahren für MCDPP
// ==================================


// Endergebnis des Branch&Bound Verfahrens (optimale Lösung oder keine Lösung)
struct BranchBoundErgebnis
{
	// Gesamtkosten aller eingebetteten Wege aller Demands des Demand-Graphen der optimalen Lösung (keine Lösung entspricht Wert -1)
	double distanz = -1;
	// Alle Wegkanten einer optimalen Lösung des MCDPP
	vector<size_t> demand_wege_kanten;

};

// Speichern Verzweigte Subinstanzen und gefundene zulässige Lösung mit kleinsten Gesamtkosten
struct VerzweigungsErgebnis
{
	// Verzweigte Subinstanzen
	vector<BB_Instanz> verzweigte_subinstanz;
	// Gesamtkosten der zulässigen Lösung mit kleinsten Gesamtkosten
	double gesamtkosten_zulaessig = UNENDLICH;
	// Gefundene zulässige Lösung mit geringsten Gesamtkosten
	vector<size_t> zulaessige_loesung;
};

// Speichern Verzweigungsknoten und gefundene zulässige Lösung mit kleinsten Gesamtkosten
struct VZK
{
	//Verzweigungsknoten
	size_t vzk;
	// Gesamtkosten der zulässigen Lösung mit kleinsten Gesamtkosten
	double gesamtkosten_zulaessig = UNENDLICH;
	// Gefundene zulässige Lösung mit geringsten Gesamtkosten
	vector<size_t> zulaessige_loesung;
};


// Verzweigungsoption
enum VerzweigungZustand
{
	MAX_FLUSS,		//Verzweigungsknoten ist Knoten mit größten Fluss
	WEG,			//Verzweigungsknoten ist mittlerer Wegknoten des längsten eingebetteten Demands
	KONFLIKTE		//Menge aller Konfliktknoten
};


// Branch and Bound Verfahren -----------------------------------------------------------------------
BranchBoundErgebnis branchAndBound(const Graph& base_graph, const Graph& demand_graph,
	size_t max_iter_sub, size_t batch_size, size_t n_iter_half,
	const LambdaZustand& lambda_option, const VerzweigungZustand& verzweigung_option);

// Anfangsinstanz des B&B initialisieren
void instanzTerminalsInitialisieren(const Graph& base_graph, const Graph& demand_graph, BB_Instanz& instanz_anfang);

// Verzweigungsknoten auswählen
VZK verzweigungKnotenBilden(const Graph& base_graph, const BB_Instanz& instanz_auswahl,
	const vector<size_t>& terminals_grad, double obere_schranke, size_t batch_size,
	size_t n_iter_half, double l_upper, const LambdaZustand& lambda_option,
	const VerzweigungZustand& verzweigung_option);

// Kandidaten für mögliche Verzweigungsknoten auswählen
vector<size_t> verzweigungsKnotenFinden(const Graph& base_graph, const BB_Instanz& neue_instanz, double l_upper, const VerzweigungZustand& verzweigung_option);

// Beliebigen nicht gelöschten Nicht-Terminalknoten als Verzweigungsknoten finden
size_t beliebigerVerzweigungsKnoten(const Graph& base_graph, const BB_Instanz& instanz_auswahl);


// Instanz beim Verzweigungsknoten verzweigen
VerzweigungsErgebnis instanzVerzweigen(const Graph& base_graph, size_t verzweigung_knoten, const BB_Instanz& instanz_auswahl,
	double obere_schranke, const vector<size_t>& terminals_grad, size_t max_iter_sub, size_t n_iter_half, double l_upper, const LambdaZustand& lambda_option);

// Bilden einer neuen Subinstanz
VerzweigungsErgebnis instanzBilden(const Graph& base_graph, const BB_Instanz& instanz_auswahl, size_t verzweigung_knoten, int demand_index, const vector<size_t>& terminals_grad,
	size_t max_iter, double obere_schranke, size_t n_iter_half, double l_upper, const LambdaZustand& lambda_option);


// Ausgabe der Kanten der optimalen Lösung -----------------------------------------------
vector<vector<size_t>> kantenAusgabe(const BranchBoundErgebnis& ergebnis, const Graph& base_graph, const Graph& demand_graph);
