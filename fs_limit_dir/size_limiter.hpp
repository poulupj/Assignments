#ifndef _SIZE_LIMITER_HPP_
#define _SIZE_LIMITER_HPP_

#include <filesystem>
#include <vector>

class SizeLimiter
{
public:
    SizeLimiter(const std::filesystem::path workingPath, const std::uintmax_t maxSize);

    /** 
     * @brief To construct the directory list of the files
     */
    void constructDirEntries();

    /**
     * @brief A helper function to sort the constructed directory list 
     */
    void sortDirEntries();

    /**
     * @brief A helper function used to calculate the current size of working directory
     */
    std::uintmax_t currentDirectorySize() const;

    /**
     * @brief A function to remove empty folders 
     */
    void removeEmptyFolders() const;

    /**
     * @brief A function to delete files if disk usage exceeds limit based on last modified
     */
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