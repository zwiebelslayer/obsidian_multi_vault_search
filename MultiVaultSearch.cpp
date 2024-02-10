#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <regex>
#include <Windows.h>


namespace fs = std::filesystem;
std::string directory_path_to_test = "";
std::regex regex_pattern_to_test("#[\\w]+"); // everything beginning with a hashtag



void findMarkdownFiles(const fs::path &directory_path, std::vector<fs::path> &markdown_files) {
    try {
        // Iterate over the directory
        for (const auto &entry: fs::directory_iterator(directory_path)) {
            if (fs::is_directory(entry)) {
                // If the entry is a subdirectory, recursively search it
                findMarkdownFiles(entry, markdown_files);
            } else if (entry.path().extension() == ".md") {
                // If the entry is a Markdown file, add its path to the vector
                markdown_files.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

// Function to search for lines containing "#Sometext" in a Markdown file
void searchForTextInMarkdown(const fs::path &markdown_file) {
    try {
        std::ifstream file(markdown_file);
        if (file.is_open()) {
            std::string line;
            int line_number = 0;

            while (std::getline(file, line)) {
                line_number++;


                std::regex pattern(regex_pattern_to_test);
                std::smatch match;
                // Check if the line contains the search text
                if (std::regex_search(line, match, pattern)) {
                    std::cout << "Found in file: " << markdown_file << " (line " << line_number << "):\n" << line
                              << std::endl;
                }
            }

            file.close();
        } else {
            std::cerr << "Unable to open file: " << markdown_file << std::endl;
        }
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

int not_main() {
    // WINDOWS UTF-8 Charset workaround :(
    // see: https://stackoverflow.com/questions/45575863/how-to-print-utf-8-strings-to-stdcout-on-windows
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);


    std::vector<fs::path> markdown_files;

    try {
        // Check if the provided path exists and is a directory
        if (fs::is_directory(directory_path_to_test)) {
            // Find Markdown files recursively within the directory
            findMarkdownFiles(directory_path_to_test, markdown_files);

            // Search for the specified text in each Markdown file
            for (const auto &markdown_file: markdown_files) {
                searchForTextInMarkdown(markdown_file);
            }
        } else {
            std::cerr << "Provided path is not a directory." << std::endl;
        }
    } catch (const fs::filesystem_error &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }


    return 0;
}
