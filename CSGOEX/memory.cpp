#include "memory.h"
#include<TlHelp32.h>


MemoryManager::MemoryManager(string processName,string moduleName)
{
	this->processName = processName;
    this->moduleName = moduleName;
	this->processPID = 0;
    
}

BOOL MemoryManager::init()
{
    this->getPIDByName(this->processName);
    //打开进程，获取模块基址
    cout << "Pid获取成功" << endl;
    this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, this->processPID);
    cout << "hProcess获取成功" << endl;
    this->getProcessModuleBase(this->moduleName);
    cout << "ProcessModuleBase获取成功" << endl;
    this->myItem = Readmemory<EntityItem>(this->processModuleBase + entityListOffset);
    return true;
}

DWORD MemoryManager::getPIDByName(string processName)
{
    //通过进程名字获取进程pid

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return(FALSE);
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return(FALSE);
    }
    BOOL getPID = FALSE;//记录getpid操作是否成功
    do
    {
        if (!strcmp(this->processName.c_str(), pe32.szExeFile))
        {
            this->processPID = pe32.th32ProcessID;
            getPID = TRUE;
            break;
        }

    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
    if (!getPID)
    {
        throw exception("PID获取失败");
    }
    return this->processPID;
}

DWORD MemoryManager::getProcessModuleBase(string moduleName)
{

    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,this->processPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        throw exception("[ERROR] Failed to CreateToolhelp32Snapshot");
    }
    me32.dwSize = sizeof(MODULEENTRY32);
    if (!Module32First(hModuleSnap, &me32))
    {
        throw exception("[ERROR] Failed to Module32First");
    }
    do {
        if (!strcmp(me32.szModule, this->moduleName.c_str()))
        {
            CloseHandle(hModuleSnap);
            this->processModuleBase = (DWORD)me32.modBaseAddr;
            return this->processModuleBase;
        }
    } while (Module32Next(hModuleSnap, &me32));
    CloseHandle(hModuleSnap);
    return 0;
}

void MemoryManager::getObjInfo(EntityItem &obj)
{
    if (obj.id == myItem.id)
    {
        this->myInfo.X = Readmemory<float>(obj.entityObj + locationOffset);
        this->myInfo.Y = Readmemory<float>(obj.entityObj + locationOffset + sizeof(float));
        this->myInfo.Z = Readmemory<float>(obj.entityObj + locationOffset + 2 * sizeof(float));
        this->myInfo.health = Readmemory<int>(obj.entityObj + healthOffset);
    }
    else
    {
        this->otherInfo.X = Readmemory<float>(obj.entityObj + locationOffset);
        this->otherInfo.Y = Readmemory<float>(obj.entityObj + locationOffset + sizeof(float));
        this->otherInfo.Z = Readmemory<float>(obj.entityObj + locationOffset + 2 * sizeof(float));
        this->otherInfo.health = Readmemory<int>(obj.entityObj + healthOffset);
    }

    
    
}

void MemoryManager::getListStart()
{
    EntityItem tmp = this->myItem;
    while (tmp.pre != 0)
    {
        tmp = Readmemory<EntityItem>((DWORD)tmp.pre);
    }
    this->startItem = tmp;
}

void MemoryManager::getViewMatrix()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            this->viewMatrix[i][j] = Readmemory<float>(this->processModuleBase + viewMartixOffset + (4 * i + j) * sizeof(float));
        }
    }
}

void MemoryManager::WorldToScreen()
{
    float w = this->viewMatrix[3][0] * this->otherInfo.X + this->viewMatrix[3][1] * this->otherInfo.Y + this->viewMatrix[3][2] * this->otherInfo.Z + this->viewMatrix[3][3];
    if (w > 0.001) //如果对象在视图中.
    {
        float fl1DBw = 1 / w;

        this->vs.X = (1920 / 2) + (0.5f * ((this->viewMatrix[0][0] * this->otherInfo.X + this->viewMatrix[0][1] * 
            this->otherInfo.Y + this->viewMatrix[0][2] * this->otherInfo.Z
            + this->viewMatrix[0][3]) * fl1DBw) * (1920) + 0.5f);

        this->vs.Y = (1080 / 2) - (0.5f * ((this->viewMatrix[1][0] * this->otherInfo.X + this->viewMatrix[1][1] * 
            this->otherInfo.Y + this->viewMatrix[1][2] * this->otherInfo.Z+ this->viewMatrix[1][3]) * fl1DBw) * (1080) + 0.5f);
    }
    else
    {
        this->vs.X = 0;
        this->vs.Y = 0;
    }
}

Rect MemoryManager::getRect()
{
    double M = sqrt(pow((this->myInfo.X - this->otherInfo.X), 2) + pow((this->myInfo.Y - this->otherInfo.Y), 2) + pow((this->myInfo.Z - this->otherInfo.Z), 2)) / 30;
    int H = 950 / M * 2;
    int W = 400 / M * 2;
    Rect rect;
    rect.x = this->vs.X - W / 2;
    rect.y = this->vs.Y - H;
    rect.height = H;
    rect.width = W;
    return rect;
}

MemoryManager::~MemoryManager()
{
    CloseHandle(this->hProcess);
}


template<typename ReadType>
ReadType MemoryManager::Readmemory(DWORD addr)
{
    ReadType buff;
    SIZE_T readSz;
    ReadProcessMemory(hProcess, (LPVOID)addr, &buff, sizeof(ReadType), &readSz);
    return buff;
}
