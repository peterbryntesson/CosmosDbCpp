#pragma once

#include <string>

class CosmosApi
{
private:
	std::string		_endpoint;
	std::string		_masterKey;
	std::string		_db;

public:
	CosmosApi();
	~CosmosApi();

public:
	void Init(const std::string& endpoint, const std::string& masterKey, const std::string& db);

	void AddDocument(const std::string& document, const std::string& partitionKey);
	const std::string& GetDocument(const std::string& key);

private:
	std::string GenerateMasterKeyAuthorizationSignature(const std::string& verb, const std::string& resourceId,
		const std::string& resourceType, const std::string& key, const std::string& keyType,
		const std::string& tokenVersion, const std::string& currentDate);
	void PostRequest(const std::string& db, const std::string& collection, const std::string& partitionkey, const std::string& body);
};

