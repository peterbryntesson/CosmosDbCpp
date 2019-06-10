// CosmosDBRestFromCpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include <curl.h>
#include <easy.h>
#include "CosmosApi.h"

#pragma comment(lib, "libcurld.lib")

int main()
{
	auto api = new CosmosApi();
	api->Init("https://rbssandbox.documents.azure.com/dbs/", "MTySJ07erLfA4BYfc41ALmwplnlUfMMU4unSxt9ocuy4g0tWNMOUJjveJLzU90FghUY4YUUuUpP8XIa9vfmw4Q==", "sandpit");
	api->AddDocument("{ \"key\": \"1\" \"value\": \"Hello World\" }");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file