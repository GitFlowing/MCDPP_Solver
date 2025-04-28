#ifndef NACHBAR_H
#define NACHBAR_H

#include <vector>
#include "konstanten.h"

using namespace std;

/**
 * Klasse fuer Graph-Nachbarn
 * Autor: Martin Oellrich
**/

class Nachbar
{
	/***  private Attribute  ***/

		// Knotenindex
	size_t _iKnoten;

	// Kantenindex
	size_t _jKante;

public:
	// Abkuerzung fuer Typ "Nachbarnliste"
	typedef  vector<Nachbar>  Liste;

	/***  Konstruktoren  ***/

		// Standard- und Initialisierungskonstruktor
		// mit Attributwerten
	Nachbar(size_t iKnoten = KEIN_INDEX,
		size_t jKante = KEIN_INDEX)
		: _iKnoten(iKnoten),
		_jKante(jKante)
	{ }

	/***  get-Methoden  ***/

		// gib Index des Knotens aus
	size_t i() const
	{
		return _iKnoten;
	}

	// gib Index der Kante aus
	size_t j() const
	{
		return _jKante;
	}

};  // class Nachbar


#endif  // NACHBAR_H
