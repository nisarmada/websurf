#include "../includes/LocationBlock.hpp"

LocationBlock::LocationBlock() : _path(""), _root(""), _index("")
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





