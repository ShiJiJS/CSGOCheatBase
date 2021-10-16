#pragma once

#include <string>
#include<dwmapi.h>
#include<iostream>
#include<Windows.h>
#include<d3d9.h>
#include<d3dx9.h>
#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

using namespace std;

static HWND target_hwnd;
static HWND overlay_hwnd;




class Overlay
{
	struct wnd_rec :public RECT {
		int width()
		{
			return right - left;
		}
		int height()
		{
			return bottom - top;
		}
	};
public:
	Overlay(string window_name, string class_name);
	~Overlay() {}
	void create_overlay(string window_name, string class_name);
	void init_dx9();
	IDirect3D9* m_d3d = nullptr;
	IDirect3DDevice9* m_device = nullptr;
private:
	static LRESULT CALLBACK m_win_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	wnd_rec target_rect;
	wnd_rec overlay_rect;
	bool m_initialized = false;
};

class Dx_renderer
{
public:
	Dx_renderer(IDirect3DDevice9* mdevice);
	~Dx_renderer();
	void begin_renderer();
	void end_renderer();
	void draw_line(int x0, int y0, int x1, int y1, unsigned long color);
	void draw_rect(int x0, int y0, int w, int h, unsigned long color);
	void draw_text(string text, int x, int y, unsigned long color);

private:
	IDirect3DDevice9* d3d_device;
	ID3DXFont* m_font = nullptr;
	ID3DXLine* m_line = nullptr;
};