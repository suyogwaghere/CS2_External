#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string_view>
#include <string>
namespace Memory
{
    class Memory
    {
    private:
        DWORD64 ProcessID = 0;
        HANDLE HProcessID = 0;
    public:
        Memory(std::string ProcessName) noexcept
        {
            PROCESSENTRY32 entry = { };
            entry.dwSize = sizeof(PROCESSENTRY32);
            const auto Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            while (Process32Next(Snapshot, &entry))
            {
                if (wcscmp(entry.szExeFile, std::wstring(ProcessName.begin(), ProcessName.end()).c_str()) == 0)
                {
                    ProcessID = entry.th32ProcessID;
                    HProcessID = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
                    break;
                }
            }
            CloseHandle(Snapshot);
        }
        DWORD64 Get_Module(std::string ModuleName) noexcept
        {
            MODULEENTRY32 entry = { };
            entry.dwSize = sizeof(MODULEENTRY32);
            const auto Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID);
            DWORD64 dwModuleBaseAddress = 0;
            while (Module32Next(Snapshot, &entry))
            {
                if (wcscmp(entry.szModule, std::wstring(ModuleName.begin(), ModuleName.end()).c_str()) == 0)
                {
                    dwModuleBaseAddress = reinterpret_cast<DWORD64>(entry.modBaseAddr);
                    break;
                }
            }
            CloseHandle(Snapshot);
            return dwModuleBaseAddress;
        }
        template<class T>
        T Write(DWORD64 Address, T value) noexcept
        {
            WriteProcessMemory(HProcessID, (LPVOID)(Address), &value, sizeof(T), NULL);
            return value;
        }
        template<class T>
        T Read(DWORD64 Address) noexcept
        {
            T Value = { };
            ReadProcessMemory(HProcessID, (LPVOID)(Address), &Value, sizeof(T), NULL);
            return Value;
        }
    };
}