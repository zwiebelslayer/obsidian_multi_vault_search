#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;
std::string directory_path = "C:\\Users\\Pascal\\Nextcloud\\ObsidianVaults";

void findMarkdownFiles(const fs::path& directory_path, std::vector<fs::path>& markdown_files) {
    try {
        // Iterate over the directory
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            if (fs::is_directory(entry)) {
                // If the entry is a subdirectory, recursively search it
                findMarkdownFiles(entry, markdown_files);
            } else if (entry.path().extension() == ".md") {
                // If the entry is a Markdown file, add its path to the vector
                markdown_files.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

// Function to search for lines containing "#Sometext" in a Markdown file
void searchForTextInMarkdown(const fs::path& markdown_file, const std::string& search_text) {
    try {
        std::ifstream file(markdown_file);
        if (file.is_open()) {
            std::string line;
            int line_number = 0;

            while (std::getline(file, line)) {
                line_number++;

                // Check if the line contains the search text
                if (line.find(search_text) != std::string::npos) {
                    std::cout << "Found in file: " << markdown_file << " (line " << line_number << "):\n" << line << std::endl;
                }
            }

            file.close();
        } else {
            std::cerr << "Unable to open file: " << markdown_file << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

int main() {

    std::vector<fs::path> markdown_files;

    try {
        // Check if the provided path exists and is a directory
        if (fs::is_directory(directory_path)) {
            // Find Markdown files recursively within the directory
            findMarkdownFiles(directory_path, markdown_files);

            // Search for the specified text in each Markdown file
            std::string search_text = "#todo";
            for (const auto& markdown_file : markdown_files) {
                searchForTextInMarkdown(markdown_file, search_text);
            }
        } else {
            std::cerr << "Provided path is not a directory." << std::endl;
        }
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
