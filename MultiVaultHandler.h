#ifndef OBSIDIAN_MULTI_VAULT_SEARCH_MULTIVAULTHANDLER_H
#define OBSIDIAN_MULTI_VAULT_SEARCH_MULTIVAULTHANDLER_H

#include <iostream>
#include <unordered_map>
#include "string"
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>




struct obsidian_result {
    std::string path;
    uint32_t line_number;
    std::string hashtag;
    std::string line;
} ;
const uint16_t MAX_DEPTH = 100;


//----------------------------------------------------------------
namespace fs = std::filesystem;
//----------------------------------------------------------------

class MultiVaultHandler {
public:
    MultiVaultHandler();
    ~MultiVaultHandler();

    bool createHashmapWithHashtags();
    void findMarkdownFiles(const fs::path& directory_path, uint16_t depth); // find all markdownfiles
    std::vector<obsidian_result*> searchForTextInMarkdown(const fs::path& markdown_file); // search for the hashtags in the markdownfiles
    bool addFolderPath();
    void deletePath(fs::path inputPath);
    std::unordered_map<std::string, std::vector<obsidian_result*> > getResults();
    std::vector<std::filesystem::path> getVaultPaths();

    std::vector<obsidian_result> searchForHashtags(const std::string& search_string);
private:
    std::vector<obsidian_result> searchForSearchTerm(const std::string &searchTerm);

    std::vector<fs::path> obsidian_vaults_path = {};

    std::unordered_map<std::string, std::vector<obsidian_result*> > results_hash_map = {};  // having a pointer here is not good code TODO: remove pointers
    std::vector<fs::path> markdown_files = {};
};


#endif //OBSIDIAN_MULTI_VAULT_SEARCH_MULTIVAULTHANDLER_H
