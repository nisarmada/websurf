#pragma once

// #include "HttpRequest.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include "HttpResponse.hpp"
#include "Client.hpp"
#define MAX_URI_LENGTH 4096
class ServerBlock;

class HttpRequest {
	public:
		HttpRequest();
		~HttpRequest();
		void setMethod(const std::string& method);
		void setUri(const std::string& uri);
		void setHttpVersion(const std::string& httpVersion);
		void addHeader(const std::string& key, const std::string& value);
		void addBody(const char* data, size_t len);
		void setError(int errorCode);
		const std::string& getMethod() const;
		const std::string& getUri() const;
		std::string getHttpVersion() const ;
		const std::string& getHeader(const std::string& key) const;
		void checkRequest(Client& client);
		int checkMethod(Client& client);
		void contentLengthCheck(Client& client);
		void parseRequestLine(const std::string& line);
		void parser(Client& client);
		void parseHostLine(const std::string& line);
		void parseBody(Client& client);
		int checkChunked();
		void parseBodyChunked(Client& client);
		const std::string parseExceptBody(Client& client);
		const std::set<std::string> extractMethods(Client& client);
		const std::vector<char>& getBody() const;
		const std::string extractLocationVariable(Client& client, std::string identifier); //this might need to be static
		int getError();
		bool isBodyComplete() const;
	private:
		std::string _method;
		std::string _uri;
		std::string _httpVersion;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<char> _body;
		int _isError = 0;
		int _isChunked = 0;
		bool _headersComplete = false;
		bool _bodyFullyParsed = false;
		size_t _bodyReadPosition = 0;
		size_t _currentChunkSize = 0;

		enum ChunkParseState {
			READING_CHUNK_SIZE,
			READING_CHUNK_DATA,
			READING_CHUNK_TERMINATOR,
			READING_FINAL_CRLF,
			CHUNK_PARSING_COMPLETE
		};
		ChunkParseState _chunkParseState = READING_CHUNK_SIZE;
		size_t hexToDec(const std::string& hex) const;
		size_t findCrlf(const std::vector<char>& buffer, size_t startPos, size_t bufferEnd) const;
		size_t findEndChunked(const std::vector<char>& buffer, size_t startPos, size_t bufferEnd) const;
		int readingChunkSize(const std::vector<char>& buffer, size_t bufferEnd);
		int readingChunkData(const std::vector<char>& buffer, size_t bufferEnd);
		int readingChunkTerminator(const std::vector<char>& buffer, size_t bufferEnd);
		void readingFinalCRLF(const std::vector<char>& buffer, size_t bufferEnd);
	};
