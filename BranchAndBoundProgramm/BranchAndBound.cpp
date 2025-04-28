#include "BranchAndBound.h"
#include "graphAlgorithmen.h"
#include <unordered_set>
#include <algorithm>

// Branch and Bound Verfahren -----------------------------------------------------------------------

BranchBoundErgebnis branchAndBound(const Graph& base_graph, const Graph& demand_graph,
	size_t max_iter_sub, size_t batch_size, size_t n_iter_half,
	const LambdaZustand& lambda_option, const VerzweigungZustand& verzweigung_option)
{
	// Endergebnis B&B initialisieren
	BranchBoundErgebnis ergebnis;
	ergebnis.distanz = -1;


	// Fehlermeldungen
	if (batch_size > max_iter_sub || batch_size == 0)
	{
		throw invalid_argument("n_sub' immer kleiner gleich n_sub und groesser 0!");
	}
	if (max_iter_sub == 0)
	{
		throw invalid_argument("n_sub immer groesser 0!");
	}
	if (n_iter_half == 0)
	{
		throw invalid_argument("n_iter_half immer groesser 0!");
	}


	// Obere Schranke  
	double kanten_minimum = minKantenGewicht(base_graph);
	double obere_schranke = kantenSummeUpgrade(base_graph, demand_graph);
	double kanten_summe = kantenSumme(base_graph);

	// l_upper muss größer sein, als obere_schranke
	double l_upper = (obere_schranke + kanten_minimum > kanten_summe) ? (obere_schranke + kanten_minimum) : (kanten_summe);


	// Verfahren funktioniert nicht
	if (l_upper >= UNENDLICH)
	{
		throw invalid_argument("Kantengewichte des Basisgrpahen zu groß. Branch and Bound nicht anwendbar!");
	}

	// Counter für Anzahl an Iterationen des B&B
	size_t counter = 0;


	// Für Subgradientenverfahren Knoten Grad zuordnen
	// Terminalknoten Grad = Grad des Demand-Demandgraphen (Anzahl der Demands)
	// Nicht-Terminalknoten Grad = 2
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


	// Warteschlange initialisieren
	WarteschlangeBranchAndBound warteschlangeBB;
	warteschlangeBB.setObereSchranke(obere_schranke);


	// Anfangsinstanzen für Warteschlange erzeugen
	BB_Instanz instanz_anfang;


	// Alle Demands durchgehen und instanz_anfang initialisieren
	instanzTerminalsInitialisieren(base_graph, demand_graph, instanz_anfang);


	// Startinstanz bilden 
	VerzweigungsErgebnis instanz_auswahl = instanzBilden(base_graph, instanz_anfang, base_graph.anzKnoten(), -1, terminals_grad, max_iter_sub, obere_schranke, n_iter_half, l_upper, lambda_option);

	// Unzulässige oder suboptimale Lösung am Anfang -> es gibt keine Lösung
	if (instanz_auswahl.verzweigte_subinstanz[0].instanz_zustand == UNZULAESSIG
		|| instanz_auswahl.verzweigte_subinstanz[0].untere_schranke >= obere_schranke)
	{
		return ergebnis;
	}


	// Optimale Lösung am Anfang -> Ende des B&B
	if (instanz_auswahl.verzweigte_subinstanz[0].instanz_zustand == OPTIMAL)
	{
		// Ergebnis initialisieren
		ergebnis.distanz = instanz_auswahl.gesamtkosten_zulaessig;
		ergebnis.demand_wege_kanten = instanz_auswahl.zulaessige_loesung;
		return ergebnis;
	}

	// Start Instanz zur Warteschlange hinzufügen (muss Kandidat sein)
	warteschlangeBB.push(instanz_auswahl.verzweigte_subinstanz[0]);


	// Größen für Loop initialisieren 
	double aktueller_wert = instanz_auswahl.verzweigte_subinstanz[0].untere_schranke;
	BranchBoundErgebnis ergebnis_obere_schranke;
	vector<vector<size_t>> wege_reihenfolge;


	// Ausgabe von Kennwerten in jeder Iteration
	cout << "------------------------------------------------------------------------------------------------------" << endl;
	cout << "| Itteration | Warteschlangen Groesse | Untere Schranke | Obere Schranke | Anzahl Verzweigungsknoten |" << endl;
	cout << "------------------------------------------------------------------------------------------------------" << endl;

	BB_Instanz verzweigende_subinstanz;
	VZK verzweigung_knoten;
	VerzweigungsErgebnis verzweigte_subinstanzen;
	double obere_schranke_kopie = warteschlangeBB.getObereSchranke();

	// Solange Warteschlange noch nicht leer ist wird B&B ausgeführt
	while (!warteschlangeBB.empty())
	{
		// Subinstanz mit kleinster unterer Schranke zum Verzweigen auswählen
		verzweigende_subinstanz = warteschlangeBB.pop();

		//Ausgabe der Kennwerte
		aktueller_wert = verzweigende_subinstanz.untere_schranke;
		cout << counter << "    ";
		cout << warteschlangeBB.size() + 1 << "    ";
		cout << aktueller_wert << "    ";
		cout << obere_schranke_kopie << "    ";

		// Verzweigungsknoten bilden
		verzweigung_knoten = verzweigungKnotenBilden(base_graph, verzweigende_subinstanz, terminals_grad, warteschlangeBB.getObereSchranke(), batch_size, n_iter_half, l_upper, lambda_option, verzweigung_option);

		// Update obere Schranke aus Verzweigungsoption KONFLIKTE
		if (verzweigung_option == KONFLIKTE)
		{
			warteschlangeBB.updateObereSchranke(verzweigung_knoten.gesamtkosten_zulaessig, verzweigung_knoten.zulaessige_loesung);
		}

		//Subinstanz am Verzweigungsknoten verzweigen
		verzweigte_subinstanzen = instanzVerzweigen(base_graph, verzweigung_knoten.vzk, verzweigende_subinstanz, warteschlangeBB.getObereSchranke(), terminals_grad, max_iter_sub, n_iter_half, l_upper, lambda_option);

		// Subinstanzen zur Warteschlange hinzufügen
		for (BB_Instanz element : verzweigte_subinstanzen.verzweigte_subinstanz)
		{
			warteschlangeBB.push(element);
		}

		// Update obere Schranke aus verzweigten Subinstanten mit Entfernen suboptimaler Subinstanzen
		warteschlangeBB.updateObereSchranke(verzweigte_subinstanzen.gesamtkosten_zulaessig, verzweigte_subinstanzen.zulaessige_loesung);


		// Wenn sich kleinste obere Schranke ändert -> Ausgabe der zulässigen Lösung
		if (warteschlangeBB.getObereSchranke() < obere_schranke_kopie)
		{
			// Ausgabe der neuen globalen oberen Schranke
			cout << endl;
			ergebnis_obere_schranke.distanz = warteschlangeBB.getObereSchranke();
			cout << "Kleinste obere Schranke: " << ergebnis_obere_schranke.distanz << endl;

			// Ausgabe der Wege der eingebetteten Demands als Kantenindices des Basisgraphen 
			ergebnis_obere_schranke.demand_wege_kanten = warteschlangeBB.getOptWegKanten();
			wege_reihenfolge = kantenAusgabe(ergebnis_obere_schranke, base_graph, demand_graph);
			cout << "Die Kantenindices der neuen oberen Schranke (zulaessige Loesung) sind: " << endl;

			for (size_t t = 0; t < wege_reihenfolge.size(); t++)
			{
				for (size_t ausgabe : wege_reihenfolge[t])
				{
					cout << ausgabe << " ";
				}
				cout << endl;
			}

			cout << endl << endl;
		}

		// Update obere Schranke für die Ausgabe
		obere_schranke_kopie = warteschlangeBB.getObereSchranke();

		counter++;
	}

	// Endergebnis initialisieren, wenn es eine optimale Lösung besitzt
	if (!warteschlangeBB.getOptWegKanten().empty())
	{
		ergebnis.distanz = warteschlangeBB.getObereSchranke();
		ergebnis.demand_wege_kanten = warteschlangeBB.getOptWegKanten();
	}

	return ergebnis;
}



// Anfangsinstanz mit Demands des Demand-Graphen und Start-Lambda initialisieren
void instanzTerminalsInitialisieren(const Graph& base_graph, const Graph& demand_graph, BB_Instanz& instanz_anfang)
{
	FUER_ALLE_IKANTEN(d, demand_graph)
	{
		// Terminal Indices im Demand Graphen
		size_t start_knoten_index = demand_graph.kante(d).iFuss();
		size_t end_knoten_index = demand_graph.kante(d).iKopf();

		// Terminal Namen im Demand Graphen
		string start_knoten_name = demand_graph.knoten(start_knoten_index).name();
		string end_knoten_name = demand_graph.knoten(end_knoten_index).name();

		// Terminal Indices im Base Graphen
		size_t base_start = base_graph.iKnoten(start_knoten_name);
		size_t base_end = base_graph.iKnoten(end_knoten_name);

		// demand als Index-Paar in Instanz aufnehmen
		pair<size_t, size_t> paar = make_pair(base_start, base_end);
		instanz_anfang.alle_demands.push_back(paar);
	}

	// Anfangswerte für lambda für das Subgradientenverfahren festlegen
	vector<double> lambda_node(base_graph.anzKnoten(), 0);
	instanz_anfang.lambda_node_start = lambda_node;
}



// Verzweigungsknoten auswählen
VZK verzweigungKnotenBilden(const Graph& base_graph, const BB_Instanz& instanz_auswahl,
	const vector<size_t>& terminals_grad, double obere_schranke, size_t batch_size,
	size_t n_iter_half, double l_upper, const LambdaZustand& lambda_option,
	const VerzweigungZustand& verzweigung_option)
{
	VZK verzweigungs_knoten;
	verzweigungs_knoten.gesamtkosten_zulaessig = obere_schranke;
	verzweigungs_knoten.vzk = base_graph.anzKnoten();

	// Suchen den Knoten, der die kleinste untere Schranke nach Verzweigung maximal erhöht
	double max_untere_schranke = (-1) * UNENDLICH;

	// Größen für Loop, zum Suchen der kleinsten unteren Schranke nach Verzweigung
	double kleinster_weg_dist = UNENDLICH;
	size_t knoten_kleinste_dist = base_graph.anzKnoten();

	// Alle Kandidaten, die als Verzweigungsknoten möglich sind
	vector<size_t> weg_knoten = verzweigungsKnotenFinden(base_graph, instanz_auswahl, l_upper, verzweigung_option);
	cout << weg_knoten.size() << endl;

	// Bei einem Verzweigungsknoten ist Wahl klar
	if (weg_knoten.size() == 1)
	{
		verzweigungs_knoten.vzk = weg_knoten[0];
		return verzweigungs_knoten;
	}

	// Jeden Verzweigungsknoten durchgehen 
	// -> Verzweigung durchführen 
	// -> kleinste untere Schranke nach Verzweigung bilden
	// -> Speicher den Knoten, der nach Verzweigung größten Wert der kleinsten untere Schranke hat
	for (size_t knoten : weg_knoten)
	{
		// Reset der Werte
		kleinster_weg_dist = UNENDLICH;
		knoten_kleinste_dist = knoten;

		// Instanz verzweigen
		VerzweigungsErgebnis knoten_verzweigen = instanzVerzweigen(base_graph, knoten_kleinste_dist, instanz_auswahl,
			verzweigungs_knoten.gesamtkosten_zulaessig, terminals_grad, batch_size, n_iter_half, l_upper,
			lambda_option);

		// Speichern der zulässigen Lösung mit geringsten Gesamtkosten
		// Update globale obere Schranke mit zulässiger Lösung
		if (!knoten_verzweigen.zulaessige_loesung.empty()
			&& knoten_verzweigen.gesamtkosten_zulaessig < verzweigungs_knoten.gesamtkosten_zulaessig)
		{
			verzweigungs_knoten.gesamtkosten_zulaessig = knoten_verzweigen.gesamtkosten_zulaessig;
			verzweigungs_knoten.zulaessige_loesung = knoten_verzweigen.zulaessige_loesung;
		}

		// Update kleinste untere Schranke nach Verzweigung
		for (BB_Instanz subinstanz : knoten_verzweigen.verzweigte_subinstanz)
		{
			if (subinstanz.untere_schranke < kleinster_weg_dist)
			{
				kleinster_weg_dist = subinstanz.untere_schranke;
			}
		}

		// Entstehen keine verzweigbaren Subinstanzen dahin verzweigen
		if (knoten_verzweigen.verzweigte_subinstanz.empty())
		{
			verzweigungs_knoten.vzk = knoten_kleinste_dist;
			break;
		}

		// Update Knoten mit größten Wert der kleinsten unteren Schranke nach Verzweigung 
		if (kleinster_weg_dist > max_untere_schranke)
		{
			max_untere_schranke = kleinster_weg_dist;
			verzweigungs_knoten.vzk = knoten_kleinste_dist;
		}
	}

	// Sollte kein Knoten gefunden werden, einfach letzten Knoten wählen
	if (verzweigungs_knoten.vzk == base_graph.anzKnoten())
	{
		verzweigungs_knoten.vzk = knoten_kleinste_dist;
	}

	return verzweigungs_knoten;
}


// Kandidaten für Verwzeigungsknoten
vector<size_t> verzweigungsKnotenFinden(const Graph& base_graph, const BB_Instanz& neue_instanz, double l_upper, const VerzweigungZustand& verzweigung_option)
{
	// Kanten- und Knotengewichte initialisieren
	vector<double> lambda_edge(base_graph.anzKanten(), 0);
	vector<double> lambda_edge_neu(base_graph.anzKanten(), 0);
	lambdaEdgeInitialisieren(base_graph, lambda_edge, lambda_edge_neu, neue_instanz.lambda_node_start, neue_instanz.geloeschte_knoten, l_upper);

	// Alle Terminalknoten im Set speichern 
	unordered_set<size_t> alle_terminal_knoten;
	for (const pair<size_t, size_t>& terminal_z : neue_instanz.alle_demands)
	{
		alle_terminal_knoten.insert(terminal_z.first);
		alle_terminal_knoten.insert(terminal_z.second);
	}

	// Wegknoten, Verzweigungsknoten und Flusszähler in jeden Knoten
	unordered_set<size_t> weg_knoten;
	vector<size_t> weg_knoten_counter(base_graph.anzKnoten(), 0);
	vector<size_t> verzweigungs_knoten;

	// Knoten mit Weg maximaler Länge und Knoten mit max. Fluss speichern
	size_t demand_weg_laenge_max = 0;
	size_t knoten_demand_weg_laenge_max = base_graph.anzKnoten();
	size_t max_fluss = 0;
	size_t knoten_max_fluss = base_graph.anzKnoten();


	for (const pair<size_t, size_t>& terminal : neue_instanz.alle_demands)
	{
		// Start- und Endknoten der Wege
		size_t start = terminal.first;
		size_t ende = terminal.second;

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
		DijkstraObj ergebnis_dijkstra = dijkstra_lambda(base_graph, start, ende, lambda_edge, l_upper);
		Weg gefundener_weg = wegFinden(base_graph, ergebnis_dijkstra.vorg, start, ende);

		// Wegknoten speichern auf Dijkstra-Wegen (keine Terminals)
		for (size_t knoten : gefundener_weg.weg_knoten)
		{
			if (knoten != start && knoten != ende)
			{
				weg_knoten.insert(knoten);
			}
		}

		// Längsten Dijkstra-Weg ermitteln -> davon mittleren Knoten -> mindestens 3 Knoten im Weg
		if (gefundener_weg.weg_knoten.size() > demand_weg_laenge_max
			&& gefundener_weg.weg_knoten.size() > 2)
		{
			demand_weg_laenge_max = gefundener_weg.weg_knoten.size();

			// Von dem längsten Weg mittleren Knoten speichern
			knoten_demand_weg_laenge_max = gefundener_weg.weg_knoten[int((gefundener_weg.weg_knoten.size() - 1) / 2)];
		}

		// Fluss in Wegknoten bestimmen
		for (size_t element : gefundener_weg.weg_knoten)
		{
			if (element != start && element != ende)
			{
				weg_knoten_counter[element]++;

				// Maximalen Fluss bestimmen
				if (weg_knoten_counter[element] > max_fluss)
				{
					max_fluss = weg_knoten_counter[element];
					knoten_max_fluss = element;
				}
			}
		}


		//Alle gelöschten Terminal-Knoten wieder zurücksetzen
		lambda_edge = lambda_edge_neu;
	}



	// Je nach Verzweigungs Option anderer Verzweigungsknoten
	if (verzweigung_option == MAX_FLUSS)
	{
		//Knoten mit maximalen Fluss ist Verzweigungsknoten, wenn er existiert
		if (knoten_max_fluss != base_graph.anzKnoten())
		{
			verzweigungs_knoten.push_back(knoten_max_fluss);
		}
		else
		{
			// beliebiger Verzweigungsknoten
			verzweigungs_knoten.push_back(beliebigerVerzweigungsKnoten(base_graph, neue_instanz));
		}

	}
	else if (verzweigung_option == WEG)
	{
		//Mittlerer Knoten des längsten Weges des eingebetteten Demands
		if (knoten_demand_weg_laenge_max != base_graph.anzKnoten())
		{
			verzweigungs_knoten.push_back(knoten_demand_weg_laenge_max);
		}
		else
		{
			// beliebiger Verzweigungsknoten
			verzweigungs_knoten.push_back(beliebigerVerzweigungsKnoten(base_graph, neue_instanz));
		}
	}
	else
	{
		// Nur mehrfach durchflossene wählbare Wegknoten als Kandidaten für Verzweigungsknoten wählen
		size_t anzahl = 0;
		for (size_t knoten : weg_knoten)
		{
			anzahl++;
			if (weg_knoten_counter[knoten] >= 2)
			{
				verzweigungs_knoten.push_back(knoten);
			}
		}

		// Spezialfall zulässige Lösung -> keine Konflikte
		if (verzweigungs_knoten.empty())
		{
			//Mittlerer Knoten des längsten Weges des eingebetteten Demands
			if (knoten_demand_weg_laenge_max != base_graph.anzKnoten())
			{
				verzweigungs_knoten.push_back(knoten_demand_weg_laenge_max);
			}
			else
			{
				// beliebiger Verzweigungsknoten
				verzweigungs_knoten.push_back(beliebigerVerzweigungsKnoten(base_graph, neue_instanz));
			}
		}
	}

	return verzweigungs_knoten;
}


// Wahl eines beliebigen Verzweigungsknotens
size_t beliebigerVerzweigungsKnoten(const Graph& base_graph, const BB_Instanz& instanz_auswahl)
{
	// Es muss mindestens ein weiterer nicht gelöschter Nicht-Terminalknoten existieren,
	// sonst wäre keine Verzweigung möglich gewesen
	unordered_set<size_t> alle_nicht_waehlbaren_knoten;

	// Alle Terminals nicht wählbar
	for (const pair<size_t, size_t>& terminal_z : instanz_auswahl.alle_demands)
	{
		alle_nicht_waehlbaren_knoten.insert(terminal_z.first);
		alle_nicht_waehlbaren_knoten.insert(terminal_z.second);
	}

	// Alle gelöschten Knoten nicht wählbar
	for (size_t element : instanz_auswahl.geloeschte_knoten)
	{
		alle_nicht_waehlbaren_knoten.insert(element);
	}

	// Wähle ersten Knoten aus Basis-Graphen der nicht in alle_nicht_waehlbaren_knoten ist
	for (size_t beliebiger_knoten = 0; beliebiger_knoten < base_graph.anzKnoten(); ++beliebiger_knoten)
	{
		if (alle_nicht_waehlbaren_knoten.find(beliebiger_knoten) == alle_nicht_waehlbaren_knoten.end())
		{
			return beliebiger_knoten;
		}
	}
}



// Instanz beim Verzweigungsknoten verzweigen
VerzweigungsErgebnis instanzVerzweigen(const Graph& base_graph, size_t verzweigung_knoten, const BB_Instanz& instanz_auswahl,
	double obere_schranke, const vector<size_t>& terminals_grad, size_t max_iter_sub, size_t n_iter_half, double l_upper, const LambdaZustand& lambda_option)
{
	// Ergebnis der Verzweigung
	VerzweigungsErgebnis ergebnis;
	ergebnis.gesamtkosten_zulaessig = obere_schranke;


	// Verzweigungsknoten als Zwischenterminal jedes Demands einfügen oder  Verzweigungsknoten löschen
	// -> Neue Subinstanzen erzeugen
	for (int i = 0; i <= instanz_auswahl.alle_demands.size(); i++)
	{
		// neue Subinstanz bilden
		VerzweigungsErgebnis neue_instanz = instanzBilden(base_graph, instanz_auswahl, verzweigung_knoten, i, terminals_grad, max_iter_sub, ergebnis.gesamtkosten_zulaessig, n_iter_half, l_upper, lambda_option);

		// Update globale obere Schranke mit zulässiger Lösung
		if (!neue_instanz.zulaessige_loesung.empty()
			&& neue_instanz.gesamtkosten_zulaessig < ergebnis.gesamtkosten_zulaessig)
		{
			ergebnis.gesamtkosten_zulaessig = neue_instanz.gesamtkosten_zulaessig;
			ergebnis.zulaessige_loesung = neue_instanz.zulaessige_loesung;
		}

		// Sammeln der verzweigbaren Subinstanzen
		// Untere Schranke = obere Schranke -> optimale Lösung der Subinstanz -> ist nicht Kandidat
		if (neue_instanz.verzweigte_subinstanz[0].instanz_zustand == KANDIDAT
			&& neue_instanz.verzweigte_subinstanz[0].untere_schranke < ergebnis.gesamtkosten_zulaessig)
		{
			ergebnis.verzweigte_subinstanz.push_back(neue_instanz.verzweigte_subinstanz[0]);
		}
	}

	return ergebnis;
}



// Neue Subinstanz bilden
VerzweigungsErgebnis instanzBilden(const Graph& base_graph, const BB_Instanz& instanz_auswahl, size_t verzweigung_knoten, int demand_index, const vector<size_t>& terminals_grad,
	size_t max_iter, double obere_schranke, size_t n_iter_half, double l_upper, const LambdaZustand& lambda_option)
{
	// Subinstanz initialisieren aus zu verzweigender Subinstanz instanz_auswahl
	BB_Instanz neue_instanz;
	neue_instanz.setAttribut(instanz_auswahl.alle_demands, instanz_auswahl.geloeschte_knoten, 0, KANDIDAT, instanz_auswahl.lambda_node_start);


	// Verzweigungsknoten in Subinstanz einfügen
	if (demand_index == -1)
	{
		// Kein Verzweigungsknoten (Gesamtinstanz)
	}
	else if (demand_index < instanz_auswahl.alle_demands.size())
	{
		// Verzweigungsknoten als Zwischenterminal einfügen
		// Neues Terminal bei demand_index geht vom Start zum Verzweigungsknoten und vom Verzweigungsknoten zum Ende
		// es werden 2 demands eingefügt und alter demand gelöscht
		pair<size_t, size_t> terminal = instanz_auswahl.alle_demands[demand_index];
		neue_instanz.alle_demands.erase(neue_instanz.alle_demands.begin() + demand_index);
		neue_instanz.alle_demands.push_back(make_pair(terminal.first, verzweigung_knoten));
		neue_instanz.alle_demands.push_back(make_pair(verzweigung_knoten, terminal.second));
	}
	else
	{
		// Verzweigungsknoten wird gelöscht
		neue_instanz.geloeschte_knoten.push_back(verzweigung_knoten);
	}

	// Subgradientenverfahren
	SubgradObj ergebnis_subgradient = subGradOpt(base_graph, neue_instanz.alle_demands, neue_instanz.geloeschte_knoten, neue_instanz.lambda_node_start,
		terminals_grad, max_iter, obere_schranke, n_iter_half, l_upper, lambda_option);

	// Neue Werte der Subinstanz setzen
	neue_instanz.instanz_zustand = ergebnis_subgradient.zustand;
	neue_instanz.lambda_node_start = ergebnis_subgradient.lambda;
	neue_instanz.untere_schranke = ergebnis_subgradient.untere_schranke;

	// Ergebnis der Verzweigung
	VerzweigungsErgebnis ergebnis;
	ergebnis.verzweigte_subinstanz.push_back(neue_instanz);
	ergebnis.zulaessige_loesung = ergebnis_subgradient.zulaessige_loesung;
	ergebnis.gesamtkosten_zulaessig = ergebnis_subgradient.gesamtkosten_zulaessig;

	return ergebnis;
}














// Ausgabe der Kanten der optimalen Lösung -----------------------------------------------
vector<vector<size_t>> kantenAusgabe(const BranchBoundErgebnis& ergebnis, const Graph& base_graph, const Graph& demand_graph)
{
	// Kanten und Knoten der Terminals im Basis-Graphen speichern
	unordered_set<size_t> terminal_kanten;
	unordered_set<size_t> terminal_knoten;

	// Terminalkanten und Terminalknoten initialisieren
	FUER_ALLE_IKNOTEN(t, demand_graph)
	{
		// Terminal Namen im Demand Graphen
		string t_name = demand_graph.knoten(t).name();

		// Terminal Indices im Base Graphen
		size_t base_t = base_graph.iKnoten(t_name);

		// Terminalknoten
		terminal_knoten.insert(base_t);

		// Terminalkanten
		FUER_ALLE_NACHBARN(n, base_graph, base_t)
		{
			terminal_kanten.insert(n.j());
		}
	}

	// Alle Demand-Wege im Set speichern
	unordered_set<size_t> alle_demand_weg_kanten;
	for (size_t i = 0; i < ergebnis.demand_wege_kanten.size(); i++)
	{
		alle_demand_weg_kanten.insert(ergebnis.demand_wege_kanten[i]);
	}

	// Aus Ergebnis Wege erstellen -> jede Kante kommt nur einmal vor -> schon_gesetzte_wegkanten speichern
	unordered_set<size_t> schon_gesetzte_wegkanten;
	vector<vector<size_t>> erstellte_wege;
	vector<size_t> weg;
	size_t fuss_knoten = base_graph.anzKnoten();
	size_t kopf_knoten = base_graph.anzKnoten();
	size_t wahl_knoten = base_graph.anzKnoten();
	size_t naechste_kante = base_graph.anzKanten();


	// Alle Weg Kanten der demands der Lösung durchgehen
	// Terminalkanten identifizieren -> Start des Weges für den Demand
	// Knoten der Terminalkante zur nächster Wegkante identifizieren und nächste Kante finden
	// gelangt man wieder zu einer Terminalkante -> Ende des Weges

	for (size_t kante : alle_demand_weg_kanten)
	{
		// Ist erste Kante Terminal-Kante und noch für keinen Weg verwendet worden, so beginnt dort der Weg
		if (terminal_kanten.find(kante) != terminal_kanten.end()
			&& schon_gesetzte_wegkanten.find(kante) == schon_gesetzte_wegkanten.end())
		{
			// Werte resetten
			fuss_knoten = base_graph.anzKnoten();
			kopf_knoten = base_graph.anzKnoten();
			wahl_knoten = base_graph.anzKnoten();
			naechste_kante = base_graph.anzKanten();
			weg.clear();

			// Terminalkante als Start hinzufügen zum Weg
			weg.push_back(kante);
			schon_gesetzte_wegkanten.insert(kante);

			// Fussknoten, Kopfknoten der Kante ermitteln -> Anfangsknoten der nächsten Kante (wahl_knoten)
			fuss_knoten = base_graph.kante(kante).iFuss();
			kopf_knoten = base_graph.kante(kante).iKopf();


			// Wenn beide Knoten Terminalknoten sind -> Weg zu Ende mit einer Kante
			// Andernfalls führt Weg weiter
			if (terminal_knoten.find(fuss_knoten) == terminal_knoten.end()
				|| terminal_knoten.find(kopf_knoten) == terminal_knoten.end())
			{
				// Nächste Kante des Weges finden mit dem Knoten, der kein Terminalknoten ist (wahl_knoten)
				wahl_knoten = (terminal_knoten.find(fuss_knoten) == terminal_knoten.end()) ? (fuss_knoten) : (kopf_knoten);

				// Nächste Kante des Weges ist in Kante zu Nachbar des Wahlknotens
				FUER_ALLE_NACHBARN(n, base_graph, wahl_knoten)
				{
					// Kanten darf noch nicht Teil eines Weges sein und muss Teil der Demand Kantenmenge sein
					// Wegen Knotendisjunktheit gibt es nur eine solche weitere Kante
					if (schon_gesetzte_wegkanten.find(n.j()) == schon_gesetzte_wegkanten.end()
						&& alle_demand_weg_kanten.find(n.j()) != alle_demand_weg_kanten.end())
					{
						// Nächste Kante des Weges bestimmen und hinzufügen
						// Nächster Knoten ist automatisch Nachbarknoten (wahl_knoten)
						naechste_kante = n.j();
						wahl_knoten = n.i();
						weg.push_back(naechste_kante);
						schon_gesetzte_wegkanten.insert(naechste_kante);
						break;
					}
				}


				// Solange nächste Kante keine Terminalkante ist, endet der Weg nicht
				while (terminal_kanten.find(naechste_kante) == terminal_kanten.end())
				{
					// Nächste Kante des Weges ist in Kante zu Nachbar des Wahlknotens
					FUER_ALLE_NACHBARN(n, base_graph, wahl_knoten)
					{
						// Kanten darf noch nicht Teil eines Weges sein und muss Teil der Demand Kantenmenge sein
						// Wegen Knotendisjunktheit gibt es nur eine solche weitere Kante
						if (schon_gesetzte_wegkanten.find(n.j()) == schon_gesetzte_wegkanten.end()
							&& alle_demand_weg_kanten.find(n.j()) != alle_demand_weg_kanten.end())
						{
							naechste_kante = n.j();
							wahl_knoten = n.i();
							weg.push_back(naechste_kante);
							schon_gesetzte_wegkanten.insert(naechste_kante);
							break;
						}
					}
				}
			}

			// Weg wurde aus Kanten erstellt -> eingebetteter Weg zu erstellte_wege hinzufügen
			erstellte_wege.push_back(weg);
		}
	}


	// Wege in die Reihenfolge der Demands im Demand-Graphen bringen -> jedem Demand lässt sich eindeutig Weg zuordnen
	vector<vector<size_t>> wege_reihenfolge;
	unordered_set<size_t> alle_knoten;

	FUER_ALLE_IKANTEN(j, demand_graph)
	{
		// Terminal Indices im Demand Graphen
		size_t start_knoten_index = demand_graph.kante(j).iFuss();
		size_t end_knoten_index = demand_graph.kante(j).iKopf();

		// Terminal Namen im Demand Graphen
		string start_knoten_name = demand_graph.knoten(start_knoten_index).name();
		string end_knoten_name = demand_graph.knoten(end_knoten_index).name();

		// Terminal Indices im Base Graphen
		size_t base_start = base_graph.iKnoten(start_knoten_name);
		size_t base_end = base_graph.iKnoten(end_knoten_name);

		// Alle erstellten Wege durchgehen und den passenden Weg zum Demand finden
		for (size_t w = 0; w < erstellte_wege.size(); w++)
		{
			// Erste Kante des Weges mit Fuss und Kopfknoten
			size_t erste_kante = erstellte_wege[w][0];
			size_t erster_fuss = base_graph.kante(erste_kante).iFuss();
			size_t erster_kopf = base_graph.kante(erste_kante).iKopf();

			// Letzte Kante des Weges mit Fuss und Kopfknoten
			size_t letzte_kante = erstellte_wege[w][erstellte_wege[w].size() - 1];
			size_t letzter_fuss = base_graph.kante(letzte_kante).iFuss();
			size_t letzter_kopf = base_graph.kante(letzte_kante).iKopf();

			// Fuß- und Kopfknoten von erster und letzter Kante in Menge vereinigen
			alle_knoten.clear();
			alle_knoten.insert(erster_fuss);
			alle_knoten.insert(erster_kopf);
			alle_knoten.insert(letzter_fuss);
			alle_knoten.insert(letzter_kopf);

			// Wenn base_start und base_end sich in alle_knoten befinden
			// -> das ist der Weg für diesen demand
			if (alle_knoten.find(base_start) != alle_knoten.end()
				&& alle_knoten.find(base_end) != alle_knoten.end())
			{
				wege_reihenfolge.push_back(erstellte_wege[w]);
				break;
			}
		}
	}

	return wege_reihenfolge;
}

