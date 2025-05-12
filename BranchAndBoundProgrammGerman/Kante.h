#ifndef KANTE_H
#define KANTE_H

#include <iostream>
#include <string>
#include "konstanten.h"

using namespace std;

/**
 * Klasse fuer (un)gerichtete Graphkanten
 * Autor: Martin Oellrich
**/

class Kante
{
	friend class Graph;

	/***  Attribute  ***/

		// eindeutiger Name (ID) des Objekts
	string _name;

	// Indizes der Endknoten des Objekts: iFuss -> iKopf.
	// Eine geloeschte Kante ist an _iFuss == KEIN_INDEX
	// zu erkennen.
	size_t _iFuss = KEIN_INDEX;
	size_t _iKopf = KEIN_INDEX;

public:
	// Gewicht des Objekts
	double gewicht = UNBENUTZT;

	/***  Konstruktoren  ***/

		// Standardkonstruktor, init. nur Speicher ohne Inhalt.
	Kante()
	{ }

	// Initialisierungskonstruktor mit Attributwerten.
	// Name und Knotenindizes muessen angegeben werden,
	// restliche Attribute sind optional
	Kante(string const& name,
		size_t        iFuss,
		size_t        iKopf,
		double        argGewicht = UNBENUTZT)
		: _name(name),
		_iFuss(iFuss),
		_iKopf(iKopf)
	{
		// init. oeffentliche Attribute
		gewicht = argGewicht;
	}

	/***  get-Methoden  ***/

		// gib Name des Objekts aus
	string const& name() const
	{
		return _name;
	}

	// gib Index des Fussknotens aus
	size_t iFuss() const
	{
		return _iFuss;
	}

	// gib Index des Kopfknotens aus
	size_t iKopf() const
	{
		return _iKopf;
	}

	// ist Objekt eine zulaessige Kante (nicht geloescht)?
	bool zulaessig() const
	{
		return !_name.empty();
	}

	/***  Ausgabe auf ostream  ***/

	friend
		ostream& operator << (ostream& ostr, Kante const& kante)
	{
		ostr << kante._name << "(" << kante._iFuss
			<< "," << kante._iKopf << ")";
		if (kante.gewicht != UNBENUTZT)
			ostr << "[" << kante.gewicht << "]";
		return ostr;
	}

};  // class Kante


#endif  // KANTE_H