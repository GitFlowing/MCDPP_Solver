#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <sstream>			// fuer ostringstream
#include <stdexcept>		// fuer out_of_range
#include <string>
#include <ios>				// fuer ios_base::failure

using namespace std;


/**
 * Klasse fuer Exceptions bei Indexbereichsverletzung
 * Autor: Martin Oellrich
**/

class IndexException
	: public out_of_range
{
	// verletzender Index
	size_t _index;

	// Laenge des betroffenen Arrays
	size_t _laenge;

public:
	/***  Konstruktoren  ***/

		// Initialisierungskonstruktor
	IndexException(string const& funkName,
		size_t        index,
		size_t        laenge)
		: out_of_range(funkName),
		_index(index),
		_laenge(laenge)
	{ }

	/*** get-Methoden  ***/

		// gebe Meldung aus
	char const* what() const noexcept override
	{
		ostringstream ostr;
		ostr << out_of_range::what() << ": Index " << _index
			<< " >= " << _laenge << " = max. Index!" << ends;

		static string str;
		str = ostr.str();
		return str.c_str();
	}
};  // class IndexException


/**
 * Klasse fuer Exceptions bei unzulaessigen Indizes
 * Autor: Martin Oellrich
**/

class UnzulException
	: public out_of_range
{
public:
	// unzulaessiger Index
	size_t _index;

	/***  Konstruktoren  ***/

		// Initialisierungskonstruktor
	UnzulException(string const& funkName,
		size_t        index)
		: out_of_range(funkName),
		_index(index)
	{ }

	/*** get-Methoden  ***/

		// gebe Meldung aus
	char const* what() const noexcept override
	{
		ostringstream ostr;
		ostr << out_of_range::what() << _index
			<< " ist unzulaessig!" << ends;

		static string str;
		str = ostr.str();
		return str.c_str();
	}
};  // class UnzulException


/**
 * Klasse fuer Exceptions bei Ein-/Ausgabe mit Dateien
 * Autor: Martin Oellrich
**/

class DateiException
	: public ios_base::failure
{
	// Dateiname
	string _dateiName;

	// was ist das Problem?
	int    _problem;

public:
	/***  oeffentliche Konstanten  ***/

		// Codes fuer die moeglichen Probleme
	enum { OEFFNEN, LESEN, SCHREIBEN };

	/***  Konstruktoren  ***/

	DateiException(string const& funkName,
		string const& dateiName,
		int           problem = OEFFNEN)
		: ios_base::failure(funkName),
		_dateiName(dateiName),
		_problem(problem)
	{ }

	/*** get-Methoden  ***/

		// gebe Meldung aus
	char const* what() const noexcept override
	{
		static string str;
		str = string(ios_base::failure::what())
			+ ": kann Datei " + _dateiName + " nicht ";
		if (_problem == OEFFNEN)
			str += "oeffnen!";
		else if (_problem == LESEN)
			str += "lesen!";
		else if (_problem == SCHREIBEN)
			str += "schreiben!";
		else
			str += "behandeln!";

		return str.c_str();
	}

};  // class DateiException


#endif  // EXCEPTIONS_H