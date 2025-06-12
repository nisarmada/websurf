/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Parsing.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: snijhuis <snijhuis@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/06/09 17:21:18 by snijhuis      #+#    #+#                 */
/*   Updated: 2025/06/12 14:32:40 by snijhuis      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"


void parsing(const char* path)
{
    std::ifstream configFile = openConfigFile(path);
    std::vector<std::string> cleanedLines = trimBeginEnd(configFile);

    std::vector<std::string> tokens = getAllTokens(cleanedLines);
    checkSyntax(tokens);
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
    for(size_t i = 0; i < tokens.size(); i ++)
        std::cout << "[" << tokens[i] << "]" << std::endl;
    return tokens;
}

bool isSpecialChar(char c)
{
    return c == '{' || c == '}' || c == ';';
}

bool checkSyntax(std::vector<std::string>tokens)
{
    if(checkSemicolons(tokens) == false)
        exit(1);
    return false;
}

bool checkSemicolons(std::vector<std::string> tokens)
{
    for (size_t i = 0; i < tokens.size(); i++)
    {
        mustHaveSemicolon(tokens, i);
        wrongPlaceSemicolon(tokens, i);
    }
    return true;
}

void mustHaveSemicolon(std::vector<std::string> tokens, size_t i)
{
        if(getType(tokens[i]) == STATEMENT)
        {
            if(i + 2 >= tokens.size() || tokens[i + 2] != ";")
            {
                std::cerr << "Error: missing semicolon \"" << tokens[i] << "\"" << std::endl;
                exit(1);
            }
        }
}

void wrongPlaceSemicolon(std::vector<std::string> tokens, size_t i)
{
        if(tokens[i] == ";")
        {
            if(i == 0 || i == 1)
            {
                std::cerr << "Error: config file cannot start with semicolon" << std::endl;
                exit(1);
            }
            if(getType(tokens[i - 2]) != STATEMENT)
            {
                std::cerr << "Error: invalid semicolon \"" << tokens[i - 2] << "\"" << std::endl;
                exit(1);
            }
    }
}

Type getType(const std::string& token)
{
    static std::map<std::string, Type> typeMap = 
    {
        {"server", BLOCK},
        {"location", BLOCK},
        {"listen", STATEMENT},
        {"server_name", STATEMENT},
        {"root", STATEMENT},
        {"index", STATEMENT}
    };
    if(typeMap.find(token) != typeMap.end()) //find returns end(represents one past the last element) if it didnt find it in the map
        return typeMap[token]; //returns the map location with the correct enum checked if it existed before otherwise it adds the token to the map as a default.
    return UNKNOWN;
}






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
