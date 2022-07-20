#include "size_limiter.hpp"
#include <iostream>
#include <cstdlib>

SizeLimiter::SizeLimiter(const std::filesystem::path workingPath, const std::uintmax_t maxSize) :
    m_workingPath(workingPath), m_maxSize(maxSize) {}

bool SizeLimiter::checkSizeExceeded() const
{
    std::string const home = std::getenv("HOME") ? std::getenv("HOME") : ".";
    std::uintmax_t size = 0;

    for(auto const& dir_entry : std::filesystem::recursive_directory_iterator(home + m_workingPath.string()))
    {
        std::filesystem::file_status currentFileStatus = std::filesystem::status(dir_entry);
        
        if(currentFileStatus.type() == std::filesystem::file_type::regular)
        {
            size += std::filesystem::file_size(dir_entry);
        }
    }
    
    // Converting present size in bytes to kilobytes
    size /= 1024;

    return size >= m_maxSize;
}

int main()
{
    SizeLimiter s("/Desktop/temp", 1);
    if(s.checkSizeExceeded())
    {
        std::cout<<"yes";
    }
    else
    {
        std::cout<<"no";
    }


    return 0;
}