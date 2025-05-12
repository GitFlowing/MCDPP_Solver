#ifndef KNOTEN_H
#define KNOTEN_H

#include <iostream>
#include <string>
#include "konstanten.h"

using namespace std;

/**
 * Klasse fuer Graphknoten
 * Autor: Martin Oellrich
**/

class Knoten
{
	friend class Graph;

	/***  Attribute  ***/

		// eindeutiger Name (ID) des Objekts
	string _name;

public:
	// ebene Koordinaten des Objekts
	double xKoord = UNBENUTZT;
	double yKoord = UNBENUTZT;

	// Gewicht des Objekts
	double gewicht = UNBENUTZT;

	/***  Konstruktoren  ***/

		// Standardkonstruktor, init. nur Speicher ohne Inhalt.
	Knoten()
	{ }

	// Initialisierungskonstruktor mit Attributwerten
	// Name muss angegeben werden, restliche Attribute optional
	Knoten(string const& name,
		double        argXkoord = UNBENUTZT,
		double	       argYkoord = UNBENUTZT,
		double        argGewicht = UNBENUTZT)
		: _name(name)
	{
		// init. oeffentliche Attribute
		xKoord = argXkoord;
		yKoord = argYkoord;
		gewicht = argGewicht;
	}

	/***  get-Methoden  ***/

		// gib Name des Objekts aus
	string const& name() const
	{
		return _name;
	}

	// ist Objekt ein zulaessiger Knoten (nicht geloescht)?
	bool zulaessig() const
	{
		return !_name.empty();
	}

	/***  Ausgabe auf ostream  ***/

	friend
		ostream& operator << (ostream& ostr, Knoten const& knoten)
	{
		ostr << knoten._name;
		if (knoten.gewicht != UNBENUTZT)
			ostr << "[" << knoten.gewicht << "]";
		return ostr;
		// Auf die Koordinaten wird hier verzichtet, weil sie fuer
		// den Graphen als kombinatorisches Objekt unwichtig sind.
	}

};  // class Knoten


#endif  // KNOTEN_H