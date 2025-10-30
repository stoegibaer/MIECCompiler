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
#include <wchar.h>
#include <time.h>
#include <string.h>
#include "Scanner.h"
#include "Parser.h"

using namespace MIEC;

/**
 * Extrahiert die Dateierweiterung aus einem Pfad
 * @param filename Der Dateipfad
 * @return Zeiger auf die Erweiterung oder NULL
 */
const wchar_t* GetFileExtension(const wchar_t* filename) {
	const wchar_t* ext = wcsrchr(filename, L'.');
	return ext ? ext : NULL;
}

/**
 * Überprüft, ob die Dateierweiterung korrekt ist
 * @param filename Der Dateipfad
 * @param expectedExt Die erwartete Erweiterung (z.B. L".miec")
 * @return true wenn Erweiterung korrekt ist
 */
bool HasCorrectExtension(const wchar_t* filename, const wchar_t* expectedExt) {
	const wchar_t* ext = GetFileExtension(filename);
	if (!ext) return false;
	return wcscmp(ext, expectedExt) == 0;
}

/**
 * Formatiert die aktuelle Zeit als String
 * @param buffer Der Puffer für das Ergebnis
 * @param bufsize Die Größe des Puffers
 */
void GetCurrentTimeString(wchar_t* buffer, size_t bufsize) {
	time_t now = time(NULL);
	struct tm* timeinfo = localtime(&now);
	wcsftime(buffer, bufsize, L"%a %b %d %H:%M:%S %Y", timeinfo);
}

/**
 * Schreibt einen Eintrag in die Report-Datei
 * @param inputFile Der Eingabedateiname
 * @param errorCount Die Anzahl der Fehler
 */
void WriteReportEntry(const wchar_t* inputFile, int errorCount) {
	FILE* reportFile = _wfopen(L"MIECCompiler.report", L"a");
	if (!reportFile) {
		wprintf(L"Fehler: Kann MIECCompiler.report nicht öffnen\n");
		return;
	}

	wchar_t timeStr[100];
	GetCurrentTimeString(timeStr, 100);

	if (errorCount == 0) {
		fwprintf(reportFile, L"%ls => %ls: OK\n", timeStr, inputFile);
		wprintf(L"%ls => %ls: OK\n", timeStr, inputFile);
	}
	else {
		fwprintf(reportFile, L"%ls => %ls: FAILED: %d error(s) detected\n", timeStr, inputFile, errorCount);
		fwprintf(stderr, L"%ls => %ls: FAILED: %d error(s) detected\n", timeStr, inputFile, errorCount);
	}

	fclose(reportFile);
}

/**
 * Hauptprogramm
 */
int wmain(int argc, wchar_t* argv[]) {
	wchar_t* inputFile = NULL;
	wchar_t* outputFile = NULL;

	// Kommandozeilenargumente parsen
	for (int i = 1; i < argc; i++) {
		if (wcscmp(argv[i], L"-in") == 0 && i + 1 < argc) {
			inputFile = argv[++i];
		}
		else if (wcscmp(argv[i], L"-out") == 0 && i + 1 < argc) {
			outputFile = argv[++i];
		}
	}

	// Überprüfe erforderliche Argumente
	if (!inputFile || !outputFile) {
		wprintf(L"Verwendung: MIECCompiler.exe -in <file.miec> -out <file.iex>\n");
		return 1;
	}

	// Überprüfe Dateierweiterungen
	if (!HasCorrectExtension(inputFile, L".miec")) {
		wprintf(L"Fehler: Eingabedatei muss die Erweiterung .miec haben\n");
		return 1;
	}

	if (!HasCorrectExtension(outputFile, L".iex")) {
		wprintf(L"Fehler: Ausgabedatei muss die Erweiterung .iex haben\n");
		return 1;
	}

	// Versuche die Eingabedatei zu öffnen
	FILE* inFile = _wfopen(inputFile, L"rb");
	if (!inFile) {
		wprintf(L"Fehler: Kann Eingabedatei nicht öffnen: %ls\n", inputFile);
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
		FILE* outFile = _wfopen(outputFile, L"w");
		if (outFile) {
			if (errorCount == 0) {
				fwprintf(outFile, L"; MiniIEC Intermediate Code\n; Generated from: %ls\n", inputFile);
			}
			fclose(outFile);
		}
		else {
			wprintf(L"Warnung: Kann Ausgabedatei nicht erstellen: %ls\n", outputFile);
		}

		// Rückgabewert basierend auf Fehleranzahl
		int returnCode = (errorCount > 0) ? 1 : 0;

		// Speicher freigeben
		delete parser;
		delete scanner;
		fclose(inFile);

		return returnCode;

	}
	catch (const wchar_t* e) {
		wprintf(L"Fehler: %ls\n", e);
		if (inFile) fclose(inFile);
		if (parser) delete parser;
		if (scanner) delete scanner;
		return 1;
	}
	catch (...) {
		wprintf(L"Unbekannter Fehler während der Kompilation\n");
		if (inFile) fclose(inFile);
		if (parser) delete parser;
		if (scanner) delete scanner;
		return 1;
	}
}