#include "windowComponent_64.h"

HWNDContainer **hwnds = NULL;
int lastIndex = 0;

//Returns the current hwnds.
HWNDContainer **getHWNDS()
{
	if (hwnds == NULL) {
		hwnds = (HWNDContainer **) malloc(sizeof(HWNDContainer *) * 2048);
		for (int i=0; i<2048; i++)
			hwnds[i] = NULL;
	}
	return hwnds;
}

//Adds an hwnd and container to the list.
void addHWNDComponent(HWND hwnd, ComponentContainer *container)
{
	hwnds[lastIndex] = new HWNDContainer();
	hwnds[lastIndex]->hwnd = hwnd;
	hwnds[lastIndex]->container = container;
	lastIndex++;
	if (lastIndex >= 2048)
		lastIndex = 0;

}

//Constructor.
ComponentContainer::ComponentContainer(AlCLogger *vLogger, HINSTANCE vInst)
{
	logger = vLogger;

	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);


	inst = vInst;
	hwnd = NULL;

	components = NULL;
	componentCount = 0;

	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	//Default window to 800 by 600.
	ZeroMemory(&position, sizeof(RECT));
	position.left = 100;
	position.top = 100;
	position.right = 800;
	position.bottom = 600;

	//Default window style.
	style = WS_OVERLAPPEDWINDOW;

	fullscreen = false;
}

//Destructor.
ComponentContainer::~ComponentContainer()
{
	logger->debug("Destroying componentContainer.\n");

	if ( fullscreen )
		ChangeDisplaySettings(NULL, 0);

	if (windowClass.lpszClassName && windowClass.hInstance)
	{
		DestroyWindow(hwnd);
		hwnd = NULL;

		//copyCharToWChar((char *)windowClass.lpszClassName, tmp);
		swprintf_s(tbuffer, L"Unregistering class: %s\n", windowClass.lpszClassName);
		logger->debug(tbuffer);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
	}

	if (components) {
		for (int i=0; i<componentCount; i++)
			if (components[i]) {

				swprintf_s(tbuffer, L"Destroying component Name: %s, Id: %i\n", components[i]->className, components[i]->id);
				logger->debug(tbuffer);

				delete components[i];
			}
		
		logger->debug("Freeing components.\n");
		free(components);
	}

	logger->debug("components destroy done...\n");

}

//Creates the window.
bool ComponentContainer::createWindow(LPWSTR windowText)
{
	//copyCharToWChar((char *)windowText, tmp);
	swprintf_s(tbuffer, L"Creating window.. Text: %s\n", windowText);

	logger->debug(tbuffer);


	if ( fullscreen )
	{
		DEVMODE dmScreenSettings;
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		position.top = 0;
		position.left = 0;
		position.right = screenWidth;
		position.bottom = screenHeight;

		style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
	}


	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW, 
		windowClass.lpszClassName, 
		windowText, 
		style, 
		position.top, 
		position.left, 
		position.right, 
		position.bottom, 
		0, 
		0, 
		inst, 
		NULL);

	if (!hwnd)
	{
		logger->debug("Creating window failed.\n");
		return 0;
	}

	addHWNDComponent(hwnd, this);

	return true;
}

//Creates the window class.
bool ComponentContainer::createWindowClass(LPWSTR vClassName)
{
	//copyCharToWChar((char *)vClassName, tmp);
	swprintf_s(tbuffer, L"Creating window class: name: %s\n", vClassName);
	logger->debug(tbuffer);

	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style =  CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = wndProc;
	windowClass.hInstance = inst;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = vClassName;
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hbrBackground  = (HBRUSH)(COLOR_WINDOW);

	if (!RegisterClassEx(&windowClass)) 
	{
		logger->debug("Register window class failed.\n");
		return false;
	}

	return true;
}

//Adds a component to the container.
bool ComponentContainer::addButton(int id, LPWSTR text, int x, int y, int width, int height, DWORD style)
{
	return addComponent(L"button", id, text, x, y, width, height, style);
}

//Is button checked?
bool ComponentContainer::isButtonChecked(int id)
{
	return (Button_GetCheck(getHWND(id)) == BST_CHECKED) ? true : false;
}

//Set button check state.
void ComponentContainer::setButtonCheck(int id, bool checked)
{
	Button_SetCheck(getHWND(id), (checked) ? BST_CHECKED : BST_UNCHECKED);
}

//Adds a component to the container.
bool ComponentContainer::addEdit(int id, LPWSTR text, int x, int y, int width, int height, DWORD style)
{
	if (!style)
		style = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;
	return addComponent(L"edit", id, text, x, y, width, height, style);
}

//Adds a list box.
bool ComponentContainer::addListBox(int id, LPWSTR text, int x, int y, int width, int height, DWORD style)
{
	if (!style)
		style = WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_HASSTRINGS | WS_BORDER | WS_VSCROLL;
	return addComponent(L"listbox", id, text, x, y, width, height, style);
}

//List View
bool ComponentContainer::addListView(int id, int x, int y, int width, int height, DWORD style)
{
	if (!style)
		style = WS_CHILD | LVS_REPORT | WS_VISIBLE | LVS_EDITLABELS | WS_BORDER | LVS_SINGLESEL;
	return addComponent(WC_LISTVIEW, id, NULL, x, y, width, height, style);
}

//Add Column to list view.
bool ComponentContainer::addListViewColumn(int listViewId, int subItemIndex, LPWSTR colText, int colWidth, int lvcfmtFmt, UINT colMask)
{
	//Create list view columns.
	LVCOLUMN lvc;
	ZeroMemory(&lvc, sizeof(LVCOLUMN));
	if (!colMask)
		colMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.mask = colMask;

	lvc.pszText = colText;			//Column text.
	lvc.cx = colWidth;				//Column width

	if (!lvcfmtFmt)
		lvcfmtFmt = LVCFMT_LEFT;
	lvc.fmt = lvcfmtFmt;			//Column alignment

	ListView_InsertColumn(getHWND(listViewId), subItemIndex, &lvc);

	return true;
}

//Add a subitem to an item.
bool ComponentContainer::addListViewItem(int listViewId, int itemIndex, LPWSTR coltext, UINT colMask)
{

	LVITEM lvI;
	ZeroMemory(&lvI, sizeof(LVITEM));

	if (!colMask)
		colMask = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
    lvI.mask = colMask;


	lvI.iItem = listViewId;
	lvI.pszText = coltext;

	ListView_InsertItem(getHWND(listViewId), &lvI);

	return true;
}

//Set a subitem to the listview.
bool ComponentContainer::setListViewSubItem(int listViewId, int itemIndex, int subItemId, LPWSTR coltext, UINT colMask)
{
	ListView_SetItemText(getHWND(listViewId), itemIndex, subItemId, coltext);
	return true;
}

//Adds a label, or static control.
bool ComponentContainer::addStatic(int id, LPWSTR text, int x, int y, int width, int height, DWORD style)
{
	if (!style)
		WS_CHILD | WS_VISIBLE | SS_NOTIFY;
	return addComponent(L"static", id, text, x, y, width, height, style);
}

//Adds a button to the container.
bool ComponentContainer::addComponent(LPWSTR className, int id, LPWSTR text, int x, int y, int width, int height, DWORD style)
{
	Component *component = new Component();
	component->position.left = x;
	component->position.top = y;
	component->position.right = width;
	component->position.bottom = height;
	component->className = className;
	component->text = text;
	if (style)
		component->style = style;
	component->id = id;

	return addComponent(component);
}

//Adds a component to the list.
bool ComponentContainer::addComponent(Component *component)
{
	swprintf_s(tbuffer, L"Adding new component: Name: %s, Id: %i\n", component->className, component->id);
	logger->debug(tbuffer);

	if (!components)
	{
		logger->debug("Initialising component array..\n");
		componentCount = 1;
		components = (Component **) malloc(sizeof(Component *) * componentCount*2048);
		for (int i=0; i<componentCount*2048; i++)
			components[i] = NULL;
	} else 
	{
		componentCount++;
	}

	//Set in last element.
	components[componentCount-1] = component;

	
	//Create component in parent.
	logger->debug("Creating component in parent window.\n");
	components[componentCount-1]->hwnd = CreateWindowExW(
			NULL, 
			components[componentCount-1]->className, 
			components[componentCount-1]->text, 
			components[componentCount-1]->style,
			components[componentCount-1]->position.left, components[componentCount-1]->position.top,
			components[componentCount-1]->position.right, components[componentCount-1]->position.bottom,
			hwnd, 
			(HMENU) components[componentCount-1]->id, 
			inst, 
			NULL);
	if (!components[componentCount-1]->hwnd)
	{
		logger->debug("Could not add component to parent.\n");
		return false;
	}

	swprintf_s(tbuffer, L"Added Component OK: ComponentCount: %i, Name: %s, Id: %i\n", componentCount, component->className, component->id);
	logger->debug(tbuffer);

	return true;
}

//Displays the current list of components.
void ComponentContainer::displayCurrentComponents()
{
	logger->debug("ComponentContainer::displayCurrentComponents -- START ==================\n");

	if (!components)
	{
		logger->debug("No Components in component array.\n");
		return;
	}

	for (int i=0; i<componentCount; i++)
	{
		if (components[i])
		{
			swprintf_s(tbuffer, L"Component: #%i, Name: %s, Index: %i\n", (i+1), components[i]->className, components[i]->id);
			logger->debug(tbuffer);
		}
	}

	logger->debug("ComponentContainer::displayCurrentComponents -- END ====================\n");
}

//Button clicked virtual.
void ComponentContainer::buttonClicked(int id) { }
void ComponentContainer::buttonDblClicked(int id) { }
void ComponentContainer::listboxSelectionChange(int id) { }

void ComponentContainer::setFont(int id, HFONT font)
{
	sendMessage(id, WM_SETFONT, (WPARAM) font,  (LPARAM) TRUE);
}

//Gets component text.
void ComponentContainer::getText(int componentId, LPWSTR out, int max)
{
	//Get HWND
	for (int i=0; i<componentCount; i++)
		if (components[i] && components[i]->id == componentId)
		{
			SendMessageW(components[i]->hwnd, WM_GETTEXT, max, (LPARAM) out);
			break;
		}
}

//Sets the text.
void ComponentContainer::setText(int componentId, LPWSTR in, int max)
{
	//Get HWND
	for (int i=0; i<componentCount; i++)
		if (components[i] && components[i]->id == componentId)
		{
			SendMessageW(components[i]->hwnd, WM_SETTEXT, max, (LPARAM) in);
			break;
		}

}

//Sends a message to a component.
LRESULT ComponentContainer::sendMessage(int componentId, UINT msg, WPARAM w, LPARAM l)
{
	//Get HWND
	for (int i=0; i<componentCount; i++)
		if (components[i] && components[i]->id == componentId)
			return SendMessageW(components[i]->hwnd, msg, w, l);

	return NULL;
}

//Gets the hwnd of a component.
HWND ComponentContainer::getHWND(int componentId)
{
	for (int i=0; i<componentCount; i++)
		if (components[i] && components[i]->id == componentId)
			return components[i]->hwnd;
	return NULL;
}

//Gets the count.
int ComponentContainer::listBoxCount(int id)
{
	return (int) sendMessage(id, LB_GETCOUNT, NULL, NULL);
}

//Removes an item from the list box at an index.
void ComponentContainer::listBoxRemoveItem(int id, int index)
{
	sendMessage(id, LB_DELETESTRING, WPARAM(index), NULL);
}

//Clears the list box.
void ComponentContainer::listBoxClear(int id)
{
	int count = listBoxCount(id);

	for (int i=0; i<count; i++)
		listBoxRemoveItem(id, 0);

}

//Adds a string to the list box.
void ComponentContainer::listBoxAddString(int id, LPWSTR string)
{
	sendMessage(id, LB_ADDSTRING, NULL, (LPARAM) string);
}

//Gets the selected items in a list box.
void ComponentContainer::listBoxGetSelectedItems(int id, int arraySize, int *indexArray)
{
	int count = listBoxCount(id);
	int idx = 0;
	for (int i=0; i<count; i++)
	{
		LRESULT sel = sendMessage(id, LB_GETSEL, (WPARAM) i, NULL);
		if (sel)
		{
			indexArray[idx] = i;
			idx++;
			if (idx >= arraySize)
				return;
		}
	}
}

//Gets the selected index of a single selection listbox.
int ComponentContainer::listBoxGetSelectedIndex(int id)
{
	//Find the selected item index
	int ia[1];
	ia[0] = -1;
	listBoxGetSelectedItems(id, 1, ia);
	if (ia[0] > -1)	
	{
		return ia[0];
	}
	return -1;
}




