#include "renderer.h"

using namespace std;

Overlay::Overlay(string window_name, string class_name)
{
	if (window_name.empty() || class_name.empty())
	{
		throw exception("窗口名称为空");
	}
	create_overlay(window_name, class_name);
}

void Overlay::create_overlay(string window_name, string class_name)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof WNDCLASSEX;
	wc.lpfnWndProc = m_win_proc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = HBRUSH(RGB(0, 0, 0));
	wc.lpszMenuName = "";
	wc.lpszClassName = "overlay";
	wc.hIconSm = NULL;
	if (!RegisterClassExA(&wc))
	{
		throw exception("窗口注册失败");
	}
	target_hwnd = FindWindow(class_name.data(), window_name.data());
	GetWindowRect(target_hwnd, &target_rect);
	overlay_hwnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, "overlay", "", WS_VISIBLE | WS_POPUP, target_rect.left, target_rect.top,
		target_rect.width(), target_rect.height(), NULL, NULL, NULL, NULL);

	cout << target_rect.width() << endl;
	cout << target_rect.height() << endl;

	MARGINS m = { target_rect.left, target_rect.top,target_rect.width(), target_rect.height() };
	DwmExtendFrameIntoClientArea(overlay_hwnd, &m);
	SetLayeredWindowAttributes(overlay_hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	ShowWindow(overlay_hwnd, SW_SHOW);

	init_dx9();

}


void Overlay::init_dx9()
{
	IDirect3D9* m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof D3DPRESENT_PARAMETERS);
	pp.Windowed = true;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.BackBufferFormat = D3DFMT_A8R8G8B8;
	pp.BackBufferWidth = target_rect.width();
	pp.BackBufferHeight = target_rect.height();
	pp.hDeviceWindow = overlay_hwnd;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, overlay_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &m_device)))
	{
		throw exception("m_device设备初始化失败");
	}

	m_initialized = true;
}

LRESULT Overlay::m_win_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		exit(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}


Dx_renderer::Dx_renderer(IDirect3DDevice9* m_device)
{
	if (NULL == m_device)
	{
		throw exception("m_device为空");
	}
	d3d_device = m_device;
	if (FAILED(D3DXCreateLine(d3d_device, &m_line)))
	{
		throw exception("线条初始化失败");
	}
	if (FAILED(D3DXCreateFont(d3d_device, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Tahoma", &m_font)))
	{
		throw exception("字体初始化失败");
	}
}

Dx_renderer::~Dx_renderer()
{
	if (m_line)
	{
		m_line->Release();
	}
	if (m_font)
	{
		m_font->Release();
	}
}

void Dx_renderer::begin_renderer()
{
	d3d_device->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.f, 0);
	d3d_device->BeginScene();
}

void Dx_renderer::end_renderer()
{
	d3d_device->EndScene();
	d3d_device->Present(NULL, NULL, NULL, NULL);
}

void Dx_renderer::draw_line(int x0, int y0, int x1, int y1, unsigned long color)
{
	D3DXVECTOR2 line[2] = { D3DXVECTOR2(x0,y0),D3DXVECTOR2(x1,y1) };
	m_line->Begin();
	m_line->Draw(line, 2, color);
	m_line->End();

}

void Dx_renderer::draw_rect(int x0, int y0, int w, int h, unsigned long color)
{
	draw_line(x0, y0, x0 + w, y0, color);
	draw_line(x0, y0, x0, y0 + h, color);
	draw_line(x0 + w, y0, x0 + w, y0 + h, color);
	draw_line(x0, y0 + h, x0 + w, y0 + h, color);

}

void Dx_renderer::draw_text(string text, int x, int y, unsigned long color)
{
	RECT r = { x,y,x,y };
	m_font->DrawTextA(NULL, text.data(), -1, &r, DT_NOCLIP, color);
}