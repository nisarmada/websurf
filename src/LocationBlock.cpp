#include "../includes/LocationBlock.hpp"

LocationBlock::LocationBlock() : _path(""), _root(""), _index("")
    ,_redirectUrl(""),_uploadPath(""), _cgiPass(""),_autoindex(false), _autoindexSet(false)
{

}

LocationBlock::~LocationBlock()
{

}

void LocationBlock::setPath(const std::string& path)
{
    _path = path;
}

const std::string& LocationBlock::getPath() const
{
    return _path;
}

void LocationBlock::setRoot(const std::string& root)
{
    _root = root;
}

const std::string& LocationBlock::getRoot() const
{
    return _root;
}

void LocationBlock::setIndex(const std::string& index)
{
    _index = index;
}
const std::string& LocationBlock::getIndex() const
{
    return _index;
}

void LocationBlock::addMethod(const std::string& method)
{
    _methods.insert(method);
}

const std::set<std::string>& LocationBlock::getMethods() const
{
    return _methods;
}

void LocationBlock::setAutoindex(bool value)
{
    _autoindex = value;
}

bool LocationBlock::getAutoindex() const
{
    return _autoindex;
}

void LocationBlock::setAutoindexDouble(bool value)
{
    _autoindexSet = value;
}
bool LocationBlock::getAutoindexDouble() const
{
    return _autoindexSet;
}

void LocationBlock::setRedirectUrl(const std::string& url)
{
    _redirectUrl = url;
}

const std::string& LocationBlock::getRedirectUrl() const
{
    return _redirectUrl;
}

void LocationBlock::setUploadPath(const std::string& path)
{
    _uploadPath = path;   
}

const std::string& LocationBlock::getUploadPath() const
{
    return _uploadPath;
}

void LocationBlock::setCgiPass(const std::string& cgi)
{
    _cgiPass = cgi;
}

const std::string& LocationBlock::getCgiPass() const
{
    return _cgiPass;
}










