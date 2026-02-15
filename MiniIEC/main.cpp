/**
 * main.cpp - MIEC Compiler
 * Compiler Engineering, FH-OÖ Hagenberg
 * Phase 1: Scanner and Parser
 *
 * Usage:
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
 * Extracts the file extension from a path
 * @param filename The file path
 * @return Pointer to the extension or NULL
 */
const char* GetFileExtension(const char* filename) {
	const char* ext = strrchr(filename, '.');
	return ext ? ext : NULL;
}
/**
 * Checks if the file extension is correct
 * @param filename The file path
 * @param expectedExt The expected extension (e.g. ".miec")
 * @return true if extension is correct
 */
bool HasCorrectExtension(const char* filename, const char* expectedExt) {
	const char* ext = GetFileExtension(filename);
	if (!ext) return false;
	return strcmp(ext, expectedExt) == 0;
}
/**
 * Formats the current time as a string
 * @param buffer The buffer for the result
 * @param bufsize The size of the buffer
 */
void GetCurrentTimeString(char* buffer, size_t bufsize) {
	time_t now = time(NULL);
	struct tm* timeinfo = localtime(&now);
	strftime(buffer, bufsize, "%a %b %d %H:%M:%S %Y", timeinfo);
}
/**
 * Writes an entry to the report file
 * @param inputFile The input file name
 * @param errorCount The number of errors
 */
void WriteReportEntry(const char* inputFile, int errorCount) {
	FILE* reportFile = fopen("MIECCompiler.report", "a");
	if (!reportFile) {
		printf("Error: Cannot open MIECCompiler.report\n");
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
 * Main program
 */
int main(int argc, char* argv[]) {
	char* inputFile = NULL;
	char* outputFile = NULL;
	// Parse command-line arguments
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-in") == 0 && i + 1 < argc) {
			inputFile = argv[++i];
		}
		else if (strcmp(argv[i], "-out") == 0 && i + 1 < argc) {
			outputFile = argv[++i];
		}
	}
	// Check required arguments
	if (!inputFile || !outputFile) {
		printf("Usage: MIECCompiler.exe -in <file.miec> -out <file.iex>\n");
		return 1;
	}
	// Check file extensions
	if (!HasCorrectExtension(inputFile, ".miec")) {
		printf("Error: Input file must have the .miec extension\n");
		return 1;
	}
	if (!HasCorrectExtension(outputFile, ".iex")) {
		printf("Error: Output file must have the .iex extension\n");
		return 1;
	}
	// Try to open the input file
	FILE* inFile = fopen(inputFile, "rb");
	if (!inFile) {
		printf("Error: Cannot open input file: %s\n", inputFile);
		return 1;
	}
	// Create scanner and parser
	Scanner* scanner = NULL;
	Parser* parser = NULL;
	try {
		scanner = new Scanner(inFile);
		parser = new Parser(scanner);
		// Start parser
		parser->Parse();
		// Check error count
		int errorCount = parser->errors->count;
		// Write report
		WriteReportEntry(inputFile, errorCount);
		// Return code based on error count
		int returnCode = (errorCount > 0) ? 1 : 0;
		// Free memory
		delete parser;
		delete scanner;
		fclose(inFile);
		return returnCode;
	}
	catch (const char* e) {
		printf("Error: %s\n", e);
		if (inFile) fclose(inFile);
		if (parser) delete parser;
		if (scanner) delete scanner;
		return 1;
	}
	catch (...) {
		printf("Unknown error during compilation\n");
		if (inFile) fclose(inFile);
		if (parser) delete parser;
		if (scanner) delete scanner;
		return 1;
	}
}