#include <nfd.h>
#include <stdio.h>
#include <stdlib.h>

int test_file_dialog( void )
{

    nfdchar_t *folder_path = NULL;
    nfdresult_t result = NFD_PickFolder(nullptr, &folder_path);
    if ( result == NFD_OKAY ) {
        puts("Success!");
        puts(folder_path);
        free(folder_path);
    }
    else if ( result == NFD_CANCEL ) {
        puts("User pressed cancel.");
    }
    else {
        printf("Error: %s\n", NFD_GetError() );
    }

    return 0;
}