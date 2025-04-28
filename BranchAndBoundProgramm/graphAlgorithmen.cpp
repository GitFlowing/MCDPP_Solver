#include "graphAlgorithmen.h"
#include "Warteschlange.h"
#include<unordered_set>
#include <queue>


// Graph-Untersuchungen ---------------------------------------------------------------------

// Zu einem vorg Vektor den Weg als Knotenfolge und Kantenfolge ermitteln 
Weg wegFinden(const Graph& g, const vector<int>& vorg, size_t start, size_t ende)
{
	// Weg erstellen
	Weg ergebnis;
	ergebnis.weg_knoten.clear();
	ergebnis.weg_kanten.clear();

	//vorg-Vektor von Ende bis Anfang über Vorgänger durchgehen
	if (vorg[ende] != UNBENUTZT) {

		// Wegknoten ermitteln
		int index_kopf = ende;
		int index_fuss = vorg[ende];
		ergebnis.weg_knoten.push_back(ende);
		ergebnis.weg_knoten.insert(ergebnis.weg_knoten.begin(), index_fuss);

		// Wegkante ermitteln
		FUER_ALLE_NACHBARN(n, g, index_kopf)
		{
			if (n.i() == index_fuss)
			{
				ergebnis.weg_kanten.push_back(n.j());
				break;
			}
		}

		while (index_fuss != start) {
			// Wegknoten ermitteln
			index_kopf = index_fuss;
			index_fuss = vorg[index_kopf];
			ergebnis.weg_knoten.insert(ergebnis.weg_knoten.begin(), index_fuss);

			// Wegkante ermitteln
			FUER_ALLE_NACHBARN(n, g, index_kopf)
			{
				if (n.i() == index_fuss)
				{
					ergebnis.weg_kanten.push_back(n.j());
					break;
				}
			}
		}
	}

	return ergebnis;
}



// minimales Kantengewicht bestimmen
double minKantenGewicht(const Graph& g)
{
	double min = UNENDLICH;
	FUER_ALLE_KANTEN(a, g)
	{
		if (a.gewicht < min)
		{
			min = a.gewicht;
		}
	}

	return min;
}





// Obere Schranke MCDPP -----------------------------------------------------------------------
// Summe aller Kanten
double kantenSumme(const Graph& base_graph)
{
	double ergebnis = 0;

	FUER_ALLE_KANTEN(a, base_graph)
	{
		ergebnis += a.gewicht;
	}

	return ergebnis;
}



double kantenSummeUpgrade(const Graph& base_graph, const  Graph& demand_graph)
{
	// Menge an ausgewählten Kanten
	unordered_set<size_t> ausgewaehlte_kanten;

	// Anzahl Kanten zuordnen, die belegt sind, wenn Knoten Teil eines Weges ist.
	// Terminals haben immer Grad des Terminals viele Kanten und Nicht Terminal
	// Knoten haben 2 Kanten
	vector<size_t> terminals_grad(base_graph.anzKnoten(), 2);
	FUER_ALLE_IKNOTEN(t, demand_graph)
	{
		// Terminal Namen im Demand Graphen
		string t_name = demand_graph.knoten(t).name();

		// Terminal Indices im Base Graphen
		size_t base_t = base_graph.iKnoten(t_name);

		// Grad einfügen
		terminals_grad[base_t] = demand_graph.grad(t);
	}


	// Alle Knoten durchgehen
	unordered_set<size_t> gesammelte_kanten;
	FUER_ALLE_IKNOTEN(i, base_graph)
	{
		gesammelte_kanten.clear();

		// Anzahl Kanten
		size_t anzahl_kanten = terminals_grad[i];

		// anzahl_kanten viele Kanten mit grössten Kantengewicht bestimmen
		for (size_t j = 0; j < anzahl_kanten; ++j)
		{
			// Bestimme Kante mit größten Kantengewicht
			double max_gewicht = 0;
			size_t kante_max_gewicht = base_graph.anzKanten();
			FUER_ALLE_NACHBARN(n, base_graph, i)
			{
				// Kante muss maximales Kantengewicht haben, darf aber nur einmal vorkommen
				if (base_graph.kante(n.j()).gewicht > max_gewicht
					&& gesammelte_kanten.find(n.j()) == gesammelte_kanten.end())
				{
					max_gewicht = base_graph.kante(n.j()).gewicht;
					kante_max_gewicht = n.j();
				}
			}

			// Kanten hinzufügen
			ausgewaehlte_kanten.insert(kante_max_gewicht);
			gesammelte_kanten.insert(kante_max_gewicht);
		}
	}

	// Kosten aller Kanten in ausgewaehlte_kanten zusammenaddieren
	double ergebnis = 0;
	for (size_t kanten_element : ausgewaehlte_kanten)
	{
		// Es kann sein, dass Knoten keine Kanten hatten 
		// ->kanten_element == base_graph.anzKanten() 
		if (kanten_element != base_graph.anzKanten())
		{
			ergebnis += base_graph.kante(kanten_element).gewicht;
		}
	}

	return ergebnis;
}
