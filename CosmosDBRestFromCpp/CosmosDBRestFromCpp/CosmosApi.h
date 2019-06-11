#pragma once

#include <string>

class CosmosApi
{
private:
	std::string			_endpoint;
	std::string			_masterKey;
	std::string			_db;
	static std::string	_response;

public:
	CosmosApi();
	~CosmosApi();

public:
	void Init(const std::string& endpoint, const std::string& masterKey, const std::string& db);

	void AddDocument(const std::string& document, const std::string& partitionKey);
	std::string GetDocument(const std::string& id, const std::string& partitionKey);

private:
	std::string GenerateMasterKeyAuthorizationSignature(const std::string& verb, const std::string& resourceId,
		const std::string& resourceType, const std::string& key, const std::string& keyType,
		const std::string& tokenVersion, const std::string& currentDate);
	void PostRequest(const std::string& db, const std::string& collection, const std::string& partitionkey, const std::string& body);
	std::string GetRequest(const std::string& db, const std::string& collection, const std::string& id, const std::string& partitionKey);
	static void function_pt(void *ptr, size_t size, size_t nmemb, void *stream);
};

