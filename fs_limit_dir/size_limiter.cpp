#include "size_limiter.hpp"
#include <iostream>
#include <algorithm>

SizeLimiter::SizeLimiter(const std::filesystem::path workingPath, const std::uintmax_t maxSize) : m_workingPath(std::filesystem::current_path() / workingPath), m_maxSize(maxSize) {}

void SizeLimiter::constructDirEntries()
{
    for (auto const &dirEntry : std::filesystem::recursive_directory_iterator(m_workingPath))
    {
        // Add the directory_entry to the list if it is a file
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
        // Add the size of each file
        size += std::filesystem::file_size(dirEntry);
    }

    return size;
}

void SizeLimiter::removeEmptyFolders() const
{
    std::vector<std::filesystem::directory_entry> dirEntriesToDelete;

    for (auto const &dirEntry : std::filesystem::recursive_directory_iterator(m_workingPath))
    {
        std::filesystem::file_status currentFileStatus = std::filesystem::status(dirEntry);

        if (currentFileStatus.type() == std::filesystem::file_type::directory)
        {
            if (std::filesystem::is_empty(dirEntry))
            {
                // If an empty directory is found, add it to delete list
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
    constructDirEntries();

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

            // Remove files until size is below the limit
            if (currentSize < maxSizeBytes)
            {
                break;
            }
        }
    }

    removeEmptyFolders();
}