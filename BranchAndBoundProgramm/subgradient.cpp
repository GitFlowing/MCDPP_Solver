#include "subgradient.h"
#include "Warteschlange.h"
#include<unordered_set>
#include<map>


/*
HINWEIS:

Das Löschen von Kanten entspricht deren Kantengewichte auf UNENDLICH zu setzen.
Das Löschen von Knoten entspricht alle Kantengewichte der Kanten des Knotens auf UNENDLICH zu setzen
*/


// Dijkstra-Algorithmus mit angepassten lambda-Kosten --------------------------------------
DijkstraObj dijkstra_lambda(const Graph& graph, size_t s, size_t t, const vector<double>& lambda, double l_upper)
{

	// Initialisiere Liste der kuerzesten Distanzen d, Warteschlange w und Vorgaenger vorg 
	vector <double> d(graph.anzKnoten(), UNENDLICH);
	vector <int> vorg(graph.anzKnoten(), UNBENUTZT);
	Warteschlange w(&d);

	//Wurzelknoten einfuegen 
	d[s] = 0;
	vorg[s] = s;
	w.push(s);


	// Zaehlen der while Iterationen
	unsigned int iter_zahl = 0;

	//Loop solange bis alle ereichbaren Knoten kuerzeste Distanz haben zu s
	//oder Endpunkt t erreicht wurde
	while (!w.empty())
	{
		// Zaehlen der while-Iterationen
		iter_zahl++;

		// Element mit kuerzester Distanz in w rausnehmen
		size_t auswahl = w.pop();

		// Wenn Ziel erreicht -> kuerzester Weg von s zu t vorhanden
		if (auswahl == t)
		{
			break;
		}

		// Alle Vorwaertsnachbarn durchgehen und wenn noetig Distanz und Vorgaenger aktualisieren
		// Mit der push-Methode der Warteschlange findet die Aktualisierung von w automatisch 
		// statt und es werden keine doppelten Elemente gepeichert.
		FUER_ALLE_VORWNACHBARN(n, graph, auswahl)
		{
			// Kantengewicht mit lambda addieren (Lagrange Relaxation) -> angepasste Kosten
			size_t kanten_index = n.j();
			// UNENDLICH bedeutet Kanten löschen -> bei großen Kantengewichten immer auf l_upper setzen
			// um Kanten nicht aus Versehen zu löschen
			double d_wert = d[auswahl] + graph.kante(kanten_index).gewicht + lambda[kanten_index];
			double d_wert_schranke = (d_wert < l_upper) ? (d_wert) : (l_upper);

			// Aktuallisierung vorg, d und w
			if (d[n.i()] > d_wert_schranke && lambda[kanten_index] < UNENDLICH)
			{
				d[n.i()] = d_wert_schranke;
				vorg[n.i()] = auswahl;
				w.push(n.i());
			}
		}
	}

	// Distanzen, Vorgaenger und Iterationszahl im Dijkstra-Objekt speichern
	DijkstraObj ergebnis;
	ergebnis.dist = d;
	ergebnis.vorg = vorg;
	ergebnis.iter_zahl = iter_zahl;

	return ergebnis;
}




// Subgradientenverfahren -------------------------------------------------
SubgradObj subGradOpt(const Graph& base_graph, const vector<pair<size_t, size_t>>& alle_demands, const vector<size_t>& geloeschte_knoten, const vector<double>& lambda_node_start,
	const vector<size_t>& terminals_grad, size_t max_iter, double obere_schranke, size_t n_iter_half, double l_upper, const LambdaZustand& lambda_option)
{
	// Output Wert
	SubgradObj ergebnis;
	ergebnis.zustand = KANDIDAT;
	ergebnis.gesamtkosten_zulaessig = UNENDLICH;


	// Kanten- und Knotengewichte initialisieren
	vector<double> lambda_edge(base_graph.anzKanten(), 0);
	vector<double> lambda_edge_neu(base_graph.anzKanten(), 0);
	vector<double> lambda_node = lambda_node_start;
	lambdaEdgeInitialisieren(base_graph, lambda_edge, lambda_edge_neu, lambda_node, geloeschte_knoten, l_upper);

	// Alle Terminalknoten im Set speichern
	unordered_set<size_t> alle_terminal_knoten;
	for (const pair<size_t, size_t>& terminal_z : alle_demands)
	{
		alle_terminal_knoten.insert(terminal_z.first);
		alle_terminal_knoten.insert(terminal_z.second);
	}

	// Alle Knotenzustände sind besetzt?
	size_t besetzte_knoten = alle_terminal_knoten.size() + geloeschte_knoten.size();
	bool alle_knoten_besetzt = (besetzte_knoten == base_graph.anzKnoten());


	// Variablen des Subgradientenverfahrens
	bool zulaessig = true;											// Ist gefundene Lösung zulässig (ja/nein)
	vector<size_t> weg_knoten_counter(base_graph.anzKnoten(), 0);	// Fluss in jedem Knoten zählen
	size_t start;													// Startknoten Dijkstra-Weg
	size_t ende;													// Endknoten Dijkstra-Weg
	DijkstraObj ergebnis_dijkstra;									// Ergebnis Dijkstra-Algorithmus angepasste Kosten
	Weg gefundener_weg;												// Weg aus Dijkstra Algorithmus
	bool fluss_erhalten;											// ist Fluss im Knoten <= 1
	double alpha = 0;												// alpha für Subgradientenverfahren
	double l_lambda = 0;											// Lagrange-Bound in jeder Iteration des Subgradientenverfahrens
	vector<int> subgradient(base_graph.anzKnoten(), 0);         	// Subgradient für Subgradientenverfahren
	double weg_kosten_summe = 0;									// Wert einer zulässigen Lösung
	bool optimal = false;											// Ist Lösung optimal?
	double theta = 2.0;												// Theta newtonische Schrittweite
	double subgradient_norm = 0;									// Norm des Subgradienten
	double l_lambda_before_n_iter_half = 0;							// Lagrange Bound merken für newtonische Schrittweite
	vector<size_t> demand_wege_kanten;								// Wegkanten jeder Lösung
	size_t counter_fluss = 0;										// Zählt Anzahl Konfliktknoten
	size_t counter_fluss_min = UINT_MAX;							// Bestimmung minimalen counter_fluss
	double min_zulaessig = UNENDLICH;								// Kosten zulässiger Lösung mit geringsten Gesamtkosten 




	// Subgradientenverfahren über max_iter Iterationen
	for (size_t k = 1; k <= max_iter; ++k)
	{
		// Variablen resetten für näachsten Durchlauf
		zulaessig = true;
		l_lambda = 0;
		counter_fluss = 0;
		demand_wege_kanten.clear();
		for (size_t c = 0; c < base_graph.anzKnoten(); c++)
		{
			weg_knoten_counter[c] = 0;
			subgradient[c] = 0;
		}
		subgradient_norm = 0;


		// Für alle Demands Dijkstra-Algorithmus mit angepassten Kosten lösen (Lagrange-Relaxation)
		for (const pair<size_t, size_t>& terminal : alle_demands)
		{
			// Start- und Endknoten der Wege
			start = terminal.first;
			ende = terminal.second;

			// Terminals nicht betretbar, außer Start- und Endterminal
			for (size_t terminal_knoten : alle_terminal_knoten)
			{
				if (terminal_knoten != start && terminal_knoten != ende)
				{
					FUER_ALLE_NACHBARN(n, base_graph, terminal_knoten)
					{
						lambda_edge[n.j()] = UNENDLICH;
					}
				}
			}

			// Dijkstra-Weg bilden
			ergebnis_dijkstra = dijkstra_lambda(base_graph, start, ende, lambda_edge, l_upper);
			gefundener_weg = wegFinden(base_graph, ergebnis_dijkstra.vorg, start, ende);

			// Wenn kein Weg gefunden werden kann gibt es keine Lösung 
			if (gefundener_weg.weg_knoten.empty())
			{
				ergebnis.zustand = UNZULAESSIG;
				ergebnis.untere_schranke = UNENDLICH;
				return ergebnis;
			}

			// Kosten der Dijkstra-Wege aufaddieren
			l_lambda += ergebnis_dijkstra.dist[ende];

			// Wegkanten aller Dijkstra-Wege speichern
			for (size_t kante : gefundener_weg.weg_kanten)
			{
				demand_wege_kanten.push_back(kante);
			}

			// Alle Knoten bis auf Terminals im Counter hochzählen 
			// Flusserhöhung in Dijkstra-Weg-Knoten und überprüfen, ob Lösung zulässig
			for (size_t element : gefundener_weg.weg_knoten)
			{
				// Nicht Terminalknoten
				if (element != start && element != ende)
				{
					// Bleibt Lösung zulässig
					weg_knoten_counter[element]++;
					fluss_erhalten = (weg_knoten_counter[element] <= 1);
					zulaessig = zulaessig && fluss_erhalten;

					// Subgradienten anpassen 
					subgradient[element] += 2;
				}
				// Terminalknoten
				else
				{
					// Bleibt Lösung zulässig
					weg_knoten_counter[element]++;
					fluss_erhalten = (weg_knoten_counter[element] <= terminals_grad[element]);
					zulaessig = zulaessig && fluss_erhalten;

					// Subgradienten anpassen (Terminals)
					subgradient[element] += 1;
				}
			}

			//Alle gelöschten Terminalknoten wieder zurücksetzen
			lambda_edge = lambda_edge_neu;
		}

		// OPTION Lambda zur minimalen Anzahl der Konflikte soll gespeichert werden
		if (lambda_option == LAMBDA_KONFLIKT)
		{
			// Anzahl Konflikte berechnen Terminalknoten immer mit dabei (konstanter Shift)
			for (size_t fluss : weg_knoten_counter)
			{
				if (fluss > 1)
				{
					counter_fluss++;
				}
			}

			// Update für minimal durchflossene Instanz
			if (counter_fluss < counter_fluss_min)
			{
				counter_fluss_min = counter_fluss;
				ergebnis.lambda = lambda_node;
			}
		}


		// Negative Konstante des Lagrange Bounds + Subgradient Update
		for (size_t index = 0; index < terminals_grad.size(); index++)
		{
			l_lambda -= terminals_grad[index] * lambda_node[index];
			subgradient[index] -= terminals_grad[index];
			subgradient_norm += subgradient[index] * subgradient[index];
		}

		// Instanz ist zulässig?
		if (zulaessig)
		{
			// Gesamtkosten der zulässigen Lösung aus Kanten im Basis-Graphen
			weg_kosten_summe = 0;
			for (size_t e = 0; e < demand_wege_kanten.size(); e++)
			{
				weg_kosten_summe += base_graph.kante(demand_wege_kanten[e]).gewicht;
			}

			// Zulässige Lösung mit geringsten Gesamtkosten wird gespeichert
			if (weg_kosten_summe < min_zulaessig)
			{
				min_zulaessig = weg_kosten_summe;
				ergebnis.gesamtkosten_zulaessig = weg_kosten_summe;
				ergebnis.zulaessige_loesung = demand_wege_kanten;

				// Update von Lambda bei Option LAMBDA_KONFLIKT
				if (lambda_option == LAMBDA_KONFLIKT)
				{
					ergebnis.lambda = lambda_node;
				}
			}

			// Prüfung auf Optimalität der Lösung für Subinstanz
			optimal = true;
			for (size_t w = 0; w < base_graph.anzKnoten(); w++)
			{
				if (subgradient[w] != 0 && lambda_node[w] != 0)
				{
					optimal = false;
					break;
				}
			}

			//Optimale Subinstanz zurückgeben oder wenn alle Knotenzustände besetzt sind
			if (optimal || alle_knoten_besetzt)
			{
				ergebnis.zustand = OPTIMAL;
				ergebnis.untere_schranke = weg_kosten_summe;
				return ergebnis;
			}
		}

		// Erster Loop -> l_lambda_before_n_iter_half, untere Schranke und lambda initialisieren 
		if (k == 1)
		{
			l_lambda_before_n_iter_half = l_lambda;
			ergebnis.untere_schranke = l_lambda;
			ergebnis.lambda = lambda_node;
		}

		// Update maximale Gesamtkosten des Lagrange Bounds (max. untere Grenze) 
		if (ergebnis.untere_schranke < l_lambda)
		{
			// Update maximale untere Schranke und BB Instanz
			ergebnis.untere_schranke = l_lambda;

			// OPTION lambda zur maximalen unteren Schranke soll gespeichert werden
			if (lambda_option == LAMBDA_MAX)
			{
				ergebnis.lambda = lambda_node;
			}
		}

		// Sollte Instanz suboptimal werden kann Subgradientenverfahren früher abgebrochen werden
		if (ergebnis.untere_schranke >= obere_schranke)
		{
			break;
		}

		// Update Theta nach n_iter_half Iterationen, falls sich l_max nicht verbessern sollte
		if (k % n_iter_half == 0)
		{
			if (ergebnis.untere_schranke > l_lambda_before_n_iter_half)
			{
				l_lambda_before_n_iter_half = ergebnis.untere_schranke;
			}
			else
			{
				theta /= 2.0;
			}
		}

		// Update Schrittweite
		alpha = theta * (l_upper - l_lambda) / subgradient_norm;

		// Update Lambda Subgradienten lambda_node in jedem Knoten für nächsten Loop
		for (int j = 0; j < base_graph.anzKnoten(); j++)
		{
			lambda_node[j] += alpha * subgradient[j];

			// Lambda ist immer positiv (Lagrange Relaxation)
			if (lambda_node[j] < 0)
			{
				lambda_node[j] = 0;
			}

			// UNENDLICH entspricht Löschen -> lambdas sollen l_upper nicht überschreiten 
			if (lambda_node[j] >= l_upper)
			{
				lambda_node[j] = l_upper;
			}
		}

		// Ändern von lambda_edge für nächste Iteration aus lambda_node
		lambdaEdgeInitialisieren(base_graph, lambda_edge, lambda_edge_neu, lambda_node, geloeschte_knoten, l_upper);

	}

	// OPTION Lambda wird bei dieser Option immer auf 0 gesetzt
	if (lambda_option == LAMBDA_NULL)
	{
		lambda_node.assign(lambda_node.size(), 0);
		ergebnis.lambda = lambda_node;
	}

	return ergebnis;
}



// Lambda_edge aus lambda_node generieren
void lambdaEdgeInitialisieren(const Graph& base_graph, vector<double>& lambda_edge,
	vector<double>& lambda_edge_neu, const vector<double>& lambda_node, const vector<size_t>& geloeschte_knoten,
	double l_upper)
{
	// Initialisierung von lambda_edge für Dijkstra-Algorithmus aus lambda_node
	size_t fuss_index = base_graph.anzKnoten();
	size_t kopf_index = base_graph.anzKnoten();
	double lambda_wert = 0;

	FUER_ALLE_IKANTEN(b, base_graph)
	{
		// Kopf - und Fussknoten der Kanten finden
		fuss_index = base_graph.kante(b).iFuss();
		kopf_index = base_graph.kante(b).iKopf();
		lambda_wert = lambda_node[fuss_index] + lambda_node[kopf_index];

		// Neue Lambdas für Kanten setzen 
		// UNENDLICH entspricht Kante löschen -> lambdas sollen l_upper nicht überschreiten 
		lambda_edge[b] = (lambda_wert < l_upper) ? (lambda_wert) : (l_upper);
		lambda_edge_neu[b] = lambda_edge[b];
	}

	// Gelöschte Knoten einsetzten
	for (size_t loesch_element : geloeschte_knoten)
	{
		FUER_ALLE_NACHBARN(n, base_graph, loesch_element)
		{
			lambda_edge_neu[n.j()] = UNENDLICH;
			lambda_edge[n.j()] = UNENDLICH;
		}
	}
}

