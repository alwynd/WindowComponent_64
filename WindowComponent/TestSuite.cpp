#include "windowComponent_64.h"

/**
	Author: Alwyn Dippenaar.
	(alwyn.j.dippenaar@gmail.com)

	The TestSuite implementation.
**/


TestSuite::TestSuite(AlCLogger* p_Logger)
{

	logger = p_Logger;

	ZeroMemory(tbuffer, sizeof(wchar_t) * 2048);

	total = 0;
	success = 0;
	fail = 0;
}


/** This function must be invoked to start running tests. */
bool TestSuite::startTests()
{
	total = 0;
	success = 0;
	fail = 0;

	//Run all test cases.
	bool res = runTests();

	//Stamp the final result.
	stampResults();

	return res;
}

/** Stamps the results so far to logger. */
void TestSuite::stampResults()
{
	swprintf_s(tbuffer, L"Test Run Completed..Total: %i, Success: %i, Fail: %i\n\0", total, success, fail);
	logger->debug(tbuffer);
}


