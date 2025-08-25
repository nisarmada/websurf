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