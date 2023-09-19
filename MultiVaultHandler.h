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
#include "secret.h"




struct obsidian_result {
    std::string path;
    uint32_t line_number;
    std::string hashtag;
    std::string line;
} ;

// defines are here for now user should be able to input them later down in the development
//----------------------------------------------------------------
namespace fs = std::filesystem;

//----------------------------------------------------------------

class MultiVaultHandler {
public:
    MultiVaultHandler();

    bool search(std::string search_string);
    void findMarkdownFiles(const fs::path& directory_path); // find all markdownfiles
    std::vector<obsidian_result*> searchForTextInMarkdown(const fs::path& markdown_file); // search for the hashtags in the markdownfiles
private:

    fs::path temp_dir_hardcoded_path = hardcoded_path;
    //std::regex regex_pattern("#[\\w]+"); // everything beginning with a hashtag
    std::unordered_map<std::string, std::vector<obsidian_result*> > results_hash_map = {};
    std::vector<fs::path> markdown_files = {};
};


#endif //OBSIDIAN_MULTI_VAULT_SEARCH_MULTIVAULTHANDLER_H
