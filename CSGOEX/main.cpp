#include<Windows.h>
#include"memory.h"
#include"renderer.h"

using namespace std;

int main()
{
    Overlay over("Counter-Strike: Global Offensive", "Valve001");
    Dx_renderer dx(over.m_device);

    MemoryManager mem("csgo.exe", "client.dll");
    mem.init();



    while (true)
    {


        mem.getViewMatrix();

        mem.getListStart();


        mem.getObjInfo(mem.myItem);

        dx.begin_renderer();

        EntityItem tmp = mem.startItem;


        while (tmp.suf != NULL)
        {

            mem.getObjInfo(tmp);

            if ((mem.otherInfo.X != 0 || mem.otherInfo.Y != 0 || mem.otherInfo.Z != 0) && (mem.otherInfo.health > 0))
            {
                mem.WorldToScreen();
                //cout << "人物id:" << tmp.id << endl;
                //cout << "坐标:(" << mem.otherInfo.X << "," << mem.otherInfo.Y << "," << mem.otherInfo.Z << ")" << endl;
                //cout << "生命值：" << mem.otherInfo.health << endl;
                //cout << "VS:" << mem.vs.X << "," << mem.vs.Y << endl;
                if ((mem.vs.X > 0) && (mem.vs.X < 1920) && (mem.vs.Y > 0) && (mem.vs.Y < 1080))
                {
                    Rect rect;
                    rect = mem.getRect();
                    dx.draw_rect(rect.x, rect.y, rect.width, rect.height, D3DCOLOR_RGBA(0, 255, 0, 255));
                }

            }

            tmp = mem.Readmemory<EntityItem>((DWORD)tmp.suf);
        }
        dx.end_renderer();
        //system("cls");
    }
}
