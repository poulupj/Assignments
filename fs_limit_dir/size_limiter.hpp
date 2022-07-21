#ifndef _SIZE_LIMITER_HPP_
#define _SIZE_LIMITER_HPP_

#include <filesystem>
#include <vector>

class SizeLimiter
{
public:
    SizeLimiter(const std::filesystem::path workingPath, const std::uintmax_t maxSize);

    void constructDirEntries();

    void sortDirEntries();

    std::uintmax_t currentDirectorySize() const;

    void removeEmptyFolders() const;

    void limitSize();

    ~SizeLimiter() = default;

private:
    // The working path
    std::filesystem::path m_workingPath;

    // Max size
    std::uintmax_t m_maxSize;

    // Stores directory entries
    std::vector<std::filesystem::directory_entry> m_dirEntries;
};

#endif