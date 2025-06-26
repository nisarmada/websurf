#pragma once
#include "Parser.hpp"


//make a validate method that checks if everything is initialised in a good way and not missing. 
class LocationBlock
{
    private:
        std::string _path;
        std::string _root;
        std::string _index;
        std::string _redirectUrl;
        std::string _uploadPath;
        std::string _cgiPass;
        std::set<std::string> _methods;
        bool _autoindex;
        bool _autoindexSet;
    
    public:
        LocationBlock();
        ~LocationBlock();
        
        void setPath(const std::string& path);
        const std::string& getPath() const;

        void setRoot(const std::string& root);
        const std::string& getRoot() const;

        void setIndex(const std::string& index);
        const std::string& getIndex() const;

        void addMethod(const std::string& method);
        const std::set<std::string>& getMethods() const;

        void setAutoindex(bool value);
        bool getAutoindex() const;

        void setAutoindexDouble(bool value);
        bool getAutoindexDouble() const;

        void setRedirectUrl(const std::string& url);
        const std::string& getRedirectUrl() const;

        void setUploadPath(const std::string& path);
        const std::string& getUploadPath() const;

        void setCgiPass(const std::string& cgi);
        const std::string& getCgiPass() const;
};