#ifndef _SIZE_LIMITER_HPP_
#define _SIZE_LIMITER_HPP_

#include <filesystem>

class SizeLimiter 
{
public:
    SizeLimiter(const std::filesystem::path workingPath, const std::uintmax_t maxSize);

    bool checkSizeExceeded() const;

    ~SizeLimiter() = default;

private:
    // The working path
    std::filesystem::path m_workingPath;

    // Max size
    std::uintmax_t m_maxSize;
};

#endif