#pragma once
#include "Parser.hpp"


//make a validate method that checks if everything is initialised in a good way and not missing. 
class LocationBlock
{
    private:
        std::string _path;
        std::string _root;
        std::string _index;
    
    public:
        LocationBlock();
        ~LocationBlock();
        
        void setPath(const std::string& path);
        const std::string& getPath() const;

        void setRoot(const std::string& root);
        const std::string& getRoot() const;

        void setIndex(const std::string& index);
        const std::string& getIndex() const;
};