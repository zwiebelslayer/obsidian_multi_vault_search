#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <regex>
#include <Windows.h>
#include "MultiVaultHandler.h"
#include <nfd.h>
#include <stdio.h>
#include <stdlib.h>



MultiVaultHandler::MultiVaultHandler() {
    // constructor empty for now
}

bool MultiVaultHandler::search(std::string search_string){

    this->findMarkdownFiles(temp_dir_hardcoded_path);
    for (const auto& file: this->markdown_files){
        std::vector<obsidian_result*> results_vector = this->searchForTextInMarkdown(file);
        for(const auto& result : results_vector){
            results_hash_map[result->hashtag].emplace_back(result);
        }


    }

    std::cout << "found the following in the files" << std::endl;
    for (const auto& pair : this->results_hash_map) {
        const std::string& key = pair.first;
        const std::vector<obsidian_result*>& values = pair.second;

        std::cout << "Key: " << key << std::endl;
        for (const auto& item : values) {
            std::cout << "  Path: " << item->path << ", Line Number: " << item->line_number <<"Hashtag: "<< item->hashtag << std::endl;
        }
    }


    return true;
}



void MultiVaultHandler::findMarkdownFiles(const fs::path &directory_path) {
    try {
        // Iterate over the directory
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            if (fs::is_directory(entry)) {
                // If the entry is a subdirectory, recursively search it
                this->findMarkdownFiles(entry);
            } else if (entry.path().extension() == ".md") {
                // If the entry is a Markdown file, add its path to the vector
                this->markdown_files.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

std::vector<obsidian_result*> MultiVaultHandler::searchForTextInMarkdown(const fs::path& markdown_file) {
       auto return_vector = std::vector<obsidian_result*>{};

    try {
        std::ifstream file(markdown_file);
        if (file.is_open()) {
            std::string line;
            int line_number = 0;

            while (std::getline(file, line)) {
                line_number++;

                //std::regex pattern("#[\\w]+");
                std::regex pattern("\\W(\\#[a-zA-Z]+\\b)");
                std::smatch match;
                // Check if the line contains the search text
                if (std::regex_search(line, match, pattern)) {
                    //std::cout << "Found in file: " << markdown_file << " (line " << line_number << "):\n" << line <<  std::endl;
                    for (size_t i = 0; i < match.size(); ++i) {
                        auto new_obrs = new obsidian_result{};
                        new_obrs->line_number = line_number;
                        new_obrs->path = fs::path(markdown_file).string();
                        new_obrs->hashtag = match[i];
                        new_obrs->line = line;
                        return_vector.push_back(new_obrs);
                    }

                }

            }

            file.close(); // very important to close the file again
        } else {
            std::cerr << "Unable to open file: " << markdown_file << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return return_vector;

}

bool MultiVaultHandler::addFolderPath() {

        char *folder_path = nullptr;
        nfdresult_t result = NFD_PickFolder(nullptr, &folder_path);
        if ( result == NFD_OKAY ) {
            puts("Success!");
            puts(folder_path);
            fs::path new_path = folder_path;
            this->obsidian_vaults_path.push_back(new_path);
            free(folder_path);
        }
        else if ( result == NFD_CANCEL ) {
            puts("User pressed cancel.");
        }
        else {
            printf("Error: %s\n", NFD_GetError() );
        }

    return false;
}
