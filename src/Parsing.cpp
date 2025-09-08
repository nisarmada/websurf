#include "../includes/Parser.hpp"
#include "../includes/ServerBlock.hpp"
#include "../includes/server.hpp"


void parsing(const char* path, WebServer& webServ)
{
    std::ifstream configFile = openConfigFile(path);
    std::vector<std::string> cleanedLines = trimBeginEnd(configFile);

    std::vector<std::string> tokens = getAllTokens(cleanedLines);
    checkSyntax(tokens);
    std::vector<std::vector<std::string>> serverBlocks = getServerBlockTokens(tokens);
    
    webServ.loadConfig(serverBlocks);

	
    // std::cout << std::endl << std::endl;
    // test.printServerBlocks();
}

std::ifstream openConfigFile(const char* path)
{
    std::ifstream configFile(path);
    if(!configFile.is_open())
    {
        std::cerr << "Error could not open the config file." << std::endl;
        exit(1);
    }
    return configFile;
}

//check which whitespaces I have to trim.
std::vector<std::string> trimBeginEnd (std::ifstream& file)
{
    std::vector<std::string> cleanedLines;
    std::string line;
    while(std::getline(file, line))
    {
        if (line.find_first_not_of(" \t\n") == std::string::npos) //makes sure it isnt an empty line npos is a very high number -1 size_t
            continue;
        line.erase(0, line.find_first_not_of(" \t\n")); //deletes length of characters starting at start_index.
        line.erase(line.find_last_not_of(" \t\n") + 1); //deletes everything form start_index to end of string.
        cleanedLines.push_back(line);
    }
    return cleanedLines;
}

std::vector<std::string> tokenizeLine(std::string line)
{
    std::vector<std::string> tokenisedLine;
    std::string currentToken;

    for(size_t i = 0; i < line.size(); i++)
    {
        char c = line[i];
        if(isSpecialChar(c)) //check for special characters that has to be tokenised
        {
            if(!currentToken.empty())
                tokenisedLine.push_back(currentToken); //push whats already in there
            tokenisedLine.push_back(std::string(1, c));
            currentToken.clear();
        }
        else if(std::isspace(c) && !currentToken.empty()) //if its a whitespace its the end of a token so push here
        {
            tokenisedLine.push_back(currentToken);
            currentToken.clear();
        }
        else if(!std::isspace(c)) //if its not whitespace I want to add it to the current token. 
            currentToken += c;
    }
    if(!currentToken.empty()) //there be a token left which the loop didnt push yet.
        tokenisedLine.push_back(currentToken);

    //print tokenised line
    // for(size_t i = 0; i < tokenisedLine.size(); i ++)
    //     std::cout << "[" << tokenisedLine[i] << "]" << std::endl;

    return tokenisedLine;
}

std::vector<std::string> getAllTokens(const std::vector<std::string>& cleanedLines)
{
    std::vector<std::string> tokens;

    for(size_t i = 0; i < cleanedLines.size(); i++)
    {
        std::vector<std::string> tokenLine = tokenizeLine(cleanedLines[i]);
        tokens.insert(tokens.end(), tokenLine.begin(), tokenLine.end()); //inserts a range of strings(where to add, what to add from where to where (vector begin end in our case))
    }
    // for(size_t i = 0; i < tokens.size(); i ++)
    //     std::cout << "[" << tokens[i] << "]" << std::endl;
    
    return tokens;
}


bool isSpecialChar(char c)
{
    return c == '{' || c == '}' || c == ';';
}

bool checkSyntax(std::vector<std::string>tokens)
{
    checkSemicolons(tokens);
    checkBrackets(tokens);
    
    return false; //check if bool is still neaded
}

void checkSemicolons(std::vector<std::string> tokens)
{
    //check if I can remove all those checks.
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if(tokens[i] == "methods") //fix for now, might change later
        {
            while (i < tokens.size() && tokens[i] != ";") //skips the method for checking semicolons.
                i ++;
            continue;
        }
        mustHaveSemicolon(tokens, i);
        wrongPlaceSemicolon(tokens, i); 
    }
}

void mustHaveSemicolon(std::vector<std::string> tokens, size_t i)
{
        if(getType(tokens[i]) == DIRECTIVE)
        {
            // std::cout << "in here" << std::endl;
            if(i + 2 >= tokens.size() || tokens[i + 2] != ";")
            {
                std::cerr << "Error: missing semicolon: " << tokens[i] << std::endl;
                exit(1);
            }
        }
        
}


//check if I can do minus 3, there is no check for it as of far.
void wrongPlaceSemicolon(std::vector<std::string> tokens, size_t i)
{
    if(tokens[i] == ";")
    {
        if(i == 0 || i == 1)
        {
            std::cerr << "Error: config file cannot start with semicolon" << std::endl;
            exit(1);
        }
        if(getType(tokens[i - 2]) == DIRECTIVE)
            return;
        
        if(static_cast<int>(i) - 3 >= 0 && getType(tokens[i - 3]) == DIRECTIVE2) // broke the code so I made a patch
            return;
        //temporary change to do correct with throw
    
        std::cerr << "Error: invalid semicolon after \"" << tokens[i - 2] << "\"" << std::endl;
        exit(1);
    }
}
//make statementone statementtwo etc. to indicate how much you have to go back with index for the semicolon checker. 
Type getType(const std::string& token)
{
    static std::map<std::string, Type> typeMap = 
    {
        {"server", BLOCK},
        {"location", BLOCK},
        {"listen", DIRECTIVE},
        {"server_name", DIRECTIVE},
        {"root", DIRECTIVE},
        {"index", DIRECTIVE},
        {"root", DIRECTIVE},
        {"index", DIRECTIVE},
        {"client_max_body_size", DIRECTIVE},
        {"error_page", DIRECTIVE2},
        {"methods", IGNORE},
        {"autoindex", DIRECTIVE},
        {"return", DIRECTIVE},
        {"upload_path", DIRECTIVE},
        {"cgi_pass", DIRECTIVE}

    };
    if(typeMap.find(token) != typeMap.end()) //find returns end(represents one past the last element) if it didnt find it in the map
        return typeMap[token]; //returns the map location with the correct enum checked if it existed before otherwise it adds the token to the map as a default.
    return UNKNOWN;
}

   void checkBrackets(std::vector<std::string> tokens)
   {
            checkBracketStructure(tokens);
            checkValidBracketOpening(tokens);
   }

void checkBracketStructure(std::vector<std::string> tokens)
{
    std::stack<char> stack;
    for(size_t i = 0; i < tokens.size(); i++)
    {
        if(tokens[i] == "{")
            stack.push('{');
        if(tokens[i] == "}")
        {
            if(stack.empty())
            {
                std::cerr << "Error: wrong bracket (TEMP)" << std::endl;
                exit(1);
            }
            stack.pop();
        }
    }
    if(!stack.empty())
    {
        std::cerr << "Error: wrong bracket (TEMP2)" << std::endl;
        exit(1);
    }
}

void checkValidBracketOpening(std::vector<std::string> tokens)
{
    for(size_t i = 0; i < tokens.size(); i++)
    {
        if(tokens[i] == "{")
        {
            if(i >= 1 && getType(tokens[i - 1]) == BLOCK) 
                continue;
            if(i >= 2 && getType(tokens[i - 2]) == BLOCK)
                continue;
            else
            {
                std::cerr << "Error: wrong placed bracked" << std::endl;
                exit(1);
            }
        }
    }
}

std::vector<std::vector<std::string>> getServerBlockTokens(std::vector<std::string> tokens)
{
    std::vector<std::string> serverTokens;
	std::vector<std::vector<std::string>> allServers;
    int bracketLevel = 0;
    bool insideBlock = false;

    for(size_t i = 0; i < tokens.size(); i++)
    {
        if(i + 1 < tokens.size() && tokens[i] == "server" && tokens[i + 1] == "{") //check if i + 1 could give crashes.
		{
            insideBlock = true;
			bracketLevel = 1;
			i += 2; //skip the server and the bracket for the next if block. 
		}
		
        if(insideBlock == true && i < tokens.size())
        {
            if(tokens[i] == "{")
            bracketLevel ++;
            if(tokens[i] == "}")
            bracketLevel --;
            
            if (bracketLevel <= 0) //check if there is still something in the severtokens before emptying it.
			{
                insideBlock = false;
				allServers.push_back(serverTokens);
				serverTokens.clear();
                continue;
			}
            serverTokens.push_back(tokens[i]);
        }   
    }


	//print test 
    // std::cout << std::endl << std::endl << std::endl;
	// for (size_t j = 0; j < allServers.size(); j++)
	// {
	// 	std::cout << "Server Block " << j << ":" << std::endl;

	// 	for (size_t i = 0; i < allServers[j].size(); i++)
	// 	{
	// 		std::cout << allServers[j][i] << std::endl;
	// 	}

	// 	std::cout << std::endl; // Add spacing between blocks
	// }

    return allServers;
}









 
            //   if(tokens[i] == "{")
            //         bracketLevel ++;
            //     else if(tokens[i] == "}")
            //         bracketLevel --;
            //     if (bracketLevel <= 0)
            //         break;
            //     serverTokens.push_back(tokens[i]);



// bool checkSemicolons(std::vector<std::vector<std::string>> tokens)
// {
//     for(size_t i = 0; i < tokens.size(); i++)
//     {
//         if(needSemicolon(tokens[i]) && tokens[i].back() != ";") 
//         {
//             std::cerr << "Error: missing semicolon: " << tokens[i].back() << std::endl;
//             return false;
//         }
//     }
//     return true;
// }



// bool needSemicolon(std::vector<std::string> tokenLine)
// {
//     if(tokenLine.back() == "{")
//         return false;
//     if(tokenLine.size() == 1 && tokenLine[0] == "}")
//         return(false);
//     return true;
// }

//make function if it needs semicolon or not. If it does check for last place is semicolon.
//check for double semicolon in the tokens. 
//server { works
//server
//{
//doesnt work. add this functionallity to needSemicolon. For "server {"
// ["server", "{"]
