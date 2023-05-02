//#include <exception>
#include <stdexcept>

class NewsgroupDoesNotExistError: public std::runtime_error 
{
public: 
    NewsgroupDoesNotExistError(const std::string& msg): 
        std::runtime_error(msg) 
    {} 
}; 

class ArticleDoesNotExistError: public std::runtime_error{
    public:
        ArticleDoesNotExistError(const std::string& msg): std::runtime_error(msg)
        {}
};