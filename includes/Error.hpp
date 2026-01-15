#ifndef ERROR_HPP
#define ERROR_HPP



#include <iostream>


class Error
{
    public:
        class password : public std::exception
        {
            public:
                virtual const char *what() const throw();
        }; 

}














#endif