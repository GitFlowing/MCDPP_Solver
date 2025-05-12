#pragma once
#include<utility> 
#include<vector>
#include<iostream>
#include<algorithm>
#include<list>
using namespace std;

// ==========================================
// Warteschlange für Branch & Bound Verfahren
// ==========================================


// Zustand einer Subinstanz
enum Zustand
{
	KANDIDAT,	// Demands als Wege zwischen Terminals, die sich überschneiden
	OPTIMAL,	// Optimale Lösung der Subinstanz
	UNZULAESSIG // Mindestens 1 Demand kann nicht als Weg eingebettet werden
};


// Instanzen des Branch and Bound Verfahrens
struct BB_Instanz
{
	// Angepasster Demand-Graph der Subinstanz
	vector<pair<size_t, size_t>> alle_demands;
	// Gelöschte Knoten der Subinstanz
	vector<size_t> geloeschte_knoten;
	// Untere Schranke der Subinstanz
	double untere_schranke = 0;
	// Zustand der Subinstanz
	Zustand instanz_zustand;
	// Start Lambda Subgradinetenverfahren für Subinstanz
	vector<double> lambda_node_start;

	// Funktion zum Setzen der Attribute
	void setAttribut(const vector<pair<size_t, size_t>>& d, const vector<size_t>& g, double u, const Zustand& z, const vector<double>& l)
	{
		alle_demands = d;
		geloeschte_knoten = g;
		untere_schranke = u;
		instanz_zustand = z;
		lambda_node_start = l;
	}
};


class WarteschlangeBranchAndBound
{
private:
	//Warteschlange besteht nur aus Branch and Bound Instanzen
	list<BB_Instanz> _warteschlange;

	// Kleinste bisher gefundene obere Schranke
	double _obere_schranke = UNENDLICH;

	// Speichern der Wegkanten für jeden Demand von der zulässigen Lösung, die zurzeit kleinste obere Schranke ist
	vector<size_t> _demand_weg_kanten;

public:

	// Konstruktor und Destruktor
	WarteschlangeBranchAndBound() = default;
	~WarteschlangeBranchAndBound() = default;

	// Getter
	// Ist Warteschlange leer?
	bool empty() const
	{
		return _warteschlange.empty();
	}

	// Größe der Warteschlange
	size_t size() const
	{
		return _warteschlange.size();
	}

	// Obere Schranke
	double getObereSchranke() const
	{
		return _obere_schranke;
	}

	// Wegkanten der oberen Schranke
	const vector<size_t> getOptWegKanten() const
	{
		return _demand_weg_kanten;
	}

	// Setter
	// B&B Instanz zur Warteschlange hinzufügen
	void push(const BB_Instanz& instanz)
	{
		list<BB_Instanz>::iterator it;

		// Die Subinstanz wird nach ihrer unteren Schranke einsortiert
		for (it = _warteschlange.begin(); it != _warteschlange.end(); ++it)
		{
			if (instanz.untere_schranke < (*it).untere_schranke)
			{
				_warteschlange.insert(it, instanz);
				break;
			}
		}

		// Geht der For Loop ohne break durch, so ist die Position der Subinstanz am Ende der Warteschlange 
		if (it == _warteschlange.end())
		{
			_warteschlange.push_back(instanz);
		}
	}

	// oberste B&B Instanz von Warteschlange löschen
	BB_Instanz pop()
	{
		if (empty())
		{
			throw runtime_error("Ungueltige Aktion: pop darf nicht bei leerer Warteschalnge ausgeführt werden.");
		}

		BB_Instanz oberste_instanz = _warteschlange.front();
		_warteschlange.pop_front();

		return oberste_instanz;
	}

	// Setter obere Schranke
	void setObereSchranke(double obere_schranke_heuristik)
	{
		if (obere_schranke_heuristik >= 0)
		{
			_obere_schranke = obere_schranke_heuristik;
		}
	}

	// Wegkanten der oberen Schranke setzen
	void setDemandWegKanten(const vector<size_t>& demand_wege)
	{
		_demand_weg_kanten = demand_wege;
	}

	// Update obere Schranke
	void updateObereSchranke(double zulaessige_loesung, const vector<size_t>& demand_wege)
	{
		if (zulaessige_loesung < _obere_schranke && !demand_wege.empty())
		{
			// Neue obere Schranke setzen
			setObereSchranke(zulaessige_loesung);

			// Suboptimale Lösungen entfernen
			while (!empty() && _obere_schranke <= _warteschlange.back().untere_schranke)
			{
				_warteschlange.pop_back();
			}

			// Kanten der neuen oberen Schranke speichern
			setDemandWegKanten(demand_wege);
		}
		// Spezialfall zulässige Lösung ist beim initialisierten Wert der oberen Schranke
		if (zulaessige_loesung == _obere_schranke
			&& _demand_weg_kanten.empty()
			&& !demand_wege.empty())
		{
			setDemandWegKanten(demand_wege);
		}
	}
};

