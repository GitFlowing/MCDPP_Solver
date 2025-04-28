#ifndef KONSTANTEN_H
#define KONSTANTEN_H

#include <climits>    // fuer UINT_MAX
#include <cstddef>    // fuer size_t


/***  globale Konstanten  ***/

// Signalwert fuer "ungueltiger Index"
size_t const KEIN_INDEX = UINT_MAX;

// Signalwert fuer "nicht benutztes Attribut"
double const UNBENUTZT = -1;

// Wert fuer "Unendlich"
double const UNENDLICH = 1e300;


#endif  // KONSTANTEN_H