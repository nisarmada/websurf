#include "../includes/Utils.hpp"

const std::string findLongestMatch(const std::string& uri, const std::map<std::string, LocationBlock>& locations){
	std::string longestMatch = "";

	for (const auto& locationPair : locations){
		if (uri.rfind(locationPair.first, 0) == 0){
			if (locationPair.first.length() > longestMatch.length()){
				longestMatch = locationPair.first;
			}
		}
	}
	return longestMatch;
}

bool isDirectory(std::string file){
	if (file.back() == '/' || file == "/"){
		return true;
	}
	return false;
}

bool isCgi(HttpRequest& request, Client& client){
	std::string cgiPass = request.extractLocationVariable(client, "_cgiPass");
	return !cgiPass.empty();
}


bool cgiPathIsValid(const std::string& fullPath)
{
	struct stat fileStats;

	if(stat(fullPath.c_str(), &fileStats) != 0 || !S_ISREG(fileStats.st_mode))
		return false;

	return (access(fullPath.c_str(), X_OK) == 0);
}
