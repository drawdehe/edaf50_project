//#include <exception>
#include <stdexcept>

class NewsgroupDoesNotExistException: public std::runtime_error 
{
public: 
    NewsgroupDoesNotExistException(const std::string& msg): 
        std::runtime_error(msg) 
    {} 
}; 

class ArticleDoesNotExistException: public std::runtime_error{
    public:
        ArticleDoesNotExistException(const std::string& msg): std::runtime_error(msg)
        {}
};