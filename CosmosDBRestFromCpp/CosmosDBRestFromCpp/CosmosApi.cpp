#include "pch.h"
#include "CosmosApi.h"

#include <curl.h>
#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr
#include <algorithm>

#include "base64.h"

#include "hmac.h"
#include "sha256.h"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

string GetCurrentUtcTime() {
	char outstr[200];
	time_t t;
	struct tm *tmp = new tm();
	const char* fmt = "%a, %d %b %Y %T GMT";

	t = time(NULL);
	gmtime_s(tmp, &t);
	if (tmp == NULL) {
		perror("gmtime error");
		exit(EXIT_FAILURE);
	}

	if (strftime(outstr, sizeof(outstr), fmt, tmp) == 0) {
		fprintf(stderr, "strftime returned 0");
		exit(EXIT_FAILURE);
	}

	return string(outstr);
}

string url_encode(const string &value) {
	ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		//escaped << uppercase;
		escaped << '%' << setw(2) << int((unsigned char)c);
		//escaped << nouppercase;
	}

	return escaped.str();
}
std::string string_format(const std::string fmt_str, ...) {
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy_s(&formatted[0], n, fmt_str.c_str());
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}

CosmosApi::CosmosApi()
{
}


CosmosApi::~CosmosApi()
{
}

void CosmosApi::Init(const std::string & endpoint, const std::string & masterKey, const std::string & db)
{
	_endpoint = endpoint;
	_masterKey = masterKey;
	_db = db;
}

void function_pt(void *ptr, size_t size, size_t nmemb, void *stream) {
	printf((char*)ptr);
}

void CosmosApi::AddDocument(const std::string & document)
{
	CURL* curl = curl_easy_init();
	if (curl) {
		CURLcode res;

		curl_easy_setopt(curl, CURLOPT_URL, _endpoint.c_str());

		std::string currentDate = GetCurrentUtcTime();
		std::string date_header = "x-ms-date: ";
		date_header = date_header.append(currentDate);
		struct curl_slist *headers = NULL;
		//headers = curl_slist_append(headers, "ContentType: text/json");
		headers = curl_slist_append(headers, date_header.c_str()); 
		headers = curl_slist_append(headers, "x-ms-version: 2015-08-06");

		auto authHeaderToken = GenerateMasterKeyAuthorizationSignature("GET", "", "dbs",
			"Your key", "master", "1.0", currentDate);
		std::string authHeader("authorization: ");
		authHeader = authHeader.append(authHeaderToken);
		headers = curl_slist_append(headers, authHeader.c_str()); 
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);
		// setup lots of stuff

		res = curl_easy_perform(curl);

		// read the results
		curl_slist_free_all(headers);

		curl_easy_cleanup(curl);
	}

}

const std::string & CosmosApi::GetDocument(const std::string & key)
{
	// TODO: insert return statement here
	return "Pelle";
}


int char2int(char input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;
	throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin(const char* src, byte* target)
{
	while (*src && src[1])
	{
		*(target++) = char2int(*src) * 16 + char2int(src[1]);
		src += 2;
	}
}

std::string CosmosApi::GenerateMasterKeyAuthorizationSignature(const std::string & verb, const std::string & resourceId, const std::string & resourceType, const std::string & key, const std::string & keyType, const std::string & tokenVersion, const std::string& currentTime)
{
	unsigned char* keyBytes = NULL;
	int keyBytesLen = 0;
	keyBytes = unbase64(key.c_str(), key.length(), &keyBytesLen);
	std::string payload = string_format("%s\n%s\n%s\n%s\n%s\n", verb.c_str(), resourceType.c_str(), resourceId.c_str(), currentTime.c_str(), "");
	std::transform(payload.begin(), payload.end(), payload.begin(), ::tolower);
	auto signatureAsHexString = hmac<SHA256>(payload.c_str(), (size_t)payload.length(), keyBytes, keyBytesLen);
	int signatureBytesLen = signatureAsHexString.length() / 2;
	byte* signatureBytes = new byte[signatureBytesLen];
	hex2bin(signatureAsHexString.c_str(), signatureBytes);
	std::string signature = base64(signatureBytes, signatureBytesLen, &keyBytesLen);
	std::string token = string_format("type=%s&ver=%s&sig=%s", keyType.c_str(), tokenVersion.c_str(), signature.c_str());

	return url_encode(token);
}

