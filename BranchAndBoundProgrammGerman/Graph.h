#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include "Knoten.h"
#include "Kante.h"
#include "Nachbar.h"
#include "exceptions.h"

using namespace std;


/**
 * Klasse fuer (un)gerichtete Graphen
 * Autor: Martin Oellrich
**/

/***  globale Makros  ***/

#define KEIN_CHECK	// entkommentieren, um Indexpruefungen abzustellen

// Schleife fuer alle Knotenindizes
#define  FUER_ALLE_IKNOTEN( i, graph )						\
	for ( size_t i = 0 ;  i < (graph).anzKnoten() ;  ++i )	\
		if ( (graph).zulKnoten( i ) )

// Schleife fuer alle zulaesigen Kantenindizes
#define  FUER_ALLE_IKANTEN( j, graph )						\
	for ( size_t j = 0 ;  j < (graph).anzKanten() ;  ++j )	\
		if ( (graph).zulKante( j ) )

// Bereichs-Schleife fuer alle Knoten
#define  FUER_ALLE_KNOTEN( v, graph )						\
	for ( auto& v : (graph).knoten() )						\
		if ( v.zulaessig() )

// Bereichs-Schleife fuer alle zulaessigen Kanten
#define  FUER_ALLE_KANTEN( a, graph )						\
	for ( auto& a : (graph).kanten() )						\
		if ( a.zulaessig() )

// Bereichs-Schleife fuer alle Vorwaerts-Nachbarn des Knotens mit Index i
#define  FUER_ALLE_VORWNACHBARN( n, graph, i )				\
	for ( Nachbar const& n : (graph).vorwNachbarn( i ) )

// Bereichs-Schleife fuer alle Rueckwaerts-Nachbarn des Knotens mit Index i
#define  FUER_ALLE_RUECKNACHBARN( n, graph, i )				\
	for ( Nachbar const& n : (graph).rueckNachbarn( i ) )

// nur wenn Objekt ungerichtet!
// Bereichs-Schleife fuer alle Nachbarn des Knotens mit Index i
#define  FUER_ALLE_NACHBARN( n, graph, i )					\
	for ( Nachbar const& n : (graph).nachbarn( i ) )


/***  Klassendeklaration  ***/

class Graph
{
private:
	/***  private Attribute  ***/

		// Gerichtetheit des Objekts
	bool				   _richtung;

	// Array der Knoten
	vector<Knoten>         _knoten;

	// Array der Kanten
	vector<Kante>          _kanten;

	// aktuelle Anzahl Knoten im Objekt
	size_t                 _anzKnoten;

	// aktuelle Anzahl Kanten im Objekt
	size_t                 _anzKanten;

	// Nachbarnlisten der Knoten
	vector<Nachbar::Liste> _vorwNachbarn;
	vector<Nachbar::Liste> _rueckNachbarn;

public:
	/***  Oeffentliche Konstanten  ***/

		// Objekt ist gerichtet
	static bool const GERICHTET = true;

	// Objekt ist ungerichtet
	static bool const UNGERICHTET = false;

	/***  Konstruktoren  ***/

		// Standard- und Initialisierungskonstruktor,
		// initialisiert leeres Objekt
	Graph(bool richtung = GERICHTET)
		: _richtung(richtung),
		_anzKnoten(0),
		_anzKanten(0)
	{ }

	// Initialisierungskonstruktor von Datei
	explicit
		Graph(string const& dateiName);

	/***  get-Methoden ohne Parameter  ***/

		// gib Gerichtetheit des Objekts aus
	bool richtung() const
	{
		return _richtung;
	}

	// gib Knoten-Array zum Lesen aus
	vector<Knoten> const& knoten() const
	{
		return _knoten;
	}

	// gib Kanten-Array zum Lesen aus
	vector<Kante> const& kanten() const
	{
		return _kanten;
	}

	// gib Anzahl zulaessiger Knoten aus
	size_t anzKnoten() const
	{
		return _anzKnoten;
	}

	// gib Anzahl zulaessiger Kanten aus
	size_t anzKanten() const
	{
		return _anzKanten;
	}

	/***  get-Methoden mit Parameter  ***/

		// ist i Index eines zulaessigen Knotens?
	bool zulKnoten(size_t i) const
	{
#ifndef KEIN_CHECK
		return     i < _knoten.size()
			&& _knoten[i].zulaessig();
#else
		return _knoten[i].zulaessig();
#endif
	}


	// ist j Index einer zulaessigen Kante?
	bool zulKante(size_t j) const
	{
#ifndef KEIN_CHECK
		return     j < _kanten.size()
			&& _kanten[j].zulaessig();
#else
		return _kanten[j].zulaessig();
#endif
	}


	// gib Knoten mit Index i zum Lesen aus
	Knoten const& knoten(size_t i) const
	{
#ifndef KEIN_CHECK
		if (i >= anzKnoten())
			throw IndexException("Graph::knoten(i) const", i, anzKnoten());
		if (!_knoten[i].zulaessig())
			throw UnzulException("Graph::knoten(i) const: Knoten", i);
#endif

		return _knoten[i];
	}


	// gib Kante mit Index j zum Lesen aus
	Kante const& kante(size_t j) const
	{
#ifndef KEIN_CHECK
		if (j >= anzKanten())
			throw IndexException("Graph::kante(j) const", j, anzKanten());
		if (!_kanten[j].zulaessig())
			throw UnzulException("Graph::kante(j) const: Kante", j);
#endif

		return _kanten[j];
	}


	// gib Knotenindex zum Namen aus oder KEIN_INDEX, falls nicht vorhanden
	size_t iKnoten(string const& name) const
	{
		FUER_ALLE_IKNOTEN(i, *this)
			if (knoten(i).name() == name)
				return i;
		return KEIN_INDEX;
	}


	// gib Kantenindex zum Namen aus oder KEIN_INDEX, falls nicht vorhanden
	size_t jKante(string const& name) const
	{
		FUER_ALLE_IKANTEN(j, *this)
			if (kante(j).name() == name)
				return j;
		return KEIN_INDEX;
	}


	// gib Vorwaerts-Nachbarnliste von Knoten mit Index i aus
	Nachbar::Liste const& vorwNachbarn(size_t i) const
	{
#ifndef KEIN_CHECK
		if (i >= anzKnoten())
			throw IndexException("Graph::vorwNachbarn(i) const", i, anzKnoten());
		if (!_knoten[i].zulaessig())
			throw UnzulException("Graph::vorwNachbarn(i) const: Knoten", i);
#endif

		return _vorwNachbarn[i];
	}


	// gib Rueckwaerts-Nachbarnliste von Knoten mit Index i aus
	Nachbar::Liste const& rueckNachbarn(size_t i) const
	{
#ifndef KEIN_CHECK
		if (i >= anzKnoten())
			throw IndexException("Graph::rueckNachbarn(i) const", i, anzKnoten());
		if (!_knoten[i].zulaessig())
			throw UnzulException("Graph::rueckNachbarn(i) const: Knoten", i);
#endif

		return _richtung == GERICHTET ? _rueckNachbarn[i]
			: _vorwNachbarn[i];
	}


	// nur falls Objekt ungerichtet!
	// gib Nachbarnliste von Knoten mit Index i aus
	Nachbar::Liste const& nachbarn(size_t i) const
	{
		return vorwNachbarn(i);
	}


	// gib Anzahl der von Knoten mit Index i ausgehenden Kanten aus.
	// Dies ist nicht unbedingt der Knotengrad, da es parallele
	// Kanten geben kann
	size_t vorwGrad(size_t i) const
	{
		return vorwNachbarn(i).size();
	}


	// gib Anzahl der in Knoten mit Index i eingehenden Kanten aus.
	// Dies ist nicht unbedingt der Knotengrad, da es parallele
	// Kanten geben kann
	size_t rueckGrad(size_t i) const
	{
		return rueckNachbarn(i).size();
	}


	// Nur falls Objekt ungerichtet!
	// Gib Anzahl der mit Knoten mit Index i inzidenten Kanten aus.
	// Dies ist nicht unbedingt der Knotengrad, da es parallele
	// Kanten geben kann
	size_t grad(size_t i) const
	{
		return vorwNachbarn(i).size();
	}

	/***  set-Methoden  ***/

		// gib Knoten-Array zum Schreiben aus
	vector<Knoten>& knoten()
	{
		return _knoten;
	}

	// gib Kanten-Array zum Schreiben aus
	vector<Kante>& kanten()
	{
		return _kanten;
	}


	// gib Knoten mit Index i als Referenz aus
	Knoten& knoten(size_t i)
	{
#ifndef KEIN_CHECK
		if (i >= anzKnoten())
			throw IndexException("Graph::knoten(i)", i, anzKnoten());
		if (!_knoten[i].zulaessig())
			throw UnzulException("Graph::knoten(i): Knoten", i);
#endif

		return _knoten[i];
	}


	// gib Kante mit Index j als Referenz aus
	Kante& kante(size_t j)
	{
#ifndef KEIN_CHECK
		if (j >= anzKanten())
			throw IndexException("Graph::kante(j)", j, anzKanten());
		if (!_kanten[j].zulaessig())
			throw UnzulException("Graph::kante(j): Kante", j);
#endif

		return _kanten[j];
	}

	// fuege neuen Knoten zum Objekt hinzu,
	// gib seinen internen Index zurueck
	size_t operator += (Knoten const& knoten);

	// fuege neue Kante zum Objekt hinzu, wenn ihre
	// Endknoten vorhanden sind; gib ihren Index zurueck
	// bzw. KEIN_INDEX, wenn sie nicht eingefuegt wurde
	size_t operator += (Kante  const& kante);

	// loesche Knoten mit Index i aus dem Objekt, wenn er
	// vorhanden ist; gib zurueck, ob er geloescht wurde
	bool   loescheKnoten(size_t i);

	// loesche Kante mit Index j aus dem Objekt, wenn sie
	// vorhanden ist; gib zurueck, ob sie geloescht wurde
	bool   loescheKante(size_t j);

	// lese Objekt neu von Datei ein; alter Inhalt geht verloren
	void   leseDatei(string const& dateiName)
	{
		*this = Graph(dateiName);
	}

	/***  Ausgabe in Datei  ***/

		// schreibe Objekt im Leseformat in die benannte Datei
	void schreibeDatei(string const& dateiName) const;

};  // class Graph


/***  Ausgabe auf ostream  ***/

// schreibe Nachbarlisten von graph auf ostr
ostream& operator << (ostream& ostr, Graph const& graph);


#endif  // GRAPH_H