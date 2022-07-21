#include "size_limiter.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>

SizeLimiter::SizeLimiter(const std::filesystem::path workingPath, const std::uintmax_t maxSize) : m_workingPath(workingPath), m_maxSize(maxSize)
{
    constructDirEntries();
}

void SizeLimiter::constructDirEntries()
{
    std::string const home = std::getenv("HOME") ? std::getenv("HOME") : ".";

    for (auto const &dirEntry : std::filesystem::recursive_directory_iterator(home + m_workingPath.string()))
    {
        std::filesystem::file_status currentFileStatus = std::filesystem::status(dirEntry);

        if (currentFileStatus.type() == std::filesystem::file_type::regular)
        {
            m_dirEntries.push_back(dirEntry);
        }
    }
}

void SizeLimiter::sortDirEntries()
{
    std::sort(m_dirEntries.begin(), m_dirEntries.end(),
              [](const std::filesystem::directory_entry &dirEntry1, const std::filesystem::directory_entry &dirEntry2)
              {
                  return std::filesystem::last_write_time(dirEntry1) < std::filesystem::last_write_time(dirEntry2);
              });
}

std::uintmax_t SizeLimiter::currentDirectorySize() const
{
    std::uintmax_t size = 0;

    for (auto const &dirEntry : m_dirEntries)
    {
        std::filesystem::file_status currentFileStatus = std::filesystem::status(dirEntry);

        if (currentFileStatus.type() == std::filesystem::file_type::regular)
        {
            size += std::filesystem::file_size(dirEntry);
        }
    }

    return size;
}

void SizeLimiter::removeEmptyFolders() const
{
    std::string const home = std::getenv("HOME") ? std::getenv("HOME") : ".";
    std::vector<std::filesystem::directory_entry> dirEntriesToDelete;

    for (auto const &dirEntry : std::filesystem::recursive_directory_iterator(home + m_workingPath.string()))
    {
        std::filesystem::file_status currentFileStatus = std::filesystem::status(dirEntry);

        if (currentFileStatus.type() == std::filesystem::file_type::directory)
        {
            if (std::filesystem::is_empty(dirEntry))
            {
                dirEntriesToDelete.push_back(dirEntry);
            }
        }
    }

    for (auto const &dirEntry : dirEntriesToDelete)
    {
        std::filesystem::remove(dirEntry);
    }
}

void SizeLimiter::limitSize()
{
    std::uintmax_t currentSize = currentDirectorySize();

    // Convert m_maxSize to bytes
    std::uintmax_t maxSizeBytes = m_maxSize * 1024;

    if (currentSize >= maxSizeBytes)
    {
        sortDirEntries();

        for (auto const &dirEntry : m_dirEntries)
        {
            currentSize -= std::filesystem::file_size(dirEntry);
            std::filesystem::remove(dirEntry);

            if (currentSize < maxSizeBytes)
            {
                break;
            }
        }
    }

    removeEmptyFolders();
}

int main()
{
    SizeLimiter s("/Desktop/temp2", 2);
    s.limitSize();
    return 0;
}