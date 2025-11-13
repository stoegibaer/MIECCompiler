/**
 * main.cpp - MIEC Compiler
 * Compiler Engineering, FH-OÖ Hagenberg
 * Phase 1: Scanner und Parser
 *
 * Verwendung:
 *   MIECCompiler.exe -in <file.miec> -out <file.iex>
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "Scanner.h"
#include "Parser.h"
#include <iostream>

using namespace MIEC;

/**
 * Extrahiert die Dateierweiterung aus einem Pfad
 * @param filename Der Dateipfad
 * @return Zeiger auf die Erweiterung oder NULL
 */
const char* GetFileExtension(const char* filename) {
	const char* ext = strrchr(filename, '.');
	return ext ? ext : NULL;
}

/**
 * Überprüft, ob die Dateierweiterung korrekt ist
 * @param filename Der Dateipfad
 * @param expectedExt Die erwartete Erweiterung (z.B. ".miec")
 * @return true wenn Erweiterung korrekt ist
 */
bool HasCorrectExtension(const char* filename, const char* expectedExt) {
	const char* ext = GetFileExtension(filename);
	if (!ext) return false;
	return strcmp(ext, expectedExt) == 0;
}

/**
 * Formatiert die aktuelle Zeit als String
 * @param buffer Der Puffer für das Ergebnis
 * @param bufsize Die Größe des Puffers
 */
void GetCurrentTimeString(char* buffer, size_t bufsize) {
	time_t now = time(NULL);
	struct tm* timeinfo = localtime(&now);
	strftime(buffer, bufsize, "%a %b %d %H:%M:%S %Y", timeinfo);
}

/**
 * Schreibt einen Eintrag in die Report-Datei
 * @param inputFile Der Eingabedateiname
 * @param errorCount Die Anzahl der Fehler
 */
void WriteReportEntry(const char* inputFile, int errorCount) {
	FILE* reportFile = fopen("MIECCompiler.report", "a");
	if (!reportFile) {
		printf("Fehler: Kann MIECCompiler.report nicht öffnen\n");
		return;
	}

	char timeStr[100];
	GetCurrentTimeString(timeStr, 100);

	if (errorCount == 0) {
		fprintf(reportFile, "%s => %s: OK\n", timeStr, inputFile);
		std::cout << timeStr << " => " << inputFile << ": OK" << std::endl;
	}
	else {
		fprintf(reportFile, "%s => %s: FAILED: %d error(s) detected\n", timeStr, inputFile, errorCount);
		std::cerr << timeStr << " => " << inputFile << ": FAILED: " << errorCount << " error(s) detected" << std::endl;
	}

	fclose(reportFile);
}

/**
 * Hauptprogramm
 */
int main(int argc, char* argv[]) {
	char* inputFile = NULL;
	char* outputFile = NULL;

	// Kommandozeilenargumente parsen
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-in") == 0 && i + 1 < argc) {
			inputFile = argv[++i];
		}
		else if (strcmp(argv[i], "-out") == 0 && i + 1 < argc) {
			outputFile = argv[++i];
		}
	}

	// Überprüfe erforderliche Argumente
	if (!inputFile || !outputFile) {
		printf("Verwendung: MIECCompiler.exe -in <file.miec> -out <file.iex>\n");
		return 1;
	}

	// Überprüfe Dateierweiterungen
	if (!HasCorrectExtension(inputFile, ".miec")) {
		printf("Fehler: Eingabedatei muss die Erweiterung .miec haben\n");
		return 1;
	}

	if (!HasCorrectExtension(outputFile, ".iex")) {
		printf("Fehler: Ausgabedatei muss die Erweiterung .iex haben\n");
		return 1;
	}

	// Versuche die Eingabedatei zu öffnen
	FILE* inFile = fopen(inputFile, "rb");
	if (!inFile) {
		printf("Fehler: Kann Eingabedatei nicht öffnen: %s\n", inputFile);
		return 1;
	}

	// Scanner und Parser erstellen
	Scanner* scanner = NULL;
	Parser* parser = NULL;

	try {
		scanner = new Scanner(inFile);
		parser = new Parser(scanner);

		// Parser starten
		parser->Parse();

		// Fehleranzahl prüfen
		int errorCount = parser->errors->count;

		// Report schreiben
		WriteReportEntry(inputFile, errorCount);

		// Ausgabedatei erstellen (auch wenn Fehler aufgetreten sind)
		FILE* outFile = fopen(outputFile, "w");
		if (outFile) {
			if (errorCount == 0) {
				fprintf(outFile, "; MiniIEC Intermediate Code\n; Generated from: %s\n", inputFile);
			}
			fclose(outFile);
		}
		else {
			printf("Warnung: Kann Ausgabedatei nicht erstellen: %s\n", outputFile);
		}

		// Rückgabewert basierend auf Fehleranzahl
		int returnCode = (errorCount > 0) ? 1 : 0;

		// Speicher freigeben
		delete parser;
		delete scanner;
		fclose(inFile);

		return returnCode;

	}
	catch (const char* e) {
		printf("Fehler: %s\n", e);
		if (inFile) fclose(inFile);
		if (parser) delete parser;
		if (scanner) delete scanner;
		return 1;
	}
	catch (...) {
		printf("Unbekannter Fehler während der Kompilation\n");
		if (inFile) fclose(inFile);
		if (parser) delete parser;
		if (scanner) delete scanner;
		return 1;
	}
}
