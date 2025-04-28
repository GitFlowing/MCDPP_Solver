#ifndef WARTESCHLANGE_H
#define WARTESCHLANGE_H

#include <iostream>
#include <vector>
#include <iterator>
#include <list>
using namespace std;

/**
 * Klasse für Prioritätswarteschlange Dijkstra-Algorithmus
**/


class Warteschlange
{
	/***  private Attribute  ***/

	//Warteschlange besteht nur aus Vektor-Indices der vergleichs_metrik
	list<size_t> _warteschlange;
	//Vergleichsmetrik ordnet jedem Index des Vektors eine Distnz-double Wert zu
	vector<double>* _vergleichs_metrik;

public:
	/***  Konstruktor  ***/

	// Konstruktor mit Vergleichsmetrik für Warteschlange
	explicit Warteschlange(vector<double>* dist) : _vergleichs_metrik(dist)
	{
		if (_vergleichs_metrik == nullptr) {
			throw std::invalid_argument("Ungueltiges Argument: Vergleichsmetrik darf kein Nullpointer sein.");
		}
	}


	/***  Destruktor  ***/

	~Warteschlange()
	{
		_vergleichs_metrik = nullptr;
	}

	/***  get-Methoden  ***/

	// ist Warteschlange leer?
	bool empty() const
	{
		return _warteschlange.empty();
	}

	// gib Anzahl Einträge im Warteschlange aus
	size_t size() const
	{
		return _warteschlange.size();
	}


	/***  set-Methoden  ***/

	// Eintrag = Index des Vectors der vergleichs_metrik
	// füge Eintrag sortiert nach vergleichs_metrik  in Warteschlange ein
	// wenn Eintrag vorhanden sortiere Warteschlange neu (update)
	void push(size_t eintrag)
	{
		// Existiert Element in Warteschlange
		list<size_t>::iterator existiert = find(_warteschlange.begin(), _warteschlange.end(), eintrag);

		// Knoten existiert -> Knoten löschen
		if (existiert != _warteschlange.end())
		{
			_warteschlange.erase(existiert);
		}

		// Neuen Knoten einfügen
		// Warteschlange durchitterieren und Knoten nach vergleichsmetrik aufsteigend sortiert einfügen
		list<size_t>::iterator it;
		for (it = _warteschlange.begin(); it != _warteschlange.end(); ++it)
		{
			if ((*_vergleichs_metrik)[eintrag] < (*_vergleichs_metrik)[(*it)])
			{
				_warteschlange.insert(it, eintrag);
				break;
			}
		}

		// An letzter Stelle einfügen
		if (it == _warteschlange.end())
		{
			_warteschlange.push_back(eintrag);
		}
	}

	// nehme obersten Eintrag aus Warteschlange heraus und gebe ihn zurück
	size_t pop()
	{
		if (empty())
		{
			throw std::runtime_error("Ungueltige Aktion: pop darf nicht bei leerer Warteschalnge ausgeführt werden.");
		}

		size_t ergebnis = _warteschlange.front();
		_warteschlange.pop_front();

		return ergebnis;
	}



};  // class Warteschlange



#endif  // WARTESCHLANGE_H