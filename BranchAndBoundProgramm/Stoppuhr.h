#ifndef STOPPUHR_H
#define STOPPUHR_H

#include <iostream>
#include <ctime>			// fuer clock()

using namespace std;


/**
 * einfache Klasse zur Zeitmessung
 * Autor: Martin Oellrich, April 2015
**/

class Stoppuhr
{
private:
	/***  private Attribute  ***/

		// letzte Startzeit
	double  _startZeit;

public:
	/***  Konstruktoren  ***/

		// Standardkonstruktor, startet Zeitmessung
	Stoppuhr()
	{
		start();
	}

	/***  get-Methoden  ***/

		// gib laufende Zeit aus, halte Zeitmessung nicht an
	double stopp() const
	{
		return (clock() - _startZeit) / CLOCKS_PER_SEC;
	}

	/***  set-Methoden  ***/

		// starte Messung neu
	void start()
	{
		_startZeit = clock();
	}

	/***  Ausgabe auf ostream  ***/

	friend ostream& operator << (ostream& ostr, Stoppuhr const& uhr)
	{
		return ostr << uhr.stopp() << " s";
	}

};  // class Stoppuhr


#endif  // STOPPUHR_H