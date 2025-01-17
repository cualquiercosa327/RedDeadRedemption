#pragma once
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "Memory/Pattern.hpp"
#undef vector
#include <vector>

class GameVariables
{
public:
   using OSKInit_t = void(*)(uintptr_t keyboardContext);
   OSKInit_t OSKInit{};

   using OSKTitle_t = void(*)(uintptr_t keyboardContext, const char* title);
   OSKTitle_t OSKTitle{};

   using OSKDescription_t = void(*)(uintptr_t keyboardContext, const char* description);
   OSKDescription_t OSKDescription{};

   using DisplayOSK_t = bool(*)(uintptr_t keyboardContext, uint32_t textView, uint32_t saveLastInput);
   DisplayOSK_t DisplayOSK{};

   using RegisterNative_t = void(*)(uint32_t nativeHash, uint32_t nativeFuntion);
   RegisterNative_t RegisterNative{};

   using ParseScrInstructions_t = uint32_t(*)(uintptr_t scriptStack, uintptr_t** globalBase, uintptr_t scrPageTable, uint32_t r6, uintptr_t scrThread);
   ParseScrInstructions_t ParseScrInstructions{};

   using GetScrProgramFromHash_t = uintptr_t(*)(uint32_t scriptHash);
   GetScrProgramFromHash_t GetScrProgramFromHash{};

   using SetVectorResults_t = void(*)(void* context);
   SetVectorResults_t RAGE_SetVectorResults{};

   using RemoteExplosionEvent_SetExplosion_t = void(*)(uintptr_t netProjectile_RemoteExplosionEvent, uintptr_t netObjectManager, uint64_t uuid);
   RemoteExplosionEvent_SetExplosion_t RemoteExplosionEvent_SetExplosion{};

   using GetSnuGamerList_t = uintptr_t(*)();
   GetSnuGamerList_t GetSnuGamerList{};

   using IsSnuGamerListValid_t = bool(*)(uintptr_t gamerList);
   IsSnuGamerListValid_t IsSnuGamerListValid{};

   using GetSagActorAddress_t = uintptr_t(*)(void* nativeContext, void* outBuf);
   GetSagActorAddress_t GetSagActorAddress{};

   using GetHudGamerList_t = uintptr_t(*)();
   GetHudGamerList_t GetHudGamerList{};

#if DEBUG
   using printf_t = int(*)(const char* format, ...);
   printf_t __printf{};

   using sprintf_t = int(*)(char* buffer, const char* format, ...);
   sprintf_t __sprintf{};

   using snprintf_t = int(*)(char* buffer, int size, const char* format, ...);
   snprintf_t __snprintf{};

   using vsnprintf_t = int(*)(char* s, size_t n, const char* format, va_list arg);
   vsnprintf_t __vsnprintf{};

   using sscanf_t = int(*)(const char* str, const char* format, ...);
   sscanf_t __sscanf{};

   using strncpy_t = char*(*)(char* destination, const char* source, size_t num);
   strncpy_t __strncpy{};

   using strtol_t = long int(*)(const char* str, char** endptr, int base);
   strtol_t __strtol{};

   using strtok_t = char*(*)(char* str, const char* delim);
   strtok_t __strtok{};

   using strcat_t = char*(*)(char* destination, const char* source);
   strcat_t __strcat{};

   using malloc_t = void*(*)(size_t size);
   malloc_t __malloc{};

   using rand_t = int(*)();
   rand_t __rand{};

   using exit_t = void(*)(int status);
   exit_t __exit{};

   using localtime_t = struct tm*(*)(const time_t* timer);
   localtime_t __localtime{};

   using wcslen_t = size_t(*)(const wchar_t* str);
   wcslen_t __wcslen{};

   using fopen_t = FILE*(*)(const char* filename, const char* mode);
   fopen_t __fopen{};

   using fread_t = size_t(*)(void* ptr, size_t size, size_t count, FILE* stream);
   fread_t __fread{};

   using fseek_t = int(*)(FILE* stream, long int offset, int origin);
   fseek_t __fseek{};

   using ftell_t = long int(*)(FILE* stream);
   ftell_t __ftell{};

   using fwrite_t = size_t(*)(const void* ptr, size_t size, size_t count, FILE* stream);
   fwrite_t __fwrite{};

   using fflush_t = int(*)(FILE* stream);
   fflush_t __fflush{};

   using fprintf_t = int(*)(FILE* stream, const char* format, ...);
   fprintf_t __fprintf{};

   using fclose_t = int(*)(FILE* stream);
   fclose_t __fclose{};
#endif // DEBUG

   bool FindSignatures();
   void Shutdown();


public:
   uintptr_t pNativeRegistration{};
   uintptr_t pRDRUIGame{};
   uintptr_t** pGlobalVars{};
   uint32_t VmOpCode44_bcctrl{};
   uint32_t pHasInfiniteHorseStamina{};
   uintptr_t pBlazingGuns{};


private:
    template <typename T>
    void SetPat(const char* name, const char* pattern, const char* mask, T* out,
        int32_t relativeOffset = 0x00,
        bool getFromHighLow = false, int32_t highAdditive = 0x00, int32_t lowAdditive = 0x00)
    {
        uintptr_t ptr = FindPatternInTextSegment((uint8_t*)pattern, mask);
        if (ptr == 0)
        {
            printf("Failed to find %s pattern.\n", name);
            *out = 0;
            return;
        }

        ptr = ptr + relativeOffset;

        if (getFromHighLow)
            ptr = ReadHighLow(ptr, highAdditive, lowAdditive);

        printf("found ptr %s at 0x%X\n", name, ptr);

        *out = (T)ptr;
    }

    template <typename T>
    void SetFn(const char* name, const char* pattern, const char* mask, T* out,
        int32_t relativeOffset = 0x00, bool resolveBranch = false)
    {
        uintptr_t ptr = FindPatternInTextSegment((uint8_t*)pattern, mask);
        if (ptr == 0)
        {
            printf("Failed to find %s pattern.\n", name);
            *out = nullptr;
            return;
        }

        uint32_t* opd = (uint32_t*)malloc(2 * sizeof(uint32_t));

        m_OpdTable.push_back(opd);

        ptr = ptr + relativeOffset;

        if (resolveBranch)
            opd[0] = ResolveBranch(ptr);
        else
            opd[0] = ptr;

        opd[1] = GetCurrentToc();

        printf("found func %s at 0x%X\n", name, opd[0]);

        *out = (T)opd;
    }

    // @occurancesIndex starts from 0
    template <typename T>
    void SetFnWithOccurances(const char* name, const char* pattern, const char* mask, T* out,
        int occurancesIndex, int32_t relativeOffset = 0x00, bool resolveBranch = false)
    {
        uintptr_t ptr = FindPatternInTextSegment((uint8_t*)pattern, mask, occurancesIndex);
        if (ptr == 0)
        {
            printf("Failed to find %s pattern.\n", name);
            *out = nullptr;
            return;
        }

        uint32_t* opd = (uint32_t*)malloc(2 * sizeof(uint32_t));

        m_OpdTable.push_back(opd);

        ptr = ptr + relativeOffset;

        if (resolveBranch)
            opd[0] = ResolveBranch(ptr);
        else
            opd[0] = ptr;

        opd[1] = GetCurrentToc();

        printf("found func %s at 0x%X\n", name, opd[0]);

        *out = (T)opd;
    }

private:
    std::vector<void*> m_OpdTable;
};

extern GameVariables* g_GameVariables;