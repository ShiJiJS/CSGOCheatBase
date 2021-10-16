#pragma once

#include <string>
#include<Windows.h>
#include<iostream>

using namespace std;

const DWORD entityListOffset = 0x4DBD5CC; 
const DWORD healthOffset = 0x100;
const DWORD locationOffset = 0x138;
const DWORD viewMartixOffset = 0x4DAEFF4;

struct EntityItem {
	DWORD entityObj;
	DWORD id;
	EntityItem* pre, * suf;
};

struct ObjInfo
{
	float X, Y, Z;
	int health;
};

struct ViewScreen {
	int X;
	int Y;
};

struct Rect {
	int x;
	int y;
	int width;
	int height;
};

class MemoryManager
{
public:
	MemoryManager(string processName,string moduleName);
	~MemoryManager();

	BOOL init();//初始化
	DWORD getPIDByName(string processName);//通过进程名获取pid
	DWORD getProcessModuleBase(string moduleName);//获取模块基址

	template<typename ReadType>
	ReadType Readmemory(DWORD addr);//用于读取其他进程的内存

	void getObjInfo(EntityItem &obj);
	void getListStart();
	void getViewMatrix();
	void WorldToScreen();
	Rect getRect();
	EntityItem myItem, startItem;
	ObjInfo myInfo, otherInfo;
	ViewScreen vs;
private:
	string processName,moduleName;
	DWORD processPID ,processModuleBase;
	HANDLE hProcess;

	
	
	float viewMatrix[4][4];
};

