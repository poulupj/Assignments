#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <thread>

#include "size_limiter.hpp"

// Function to create a dummy file
void createDummyFile(const std::string &pathToFile, const int numberOfLines)
{
    std::ofstream file(pathToFile);

    int count = 0;
    long data = 100;

    while (count < numberOfLines)
    {
        file << data << "\n";
        data += 10;
        ++count;
    }
}

// Function to create the test directory
void generateDirectory(const std::filesystem::path &inputPath)
{
    std::filesystem::path pwd = std::filesystem::current_path() / inputPath;

    std::filesystem::create_directory(pwd);

    createDummyFile(pwd.string() + "/f1.txt", 100);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::filesystem::create_directory(pwd.string() + "/dir1");
    createDummyFile(pwd.string() + "/dir1/f2.txt", 500);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    createDummyFile(pwd.string() + "/dir1/f3.txt", 1000);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::filesystem::create_directory(pwd.string() + "/dir2");
    createDummyFile(pwd.string() + "/dir2/f4.txt", 1200);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    createDummyFile(pwd.string() + "/dir2/f5.txt", 800);
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// To get the present directory structure to compare
std::vector<std::filesystem::directory_entry> getSortedDirectoryEntries(const std::filesystem::path &inputPath)
{
    std::filesystem::path pwd = std::filesystem::current_path() / inputPath;

    std::vector<std::filesystem::directory_entry> currentDirectoryEntries;

    for (auto const &dirEntry : std::filesystem::recursive_directory_iterator(pwd))
    {
        std::filesystem::file_status currentFileStatus = std::filesystem::status(dirEntry);

        if (currentFileStatus.type() == std::filesystem::file_type::regular)
        {
            currentDirectoryEntries.push_back(dirEntry);
        }
    }

    std::sort(currentDirectoryEntries.begin(), currentDirectoryEntries.end(),
              [](const std::filesystem::directory_entry &dirEntry1, const std::filesystem::directory_entry &dirEntry2)
              {
                  return std::filesystem::last_write_time(dirEntry1) < std::filesystem::last_write_time(dirEntry2);
              });

    return currentDirectoryEntries;
}

// tests with gtest framework

// No files should be deleted if size is under the limit
TEST(NoDeletionTest, SizeUnderLimit)
{
    std::filesystem::path p = "sample_dir";
    std::uintmax_t s = 30;

    generateDirectory(p);

    SizeLimiter sizeLimiterObject(p, s);
    sizeLimiterObject.limitSize();

    std::vector<std::filesystem::directory_entry> expectedDirectoryEntries;

    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/f1.txt"));
    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir1/f2.txt"));
    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir1/f3.txt"));
    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir2/f4.txt"));
    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir2/f5.txt"));

    std::vector<std::filesystem::directory_entry> actualDirectoryEntries = getSortedDirectoryEntries(p);

    ASSERT_EQ(expectedDirectoryEntries.size(), actualDirectoryEntries.size());

    for (auto i = 0; i < expectedDirectoryEntries.size(); i++)
    {
        EXPECT_EQ(expectedDirectoryEntries[i], actualDirectoryEntries[i]);
    }

    std::filesystem::remove_all(std::filesystem::current_path() / p);
}

// Delete the oldest two files when the limit is exceeded
TEST(DeletionTest, SizeOverLimit)
{
    std::filesystem::path p = "sample_dir";
    std::uintmax_t s = 15;

    generateDirectory(p);

    SizeLimiter sizeLimiterObject(p, s);
    sizeLimiterObject.limitSize();

    std::vector<std::filesystem::directory_entry> expectedDirectoryEntries;

    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir1/f3.txt"));
    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir2/f4.txt"));
    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir2/f5.txt"));

    std::vector<std::filesystem::directory_entry> actualDirectoryEntries = getSortedDirectoryEntries(p);

    ASSERT_EQ(expectedDirectoryEntries.size(), actualDirectoryEntries.size());

    for (auto i = 0; i < expectedDirectoryEntries.size(); i++)
    {
        EXPECT_EQ(expectedDirectoryEntries[i], actualDirectoryEntries[i]);
    }

    std::filesystem::remove_all(std::filesystem::current_path() / p);
}

// Remove the folder if deletions resulted in it being empty
TEST(DeletionTest, FolderRemoved)
{
    std::filesystem::path p = "sample_dir";
    std::uintmax_t s = 11;

    generateDirectory(p);

    SizeLimiter sizeLimiterObject(p, s);
    sizeLimiterObject.limitSize();

    std::vector<std::filesystem::directory_entry> expectedDirectoryEntries;

    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir2/f4.txt"));
    expectedDirectoryEntries.push_back(std::filesystem::directory_entry(std::filesystem::current_path().string() + "/sample_dir/dir2/f5.txt"));

    std::vector<std::filesystem::directory_entry> actualDirectoryEntries = getSortedDirectoryEntries(p);

    ASSERT_EQ(expectedDirectoryEntries.size(), actualDirectoryEntries.size());

    ASSERT_FALSE(std::filesystem::exists(std::filesystem::current_path().string() + "/sample_dir/dir1/"));

    for (auto i = 0; i < expectedDirectoryEntries.size(); i++)
    {
        EXPECT_EQ(expectedDirectoryEntries[i], actualDirectoryEntries[i]);
    }

    std::filesystem::remove_all(std::filesystem::current_path() / p);
}
