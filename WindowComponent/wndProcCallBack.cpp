#include "windowComponent_64.h"

//The default overridable Windows Message Proc.
LRESULT ComponentContainer::wndMsgProc(HWND hwnd, UINT m, WPARAM w, LPARAM l, bool defaultX)
{
	int width, height;

	switch (m)
	{
	case WM_COMMAND:
		if (HIWORD(w) == BN_CLICKED || HIWORD(w) == STN_CLICKED)
		{
			int id = (int) LOWORD(w);
			//Indicate a button was clicked.
			buttonClicked(id);
		}
		if (HIWORD(w) == LBN_DBLCLK)
		{
			int id = (int) LOWORD(w);
			//Indicate a button was double clicked.
			buttonDblClicked(id);
		}
		if (HIWORD(w) == LBN_SELCHANGE)
		{
			int id = (int) LOWORD(w);
			//Indicate a listbox selection changed.
			listboxSelectionChange(id);
		}

		break;
	case WM_CLOSE:
	case WM_DESTROY:
		destroyStatic(hwnd);
		PostQuitMessage(0);
		return 0;
		break;
	case WM_SIZE:
		height = HIWORD(l);
		width = LOWORD(l);
		if (height == 0)
			height = 1;
		return 0;
		break;
	default:
		break;
	};

	if (defaultX)
		return DefWindowProc(hwnd, m, w, l);
	return 0;
}

//Window call back.
LRESULT CALLBACK wndProc(HWND hwnd, UINT m, WPARAM w, LPARAM l)
{
	//if (destroyed())
	//	return DefWindowProc(hwnd, m, w, l);

	//Find the window, and call...
	HWNDContainer **g = getHWNDS();
	if (!g)
		return DefWindowProc(hwnd, m, w, l);

	for (int i=0; i<2048; i++)	
		if (g[i] && g[i]->container)
			if (g[i]->hwnd == hwnd)
				return g[i]->container->wndMsgProc(hwnd, m, w, l, true);

	return DefWindowProc(hwnd, m, w, l);
}
