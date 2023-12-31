#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <String>
#include "imgui_stdlib.h"
#include "MultiVaultHandler.h"
#include <iostream>
#include "MultiVaultHandler.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D_(HWND hWnd);

void CleanupDeviceD3D_();

void ResetDevice_();

//GLOBAL VARIABLES
std::string user_search_text;
std::vector<obsidian_result> searchResults = {};


LRESULT WINAPI WndProc_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WNDCLASSEXW wc;
HWND hwnd;
ImGuiIO io;
// https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
 std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}


void render_dear_imgui_with_obsidian(MultiVaultHandler *obsidian_handle) {
    // this links the dear imgui window with the obsidian instance (logic)
    ImGui::SetNextWindowSize(ImVec2(500, 500));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Obsidian MultiVault Search", NULL, ImGuiWindowFlags_NoCollapse |
                                                     ImGuiWindowFlags_NoResize | ImGuiCond_FirstUseEver);
    ImGui::InputText("Search Term", &user_search_text);
    // TODO: this needs a way better interface, e.g memory leaks
    if (ImGui::Button("Search")) {
        searchResults = obsidian_handle->searchForHashtags(
                user_search_text); // problem this result might go out of scope
    }

    if (ImGui::Button("Set Path")) {
        obsidian_handle->addFolderPath();
    }

    for (const auto &result: obsidian_handle->getResults()) {
        auto multiple_results = result.second;
        for (const auto &single_entry: multiple_results) {
            ImGui::Text(single_entry.hashtag.c_str());
            ImGui::Text(single_entry.path.c_str());
        }

    }
    ImGui::End();
    // Second window for the vault paths
    ImGui::SetNextWindowPos(ImVec2(0, 500));
    ImGui::SetNextWindowSize(ImVec2(500, 500));
    ImGui::Begin("Vault Paths", NULL, ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoResize | ImGuiCond_FirstUseEver);

    for (auto const &path: obsidian_handle->getVaultPaths()) {
        // is this bad code?
        std::string string_path = path.string();
        const char *text_pointer = string_path.c_str();

        if(ImGui::Button(text_pointer)){
            obsidian_handle->deletePath(path);
        }
    }

    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
    //ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(500, 0));
    ImGui::SetNextWindowSize(ImVec2(500, 500));
    ImGui::Begin("Results", NULL, ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoResize | ImGuiCond_FirstUseEver);

    for (auto const &single_result: searchResults) {
        std::string button_text = single_result.path;
        button_text += "in line " + single_result.line_number;
        if (ImGui::Button(button_text.c_str())) {
            //std::string command_to_open = "start obsidian://open?file=" + single_result.path;
            std::string command_to_open = "%windir%\\explorer.exe /select, "+ single_result.path ;
            std::cout << "User pressed " << command_to_open << single_result.path << std::endl;
            system(command_to_open.c_str());
        }
    }

    ImGui::End();

}


int create_dear_imgui() {
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    wc = {sizeof(wc), CS_CLASSDC, WndProc_, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
          L"Obsidian Multi Vault Search", NULL};
    ::RegisterClassExW(&wc);
    hwnd = ::CreateWindowW(wc.lpszClassName, L"Obsidian Multi Vault Search", WS_OVERLAPPEDWINDOW, 100, 100, 800,
                           600, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D_(hwnd)) {
        CleanupDeviceD3D_();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO();
    (void) io;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // TODO change window size

    return 0;
}

int stop_dear_imgui() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D_();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    std::cout << "Ending Programm now" << std::endl;
    return 0;
}

void render_dear_imgui(bool *done, MultiVaultHandler *obsidian_handle) {
    // function to handle rendering everything is copied from the example
    //
    // Poll and handle messages (inputs, window resize, etc.)
    // See the WndProc_() function below for our to dispatch events to the Win32 backend.
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            // USER Closes the app delete more stuff otherwise this will leak memory
            *done = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    render_dear_imgui_with_obsidian(obsidian_handle);


    // Rendering
    ImGui::EndFrame();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int) (clear_color.x * clear_color.w * 255.0f),
                                          (int) (clear_color.y * clear_color.w * 255.0f),
                                          (int) (clear_color.z * clear_color.w * 255.0f),
                                          (int) (clear_color.w * 255.0f));
    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    if (g_pd3dDevice->BeginScene() >= 0) {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        g_pd3dDevice->EndScene();
    }
    HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice_();

}


bool CreateDeviceD3D_(HWND hWnd) {
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp,
                             &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D_() {
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }
    if (g_pD3D) {
        g_pD3D->Release();
        g_pD3D = NULL;
    }
}

void ResetDevice_() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
                g_d3dpp.BackBufferWidth = LOWORD(lParam);
                g_d3dpp.BackBufferHeight = HIWORD(lParam);
                ResetDevice_();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
