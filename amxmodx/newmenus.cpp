/* AMX Mod X 
*
* by the AMX Mod X Development Team
*  originally developed by OLO
*
*  This program is free software; you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by the
*  Free Software Foundation; either version 2 of the License, or (at
*  your option) any later version.
*
*  This program is distributed in the hope that it will be useful, but
*  WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
*  General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software Foundation,
*  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*  In addition, as a special exception, the author gives permission to
*  link the code of this program with the Half-Life Game Engine ("HL
*  Engine") and Modified Game Libraries ("MODs") developed by Valve,
*  L.L.C ("Valve"). You must obey the GNU General Public License in all
*  respects for all of the code used other than the HL Engine and MODs
*  from Valve. If you modify this file, you may extend this exception
*  to your version of the file, but you are not obligated to do so. If
*  you do not wish to do so, delete this exception statement from your
*  version.
*/

#include "amxmodx.h"
#include "newmenus.h"

CVector<Menu *> g_NewMenus;

void ClearMenus()
{
	for (size_t i = 0; i < g_NewMenus.size(); i++)
		delete g_NewMenus[i];
	g_NewMenus.clear();
}

Menu::Menu(const char *title, int mid, int tid)
{
	m_Title.assign(title);
	menuId = mid;
	thisId = tid;
}

Menu::~Menu()
{
	for (size_t i = 0; i < m_Items.size(); i++)
		delete m_Items[i];
	m_Items.clear();
}

menuitem *Menu::AddItem(const char *name, const char *cmd, int access)
{
	menuitem *pItem = new menuitem;

	pItem->name.assign(name);
	pItem->cmd.assign(cmd);
	pItem->access = access;
	pItem->id = m_Items.size();
	pItem->handler = -1;
	pItem->pfn = NULL;

	m_Items.push_back(pItem);

	return pItem;
}

menuitem *Menu::GetMenuItem(item_t item)
{
	if (item >= m_Items.size())
		return NULL;

	return m_Items[item];
}

size_t Menu::GetItemCount()
{
	return m_Items.size();
}

size_t Menu::GetPageCount()
{
	size_t items = GetItemCount();
	page_t numPages = (items / MENUITEMS) + 1;

	if (!items)
		return 0;

	if (numPages % MENUITEMS == 0)
		numPages--;

	return numPages;
}

int Menu::PagekeyToItem(page_t page, item_t key)
{
	page_t pages = GetPageCount();
	item_t numItems = GetItemCount();

	if (page >= pages)
		return MENU_EXIT;

	item_t start = page * 7;

	if (page == 0)
	{
		item_t rem = numItems >= 7 ? 7 : numItems;
		if (key == rem)
		{
			if (pages > 1)
				return MENU_MORE;
			else
				return MENU_EXIT;
		} 
		else if (key == rem + 1)
		{
			return MENU_EXIT;
		}
	}
	else if (page == pages - 1)
	{
		//find number of remaining items
		//for example, 11 items on page 1... means start=7, 11-7=4
		item_t rem = numItems - start;
		//however, the last item is actually this -1, so...
		if (key == rem)
		{
			return MENU_EXIT;
		}
		else if (key == rem + 1)
		{
			return MENU_BACK;
		}
	} else {
		if (key == 7)
		{
			return MENU_MORE;
		} 
		else if (key == 8)
		{
			return MENU_BACK;
		}
	}

	return (start + key);
}

bool Menu::Display(int player, page_t page)
{
	int keys = 0;
	const char *str = GetTextString(player, page, keys);

	if (!str)
		return false;

	static char buffer[2048];
	int len = _snprintf(buffer, sizeof(buffer)-1, "%s", str);

	CPlayer *pPlayer = GET_PLAYER_POINTER_I(player);

	pPlayer->keys = keys;
	pPlayer->menu = menuId;
	pPlayer->newmenu = thisId;
	pPlayer->page = (int)page;

	UTIL_ShowMenu(pPlayer->pEdict, keys, -1, buffer, len);

	return true;
}

const char *Menu::GetTextString(int player, page_t page, int &keys)
{
	page_t pages = GetPageCount();
	item_t numItems = GetItemCount();

	if (page >= pages)
		return NULL;

	m_Text.clear();

	char buffer[255];
	if (g_coloredmenus)
		_snprintf(buffer, sizeof(buffer)-1, "\\y%s %d/%d\n\\w\n", m_Title.c_str(), page + 1, pages);
	else
		_snprintf(buffer, sizeof(buffer)-1, "%s %d/%d\n\n", m_Title.c_str(), page + 1, pages);
	m_Text.append(buffer);

	item_t start = page * 7;
	item_t end = 0;
	if (start + 7 <= numItems)
	{
		end = start + 7;
	} else {
		end = numItems;
	}

	menuitem *pItem = NULL;
	int option = 0;
	keys = 0;
	bool enabled = true;
	int ret = 0;
	for (item_t i = start; i < end; i++)
	{
		pItem = m_Items[i];
		if (pItem->access && !(pItem->access & g_players[player].flags[0]))
			enabled = false;
		if (pItem->handler != -1)
		{
			ret = executeForwards(pItem->handler, player, thisId, i);
			if (ret == ITEM_ENABLED)
				enabled = true;
			else if (ret == ITEM_DISABLED)
				enabled = false;
		}
		if (pItem->pfn)
		{
			ret = (pItem->pfn)(player, thisId, i);
			if (ret == ITEM_ENABLED)
				enabled = true;
			else if (ret == ITEM_DISABLED)
				enabled = false;
		}
		if (enabled)
		{
			keys |= (1<<option);
			_snprintf(buffer, sizeof(buffer)-1, "%d. %s\n", ++option, pItem->name.c_str());
		} else {
			if (g_coloredmenus)
			{
				_snprintf(buffer, sizeof(buffer)-1, "\\d%d. %s\n\\w", ++option, pItem->name.c_str());
			} else {
				_snprintf(buffer, sizeof(buffer)-1, "#. %s\n", pItem->name.c_str());
				option++;
			}
		}
		m_Text.append(buffer);
	}
	//now for a weird part >:o
	//this will either be MORE or BACK..
	keys |= (1<<option++);
	if ((page < pages - 1) && (pages > 1))
	{
		_snprintf(buffer, sizeof(buffer)-1, "%d. %s\n", option, "More");
	} else {
		_snprintf(buffer, sizeof(buffer)-1, "%d. %s\n", option, "Exit");
	}
	m_Text.append(buffer);
	if (pages > 1)
	{
		keys |= (1<<option++);
		if (pages == 0)
		{
			_snprintf(buffer, sizeof(buffer)-1, "%d. %s\n", option, "Exit");
		} else {
			_snprintf(buffer, sizeof(buffer)-1, "%d. %s\n", option, "Back");
		}
		m_Text.append(buffer);
	}

	return m_Text.c_str();
}

#define GETMENU(p) if (p >= (int)g_NewMenus.size() || p < 0) { \
	LogError(amx, AMX_ERR_NATIVE, "Invalid menu id %d", p); \
	return 0; } \
	Menu *pMenu = g_NewMenus[p];

//Makes a new menu handle (-1 for failure)
//native csdm_makemenu(title[]);
static cell AMX_NATIVE_CALL menu_create(AMX *amx, cell *params)
{
	int len;
	char *title = get_amxstring(amx, params[1], 0, len);
	char *handler = get_amxstring(amx, params[2], 1, len);

	int func = registerSPForwardByName(amx, handler, FP_CELL, FP_CELL, FP_CELL, FP_DONE);
	if (func == -1)
	{
		LogError(amx, AMX_ERR_NOTFOUND, "Invalid function \"%s\"", handler);
		return 0;
	}

	int id = g_menucmds.registerMenuId(title, amx);
	g_menucmds.registerMenuCmd( g_plugins.findPluginFast(amx), id, 1023, func );

	Menu *pMenu = new Menu(title, id, (int)g_NewMenus.size());
	g_NewMenus.push_back(pMenu);

	return (int)g_NewMenus.size() - 1;
}

//Adds an item to the menu (returns current item count - 1)
//native menu_additem(menu, const name[], const command[]="", access=0);
static cell AMX_NATIVE_CALL menu_additem(AMX *amx, cell *params)
{
	int len;
	char *name, *cmd;
	int access;

	GETMENU(params[1]);

	name = get_amxstring(amx, params[2], 0, len);
	cmd = get_amxstring(amx, params[3], 1, len);
	access = params[4];
    
	menuitem *pItem = pMenu->AddItem(name, cmd, access);

	pItem->handler = params[5];

	return 1;
}

//Returns the number of pages in a menu
//native csdm_menu_pages(menu);
static cell AMX_NATIVE_CALL menu_pages(AMX *amx, cell *params)
{
	GETMENU(params[1]);
	return pMenu->GetPageCount();
}

//Returns the number of items in a menu
//native csdm_menu_items(menu);
static cell AMX_NATIVE_CALL menu_items(AMX *amx, cell *params)
{
	GETMENU(params[1]);

	return pMenu->GetItemCount();
}

//Builds the menu string for a specific page (set title to 0 to not include title)
//page indices start at 0!
static cell AMX_NATIVE_CALL menu_display(AMX *amx, cell *params)
{
	GETMENU(params[2]);

	int player = params[1];
	int page = params[3];

	return pMenu->Display(player, page);
}

//Finds the id of a menu item for a specific page and key value.
//Note that key should be from 0-6, as it only displays 7 per page.
//page indices start at 0
//native menu_keyid(menu, page, key);
static cell AMX_NATIVE_CALL menu_find_id(AMX *amx, cell *params)
{
	GETMENU(params[1]);

	page_t page = static_cast<page_t>(params[2]);
	item_t key = static_cast<item_t>(params[3]);

	return pMenu->PagekeyToItem(page, key);
}

//Gets info about a menu option
//native menu_item_getinfo(menu, item, &access, command[], cmdlen, name[]="", namelen=0, &callback);
static cell AMX_NATIVE_CALL menu_item_getinfo(AMX *amx, cell *params)
{
	GETMENU(params[1]);

	menuitem *pItem = pMenu->GetMenuItem(static_cast<item_t>(params[2]));

	if (!pItem)
		return 0;

	cell *addr = get_amxaddr(amx, params[3]);
	addr[0] = pItem->access;

	set_amxstring(amx, params[4], pItem->cmd.c_str(), params[5]);
	set_amxstring(amx, params[6], pItem->name.c_str(), params[7]);

	if (params[8])
	{
		addr = get_amxaddr(amx, params[8]);
		if (addr)
			addr[0] = pItem->handler;
	}

	return 1;
}

static cell AMX_NATIVE_CALL menu_makecallback(AMX *amx, cell *params)
{
	int len;
	char *name = get_amxstring(amx, params[1], 0, len);

	int id = registerSPForwardByName(amx, name, FP_CELL, FP_CELL, FP_CELL, FP_DONE);

	if (id == -1)
	{
		LogError(amx, AMX_ERR_NOTFOUND, "Invalid function %s", name);
		return -1;
	}

	return id;
}

static cell AMX_NATIVE_CALL menu_item_setname(AMX *amx, cell *params)
{
    GETMENU(params[1]);

    menuitem *pItem = pMenu->GetMenuItem(static_cast<item_t>(params[2]));

    if (!pItem)
        return 0;

    int len;
    char *name;

    name = get_amxstring(amx, params[3], 0, len);

    pItem->name.assign(name);

    return 1;
}

static cell AMX_NATIVE_CALL menu_item_setcmd(AMX *amx, cell *params)
{
    GETMENU(params[1]);

    menuitem *pItem = pMenu->GetMenuItem(static_cast<item_t>(params[2]));

    if (!pItem)
        return 0;

    int len;
    char *cmd;

    cmd = get_amxstring(amx, params[3], 0, len);

    pItem->cmd.assign(cmd);

    return 1;
}

static cell AMX_NATIVE_CALL menu_item_setcall(AMX *amx, cell *params)
{
    GETMENU(params[1]);

    menuitem *pItem = pMenu->GetMenuItem(static_cast<item_t>(params[2]));

    if (!pItem)
        return 0;

    pItem->handler = params[3];

    return 1;
} 

AMX_NATIVE_INFO g_NewMenuNatives[] = 
{
	{"menu_create",				menu_create},
	{"menu_additem",			menu_additem},
	{"menu_pages",				menu_pages},
	{"menu_items",				menu_items},
	{"menu_display",			menu_display},
	{"menu_find_id",			menu_find_id},
	{"menu_item_getinfo",		menu_item_getinfo},
	{"menu_makecallback",		menu_makecallback},
	{"menu_item_setcall",		menu_item_setcall},
	{"menu_item_setcmd",		menu_item_setcmd},
	{"menu_item_setname",		menu_item_setname},
	{NULL,						NULL},
};
