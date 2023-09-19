

#ifndef OBSIDIAN_MULTI_VAULT_SEARCH_IMGUIWRAPPER_H
#define OBSIDIAN_MULTI_VAULT_SEARCH_IMGUIWRAPPER_H
#include "MultiVaultHandler.h"


int create_dear_imgui();
void render_dear_imgui(bool *done, MultiVaultHandler* obsidian_handle);
int stop_dear_imgui();
void render_dear_imgui_with_obsidian(MultiVaultHandler* obsidian_handle);

#endif //OBSIDIAN_MULTI_VAULT_SEARCH_IMGUIWRAPPER_H
