#include <iostream>
#include <windows.h>                
#include <string>
#include <thread>
#include <libloaderapi.h>

using namespace std;

//function to get the process id of the window
void get_proc_id(const char* window_title, DWORD &process_id){ 
    GetWindowThreadProcessId(FindWindow(NULL, window_title), &process_id);
}
//function to show an error message and exit the program
void error_check(const char* error_message, const char* error_title){
    MessageBoxA(0, error_message, error_title, 0);
    exit(-1);
}
//function to check if the dll file exists
bool dll_checker(string file_name){
    struct stat buffer;
    return (stat(file_name.c_str(), &buffer) == 0);
}




//main injector function
int main() {
    DWORD proc_id = NULL;
    char dll_path[MAX_PATH];
    const char* dll_name;
    const char* window_title;

    if(!dll_checker(dll_name)){
        error("DLL not found", "Error");
    }

    if(!GetFullPathNameA(dll_name, MAX_PATH, dll_path, NULL)){
        error("Failed to get full path name", "Error");
    }

    get_proc_id(window_title, proc_id);
    if (proc_id == NULL){
        error("Process not found", "Error");
    }

    HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);
    if(!h_process){
        error("Failed to open process", "Error");
    }
    void* alloc_mem = VirtualAllocEx(h_process, NULL, strlen(dll_path), MEM_COMMIT, PAGE_READWRITE);
    if(!alloc_mem){
        error("Failed to allocate memory", "Error");
    }

    if(!WriteProcessMemory(h_process, alloc_mem, dll_path, strlen(dll_path), NULL)){
        error("Failed to write memory", "Error");
    }

    HANDLE h_thread = CreateRemoteThread(h_process, NULL, 0, LPTHREAD_START_ROUTINE(LoadLibraryA), alloc_mem, 0, NULL);
    if(!h_thread){
        error("Failed to create remote thread", "Error");
    }
    CloseHandle(h_process);
    VirtualFreeEx(h_process, alloc_mem, 0, MEM_RELEASE);
    MessageBoxA(0, "DLL injected successfully", "Success", 0);
}