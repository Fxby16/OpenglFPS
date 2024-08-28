#include <nfd.h>

char* OpenFile(const char* filter)
{
    nfdchar_t* out_path = nullptr;
    nfdresult_t result = NFD_OpenDialog(filter, nullptr, &out_path);
    
    switch(result){
        case NFD_OKAY:
            return out_path;
        case NFD_CANCEL:
            return nullptr;
        default:
            return nullptr;
    }
}