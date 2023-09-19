#include "ImGuiWrapper.h"
#include "MultiVaultHandler.h"
#include "iostream"

bool done = false;


int main(){
    create_dear_imgui();
    auto obsidian_handler = new MultiVaultHandler{};

    while(!done){
        render_dear_imgui(&done, obsidian_handler);
    }

    if(done){
        std::cout << stop_dear_imgui() << std::endl;
    }

    delete obsidian_handler;

    return 0;
}