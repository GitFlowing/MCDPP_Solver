#include "Graph.h"
#include <string>
#include "Stoppuhr.h"
#include <fstream>
#include <iomanip>
#include "graphAlgorithmen.h"
#include "BranchAndBound.h"
#include<set>


//=====================================
// Hauptprogramm Branch-and-Bound MCDPP
//=====================================

int main(int narg, char* argv[])
try
{

	if (narg < 2)
	{
		// ermittle Programmname ohne Pfad
		string progName = argv[0];
		size_t posSlash = progName.find_last_of("/\\");
		if (posSlash != string::npos)
			progName.erase(0, posSlash + 1);

		// gebe Usage aus
		cout << "Usage: " << progName << "  Dateiname" << endl;
		cout << "Fuehrt auf angegebenen Basis- und Demand-Graphen" << endl;
		cout << "Branch and Bound Verfahren zum Loesen des MCDPP aus." << endl;
		cout << "1. Argument: Basis-Graph als .gra-Datei" << endl;
		cout << "2. Argument: Demand-Graph als .gra-Datei" << endl;
		cout << "3. Argument: Anzahl der Iterationen fuer das Subgradientenverfahren" << endl;
		cout << "4. Argument: Anzahl Iterationen fuer das Update Schrittweite" << endl;
		cout << "5. Argument: Anzahl Iterationen Subgradientenverfahren zur Bestimmung VZK (nur bei VZK_Option = KONFLIKTE)" << endl;
		cout << "6. Argument: 1 = lambda_NULL, 2 = lambda_MAX, 3 = lambda_KONFLIKT" << endl;
		cout << "7. Argument: 1 = MAX_FLUSS, 2 = WEG, 3 = KONFLIKTE" << endl;

		return 0;
	}


	//  lese Graph und Parameter ein
	string dateiName1 = argv[1];
	string dateiName2 = argv[2];
	string max_iter_sub_input = argv[3];
	string n_iter_half_input = argv[4];
	string batch_size_input = argv[5];
	string lambda_option_input = argv[6];
	string verzweigung_option_input = argv[7];

	// Erstelle aus eingelesener Graphen Objekte
	Graph base_graph(dateiName1);
	Graph demand_graph(dateiName2);
	int max_iter_sub = std::stoi(max_iter_sub_input);
	int n_iter_half = std::stoi(n_iter_half_input);
	int batch_size = std::stoi(batch_size_input);
	int lambda_option_input_number = std::stoi(lambda_option_input);
	int verzweigung_option_input_number = std::stoi(verzweigung_option_input);
	LambdaZustand lambda_option = LAMBDA_MAX;
	VerzweigungZustand verzweigung_option = MAX_FLUSS;

	switch (lambda_option_input_number)
	{
	case 1:
		lambda_option = LAMBDA_NULL;
		break;
	case 2:
		lambda_option = LAMBDA_MAX;
		break;
	case 3:
		lambda_option = LAMBDA_KONFLIKT;
		break;
	default:
		throw invalid_argument("6.Argument ist 1,2 oder 3 !");
	}

	switch (verzweigung_option_input_number)
	{
	case 1:
		verzweigung_option = MAX_FLUSS;
		break;
	case 2:
		verzweigung_option = WEG;
		break;
	case 3:
		verzweigung_option = KONFLIKTE;
		break;
	default:
		throw invalid_argument("7. Argument ist 1,2 oder 3 !");
	}

	//Fehlermeldungen
	if (max_iter_sub <= 0)
	{
		throw invalid_argument("3. Argument immer groesser 0!");
	}
	if (n_iter_half <= 0)
	{
		throw invalid_argument("4. Argument immer groesser 0!");
	}
	if (batch_size <= 0)
	{
		throw invalid_argument("5. Argument immer groesser 0!");
	}


	cout << "==========================" << endl;
	cout << "Branch and Bound Verfahren" << endl;
	cout << "==========================" << endl << endl;


	// Gute Default-Einstellung
	/*
	int max_iter_sub = 200;
	int n_iter_half = 10;
	int batch_size = 40;
	LambdaZustand lambda_option = LAMBDA_MAX;
	VerzweigungZustand verzweigung_option = MAX_FLUSS;
	*/

	// Branch and Bound Verfahren starten
	Stoppuhr uhr = Stoppuhr();
	uhr.start();
	BranchBoundErgebnis ergebnis = branchAndBound(base_graph, demand_graph, max_iter_sub, batch_size, n_iter_half, lambda_option, verzweigung_option);
	double ende = uhr.stopp();


	// Schreibe in eine ergebnis.txt Datei
	std::ofstream datei("ergebnis_output.txt"); // Datei zum Schreiben öffnen

	if (datei.is_open()) { // prüfen, ob erfolgreich geöffnet
		datei << "Zeit in s: " << ende << " \n";
		
		// Branch and Bound Verfahren endet und hat zulässige Lösung gefunden
		if (!ergebnis.demand_wege_kanten.empty()) {
			vector<vector<size_t>> wege_reihenfolge = kantenAusgabe(ergebnis, base_graph, demand_graph);

			datei << "Gesamtkosten einer optimalen Loesung des MCDPP sind: " << ergebnis.distanz << " \n";
			datei << "Die Kanten einer optimalen Loesung beschrieben als Kanten-Indices des Basis-Graphen sind: \n";

			// Ausgabe der Wege 
			for (size_t t = 0; t < wege_reihenfolge.size(); t++)
			{
				for (size_t ausgabe : wege_reihenfolge[t])
				{
					datei << ausgabe << " ";
				}
				datei << "\n";
			}
		}
		// Branch and Bound Verfahren hat Nicht Existenz optimaler Lösung festgestellt
		else {
			datei << "MCDPP besitzt keine Loesung.\n";
		}
		
		datei.close(); // immer Datei schließen
		std::cout << "Datei erfolgreich geschrieben." << std::endl;
	}
	else {
		std::cout << "Fehler beim Öffnen der Datei." << std::endl;
	}

	return 0;


}  // main()


catch (exception const& exc)
{
	cerr << exc.what() << endl;
}

