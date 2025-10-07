#include "../includes/HttpRequest.hpp"


size_t HttpRequest::hexToDec(const std::string& hex) const{
	size_t dec;
	std::stringstream ss;
	ss << std::hex << hex;
	ss >> dec;
	return dec;
}

size_t HttpRequest::findCrlf(const std::vector<char>& buffer, size_t startPos, size_t bufferEnd) const{
	const char crlf[] = "\r\n";
	
	auto it = std::search(buffer.begin() + startPos, buffer.begin() + bufferEnd, std::begin(crlf), std::end(crlf) + 2); // maybe we need to do -1 for null terminator

	if (it != buffer.begin() + bufferEnd){
		return std::distance(buffer.begin(), it);
	}
	return std::string::npos;
}

size_t HttpRequest::findEndChunked(const std::vector<char>& buffer, size_t startPos, size_t bufferEnd) const{
	const char crlfcrlf[] = "\r\n\r\n";

	auto it = std::search(buffer.begin() + startPos, buffer.begin() + bufferEnd,\
		std::begin(crlfcrlf), std::end(crlfcrlf) + 4);
	if (it != buffer.begin() + bufferEnd){
		return std::distance(buffer.begin(), it);
	}
	return std::string::npos;
}

int HttpRequest::readingChunkSize(const std::vector<char>& buffer, size_t bufferEnd){
	size_t endOfLine = findCrlf(buffer, _bodyReadPosition, bufferEnd);
	if (endOfLine == std::string::npos){
		return -1;
	}
	std::string hexString(buffer.begin() + _bodyReadPosition, buffer.begin() + endOfLine);
	try {
		_currentChunkSize = hexToDec(hexString);
	}catch(const std::exception& e){
		setError(400);
		throw std::runtime_error("Error in chunked size");
	}
	if (_currentChunkSize == 0){
		_chunkParseState = READING_FINAL_CRLF;
	} else{
		_chunkParseState = READING_CHUNK_DATA;
	}
	return 0;
}

int HttpRequest::readingChunkData(const std::vector<char>& buffer, size_t bufferEnd){
	size_t bytesAvailable = bufferEnd - _bodyReadPosition;
	size_t bytesToRead = std::min(bytesAvailable, _currentChunkSize);
	
	_body.insert(buffer.end(), buffer.begin() + _bodyReadPosition, buffer.begin() + _bodyReadPosition + bytesToRead);
	_bodyReadPosition += bytesToRead;
	_currentChunkSize -= bytesToRead;
	if (_currentChunkSize == 0){
		_chunkParseState = READING_CHUNK_TERMINATOR;
	} else {
		return -1;
	}
	return 0;
}


int HttpRequest::readingChunkTerminator(const std::vector<char>& buffer, size_t bufferEnd){
	if (bufferEnd - _bodyReadPosition < 2){
		return -1;
	}
	if (buffer[_bodyReadPosition] == '\r' && buffer[_bodyReadPosition + 1] == '\n'){
		_chunkParseState = READING_CHUNK_SIZE;
		_bodyReadPosition += 2;
		return 0;
	}
		setError(400);
		return -1;
	}



void HttpRequest::readingFinalCRLF(const std::vector<char>& buffer, size_t bufferEnd){
	size_t endOfChunked = findEndChunked(buffer, _bodyReadPosition, bufferEnd);
	if (endOfChunked == std::string::npos){
		return ;
	}
	_bodyReadPosition = endOfChunked + 4;
	_chunkParseState = CHUNK_PARSING_COMPLETE;
	_bodyFullyParsed = true;
	return ;
}

void HttpRequest::parseBodyChunked(Client& client){
	const std::vector<char>& requestBuffer = client.getRequestBuffer();
	size_t bufferSize = requestBuffer.size();
	while (_bodyReadPosition < bufferSize || _chunkParseState == CHUNK_PARSING_COMPLETE){
		switch (_chunkParseState){
			case READING_CHUNK_SIZE : {
				if (readingChunkSize(requestBuffer, bufferSize) == -1){
					return;
				}else{

					break;
				}
			}
			case READING_CHUNK_DATA : {
				if (readingChunkData(requestBuffer, bufferSize) == -1){
					
					return ;
				} else {
					break;
				}
			}
			case READING_CHUNK_TERMINATOR : {
				if (readingChunkTerminator(requestBuffer, bufferSize) == -1){
					
					return ;
				} else {
					break;
				}
			}
			case READING_FINAL_CRLF : {
				readingFinalCRLF(requestBuffer, bufferSize);

				return;
			}
			case CHUNK_PARSING_COMPLETE : {

				return ;
			}
		}
	}
}