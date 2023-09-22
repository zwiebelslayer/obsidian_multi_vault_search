#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <regex>
#include <Windows.h>
#include "MultiVaultHandler.h"
#include <nfd.h>
#include <stdio.h>
#include <stdlib.h>


const std::string vault_file_name = "vault_paths.txt";

MultiVaultHandler::MultiVaultHandler() {
    // read file of existing vault paths
    std::ifstream vault_file(vault_file_name);

    if (!vault_file || vault_file.bad() || vault_file.fail()) {
        std::cout << "Failed to read existing paths" << std::endl;
    } else if (vault_file.good()) {
        for (std::string line; getline(vault_file, line);) {
            std::cout << line << std::endl;
            try {
                this->obsidian_vaults_path.emplace_back(fs::path(line.c_str()));
            }
            catch (std::filesystem::filesystem_error const &ex) {
                std::cout << "caught an error with file " << line << ex.what() << std::endl;
            }
        }
    }

    for (const auto &path: obsidian_vaults_path) {
        std::cout << path << std::endl;
    }

    // create the hashmap
    this->createHashmapWithHashtags();

}

MultiVaultHandler::~MultiVaultHandler() {
    std::cout << "saving paths" << std::endl;
    for (const auto &path: obsidian_vaults_path) {
        std::cout << "path " << path << std::endl;
    }

    std::fstream vault_paths;
    vault_paths.open(vault_file_name, std::ios::out);
    if (!vault_paths) {
        std::cout << "Error saving vault paths";
    } else {
        for (const auto &path: obsidian_vaults_path) {
            vault_paths << path.string() << std::endl;
        }

        vault_paths.close();
    }

    // Free memory
    obsidian_vaults_path.clear();
    markdown_files.clear();
    results_hash_map.clear();

}


bool MultiVaultHandler::createHashmapWithHashtags() {


    auto iterator = this->obsidian_vaults_path.begin();
    while (iterator != this->obsidian_vaults_path.end()) {

        if (!std::filesystem::is_directory(*iterator)) {
            std::cout << "deleting vault path is not valid " << *iterator << std::endl;
            iterator = this->obsidian_vaults_path.erase(
                    iterator); // delete this entry NOTE: this seems to be a slow operation since vector is shifted everytime
            continue;
        }

        this->findMarkdownFiles(*iterator, 0);
        iterator++;
    }

    for (const auto &file: this->markdown_files) {
        std::vector<obsidian_result *> results_vector = this->searchForTextInMarkdown(file);
        for (const auto &result: results_vector) {
            results_hash_map[result->hashtag].emplace_back(result);
        }
    }

    for (const auto &pair: this->results_hash_map) {
        const std::string &key = pair.first;
        const std::vector<obsidian_result *> &values = pair.second;

        std::cout << "Key: " << key << std::endl;
        for (const auto &item: values) {
            std::cout << "  Path: " << item->path << ", Line Number: " << item->line_number << "Hashtag: "
                      << item->hashtag << std::endl;
        }
    }


    return true;
}


void MultiVaultHandler::findMarkdownFiles(const fs::path &directory_path, uint16_t depth) {
    if (depth > MAX_DEPTH) {
        std::cout << "max depth reached " << std::endl;
        return;
    }

    try {
        // Iterate over the directory
        for (const auto &entry: fs::directory_iterator(directory_path)) {
            if (fs::is_directory(entry)) {
                // If the entry is a subdirectory, recursively search it
                this->findMarkdownFiles(entry, depth++);
            } else if (entry.path().extension() == ".md") {
                // If the entry is a Markdown file, add its path to the vector
                this->markdown_files.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;

    }
}

std::vector<obsidian_result *> MultiVaultHandler::searchForTextInMarkdown(const fs::path &markdown_file) {
    auto return_vector = std::vector<obsidian_result *>{};

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
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return return_vector;

}

bool MultiVaultHandler::addFolderPath() {

    char *folder_path = nullptr;
    nfdresult_t result = NFD_PickFolder(nullptr, &folder_path);
    if (result == NFD_OKAY) {
        puts("Success!");
        puts(folder_path);
        fs::path new_path = folder_path;
        //
        this->obsidian_vaults_path.push_back(new_path);
        // remove duplicates
        std::sort(obsidian_vaults_path.begin(), obsidian_vaults_path.end());
        obsidian_vaults_path.erase(std::unique(obsidian_vaults_path.begin(), obsidian_vaults_path.end()), obsidian_vaults_path.end());
        //vec.erase( unique( vec.begin(), vec.end() ), vec.end() );
        free(folder_path);
    } else if (result == NFD_CANCEL) {
        puts("User pressed cancel.");
    } else {
        printf("Error: %s\n", NFD_GetError());
    }
    this->createHashmapWithHashtags();
    return false;
}

std::unordered_map<std::string, std::vector<obsidian_result *> > MultiVaultHandler::getResults() {
    return this->results_hash_map;
}
std::vector<std::filesystem::path> MultiVaultHandler::getVaultPaths() {
    return this->obsidian_vaults_path;
}

std::vector<obsidian_result> MultiVaultHandler::searchForHashtags(const std::string &search){
    std::string search_string_w_hashtag = "#" + search;
    auto searchResult = this->results_hash_map.find(search_string_w_hashtag);
    std::vector<obsidian_result> return_results = {};

    if(searchResult == results_hash_map.end())
    {
        std::cout << "could not find the given key in the files " << std::endl;
        return return_results;
    }


    for(auto vector_iter : searchResult->second){
        obsidian_result res = obsidian_result{vector_iter->path, vector_iter->line_number, vector_iter->hashtag, vector_iter->line};
        std::cout << "Found in file " << vector_iter->path << std::endl;
        return_results.push_back(res);
    }

    return return_results;
}
