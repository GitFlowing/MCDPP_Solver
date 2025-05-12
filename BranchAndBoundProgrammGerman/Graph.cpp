#include <iomanip>		// fuer setw()
#include <fstream>		// fuer ifstream, ofstream
#include <sstream>		// fuer istringstream, ostringstream
#include <algorithm>	// fuer max()
#include <unordered_map>
#include "Graph.h"

using namespace std;

#define  TALK  0		// Debugging-Output?


/**
 * Klasse fuer (un)gerichtete Graphen
 * Autor: Martin Oellrich
**/

/***  Hilfsfunktionen zum Einlesen von istream  ***/

// Kommentarzeichen in der Datei
static char const KOMMENTAR = '#';


// lese und ignoriere bis zum Ende der aktuellen Zeile
static
void ignoriereZeile(ifstream& istr)
{
	string dummy;
	getline(istr, dummy);
}


// lese solange Zeilen von istr, bis ein nichttrivialer
// Inhalt gefunden wird, dann gib Zeile zuruek;
// falls Datei erschoepft ist, gib Leerzeile zuruek
static
string leseZeile(istream& istr)
{
	string zeile;

	// lese Zeichen bis zum Zeilenende ein
	while (getline(istr, zeile))
	{
		// suche erstes Nicht-Space
		size_t pos = zeile.find_first_not_of(" \b\f\n\r\t\v");

		if (pos != string::npos
			&& zeile[pos] != KOMMENTAR)
		{
			// loesche Zeilenrest ab einem Kommentarzeichen
			pos = zeile.find(KOMMENTAR, pos + 1);
			if (pos != string::npos)
				zeile.erase(pos);

			return zeile;
		}
	}

	// es wurde nichts gelesen -> gib leere Zeile als Signalwert
	return string();

}  // leseZeile()


// Initialisierungskonstruktor von Datei
Graph::Graph(string const& dateiName)
{
	ifstream datei(dateiName.c_str());

	if (!datei)
		throw DateiException("Graph::Graph()", dateiName,
			DateiException::OEFFNEN);
	cout << "Lese Input-Graph " << dateiName << endl;

	/***  lese Graphparameter aus  ***/

	size_t nKnoten, nKanten;
	string zeile, richtung;

	// ignoriere white spaces
	datei >> skipws;

	// lese Knotenanzahl (und sonst nichts)
	zeile = leseZeile(datei);
	istringstream(zeile) >> nKnoten;

	// lese Kantenanzahl (und sonst nichts)
	zeile = leseZeile(datei);
	istringstream(zeile) >> nKanten;

	// lese Gerichtetheit (und sonst nichts)
	zeile = leseZeile(datei);
	istringstream(zeile) >> richtung;

	// init. Attribute gemaess dieser Angaben
	_richtung = (richtung.substr(0, 2) == "un" ? UNGERICHTET : GERICHTET);
	_knoten.resize(nKnoten);
	_kanten.resize(nKanten);
	_vorwNachbarn.resize(nKnoten);
	if (_richtung == GERICHTET)
		_rueckNachbarn.resize(nKnoten);
	_anzKnoten = nKnoten;
	_anzKanten = nKanten;

	cout << nKnoten << " Knoten" << endl;
	cout << nKanten << " Kanten" << endl;
	cout << richtung << endl;

#if TALK
	cout << endl;
#endif
	/***  lese Knoten aus  ***/

		// Abkuerzung fuer Typ der Hash-Tabelle
	typedef  unordered_map<string, size_t>  hashMap;

	hashMap hashTable;			// fuer Zuordnung Name -> Index

	for (size_t i = 0; i < nKnoten; ++i)
	{
		// lese ganze Zeile, damit optionale Attribute fehlen koennen
		string zeile = leseZeile(datei);
		if (zeile.empty())
			throw DateiException("Graph::Graph()", dateiName,
				DateiException::LESEN);
		// lese alle Komponenten
		string name;
		double xKoord = UNBENUTZT;
		double yKoord = UNBENUTZT;
		double gewicht = UNBENUTZT;

		istringstream(zeile) >> name >> xKoord >> yKoord >> gewicht;

#if TALK
		cout << name << " " << xKoord << " " << yKoord << " " << gewicht << endl;
#endif

		// fuege Datensatz als neuen Knoten zu
		_knoten[i] = Knoten(name, xKoord, yKoord, gewicht);

		// uebertrage Namen in Index -> fuer Boegen
		hashTable[name] = i;

	}  // for ( i )

#if TALK
	cout << endl;
#endif
	/***  lese Kanten aus  ***/

	for (size_t j = 0; j < nKanten; ++j)
	{
		// lese ganze Zeile, damit optionale Attribute fehlen koenen
		string zeile = leseZeile(datei);
		if (zeile.empty())
			throw DateiException("Graph::Graph()", dateiName,
				DateiException::LESEN);
		// lese alle Komponenten
		string name, fuss, kopf;
		double gewicht = UNBENUTZT;

		istringstream(zeile) >> name >> fuss >> kopf >> gewicht;

#if TALK
		cout << name << " " << fuss << " " << kopf << " " << gewicht << endl;
#endif

		size_t iFuss = hashTable[fuss];
		size_t iKopf = hashTable[kopf];

		// fuege Datensatz als neue Kante zu
		_kanten[j] = Kante(name, iFuss, iKopf, gewicht);

		// setze Nachbarnlisten
		_vorwNachbarn[iFuss].push_back(Nachbar(iKopf, j));
		if (_richtung == GERICHTET)
			_rueckNachbarn[iKopf].push_back(Nachbar(iFuss, j));
		else
			_vorwNachbarn[iKopf].push_back(Nachbar(iFuss, j));

	}  // for ( i )

	datei.close();

#if TALK
	cout << endl;
	cout << *this;
#endif
}  // Graph::Graph()


/***  set-Methoden  ***/

// fuege neuen Knoten zum Objekt hinzu,
// gib seinen internen Index zurueck
size_t Graph::operator += (Knoten const& knoten)
{
	size_t iKnoten;

	// es gibt geloeschte Knoten -> schreibe
	// neuen Knoten an geloeschte Position
	if (_anzKnoten < _knoten.size())
	{
		for (iKnoten = 0; iKnoten < _knoten.size(); ++iKnoten)
			if (!zulKnoten(iKnoten))
				break;

		_knoten[iKnoten] = knoten;
		// die Nachbarnlisten sind von der Loeschung leer
	}

	// fuege neuen Knoten hinten an
	else
	{
		_knoten.push_back(knoten);
		iKnoten = _knoten.size() - 1;

		_vorwNachbarn.push_back(Nachbar::Liste());
		if (_richtung == GERICHTET)
			_rueckNachbarn.push_back(Nachbar::Liste());
	}

	// zaehle neuen Knoten
	++_anzKnoten;

	return iKnoten;

}  // Graph::operator += ( Knoten )


// fuege neue Kante zum Objekt hinzu, wenn ihre
// Endknoten vorhanden sind; gib ihren Index zurueck
// bzw. KEIN_INDEX, wenn sie nicht eingefuegt wurde
size_t Graph::operator += (Kante const& kante)
{
	// ein Endknoten ist unzulaessig -> nichts zu tun
	if (!zulKnoten(kante.iFuss())
		|| !zulKnoten(kante.iKopf()))
		return KEIN_INDEX;

	size_t jKante;

	// es gibt geloeschte Kanten -> schreibe
	// neue Kante an geloeschte Position
	if (_anzKanten < _kanten.size())
	{
		for (jKante = 0; jKante < _kanten.size(); ++jKante)
			if (!zulKante(jKante))
				break;

		_kanten[jKante] = kante;
	}

	// fuege neue Kante hinten an
	else
	{
		_kanten.push_back(kante);
		jKante = _kanten.size() - 1;
	}

	// zaehle neue Kante
	++_anzKanten;

	// setze neue Nachbarn
	_vorwNachbarn[kante.iFuss()].push_back(Nachbar(kante.iKopf(), jKante));
	if (_richtung == GERICHTET)
		_rueckNachbarn[kante.iKopf()].push_back(Nachbar(kante.iFuss(), jKante));
	else
		_vorwNachbarn[kante.iKopf()].push_back(Nachbar(kante.iFuss(), jKante));

	return jKante;

}  // Graph::operator += ( Kante )


// loesche Knoten mit Index i aus dem Objekt, wenn er
// vorhanden ist; gib zurueck, ob er geloescht wurde
bool Graph::loescheKnoten(size_t i)
{
	// Knoten unzulaessig -> nichts zu tun
	if (!zulKnoten(i))
		return false;

	// loesche alle Kanten inzident zu _knoten[ i ]
	// aus den Nachbarnlisten
	Nachbar::Liste nachbarn = _vorwNachbarn[i];	// Kopie!
	for (Nachbar const& nach : nachbarn)
		loescheKante(nach.j());

	if (_richtung == GERICHTET)
	{
		nachbarn = _rueckNachbarn[i];			// Kopie!
		for (Nachbar const& nach : nachbarn)
			loescheKante(nach.j());
	}

	// Ueberschreibe _knoten[ i ] mit Standard-Knoten
	// -> wird dadurch unzulaessig
	_knoten[i] = Knoten();
	--_anzKnoten;

	return true;

}  // Graph::loescheKnoten()


// loesche Kante mit Index j aus dem Objekt, wenn sie
// vorhanden ist; gib zurueck ob sie geloescht wurde
bool Graph::loescheKante(size_t j)
{
	// Kante unzulaessig -> nichts zu tun
	if (!zulKante(j))
		return false;

	// loesche _kanten[ j ] aus Vorwaerts-Nachbarnliste
	Nachbar::Liste& vorwNachbarn = _vorwNachbarn[_kanten[j].iFuss()];

	for (Nachbar& nach : vorwNachbarn)
		if (nach.j() == j)
		{
			nach = vorwNachbarn.back();
			vorwNachbarn.pop_back();
			break;
		}

	// loesche _kanten[ j ] aus Rueckwaerts-Nachbarnliste
	Nachbar::Liste& rueckNachbarn = (_richtung == GERICHTET
		? _rueckNachbarn : _vorwNachbarn)
		[_kanten[j].iKopf()];

	for (Nachbar& nach : rueckNachbarn)
		if (nach.j() == j)
		{
			nach = rueckNachbarn.back();
			rueckNachbarn.pop_back();
			break;
		}

	// Ueberschreibe _kanten[ j ] mit Standard-Kante
	// -> wird dadurch unzulaessig
	_kanten[j] = Kante();
	--_anzKanten;

	return true;

}  // Graph::loescheKante()


/***  Ausgabe in Datei  ***/

// schreibe Objekt im Dateiformat in die benannte Datei
void Graph::schreibeDatei(string const& dateiName) const
{
	ofstream datei(dateiName.c_str());

	if (!datei)
		throw DateiException("Graph::schreibeDatei()", dateiName,
			DateiException::OEFFNEN);
	/***  schreibe Anzahlen  ***/

	datei << anzKnoten() << "  " << KOMMENTAR << " Knoten" << endl;
	datei << anzKanten() << "  " << KOMMENTAR << " Kanten" << endl;
	datei << (_richtung == UNGERICHTET ? "un" : "") << "gerichtet" << endl;
	datei << endl;

	/***  schreibe Knotenblock  ***/

		// keine Knoten -> nichts zu tun
	if (knoten().empty())
	{
		datei.close();
		return;
	}

	bool knotenGewichte = (knoten(0).gewicht != UNBENUTZT);
	bool koordinaten = (knoten(0).xKoord != UNBENUTZT);

	datei << KOMMENTAR << " Knotenname";
	if (koordinaten)
		datei << "   x-Koord   y-Koord";
	if (knotenGewichte)
		datei << "   Gewicht";
	datei << endl;
	datei << endl;

	FUER_ALLE_KNOTEN(v, *this)
	{
		datei << v.name();
		if (koordinaten)
			datei << " " << v.xKoord
			<< " " << v.yKoord;
		if (knotenGewichte)
			datei << " " << v.gewicht;
		datei << endl;
	}
	datei << endl;

	/***  schreibe Kantenblock  ***/

		// keine Kanten -> nichts zu tun
	if (kanten().empty())
	{
		datei.close();
		return;
	}

	bool kantenGewichte = (kante(0).gewicht != UNBENUTZT);

	datei << KOMMENTAR << " Kantenname   Knoten A   Knoten B";
	if (kantenGewichte)
		datei << "   Gewicht";
	datei << endl;
	datei << endl;

	FUER_ALLE_KANTEN(a, *this)
	{
		datei << a.name()
			<< " " << knoten(a.iFuss()).name()
			<< " " << knoten(a.iKopf()).name();
		if (kantenGewichte)
			datei << " " << a.gewicht;
		datei << endl;
	}
	datei << endl;

	datei.close();

}  // Graph::schreibeDatei()


/***  Ausgabe auf ostream  ***/

// schreibe Nachbarlisten von graph auf ostr
ostream& operator << (ostream& ostr, Graph const& graph)
{
	/***  berechne Breiten fuer Spaltensatz  ***/

		// breite1 fuer Knoten-/Kantenanzahlen
	size_t breite1 = to_string(max(graph.anzKnoten(), graph.anzKanten())).size();
	size_t breite2 = 0;
	size_t breite3 = 0;

	FUER_ALLE_IKNOTEN(i, graph)
	{
		// breite2 fuer Knoten-Outputs
		ostringstream sstr;
		sstr << graph.knoten(i);
		if (breite2 < sstr.str().size())
			breite2 = sstr.str().size();

		// breite3 fuer Anzahl Nachbarn
		if (breite3 < graph.vorwGrad(i))
			breite3 = graph.vorwGrad(i);

		if (graph.richtung() == Graph::GERICHTET
			&& breite3 < graph.rueckGrad(i))
			breite3 = graph.rueckGrad(i);

	}  // FUER_ALLE_IKNOTEN( i )

	breite3 = to_string(breite3).size();

	/***  schreibe Daten auf ostr  ***/

	ostr << setw(breite1) << graph.anzKnoten() << " Knoten" << endl;
	ostr << setw(breite1) << graph.anzKanten() << " Kanten" << endl;
	ostr << (graph.richtung() == Graph::UNGERICHTET ? "un" : "")
		<< "gerichtet" << endl;
	ostr << endl;

	FUER_ALLE_IKNOTEN(i, graph)
	{
		ostr << setw(breite2) << graph.knoten(i)
			<< " <" << setw(breite3) << graph.vorwGrad(i) << ">";
		FUER_ALLE_VORWNACHBARN(n, graph, i)
			ostr << " " << graph.kante(n.j());
		ostr << endl;

		if (graph.richtung() == Graph::GERICHTET)
		{
			ostr << setw(breite2 + 2)
				<< " <" << setw(breite3) << graph.rueckGrad(i) << ">";
			FUER_ALLE_RUECKNACHBARN(n, graph, i)
				ostr << " " << graph.kante(n.j());
			ostr << endl;
		}

	}  // FUER_ALLE_IKNOTEN( i )

	return ostr;

}  // operator <<
