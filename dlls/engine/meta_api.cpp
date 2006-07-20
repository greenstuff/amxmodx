/* AMX Mod X
*   Engine Module
*
* by the AMX Mod X Development Team
*  thanks to Vexd
*
* This file is part of AMX Mod X.
*
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

#include <extdll.h>
#include <meta_api.h>
#include <dllapi.h>
#include "sdk_util.h"
#include <h_export.h>
#include <modules.h>
#include <vector>
#include <limits.h>
#include "engine.h"

GlobalInfo GlInfo;
MessageInfo *msgd = NULL;
bool isMsgHooked[MAX_MESSAGES];
bool is_PlayerOn[33];
int inHookProcess;
edict_t *valid_ent(int ent);
edict_t *valid_player(int ent);

void UTIL_SetSize(edict_t *pev, const Vector &vecMin, const Vector &vecMax)
{
	SET_SIZE(ENT(pev), vecMin, vecMax);
}

edict_t *valid_player(int ent)
{
	if (ent < 1 || ent > gpGlobals->maxClients) {
		return NULL;
	}

	edict_t *e = INDEXENT(ent);

	if (FNullEnt(e)) {
		return NULL;
	}

	return e;
}

edict_t *valid_ent(int ent)
{
	if (ent < 1 || ent > gpGlobals->maxEntities) {
		return NULL;
	}

	edict_t *e = INDEXENT(ent);

	if (FNullEnt(e)) {
		return NULL;
	}

	return e;
}

//from OLO
char* get_amxstring(AMX *amx,cell amx_addr,int id, int& len)
{
  static char buffor[4][3072];
  register cell* source = (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
  register char* dest = buffor[id];
  char* start = dest;
  while (*dest++=(char)*source++)
    ;
  len = --dest - start;
  return start;
}

cell* get_amxaddr(AMX *amx,cell amx_addr)
{
  return (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
}
 
/********************************************************
  vexd's utility funcs
  ******************************************************/
// Finds edict points that are in a sphere, used in RadiusDamage.
edict_t *UTIL_FindEntityInSphere(edict_t *pStart, const Vector &vecCenter, float flRadius) {
	if (!pStart) pStart = NULL;

	pStart = FIND_ENTITY_IN_SPHERE(pStart, vecCenter, flRadius);

	if (!FNullEnt(pStart)) return pStart;
	return NULL;
}

// Makes A Half-Life string (which is just an integer, an index of the string
// half-life uses to refer to the string) out of an AMX cell.
int AMX_MAKE_STRING(AMX *oPlugin, cell tParam, int &iLength) {
	char *szNewValue = GET_AMXSTRING(oPlugin, tParam, 0, iLength);

	char* szCopyValue = new char[iLength + 1]; 
	strncpy(szCopyValue , szNewValue, iLength); 
	*(szCopyValue + iLength) = '\0'; 

	return MAKE_STRING(szCopyValue);
}

// Makes a char pointer out of an AMX cell.
char *AMX_GET_STRING(AMX *oPlugin, cell tParam, int &iLength) {
	char *szNewValue = GET_AMXSTRING(oPlugin, tParam, 0, iLength);

	char* szCopyValue = new char[iLength + 1]; 
	strncpy(szCopyValue , szNewValue, iLength); 
	*(szCopyValue + iLength) = '\0';

	return szCopyValue;
}

/********************************************************
  exported functions
  ******************************************************/

//(BAILOPAN)
//Calls think
static cell AMX_NATIVE_CALL call_think(AMX *amx, cell *params)
{
	int iEnt = params[1];

	edict_t *pEnt = INDEXENT(iEnt);

	if (FNullEnt(pEnt)) {
		return 0;
	}

	MDLL_Think(pEnt);

	return 1;
}

//(BAILOPAN)
//Hooks a register_message()
static cell AMX_NATIVE_CALL register_message(AMX *amx, cell *params)
{
	int iLen;
	int iFunctionIndex;
	int iMessage = params[1];
	char *szFunction = AMX_GET_STRING(amx, params[2], iLen);

	if (iMessage > 0 && iMessage < MAX_MESSAGES) {
		if (AMX_FINDPUBLIC(amx, szFunction, &iFunctionIndex) == AMX_ERR_NONE) {
			isMsgHooked[iMessage] = true;
			Msgs.put(amx, iFunctionIndex);
		} else {
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return 1;
}

//(BAILOPAN)
//Gets the argument type of a message argument
static cell AMX_NATIVE_CALL get_msg_argtype(AMX *amx, cell *params)
{
	int argn = params[1];

	if ((inHookProcess) && (msgd != NULL)) {
		return msgd->ArgType(argn);
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return 1;
}

//(BAILOPAN)
//Gets the argument count for a message.
static cell AMX_NATIVE_CALL get_msg_args(AMX *amx, cell *params)
{
	if ((inHookProcess) && (msgd != NULL)) {
		return msgd->args();
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
}

//(BAILOPAN)
//gets a message argument as an integer
static cell AMX_NATIVE_CALL get_msg_arg_int(AMX *amx, cell *params)
{
	int argn = params[1];
	
	if (inHookProcess && msgd!=NULL) {
		if (argn <= msgd->args() && argn > 0) {
			return msgd->RetArg_Int(argn);
		} else {
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return 1;
}

//(BAILOPAN)
//gets a message argument as a float
static cell AMX_NATIVE_CALL get_msg_arg_float(AMX *amx, cell *params)
{
	int argn = params[1];

	float retVal;
	
	if (inHookProcess && msgd!=NULL) {
		if (argn <= msgd->args() && argn > 0) {
			retVal = msgd->RetArg_Float(argn);
			return *(cell*)((void *)&retVal);
		} else {
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return 1;
}

//(BAILOPAN)
//gets a message argument as a string
static cell AMX_NATIVE_CALL get_msg_arg_string(AMX *amx, cell *params)
{
	int argn = params[1];
	int iLen = 0;
	char *szValue = '\0';
	char *szRetr;
	
	if (inHookProcess && msgd!=NULL) {
		if (argn <= msgd->args() && argn > 0) {
			iLen = msgd->RetArg_Strlen(argn);
			szValue = new char[iLen+1];
			szRetr = (char *)msgd->RetArg_String(argn);
			if (szRetr != NULL) {
				strcpy(szValue, szRetr);
				if (strlen(szValue)) {
					return SET_AMXSTRING(amx, params[2], szValue, params[3]);
				} else {
					return SET_AMXSTRING(amx, params[2], "", params[3]);
				}
			}
		} else {
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return 1;
}

//(BAILOPAN)
static cell AMX_NATIVE_CALL set_msg_arg_string(AMX *amx, cell *params)
{
	int argn = params[1];
	int iLen;
	char *szData = AMX_GET_STRING(amx, params[2], iLen);
	
	if (inHookProcess && msgd!=NULL) {
		if (argn <= msgd->args() && argn > 0) {
			if (msgd->Set(argn, arg_string, szData)) {
				return 1;
			} else {
				return 0;
			}
		} else {
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return 1;
}

//(BAILOPAN)
static cell AMX_NATIVE_CALL set_msg_arg_float(AMX *amx, cell *params)
{
	int argn = params[1];
	int argtype = params[2];
	
	if (inHookProcess && msgd!=NULL) {
		if (argn <= msgd->args() && argn > 0) {
			if (msgd->Set(argn, argtype, params[3])) {
				return 1;
			} else {
				return 0;
			}
		} else {
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return 1;
}

//(BAILOPAN)
static cell AMX_NATIVE_CALL set_msg_arg_int(AMX *amx, cell *params)
{
	int argn = params[1];
	int argtype = params[2];
	int iData = params[3];
	
	if (inHookProcess && msgd!=NULL) {
		if (argn <= msgd->args() && argn > 0) {
			if (msgd->Set(argn, argtype, iData)) {
				return 1;
			} else {
				return 0;
			}
		} else {
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}
	} else {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return 1;
}

//(BAILOPAN)
//Sets a pvPrivateData offset for a player (player, offset, val, float=0)
static cell AMX_NATIVE_CALL set_offset_short(AMX *amx, cell *params)
{
	int index = params[1];
	int off = params[2];
	
	if (index < 1 || index > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	if ((index <= gpGlobals->maxClients) && !is_PlayerOn[index]) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
	
	edict_t *pEnt = INDEXENT(index);
	
#ifndef __linux__
       off -= 5;
#endif
	
	*((short *)pEnt->pvPrivateData + off) = (short)params[3];
	
	return 1;
}

//(BAILOPAN)
//Sets a pvPrivateData offset for a player (player, offset, val)
static cell AMX_NATIVE_CALL set_offset(AMX *amx, cell *params)
{
	int index = params[1];
	int off = params[2];
	
	if (index < 1 || index > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	if ((index <= gpGlobals->maxClients) && !is_PlayerOn[index]) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
	
	edict_t *pEnt = INDEXENT(index);
	
#ifndef __linux__
       off -= 5;
#endif
	
	*((int *)pEnt->pvPrivateData + off) = params[3];
	
	return 1;
}

//(BAILOPAN)
//Sets a pvPrivateData offset for a player (player, offset, val)
static cell AMX_NATIVE_CALL set_offset_char(AMX *amx, cell *params)
{
	int index = params[1];
	int off = params[2];
	
	if (index < 1 || index > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	if ((index <= gpGlobals->maxClients) && !is_PlayerOn[index]) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
	
	edict_t *pEnt = INDEXENT(index);
	
#ifndef __linux__
       off -= 5;
#endif
	

	char data = params[3];
	*((char *)pEnt->pvPrivateData + off) = data;
	
	return 1;
}

//(BAILOPAN)
//Sets a pvPrivateData offset for a player (player, offset, Float:val)
static cell AMX_NATIVE_CALL set_offset_float(AMX *amx, cell *params)
{
	int index = params[1];
	int off = params[2];

	float fNewValue = *(float *)((void *)&params[3]);
	
	if (index < 1 || index > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	if ((index <= gpGlobals->maxClients) && !is_PlayerOn[index]) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
	
	edict_t *pEnt = INDEXENT(index);
	
#ifndef __linux__
       off -= 5;
#endif

	*((float *)pEnt->pvPrivateData + off) = fNewValue;
	
	return 1;
}

//(BAILOPAN)
//Gets a pvPrivateData offset for a player (player, offset)
static cell AMX_NATIVE_CALL get_offset_short(AMX *amx, cell *params)
{
	int index = params[1];
	int off = params[2];
	
	if (index < 1 || index > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	if ((index <= gpGlobals->maxClients) && !is_PlayerOn[index]) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
	
	edict_t *pEnt = INDEXENT(index);
	
#ifndef __linux__
	off -= 5;
#endif
	
	return *((short *)pEnt->pvPrivateData + off);
	
}

//(BAILOPAN)
//Gets a pvPrivateData offset for a player (player, offset)
static cell AMX_NATIVE_CALL get_offset_char(AMX *amx, cell *params)
{
	int index = params[1];
	int off = params[2];
	
	if (index < 1 || index > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	//!!!1111 don't uncomment jghg or I will pull my hair out
	if ((index <= gpGlobals->maxClients) && !is_PlayerOn[index]) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
	
	edict_t *pEnt = INDEXENT(index);
	
#ifndef __linux__
	off -= 5;
#endif
	
	return *((char *)pEnt->pvPrivateData + off);
	
}

//(BAILOPAN)
//Gets a pvPrivateData offset for a player (player, offset, float=0)
static cell AMX_NATIVE_CALL get_offset(AMX *amx, cell *params)
{
	int index = params[1];
	int off = params[2];
	
	if (index < 1 || index > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	//jghg comment this out again and I bite you
	if ((index <= gpGlobals->maxClients) && !is_PlayerOn[index]) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
	
	edict_t *pEnt = INDEXENT(index);
	
#ifndef __linux__
	off -= 5;
#endif
	
	return *((int *)pEnt->pvPrivateData + off);
}

//(BAILOPAN)
//Gets a pvPrivateData offset for a player (player, offset)
static cell AMX_NATIVE_CALL get_offset_float(AMX *amx, cell *params)
{
	int index = params[1];
	int off = params[2];
	float retVal;
	
	if (index < 1 || index > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	//jghg comment this out again and I bite you
	if ((index <= gpGlobals->maxClients) && !is_PlayerOn[index]) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}
	
	edict_t *pEnt = INDEXENT(index);
	
#ifndef __linux__
   off -= 5;
#endif
	
	//no jghg this actually works!
	retVal = ((float)*((float *)pEnt->pvPrivateData + off));

	return *(cell*)((void *)&retVal);
}

//jghg2
static cell AMX_NATIVE_CALL get_entity_pointer(AMX *amx, cell *params) // get_entity_pointer(index, pointer[], len); = 3 params
{
	// Valid entity should be within range
	if (params[1] < 1 || params[1] > gpGlobals->maxEntities)
	{
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	// Make into class pointer
	edict_t *pEdict = INDEXENT(params[1]);

	if (FNullEnt(pEdict)) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	char buffer[100];
	sprintf(buffer, "%d", pEdict);

	if (params[3] == -1)
		return (cell)pEdict;

	return SET_AMXSTRING(amx, params[2], buffer, params[3]);
}

//is an entity valid?
//(BAILOPAN)
static cell AMX_NATIVE_CALL is_valid_ent(AMX *amx, cell *params) {
	int iEnt = params[1];

	if (iEnt < 1 || iEnt > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t *pEntity = INDEXENT(iEnt);

	if (FNullEnt(pEntity)) {
		return 0;
	}

	return 1;
}

// Get an integer from an entities entvars.
// (vexd)
static cell AMX_NATIVE_CALL entity_get_int(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	int iRetValue = 0;

	// Is it a valid entity?
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return -1;
	}

	// get its edict pointer.
	edict_t* pEntity = INDEXENT(iTargetEntity);

	// Is it a real entity?
	if(FNullEnt(pEntity)) {
		return -1;
	}

	switch(iValueSet) {
		case gamestate:
			iRetValue = pEntity->v.gamestate;
			break;
		case oldbuttons:
			iRetValue = pEntity->v.oldbuttons;
			break;
		case groupinfo:
			iRetValue = pEntity->v.groupinfo;
			break;
		case iuser1:
			iRetValue = pEntity->v.iuser1;
			break;
		case iuser2:
			iRetValue = pEntity->v.iuser2;
			break;
		case iuser3:
			iRetValue = pEntity->v.iuser3;
			break;
		case iuser4:
			iRetValue = pEntity->v.iuser4;
			break;
		case weaponanim:
			iRetValue = pEntity->v.weaponanim;
			break;
		case pushmsec:
			iRetValue = pEntity->v.pushmsec;
			break;
		case bInDuck:
			iRetValue = pEntity->v.bInDuck;
			break;
		case flTimeStepSound:
			iRetValue = pEntity->v.flTimeStepSound;
			break;
		case flSwimTime:
			iRetValue = pEntity->v.flSwimTime;
			break;
		case flDuckTime:
			iRetValue = pEntity->v.flDuckTime;
			break;
		case iStepLeft:
			iRetValue = pEntity->v.iStepLeft;
			break;
		case movetype:
			iRetValue = pEntity->v.movetype;
			break;
		case solid:
			iRetValue = pEntity->v.solid;
			break;
		case skin:
			iRetValue = pEntity->v.skin;
			break;
		case body:
			iRetValue = pEntity->v.body;
			break;
		case effects:
			iRetValue = pEntity->v.effects;
			break;
		case light_level:
			iRetValue = pEntity->v.light_level;
			break;
		case sequence:
			iRetValue = pEntity->v.sequence;
			break;
		case gaitsequence:
			iRetValue = pEntity->v.gaitsequence;
			break;
		case modelindex:
			iRetValue = pEntity->v.modelindex;
			break;
		case playerclass:
			iRetValue = pEntity->v.playerclass;
			break;
		case waterlevel:
			iRetValue = pEntity->v.waterlevel;
			break;
		case watertype:
			iRetValue = pEntity->v.watertype;
			break;
		case spawnflags:
			iRetValue = pEntity->v.spawnflags;
			break;
		case flags:
			iRetValue = pEntity->v.flags;
			break;
		case colormap:
			iRetValue = pEntity->v.colormap;
			break;
		case team:
			iRetValue = pEntity->v.team;
			break;
		case fixangle:
			iRetValue = pEntity->v.fixangle;
			break;
		case weapons:
			iRetValue = pEntity->v.weapons;
			break;
		case rendermode:
			iRetValue = pEntity->v.rendermode;
			break;
		case renderfx:
			iRetValue = pEntity->v.renderfx;
			break;
		case button:
			iRetValue = pEntity->v.button;
			break;
		case impulse:
			iRetValue = pEntity->v.impulse;
			break;
		case deadflag:
			iRetValue = pEntity->v.deadflag;
			break;
		default:
			return 0;
			break;
	}

	return iRetValue;
}

//vexd
static cell AMX_NATIVE_CALL entity_set_int(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	int iNewValue = params[3];
  
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	switch(iValueSet) {
		case gamestate:
			pEntity->v.gamestate = iNewValue;
			break;
		case oldbuttons:
			pEntity->v.oldbuttons = iNewValue;
			break;
		case groupinfo:
			pEntity->v.groupinfo = iNewValue;
			break;
		case iuser1:
			pEntity->v.iuser1 = iNewValue;
			break;
		case iuser2:
			pEntity->v.iuser2 = iNewValue;
			break;
		case iuser3:
			pEntity->v.iuser3 = iNewValue;
			break;
		case iuser4:
			pEntity->v.iuser4 = iNewValue;
			break;
		case weaponanim:
			pEntity->v.weaponanim = iNewValue;
			break;
		case pushmsec:
			pEntity->v.pushmsec = iNewValue;
			break;
		case bInDuck:
			pEntity->v.bInDuck = iNewValue;
			break;
		case flTimeStepSound:
			pEntity->v.flTimeStepSound = iNewValue;
			break;
		case flSwimTime:
			pEntity->v.flSwimTime = iNewValue;
			break;
		case flDuckTime:
			pEntity->v.flDuckTime = iNewValue;
			break;
		case iStepLeft:
			pEntity->v.iStepLeft = iNewValue;
			break;
		case movetype:
			pEntity->v.movetype = iNewValue;
			break;
		case solid:
			pEntity->v.solid = iNewValue;
			break;
		case skin:
			pEntity->v.skin = iNewValue;
			break;
		case body:
			pEntity->v.body = iNewValue;
			break;
		case effects:
			pEntity->v.effects = iNewValue;
			break;
		case light_level:
			pEntity->v.light_level = iNewValue;
			break;
		case sequence:
			pEntity->v.sequence = iNewValue;
			break;
		case gaitsequence:
			pEntity->v.gaitsequence = iNewValue;
			break;
		case modelindex:
			pEntity->v.modelindex = iNewValue;
			break;
		case playerclass:
			pEntity->v.playerclass = iNewValue;
			break;
		case waterlevel:
			pEntity->v.waterlevel = iNewValue;
			break;
		case watertype:
			pEntity->v.watertype = iNewValue;
			break;
		case spawnflags:
			pEntity->v.spawnflags = iNewValue;
			break;
		case flags:
			pEntity->v.flags = iNewValue;
			break;
		case colormap:
			pEntity->v.colormap = iNewValue;
			break;
		case team:
			pEntity->v.team = iNewValue;
			break;
		case fixangle:
			pEntity->v.fixangle = iNewValue;
			break;
		case weapons:
			pEntity->v.weapons = iNewValue;
			break;
		case rendermode:
			pEntity->v.rendermode = iNewValue;
			break;
		case renderfx:
			pEntity->v.renderfx = iNewValue;
			break;
		case button:
			pEntity->v.button = iNewValue;
			break;
		case impulse:
			pEntity->v.impulse = iNewValue;
			break;
		case deadflag:
			pEntity->v.deadflag = iNewValue;
			break;
		default:
			return 0;
			break;
	}

	return 1;
}

// Gets a float out of entvars.
//(vexd)
static cell AMX_NATIVE_CALL entity_get_float(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	float fRetValue = 0;
  
	// Valid entity?
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return -1;
	}

	// Get pointer.
	edict_t* pEntity = INDEXENT(iTargetEntity);

	// is it a valid entity?
	if(FNullEnt(pEntity)) {
		return -1;
	}

	switch(iValueSet) {
		case impacttime:
			fRetValue = pEntity->v.impacttime;
			break;
		case starttime:
			fRetValue = pEntity->v.starttime;
			break;
		case idealpitch:
			fRetValue = pEntity->v.idealpitch;
			break;
		case pitch_speed:
			fRetValue = pEntity->v.pitch_speed;
			break;
		case ideal_yaw:
			fRetValue = pEntity->v.ideal_yaw;
			break;
		case yaw_speed:
			fRetValue = pEntity->v.yaw_speed;
			break;
		case ltime:
			fRetValue = pEntity->v.ltime;
			break;
		case nextthink:
			fRetValue = pEntity->v.nextthink;
			break;
		case gravity:
			fRetValue = pEntity->v.gravity;
			break;
		case friction:
			fRetValue = pEntity->v.friction;
			break;
		case frame:
			fRetValue = pEntity->v.frame;
			break;
		case animtime:
			fRetValue = pEntity->v.animtime;
			break;
		case framerate:
			fRetValue = pEntity->v.framerate;
			break;
		case health:
			fRetValue = pEntity->v.health;
			break;
		case frags:
			fRetValue = pEntity->v.frags;
			break;
		case takedamage:
			fRetValue = pEntity->v.takedamage;
			break;
		case max_health:
			fRetValue = pEntity->v.max_health;
			break;
		case teleport_time:
			fRetValue = pEntity->v.teleport_time;
			break;
		case armortype:
			fRetValue = pEntity->v.armortype;
			break;
		case armorvalue:
			fRetValue = pEntity->v.armorvalue;
			break;
		case dmg_take:
			fRetValue = pEntity->v.dmg_take;
			break;
		case dmg_save:
			fRetValue = pEntity->v.dmg_save;
			break;
		case dmg:
			fRetValue = pEntity->v.dmg;
			break;
		case dmgtime:
			fRetValue = pEntity->v.dmgtime;
			break;
		case speed:
			fRetValue = pEntity->v.speed;
			break;
		case air_finished:
			fRetValue = pEntity->v.air_finished;
			break;
		case pain_finished:
			fRetValue = pEntity->v.pain_finished;
			break;
		case radsuit_finished:
			fRetValue = pEntity->v.radsuit_finished;
			break;
		case scale:
			fRetValue = pEntity->v.scale;
			break;
		case renderamt:
			fRetValue = pEntity->v.renderamt;
			break;
		case maxspeed:
			fRetValue = pEntity->v.maxspeed;
			break;
		case fov:
			fRetValue = pEntity->v.fov;
			break;
		case flFallVelocity:
			fRetValue = pEntity->v.flFallVelocity;
			break;
		case fuser1:
			fRetValue = pEntity->v.fuser1;
			break;
		case fuser2:
			fRetValue = pEntity->v.fuser2;
			break;
		case fuser3:
			fRetValue = pEntity->v.fuser3;
			break;
		case fuser4:
			fRetValue = pEntity->v.fuser4;
			break;
		default:
			return 0;
			break;
	}

	return *(cell*)((void *)&fRetValue);
}

// Almost the same as Get_float, look there for comments.
static cell AMX_NATIVE_CALL entity_set_float(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	float fNewValue = *(float *)((void *)&params[3]);
  
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	switch(iValueSet) {
		case impacttime:
			pEntity->v.impacttime = fNewValue;
			break;
		case starttime:
			pEntity->v.starttime = fNewValue;
			break;
		case idealpitch:
			pEntity->v.idealpitch = fNewValue;
			break;
		case pitch_speed:
			pEntity->v.pitch_speed = fNewValue;
			break;
		case ideal_yaw:
			pEntity->v.ideal_yaw = fNewValue;
			break;
		case yaw_speed:
			pEntity->v.yaw_speed = fNewValue;
			break;
		case ltime:
			pEntity->v.ltime = fNewValue;
			break;
		case nextthink:
			pEntity->v.nextthink = fNewValue;
			break;
		case gravity:
			pEntity->v.gravity = fNewValue;
			break;
		case friction:
			pEntity->v.friction = fNewValue;
			break;
		case frame:
			pEntity->v.frame = fNewValue;
			break;
		case animtime:
			pEntity->v.animtime = fNewValue;
			break;
		case framerate:
			pEntity->v.framerate = fNewValue;
			break;
		case health:
			pEntity->v.health = fNewValue;
			break;
		case frags:
			pEntity->v.frags = fNewValue;
			break;
		case takedamage:
			pEntity->v.takedamage = fNewValue;
			break;
		case max_health:
			pEntity->v.max_health = fNewValue;
			break;
		case teleport_time:
			pEntity->v.teleport_time = fNewValue;
			break;
		case armortype:
			pEntity->v.armortype = fNewValue;
			break;
		case armorvalue:
			pEntity->v.armorvalue = fNewValue;
			break;
		case dmg_take:
			pEntity->v.dmg_take = fNewValue;
			break;
		case dmg_save:
			pEntity->v.dmg_save = fNewValue;
			break;
		case dmg:
			pEntity->v.dmg = fNewValue;
			break;
		case dmgtime:
			pEntity->v.dmgtime = fNewValue;
			break;
		case speed:
			pEntity->v.speed = fNewValue;
			break;
		case air_finished:
			pEntity->v.air_finished = fNewValue;
			break;
		case pain_finished:
			pEntity->v.pain_finished = fNewValue;
			break;
		case radsuit_finished:
			pEntity->v.radsuit_finished = fNewValue;
			break;
		case scale:
			pEntity->v.scale = fNewValue;
			break;
		case renderamt:
			pEntity->v.renderamt = fNewValue;
			break;
		case maxspeed:
			pEntity->v.maxspeed = fNewValue;
			break;
		case fov:
			pEntity->v.fov = fNewValue;
			break;
		case flFallVelocity:
			pEntity->v.flFallVelocity = fNewValue;
			break;
		case fuser1:
			pEntity->v.fuser1 = fNewValue;
			break;
		case fuser2:
			pEntity->v.fuser2 = fNewValue;
			break;
		case fuser3:
			pEntity->v.fuser3 = fNewValue;
			break;
		case fuser4:
			pEntity->v.fuser4 = fNewValue;
			break;
		default:
			return 0;
			break;
	}

	return 1;
}

// Gets a vector from entvars. I use a float[3] to describe half life vectors,
// as this is how half-life handles vectors. also angle vectors and the such are
// less then 1, so you need floats. All functions that i have that work with vectors
// use the float[3], this makes it easier.
//(vexd)
static cell AMX_NATIVE_CALL entity_get_vector(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];

	cell *vReturnTo = GET_AMXADDR(amx,params[3]);

	Vector vRetValue = Vector(0, 0, 0);
  
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	switch(iValueSet) {
		case origin:
			vRetValue = pEntity->v.origin;
			break;
		case oldorigin:
			vRetValue = pEntity->v.oldorigin;
			break;
		case velocity:
			vRetValue = pEntity->v.velocity;
			break;
		case basevelocity:
			vRetValue = pEntity->v.basevelocity;
			break;
		case clbasevelocity:
			vRetValue = pEntity->v.clbasevelocity;
			break;
		case movedir:
			vRetValue = pEntity->v.movedir;
			break;
		case angles:
			vRetValue = pEntity->v.angles;
			break;
		case avelocity:
			vRetValue = pEntity->v.avelocity;
			break;
		case punchangle:
			vRetValue = pEntity->v.punchangle;
			break;
		case v_angle:
			vRetValue = pEntity->v.v_angle;
			break;
		case endpos:
			vRetValue = pEntity->v.endpos;
			break;
		case startpos:
			vRetValue = pEntity->v.startpos;
			break;
		case absmin:
			vRetValue = pEntity->v.absmin;
			break;
		case absmax:
			vRetValue = pEntity->v.absmax;
			break;
		case mins:
			vRetValue = pEntity->v.mins;
			break;
		case maxs:
			vRetValue = pEntity->v.maxs;
			break;
		case size:
			vRetValue = pEntity->v.size;
			break;
		case rendercolor:
			vRetValue = pEntity->v.rendercolor;
			break;
		case view_ofs:
			vRetValue = pEntity->v.view_ofs;
			break;
		case vuser1:
			vRetValue = pEntity->v.vuser1;
			break;
		case vuser2:
			vRetValue = pEntity->v.vuser2;
			break;
		case vuser3:
			vRetValue = pEntity->v.vuser3;
			break;
		case vuser4:
			vRetValue = pEntity->v.vuser4;
			break;
		default:
			return 0;
			break;
	}


	vReturnTo[0] = *(cell*)((void *)&vRetValue.x); 
	vReturnTo[1] = *(cell*)((void *)&vRetValue.y); 
	vReturnTo[2] = *(cell*)((void *)&vRetValue.z); 

	return 1;
}

// Set is close to get, these functions are pretty self-explanitory.
static cell AMX_NATIVE_CALL entity_set_vector(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	cell *vInput = GET_AMXADDR(amx,params[3]);

	float fNewX = *(float *)((void *)&vInput[0]);
	float fNewY = *(float *)((void *)&vInput[1]);
	float fNewZ = *(float *)((void *)&vInput[2]);

	Vector vNewValue = Vector(fNewX, fNewY, fNewZ);
  
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	switch(iValueSet) {
		case origin:
			pEntity->v.origin = vNewValue;
			break;
		case oldorigin:
			pEntity->v.oldorigin = vNewValue;
			break;
		case velocity:
			pEntity->v.velocity = vNewValue;
			break;
		case basevelocity:
			pEntity->v.basevelocity = vNewValue;
			break;
		case clbasevelocity:
			pEntity->v.clbasevelocity = vNewValue;
			break;
		case movedir:
			pEntity->v.movedir = vNewValue;
			break;
		case angles:
			pEntity->v.angles = vNewValue;
			break;
		case avelocity:
			pEntity->v.avelocity = vNewValue;
			break;
		case punchangle:
			pEntity->v.punchangle = vNewValue;
			break;
		case v_angle:
			pEntity->v.v_angle = vNewValue;
			break;
		case endpos:
			pEntity->v.endpos = vNewValue;
			break;
		case startpos:
			pEntity->v.startpos = vNewValue;
			break;
		case absmin:
			pEntity->v.absmin = vNewValue;
			break;
		case absmax:
			pEntity->v.absmax = vNewValue;
			break;
		case mins:
			pEntity->v.mins = vNewValue;
			break;
		case maxs:
			pEntity->v.maxs = vNewValue;
			break;
		case size:
			pEntity->v.size = vNewValue;
			break;
		case rendercolor:
			pEntity->v.rendercolor = vNewValue;
			break;
		case view_ofs:
			pEntity->v.view_ofs = vNewValue;
			break;
		case vuser1:
			pEntity->v.vuser1 = vNewValue;
			break;
		case vuser2:
			pEntity->v.vuser2 = vNewValue;
			break;
		case vuser3:
			pEntity->v.vuser3 = vNewValue;
			break;
		case vuser4:
			pEntity->v.vuser4 = vNewValue;
			break;
		default:
			return 0;
			break;
	}

	return 1;
}

// Get an edict pointer (or index, amx does not have pointers) from entvars.
//(vexd)
static cell AMX_NATIVE_CALL entity_get_edict(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];

	// Return edict structure.
	edict_t *pRetValue;
  
	// Valid entity?
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	// get edict pointer of target entity
	edict_t* pEntity = INDEXENT(iTargetEntity);

	// is it valid?
	if(FNullEnt(pEntity)) {
		return 0;
	}

	switch(iValueSet) {
		case chain:
			pRetValue = pEntity->v.chain;
			break;
		case dmg_inflictor:
			pRetValue = pEntity->v.dmg_inflictor;
			break;
		case enemy:
			pRetValue = pEntity->v.enemy;
			break;
		case aiment:
			pRetValue = pEntity->v.aiment;
			break;
		case owner:
			pRetValue = pEntity->v.owner;
			break;
		case groundentity:
			pRetValue = pEntity->v.groundentity;
			break;
		case pContainingEntity:
			pRetValue = pEntity->v.pContainingEntity;
			break;
		case euser1:
			pRetValue = pEntity->v.euser1;
			break;
		case euser2:
			pRetValue = pEntity->v.euser2;
			break;
		case euser3:
			pRetValue = pEntity->v.euser3;
			break;
		case euser4:
			pRetValue = pEntity->v.euser4;
			break;
		default:
			return 0;
			break;
	}

	if(!FNullEnt(pRetValue)) {
		return ENTINDEX(pRetValue);
	} else {
		return 0;
	}
}

// Set edict is almost the same as get, look there for comments.
//(vexd)
static cell AMX_NATIVE_CALL entity_set_edict(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	edict_t *pNewValue = INDEXENT(params[3]);
  
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	if(FNullEnt(pNewValue)) {
		return 0;
	}

	switch(iValueSet) {
		case chain:
			pEntity->v.chain = pNewValue;
			break;
		case dmg_inflictor:
			pEntity->v.dmg_inflictor = pNewValue;
			break;
		case enemy:
			pEntity->v.enemy = pNewValue;
			break;
		case aiment:
			pEntity->v.aiment = pNewValue;
			break;
		case owner:
			pEntity->v.owner = pNewValue;
			break;
		case groundentity:
			pEntity->v.groundentity = pNewValue;
			break;
		case pContainingEntity:
			pEntity->v.pContainingEntity = pNewValue;
			break;
		case euser1:
			pEntity->v.euser1 = pNewValue;
			break;
		case euser2:
			pEntity->v.euser2 = pNewValue;
			break;
		case euser3:
			pEntity->v.euser3 = pNewValue;
			break;
		case euser4:
			pEntity->v.euser4 = pNewValue;
			break;
		default:
			return 0;
			break;
	}

	return 1;
}

// Strings. I remember now that weaponmodel and viewmodel are strings as well,
// even though half-life declares them as integers. (a half-life string is just a
// typedefed integer.).
//(vexd)
static cell AMX_NATIVE_CALL entity_get_string(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	int iszRetValue = 0;
  
	// Valid entity?
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	// Get pointer of entity.
	edict_t* pEntity = INDEXENT(iTargetEntity);

	// Is entity valid again?
	if(FNullEnt(pEntity)) {
		return 0;
	}

	switch(iValueSet) {
		case classname:
			iszRetValue = pEntity->v.classname;
			break;
		case globalname:
			iszRetValue = pEntity->v.globalname;
			break;
		case model:
			iszRetValue = pEntity->v.model;
			break;
		case target:
			iszRetValue = pEntity->v.target;
			break;
		case targetname:
			iszRetValue = pEntity->v.targetname;
			break;
		case netname:
			iszRetValue = pEntity->v.netname;
			break;
		case message:
			iszRetValue = pEntity->v.message;
			break;
		case noise:
			iszRetValue = pEntity->v.noise;
			break;
		case noise1:
			iszRetValue = pEntity->v.noise1;
			break;
		case noise2:
			iszRetValue = pEntity->v.noise2;
			break;
		case noise3:
			iszRetValue = pEntity->v.noise3;
			break;
		case viewmodel:
			iszRetValue = pEntity->v.viewmodel;
			break;
		case weaponmodel:
			iszRetValue = pEntity->v.weaponmodel;
			break;
		default:
			return 0;
			break;
	}

	return SET_AMXSTRING(amx, params[3], STRING(iszRetValue), params[4]); 
}

// Almost the same as Get_String, look there for comments.
//(vexd)
static cell AMX_NATIVE_CALL entity_set_string(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	int iLength;
	int iszNewValue = AMX_MAKE_STRING(amx, params[3], iLength);

	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	switch(iValueSet) {
		case classname:
			pEntity->v.classname = iszNewValue;
			break;
		case globalname:
			pEntity->v.globalname = iszNewValue;
			break;
		case model:
			pEntity->v.model = iszNewValue;
			break;
		case target:
			pEntity->v.target = iszNewValue;
			break;
		case targetname:
			pEntity->v.targetname = iszNewValue;
			break;
		case netname:
			pEntity->v.netname = iszNewValue;
			break;
		case message:
			pEntity->v.message = iszNewValue;
			break;
		case noise:
			pEntity->v.noise = iszNewValue;
			break;
		case noise1:
			pEntity->v.noise1 = iszNewValue;
			break;
		case noise2:
			pEntity->v.noise2 = iszNewValue;
			break;
		case noise3:
			pEntity->v.noise3 = iszNewValue;
			break;
		case viewmodel:
			pEntity->v.viewmodel = iszNewValue;
			break;
		case weaponmodel:
			pEntity->v.weaponmodel = iszNewValue;
			break;
		default:
			return 0;
			break;
	}

	return 1;
}

// Bytes, these were used for some things, mostly useless, but might be useful.
// They are arrays, but we just use numbers in naming of the enum variables to
// let us pass get different positions in the array.
//(vexd)
static cell AMX_NATIVE_CALL entity_get_byte(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	int iRetValue = 0;

	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	switch(iValueSet) {
		case controller1:
			iRetValue = pEntity->v.controller[0];
			break;
		case controller2:
			iRetValue = pEntity->v.controller[1];
			break;
		case controller3:
			iRetValue = pEntity->v.controller[2];
			break;
		case controller4:
			iRetValue = pEntity->v.controller[3];
			break;
		case blending1:
			iRetValue = pEntity->v.blending[0];
			break;
		case blending2:
			iRetValue = pEntity->v.blending[1];
			break;
		default:
			return 0;
			break;
	}

	return iRetValue;
}

// Like Get_Byte, but sets.
//(vexd)
static cell AMX_NATIVE_CALL entity_set_byte(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iValueSet = params[2];
	int iNewValue = params[3];

	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	if(iNewValue > 255) iNewValue = 255;
	if(iNewValue < 0) iNewValue = 0;

	switch(iValueSet) {
		case controller1:
			pEntity->v.controller[1] = iNewValue;
			break;
		case controller2:
			pEntity->v.controller[2] = iNewValue;
			break;
		case controller3:
			pEntity->v.controller[3] = iNewValue;
			break;
		case controller4:
			pEntity->v.controller[4] = iNewValue;
			break;
		case blending1:
			pEntity->v.blending[1] = iNewValue;
			break;
		case blending2:
			pEntity->v.blending[2] = iNewValue;
			break;
		default:
			return 0;
			break;
	}

	return 1;
}

// VelocityByAim, this function will take the aimvectors of an entity, and create a velocity
// of iVelocity in the direction of the aimvec.
//(vexd)
static cell AMX_NATIVE_CALL VelocityByAim(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	int iVelocity = params[2];
	cell *vReturnTo = GET_AMXADDR(amx,params[3]);

	Vector vRetValue = Vector(0, 0, 0);
  
	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pEntity = INDEXENT(iTargetEntity);

	if(FNullEnt(pEntity)) {
		return 0;
	}

	MAKE_VECTORS(pEntity->v.v_angle);
	vRetValue = gpGlobals->v_forward * iVelocity;

	vReturnTo[0] = *(cell*)((void *)&vRetValue.x); 
	vReturnTo[1] = *(cell*)((void *)&vRetValue.y); 
	vReturnTo[2] = *(cell*)((void *)&vRetValue.z); 

	return 1;
}

// RadiusDamage. Damages players within a certain radius. ToDo: add the
// damage messaging so players know where the damage is coming from
// (the red arrow-like things on the screen).
//(vexd)
static cell AMX_NATIVE_CALL RadiusDamage(AMX *amx, cell *params) {
	cell *vInput = GET_AMXADDR(amx,params[1]);

	float fCurrentX = *(float *)((void *)&vInput[0]);
	float fCurrentY = *(float *)((void *)&vInput[1]);
	float fCurrentZ = *(float *)((void *)&vInput[2]);
	int iDamageMultiplier = params[2];
	int iRadiusMultiplier = params[3];

	Vector vOrigin = Vector(fCurrentX, fCurrentY, fCurrentZ);

	edict_t *pSearchEnt = NULL;
	while((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, vOrigin, 5 * iRadiusMultiplier)) != NULL) {
		if(FStrEq(STRING(pSearchEnt->v.classname), "player")) {
			if(pSearchEnt->v.takedamage != DAMAGE_NO) {
				pSearchEnt->v.health -= 10 + RANDOM_FLOAT(0,1 * iDamageMultiplier);
				if(pSearchEnt->v.health < 1) {
					pSearchEnt->v.health = 1;
					MDLL_ClientKill(pSearchEnt);
				}
			}
		}
	}

	pSearchEnt = NULL;

	while((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, vOrigin, 4 * iRadiusMultiplier)) != NULL) {
		if(FStrEq(STRING(pSearchEnt->v.classname), "player")) {
			if(pSearchEnt->v.takedamage != DAMAGE_NO) {
				pSearchEnt->v.health -= 25 + RANDOM_FLOAT(0,2 * iDamageMultiplier);
				if(pSearchEnt->v.health < 1) {
					pSearchEnt->v.health = 1;
					MDLL_ClientKill(pSearchEnt);
				}
			}
		}
	}

	pSearchEnt = NULL;

	while((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, vOrigin, 3 * iRadiusMultiplier)) != NULL) {
		if(FStrEq(STRING(pSearchEnt->v.classname), "player")) {
			if(pSearchEnt->v.takedamage != DAMAGE_NO) {
				pSearchEnt->v.health -= 50 + RANDOM_FLOAT(0,3 * iDamageMultiplier);
				if(pSearchEnt->v.health < 1) {
					pSearchEnt->v.health = 1;
					MDLL_ClientKill(pSearchEnt);
				}
			}
		}
	}

	pSearchEnt = NULL;

	while((pSearchEnt = UTIL_FindEntityInSphere(pSearchEnt, vOrigin, 2 * iRadiusMultiplier)) != NULL) {
		if(FStrEq(STRING(pSearchEnt->v.classname), "player")) {
			if(pSearchEnt->v.takedamage != DAMAGE_NO) MDLL_ClientKill(pSearchEnt);
		}
	}

	return 1;
}

// Gets the contents of a point. Return values for this are probably in const.h.
//(vexd)
static cell AMX_NATIVE_CALL PointContents(AMX *amx, cell *params) {
	cell *vInput = GET_AMXADDR(amx,params[1]);

	float fCurrentX = *(float *)((void *)&vInput[0]);
	float fCurrentY = *(float *)((void *)&vInput[1]);
	float fCurrentZ = *(float *)((void *)&vInput[2]);

	Vector vTestValue = Vector(fCurrentX, fCurrentY, fCurrentZ);

	return POINT_CONTENTS(vTestValue);
}


// CreateEntity. Makes an entity.
//(vexd)
static cell AMX_NATIVE_CALL create_entity(AMX *amx, cell *params) { 
	int iLength;
	int iszNewClassName = AMX_MAKE_STRING(amx, params[1], iLength);

	edict_t* pNewEntity = CREATE_NAMED_ENTITY(iszNewClassName);

	if(FNullEnt(pNewEntity)) {
		return 0;
	}

	return ENTINDEX(pNewEntity);
}

//from jghg2
static cell AMX_NATIVE_CALL find_ent_in_sphere(AMX *amx, cell *params)
{
	if (params[1] < 0 || params[1] > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	edict_t *pStartAfterEnt = INDEXENT(params[1]);

	cell *originLong = GET_AMXADDR(amx, params[2]);
	float origin[3] = {*(float *)((void *)&originLong[0]), *(float *)((void *)&originLong[1]), *(float *)((void *)&originLong[2])}; // float origin[3] = {originLong[0], originLong[1], originLong[2]};
	
	float radius = *(float *)((void *)&params[3]);

	int returnEnt = ENTINDEX(FIND_ENTITY_IN_SPHERE(pStartAfterEnt, origin, radius));

	if (FNullEnt(returnEnt)) {
		return 0;
	}

	return returnEnt;
}


//ej ref'd by jghg
static cell AMX_NATIVE_CALL find_ent_by_class(AMX *amx, cell *params) /* 3 param */
{
	edict_t *pEnt = INDEXENT(params[1]);

	int len;
	char* sValue = GET_AMXSTRING(amx, params[2], 1, len);

	pEnt = FIND_ENTITY_BY_STRING(pEnt, "classname", sValue);

	if(pEnt) // This even needed?
		return ENTINDEX(pEnt);
	else
		return 0;
}

//from jghg2
static cell AMX_NATIVE_CALL find_sphere_class(AMX *amx, cell *params) // find_sphere_class(aroundent, _lookforclassname[], Float:radius, entlist[], maxents, Float:origin[3] = {0.0, 0.0, 0.0}); // 6 params
{
	// params[1] = index to find around, if this is less than 1 then use around origin in last parameter.
	// params[2] = classname to find
	int len;
	char* classToFind = GET_AMXSTRING(amx, params[2], 0, len);
	// params[3] = radius, float...
	float radius = *(float *)((void *)&params[3]);
	// params[4] = store ents in this list
	cell *entList = GET_AMXADDR(amx, params[4]);
	// params[5] = maximum ents to store in entlist[] in params[4]
	// params[6] = origin, use this if params[1] is less than 1

	vec3_t vecOrigin;
	if (params[1] > 0) {
		if (params[1] > gpGlobals->maxEntities)
		{
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}

		edict_t* pEntity = INDEXENT(params[1]);
		if (FNullEnt(pEntity)) {
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
		}

		vecOrigin = pEntity->v.origin;
	}
	else {
		cell *newVectorCell = GET_AMXADDR(amx, params[6]);
		vecOrigin = Vector(*(float *)((void *)&newVectorCell[0]), *(float *)((void *)&newVectorCell[1]), *(float *)((void *)&newVectorCell[2]));
	}
	
	int entsFound = 0;
	edict_t* pSearchEnt = INDEXENT(0);

	while (entsFound < params[5]) {
		pSearchEnt = FIND_ENTITY_IN_SPHERE(pSearchEnt, vecOrigin, radius); // takes const float origin
		if (FNullEnt(pSearchEnt))
			break;
		else {
			if (strcmp(STRING(pSearchEnt->v.classname), classToFind) == 0) {
				// Add to entlist (params[4])
				entList[entsFound++] = ENTINDEX(pSearchEnt); // raise entsFound
			}
		}
	}

	return entsFound;
}

// DispatchKeyValue, sets a key-value pair for a newly created entity.
// Use DispatchSpawn after doing ALL DispatchKeyValues on an entity.
//(vexd)
static cell AMX_NATIVE_CALL DispatchKeyValue(AMX *amx, cell *params) { 
	edict_t* pTarget = INDEXENT(params[1]);
	int iKeyLength;
	int iValueLength;
	char *szKey = AMX_GET_STRING(amx, params[2], iKeyLength);
	char *szValue = AMX_GET_STRING(amx, params[3], iValueLength);


	if(FNullEnt(pTarget)) {
		return 0;
	}

	KeyValueData pkvd;
	pkvd.szClassName = (char *)STRING(pTarget->v.classname);
	pkvd.szKeyName = szKey;
	pkvd.szValue = szValue;
	pkvd.fHandled = false;

	MDLL_KeyValue(pTarget, &pkvd);
	return 1;
}

// DispatchSpawn. Call this after doing any DispatchKeyValues you are
// doing on an entity your creating.
//(vexd)
static cell AMX_NATIVE_CALL DispatchSpawn(AMX *amx, cell *params) { 
	edict_t* pTarget = INDEXENT(params[1]);

	if(FNullEnt(pTarget)) {
		return 0;
	}

	MDLL_Spawn(pTarget);

	return 1;
}

// Set origin for entities. Use this instead of entvars, as it updates the engine.
// It also does a SetSize, this way we can set the size at the same time as origin.
//(vexd)
static cell AMX_NATIVE_CALL entity_set_origin(AMX *amx, cell *params) { 
	int iTargetEntity = params[1];
	cell *vInput = GET_AMXADDR(amx,params[2]);

	float fNewX = *(float *)((void *)&vInput[0]);
	float fNewY = *(float *)((void *)&vInput[1]);
	float fNewZ = *(float *)((void *)&vInput[2]);

	Vector vNewValue = Vector(fNewX, fNewY, fNewZ);

	if (iTargetEntity < 1 || iTargetEntity > gpGlobals->maxEntities) {
		return 0;
	}

	edict_t* pTarget = INDEXENT(iTargetEntity);

	if(FNullEnt(pTarget)) {
		return 0;
	}

	SET_SIZE(pTarget, pTarget->v.mins, pTarget->v.maxs);
	SET_ORIGIN(pTarget, vNewValue);

	return 1;
}


// Sets a model on an entity. Dont set models in entvars, it wont
// update the engine of the model change.
//(vexd)
static cell AMX_NATIVE_CALL entity_set_model(AMX *amx, cell *params) { 
	edict_t* pTarget = INDEXENT(params[1]);

	int iLength;
	char *szNewValue = AMX_GET_STRING(amx, params[2], iLength);

	if(FNullEnt(pTarget)) {
		return 0;
	}

	SET_MODEL(pTarget, szNewValue);

	return 1;
}

//FindEntityByTarget (BAILOPAN)
// Works like FindEntity except finds by Target
static cell AMX_NATIVE_CALL find_ent_by_target(AMX *amx, cell *params)
{
	int iStart = params[1];
	int iLength;
	char *szValue = AMX_GET_STRING(amx, params[2], iLength);

	edict_t *pStart;

	if (iStart == -1) {
		pStart = NULL;
	} else {
		pStart = INDEXENT(iStart);
		if (FNullEnt(pStart)) {
			return 0;
		}
	}

	int iReturnEnt = ENTINDEX(FIND_ENTITY_BY_TARGET(pStart, szValue));

	if (!iReturnEnt || FNullEnt(iReturnEnt)) {
		return 0;
	}

	return iReturnEnt;
}

//FindEntityByModel (BAILOPAN)
//Takes in a classname and model...
static cell AMX_NATIVE_CALL find_ent_by_model(AMX *amx, cell *params) { 
	int iStart = params[1];
	int iLength, iLength2;
	char *szClass = AMX_GET_STRING(amx, params[2], iLength);
	char *szModel = AMX_GET_STRING(amx, params[3], iLength2);
	int iModel = MAKE_STRING(szModel);

	edict_t *pStart;

	if (iStart == -1) {
		pStart = NULL;
	} else {
		pStart = INDEXENT(iStart);
		if (FNullEnt(pStart)) {
			return 0;
		}
	}

	int checkEnt = ENTINDEX(FIND_ENTITY_BY_STRING(pStart, "classname", szClass));
	int iCheckModel = 0;

	while ((checkEnt && FNullEnt(checkEnt))) {
		iCheckModel = pStart->v.model;
		if (iCheckModel == iModel) {
			return checkEnt;
		} else {
			pStart = INDEXENT(checkEnt);
			checkEnt = ENTINDEX(FIND_ENTITY_BY_STRING(pStart, "classname", szClass));
		}
	}

	if(!checkEnt || FNullEnt(checkEnt)) {
		return 0;
	}

	return checkEnt;
}

//FindEntityByTName (BAILOPAN)
// Works like FindEntity except finds by TargetName
static cell AMX_NATIVE_CALL find_ent_by_tname(AMX *amx, cell *params) {
	int iStart = params[1];
	int iLength;
	char *szValue = AMX_GET_STRING(amx, params[2], iLength);

	edict_t *pStart;

	if (iStart == -1) {
		pStart = NULL;
	} else {
		pStart = INDEXENT(iStart);
		if (FNullEnt(pStart)) {
			return 0;
		}
	}

	int iReturnEnt = ENTINDEX(FIND_ENTITY_BY_TARGETNAME(pStart, szValue));

	if (!iReturnEnt || FNullEnt(iReturnEnt)) {
		return 0;
	}

	return iReturnEnt;
}

static cell AMX_NATIVE_CALL find_ent_by_owner(AMX *amx, cell *params)  // native find_ent_by_owner(start_from_ent, classname[], owner_index); = 3 params
{
	// Check index to start searching at, 0 must be possible.
	if (params[1] < 0 || params[1] > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	// Check index of owner
	if (params[3] < 1 || params[3] > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	edict_t *pEnt = INDEXENT(params[1]);
	edict_t *entOwner = INDEXENT(params[3]);

	//optional fourth parameter is for jghg2 compatibility
	char* sCategory = NULL; 
	switch(params[4]){ 
		case 1: sCategory = "target"; break; 
		case 2: sCategory = "targetname"; break; 
		default: sCategory = "classname"; 
	}

	// No need to check if there is a real ent where entOwner points at since we don't access it anyway.

	int len;
	char* classname = GET_AMXSTRING(amx, params[2], 1, len);

	while (true) {
		pEnt = FIND_ENTITY_BY_STRING(pEnt, sCategory, classname);
		if (!pEnt || FNullEnt(pEnt)) // break and return 0 if bad
			break;
		else if (pEnt->v.owner == entOwner) // compare pointers
			return ENTINDEX(pEnt);
	}

	// If it comes here, the while loop ended because an ent failed (FNullEnt() == true)
	return 0;
}

//returns current number of entities in game (BAILOPAN)
static cell AMX_NATIVE_CALL entity_count(AMX *amx, cell *params)
{ 
	return NUMBER_OF_ENTITIES(); 
}

// RemoveEntity, this removes an entity from the world.
// Could also be done setting the flag FL_KILLME in entvars.
//(vexd)
static cell AMX_NATIVE_CALL remove_entity(AMX *amx, cell *params) { 
	int iTarget = params[1];

	edict_t* pTarget = INDEXENT(iTarget);

	if(FNullEnt(pTarget)) {
		return 0;
	}

	REMOVE_ENTITY(pTarget);

	return 1;
}

// VecToAngles, this is a half-life function for making a vector out of
// angles.
//(vexd)
static cell AMX_NATIVE_CALL vector_to_angle(AMX *amx, cell *params) { 
	cell *vInput = GET_AMXADDR(amx,params[1]);
	float fInX = *(float *)((void *)&vInput[0]);
	float fInY = *(float *)((void *)&vInput[1]);
	float fInZ = *(float *)((void *)&vInput[2]);

	Vector vVector = Vector(fInX, fInY, fInZ);
	Vector vAngle = Vector(0, 0, 0);
	VEC_TO_ANGLES(vVector, vAngle);

	cell *vReturnTo = GET_AMXADDR(amx,params[2]);
	vReturnTo[0] = *(cell*)((void *)&vAngle.x);
	vReturnTo[1] = *(cell*)((void *)&vAngle.y);
	vReturnTo[2] = *(cell*)((void *)&vAngle.z);

	return 1;
}

// VecLength, this gives you the length of a vector (float[3] type).
//(vexd)
static cell AMX_NATIVE_CALL vector_length(AMX *amx, cell *params) { 
	cell *vInput = GET_AMXADDR(amx,params[1]);
	float fInX = *(float *)((void *)&vInput[0]);
	float fInY = *(float *)((void *)&vInput[1]);
	float fInZ = *(float *)((void *)&vInput[2]);

	Vector vVector = Vector(fInX, fInY, fInZ);

	float flLength = vVector.Length();

	return *(cell*)((void *)&flLength);
}

// VecDist, this gives you the distance between 2 vectors (float[3] type).
//(vexd)
static cell AMX_NATIVE_CALL vector_distance(AMX *amx, cell *params) { 
	cell *vInput = GET_AMXADDR(amx,params[1]);
	float fInX = *(float *)((void *)&vInput[0]);
	float fInY = *(float *)((void *)&vInput[1]);
	float fInZ = *(float *)((void *)&vInput[2]);

	cell *vInput2 = GET_AMXADDR(amx,params[2]);
	float fInX2 = *(float *)((void *)&vInput2[0]);
	float fInY2 = *(float *)((void *)&vInput2[1]);
	float fInZ2 = *(float *)((void *)&vInput2[2]);

	Vector vVector = Vector(fInX, fInY, fInZ);
	Vector vVector2 = Vector(fInX2, fInY2, fInZ2);

	float flLength = (vVector - vVector2).Length();

	return *(cell*)((void *)&flLength);
}

// TraceNormal. This is just like TraceLine, but it gives back the
// Normal of whatever plane it hit. Use with TraceLine and you can have
// A point on a plane, with its normal, good for creating things that attach
// to walls (like tripmines).
//(vexd)
static cell AMX_NATIVE_CALL trace_normal(AMX *amx, cell *params) { 
	int iIgnoreEnt = params[1];

	cell *fpStart = GET_AMXADDR(amx,params[2]);
	float fStartX = *(float *)((void *)&fpStart[0]);
	float fStartY = *(float *)((void *)&fpStart[1]);
	float fStartZ = *(float *)((void *)&fpStart[2]);

	cell *fpEnd = GET_AMXADDR(amx,params[3]);
	float fEndX = *(float *)((void *)&fpEnd[0]);
	float fEndY = *(float *)((void *)&fpEnd[1]);
	float fEndZ = *(float *)((void *)&fpEnd[2]);


	cell *vReturnTo = GET_AMXADDR(amx,params[4]);

	Vector vStart = Vector(fStartX, fStartY, fStartZ);
	Vector vEnd = Vector(fEndX, fEndY, fEndZ);

	TraceResult tr;
	TRACE_LINE(vStart, vEnd, dont_ignore_monsters, INDEXENT(iIgnoreEnt), &tr);

	vReturnTo[0] = *(cell*)((void *)&tr.vecPlaneNormal.x); 
	vReturnTo[1] = *(cell*)((void *)&tr.vecPlaneNormal.y); 
	vReturnTo[2] = *(cell*)((void *)&tr.vecPlaneNormal.z); 

	if (tr.flFraction >= 1.0) return 0;

	return 1;
}

// TraceLn, Traces a Line in space. This is useful for making beams, or checking if
// An entity is at the end of a line.
// The return value is either the end of the line (where it crashed into an object,
// or the end you supplied.)
//(vexd)
static cell AMX_NATIVE_CALL trace_line(AMX *amx, cell *params) { 
	int iIgnoreEnt = params[1];

	cell *fpStart = GET_AMXADDR(amx,params[2]);
	float fStartX = *(float *)((void *)&fpStart[0]);
	float fStartY = *(float *)((void *)&fpStart[1]);
	float fStartZ = *(float *)((void *)&fpStart[2]);

	cell *fpEnd = GET_AMXADDR(amx,params[3]);
	float fEndX = *(float *)((void *)&fpEnd[0]);
	float fEndY = *(float *)((void *)&fpEnd[1]);
	float fEndZ = *(float *)((void *)&fpEnd[2]);


	cell *vReturnTo = GET_AMXADDR(amx,params[4]);

	Vector vStart = Vector(fStartX, fStartY, fStartZ);
	Vector vEnd = Vector(fEndX, fEndY, fEndZ);

	TraceResult tr;

	if(iIgnoreEnt == -1) {
		TRACE_LINE(vStart, vEnd, ignore_monsters, NULL, &tr);
	} else {
		TRACE_LINE(vStart, vEnd, dont_ignore_monsters, INDEXENT(iIgnoreEnt), &tr);
	}

	edict_t *pHit = tr.pHit;

	vReturnTo[0] = *(cell*)((void *)&tr.vecEndPos.x); 
	vReturnTo[1] = *(cell*)((void *)&tr.vecEndPos.y); 
	vReturnTo[2] = *(cell*)((void *)&tr.vecEndPos.z); 

	if(FNullEnt(pHit)) {
		return 0;
	}

	return ENTINDEX(pHit);
}

// Get gpGlobals->time, this is useful for timing things.
//(vexd)
static cell AMX_NATIVE_CALL halflife_time(AMX *amx, cell *params) {
	float fRetValue = gpGlobals->time;
	return *(cell*)((void *)&fRetValue);
}

//simulate two objects coliding
//(vexd)
static cell AMX_NATIVE_CALL fake_touch(AMX *amx, cell *params) {
	int iToucher = params[1];
	int iTouched = params[2];

	if(iToucher < 1 || iTouched < 1) return 0;
	if(iToucher > gpGlobals->maxEntities || iTouched > gpGlobals->maxEntities) return 0;

	edict_t *pToucher = INDEXENT(iToucher);
	edict_t *pTouched = INDEXENT(iTouched);

	if(FNullEnt(pToucher) || FNullEnt(pTouched)) return 0;

	MDLL_Touch(pToucher, pTouched);

	return 1;
}

//(SpaceDude)
static cell AMX_NATIVE_CALL get_grenade_id(AMX *amx, cell *params)  /* 4 param */ 
{
	int index = params[1];
	char* szModel;

	if (index<1||index>gpGlobals->maxClients)
	{
		return 0; 
	}

	edict_t* pentFind = INDEXENT(params[4]);
	edict_t* pentOwner = INDEXENT(index);

	pentFind = FIND_ENTITY_BY_CLASSNAME( pentFind, "grenade" );
	while ( !FNullEnt( pentFind ) )
	{
		if (pentFind->v.owner == pentOwner)
		{
			szModel = new char[params[3]];
			szModel = (char*)STRING(pentFind->v.model);
			SET_AMXSTRING(amx,params[2],szModel,params[3]);
			delete [] szModel;
			return ENTINDEX(pentFind);
		}
		pentFind = FIND_ENTITY_BY_CLASSNAME( pentFind, "grenade" );
	}
	return 0;
}

// Sets a message block.
static cell AMX_NATIVE_CALL set_msg_block(AMX *amx, cell *params) { 
	int iMessage = params[1];
	int iMessageFlags = params[2];

	if (iMessage < 1 || iMessage > MAX_MESSAGES) {
		return 0;
	}

	GlInfo.iMessageBlock[iMessage] = iMessageFlags;

	return 1;
}

// Gets a message block.
static cell AMX_NATIVE_CALL get_msg_block(AMX *amx, cell *params) { 
	int iMessage = params[1];

	if (iMessage < 1 || iMessage > MAX_MESSAGES) {
		AMX_RAISEERROR(amx,AMX_ERR_NATIVE);
		return 0;
	}

	return GlInfo.iMessageBlock[iMessage];
}

// SetLights, this sets the lights for the map.
//(vexd)
static cell AMX_NATIVE_CALL set_lights(AMX *amx, cell *params) { 
	int iLength;

	char *szLights = AMX_GET_STRING(amx, params[1], iLength);

	if(FStrEq(szLights, "#OFF")) {
		GlInfo.bLights = false;
		memset(GlInfo.szLastLights, 0x0, 128);
		(g_engfuncs.pfnLightStyle)(0, (char *)GlInfo.szRealLights);
		return 1;
	}

	GlInfo.bLights = true;

	//Reset LastLights
	memset(GlInfo.szLastLights, 0x0, 128);
	//Store the previous lighting.
	memcpy(GlInfo.szLastLights, szLights, strlen(szLights));

	(g_engfuncs.pfnLightStyle)(0, szLights);

	// These make it so that players/weaponmodels look like whatever the lighting is
	// at. otherwise it would color players under the skybox to these values.
	SERVER_COMMAND("sv_skycolor_r 0\n");
	SERVER_COMMAND("sv_skycolor_g 0\n");
	SERVER_COMMAND("sv_skycolor_b 0\n");

	return 1;
}


// SetView, this sets the view of a player. This is done by
// Creating a camera entity, which follows the player.
//(vexd)
static cell AMX_NATIVE_CALL set_view(AMX *amx, cell *params) { 
	int iIndex = params[1];
	int iCameraType = params[2];

	if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
		AMX_RAISEERROR(amx,AMX_ERR_NATIVE);
		return 0;
	}

	edict_t *pPlayer = INDEXENT(iIndex);
	edict_t *pNewCamera;

	switch(iCameraType) {
		case CAMERA_NONE:
			SET_VIEW(pPlayer, pPlayer);
			if(PlInfo[ENTINDEX(pPlayer)].pViewEnt) {
				REMOVE_ENTITY(PlInfo[ENTINDEX(pPlayer)].pViewEnt);
			}
			PlInfo[ENTINDEX(pPlayer)].iViewType = CAMERA_NONE;
			PlInfo[ENTINDEX(pPlayer)].pViewEnt = NULL;

			pPlayer->v.rendermode = PlInfo[ENTINDEX(pPlayer)].iRenderMode;
			pPlayer->v.renderamt = PlInfo[ENTINDEX(pPlayer)].fRenderAmt;

			PlInfo[ENTINDEX(pPlayer)].iRenderMode = 0;
			PlInfo[ENTINDEX(pPlayer)].fRenderAmt = 0;

			return 1;
			break;
		case CAMERA_3RDPERSON:
			if(PlInfo[ENTINDEX(pPlayer)].iViewType != CAMERA_NONE) {
				PlInfo[ENTINDEX(pPlayer)].iViewType = CAMERA_3RDPERSON;
				return 1;
			}

			PlInfo[ENTINDEX(pPlayer)].iRenderMode = pPlayer->v.rendermode;
			PlInfo[ENTINDEX(pPlayer)].fRenderAmt = pPlayer->v.renderamt;

			PlInfo[ENTINDEX(pPlayer)].iViewType = CAMERA_3RDPERSON;
			pNewCamera = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
			pNewCamera->v.classname = MAKE_STRING("VexdCam");

			SET_MODEL(pNewCamera, "models/rpgrocket.mdl");
			SET_SIZE(pNewCamera, Vector(0, 0, 0), Vector(0, 0, 0));

			pNewCamera->v.movetype = MOVETYPE_NOCLIP;
			pNewCamera->v.solid = SOLID_NOT;
			pNewCamera->v.takedamage = DAMAGE_NO;
			pNewCamera->v.gravity = 0;
			pNewCamera->v.owner = pPlayer;
			pNewCamera->v.rendermode = kRenderTransColor;
			pNewCamera->v.renderamt = 0;
			pNewCamera->v.renderfx = kRenderFxNone;

			SET_VIEW(pPlayer, pNewCamera);

			PlInfo[ENTINDEX(pPlayer)].pViewEnt = pNewCamera;
			break;
		case CAMERA_UPLEFT:
			if(PlInfo[ENTINDEX(pPlayer)].iViewType != CAMERA_NONE) {
				PlInfo[ENTINDEX(pPlayer)].iViewType = CAMERA_UPLEFT;
				return 1;
			}

			PlInfo[ENTINDEX(pPlayer)].iRenderMode = pPlayer->v.rendermode;
			PlInfo[ENTINDEX(pPlayer)].fRenderAmt = pPlayer->v.renderamt;

			PlInfo[ENTINDEX(pPlayer)].iViewType = CAMERA_3RDPERSON;
			pNewCamera = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
			pNewCamera->v.classname = MAKE_STRING("VexdCam");

			SET_MODEL(pNewCamera, "models/rpgrocket.mdl");
			SET_SIZE(pNewCamera, Vector(0, 0, 0), Vector(0, 0, 0));

			pNewCamera->v.movetype = MOVETYPE_NOCLIP;
			pNewCamera->v.solid = SOLID_NOT;
			pNewCamera->v.takedamage = DAMAGE_NO;
			pNewCamera->v.gravity = 0;
			pNewCamera->v.owner = pPlayer;
			pNewCamera->v.rendermode = kRenderTransColor;
			pNewCamera->v.renderamt = 0;
			pNewCamera->v.renderfx = kRenderFxNone;

			SET_VIEW(pPlayer, pNewCamera);

			PlInfo[ENTINDEX(pPlayer)].pViewEnt = pNewCamera;
			break;
		case CAMERA_TOPDOWN:
			if(PlInfo[ENTINDEX(pPlayer)].iViewType != CAMERA_NONE) {
				PlInfo[ENTINDEX(pPlayer)].iViewType = CAMERA_TOPDOWN;
				return 1;
			}

			PlInfo[ENTINDEX(pPlayer)].iRenderMode = pPlayer->v.rendermode;
			PlInfo[ENTINDEX(pPlayer)].fRenderAmt = pPlayer->v.renderamt;

			PlInfo[ENTINDEX(pPlayer)].iViewType = CAMERA_TOPDOWN;
			pNewCamera = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
			pNewCamera->v.classname = MAKE_STRING("VexdCam");

			SET_MODEL(pNewCamera, "models/rpgrocket.mdl");
			SET_SIZE(pNewCamera, Vector(0, 0, 0), Vector(0, 0, 0));

			pNewCamera->v.movetype = MOVETYPE_NOCLIP;
			pNewCamera->v.solid = SOLID_NOT;
			pNewCamera->v.takedamage = DAMAGE_NO;
			pNewCamera->v.gravity = 0;
			pNewCamera->v.owner = pPlayer;
			pNewCamera->v.rendermode = kRenderTransColor;
			pNewCamera->v.renderamt = 0;
			pNewCamera->v.renderfx = kRenderFxNone;

			SET_VIEW(pPlayer, pNewCamera);

			PlInfo[ENTINDEX(pPlayer)].pViewEnt = pNewCamera;
			break;
		default:
			break;
	}

	return 1;
}

// Attachview, this allows you to attach a player's view to an entity.
// use AttachView(player, player) to reset view.
//(vexd)
static cell AMX_NATIVE_CALL attach_view(AMX *amx, cell *params) { 
	int iIndex = params[1];
	int iTargetIndex = params[2];

	if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
		AMX_RAISEERROR(amx,AMX_ERR_NATIVE);
		return 0;
	}

	if(iTargetIndex < 1 || iTargetIndex > gpGlobals->maxEntities) {
		AMX_RAISEERROR(amx,AMX_ERR_NATIVE);
		return 0;
	}

	SET_VIEW(INDEXENT(iIndex), INDEXENT(iTargetIndex));

	return 1;
}

static cell AMX_NATIVE_CALL precache_generic(AMX *amx, cell *params)
{
	int len;
	char* szPreCache = GET_AMXSTRING(amx,params[1],0,len);
	PRECACHE_GENERIC((char*)STRING(ALLOC_STRING(szPreCache)));
	return 1;
}

//ported from jghg2

static cell AMX_NATIVE_CALL drop_to_floor(AMX *amx, cell *params)
{
	int iEnt = params[1];
	edict_t* e = valid_ent(iEnt);

	if (e!=NULL) {
		return DROP_TO_FLOOR(e);
	}

	return -1;
}

static cell AMX_NATIVE_CALL get_info_keybuffer(AMX *amx, cell *params)
{
	int iEnt = params[1];
	edict_t *e = valid_ent(iEnt);

	if (e!=NULL) {
		char *info = GETINFOKEYBUFFER(e);
	
		return SET_AMXSTRING(amx, params[2], info, params[3]);
	} else {
		return 0;
	}
}

static cell AMX_NATIVE_CALL force_use(AMX *amx, cell *params)
{
	int pev = params[1];
	int ent = params[2];
	edict_t *pPlayer = valid_player(pev);
	edict_t *pEntity = valid_ent(ent);

	MDLL_Use(pEntity, pPlayer);

	return 1;
}

static cell AMX_NATIVE_CALL get_global_float(AMX *amx, cell *params)
{
	int global = params[1];

	float returnValue;

	switch (params[1]) {
		case GL_coop:
			returnValue = gpGlobals->coop;
			break;
		case GL_deathmatch:
			returnValue = gpGlobals->deathmatch;
			break;
		case GL_force_retouch:
			returnValue = gpGlobals->force_retouch;
			break;
		case GL_found_secrets:
			returnValue = gpGlobals->found_secrets;
			break;
		case GL_frametime:
			returnValue = gpGlobals->frametime;
			break;
		case GL_serverflags:
			returnValue = gpGlobals->serverflags;
			break;
		case GL_teamplay:
			returnValue = gpGlobals->teamplay;
			break;
		case GL_time:
			returnValue = gpGlobals->time;
			break;
		case GL_trace_allsolid:
			returnValue = gpGlobals->trace_allsolid;
			break;
		case GL_trace_fraction:
			returnValue = gpGlobals->trace_fraction;
			break;
		case GL_trace_inopen:
			returnValue = gpGlobals->trace_inopen;
			break;
		case GL_trace_inwater:
			returnValue = gpGlobals->trace_inwater;
			break;
		case GL_trace_plane_dist:
			returnValue = gpGlobals->trace_plane_dist;
			break;
		case GL_trace_startsolid:
			returnValue = gpGlobals->trace_startsolid;
			break;
		default:
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
	}

	return *(cell*)((void *)&returnValue);
}

static cell AMX_NATIVE_CALL get_global_int(AMX *amx, cell *params)
{
	int returnValue = 0;

	switch (params[1]) {
		case GL_cdAudioTrack:
			returnValue = gpGlobals->cdAudioTrack;
			break;
		case GL_maxClients:
			returnValue = gpGlobals->maxClients;
			break;
		case GL_maxEntities:
			returnValue = gpGlobals->maxEntities;
			break;
		case GL_msg_entity:
			returnValue = gpGlobals->msg_entity;
			break;
		case GL_trace_flags:
			returnValue = gpGlobals->trace_flags;
			break;
		case GL_trace_hitgroup:
			returnValue = gpGlobals->trace_hitgroup;
			break;
		default:
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
	}

	return returnValue;
}

static cell AMX_NATIVE_CALL get_global_string(AMX *amx, cell *params)
{
	string_t* returnValue;

	switch(params[1]) {
		case GL_pStringBase: // const char *, so no string_t
			return SET_AMXSTRING(amx, params[2], gpGlobals->pStringBase, params[3]);
		// The rest are string_t:s...
		case GL_mapname:
			returnValue = &(gpGlobals->mapname);
			break;
		case GL_startspot:
			returnValue = &(gpGlobals->startspot);
			break;
		default:
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
	}

	return SET_AMXSTRING(amx, params[2], STRING(*returnValue), params[3]);
}

static cell AMX_NATIVE_CALL get_global_vector(AMX *amx, cell *params) // globals_get_vector(variable, Float:vector[3]); = 2 params
{
	cell *returnVector = GET_AMXADDR(amx, params[2]);
	vec3_t fetchedVector;

	switch (params[1]) {
		case GL_trace_endpos:
			fetchedVector = gpGlobals->trace_endpos;
			break;
		case GL_trace_plane_normal:
			fetchedVector = gpGlobals->trace_plane_normal;
			break;
		case GL_v_forward:
			fetchedVector = gpGlobals->v_forward;
			break;
		case GL_v_right:
			fetchedVector = gpGlobals->v_right;
			break;
		case GL_v_up:
			fetchedVector = gpGlobals->v_up;
			break;
		case GL_vecLandmarkOffset:
			fetchedVector = gpGlobals->vecLandmarkOffset;
			break;
		default:
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
	}

	returnVector[0] = *(cell*)((void *)&fetchedVector.x); 
	returnVector[1] = *(cell*)((void *)&fetchedVector.y); 
	returnVector[2] = *(cell*)((void *)&fetchedVector.z); 

	return 1;
}

static cell AMX_NATIVE_CALL get_global_edict(AMX *amx, cell *params) // globals_get_edict(variable); = 1 param
{
	edict_t* pReturnEntity;

	switch (params[1]) {
		case GL_trace_ent:
			pReturnEntity = gpGlobals->trace_ent;
			break;
		default:
			AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
			return 0;
	}

	// Will crash if ENTINDEX() is called on bad pointer?
	if(!FNullEnt(pReturnEntity))
		return ENTINDEX(pReturnEntity);

	return 0;
}

static cell AMX_NATIVE_CALL precache_event(AMX *amx, cell *params)
{
	int len;
	char *szEvent = FORMAT_AMXSTRING(amx, params, 2, len);
	PRECACHE_EVENT(params[1], (char *)STRING(ALLOC_STRING(szEvent)));

	return 1;
}

static cell AMX_NATIVE_CALL get_decal_index(AMX *amx, cell *params)
{
	int len;
	char *szDecal = GET_AMXSTRING(amx, params[1], 0, len);
	return DECAL_INDEX(szDecal);
}

static cell AMX_NATIVE_CALL set_size(AMX *amx, cell *params)
{
	int ent = params[1];
	edict_t *e = valid_ent(ent);

	if (e == NULL) {
		return 0;
	}

	cell *cMin = GET_AMXADDR(amx, params[2]);
	float x1 = *(float *)((void *)&cMin[0]);
	float y1 = *(float *)((void *)&cMin[1]);
	float z1 = *(float *)((void *)&cMin[2]);
	Vector vMin = Vector(x1, y1, z1);

	cell *cMax = GET_AMXADDR(amx, params[3]);
	float x2 = *(float *)((void *)&cMax[0]);
	float y2 = *(float *)((void *)&cMax[1]);
	float z2 = *(float *)((void *)&cMax[2]);
	Vector vMax = Vector(x2, y2, z2);

	UTIL_SetSize(e, vMin, vMax);

	return 1;
}

// SetSpeak, this sets who a player can speak to/who he can listen to.
static cell AMX_NATIVE_CALL set_speak(AMX *amx, cell *params) { 
	int iIndex = params[1];
	int iNewSpeakFlags = params[2];

	if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
		AMX_RAISEERROR(amx,AMX_ERR_NATIVE);
		return 0;
	}

	PlInfo[iIndex].iSpeakFlags = iNewSpeakFlags;

	return 1;
}

//GetSpeak, this gets whether a player can speak to
// (BAILOPAN)
static cell AMX_NATIVE_CALL get_speak(AMX *amx, cell *params) {
	int iIndex = params[1];

	if (iIndex < 1 || iIndex > gpGlobals->maxClients)
	{
		AMX_RAISEERROR(amx, AMX_ERR_NATIVE);
		return 0;
	}

	return PlInfo[iIndex].iSpeakFlags;
}

/********************************************
   METAMOD HOOKED FUNCTIONS
   *****************************************/

// This checks who can hear who through voice comm. this reads flags set,
// and lets us choose who hears who based on the previously set flags.
qboolean Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen) {
	if((PlInfo[iSender].iSpeakFlags & SPEAK_MUTED) != 0) {
		(g_engfuncs.pfnVoice_SetClientListening)(iReceiver, iSender, false);
		RETURN_META_VALUE(MRES_SUPERCEDE, false);
	}

	if((PlInfo[iSender].iSpeakFlags & SPEAK_ALL) != 0) {
		(g_engfuncs.pfnVoice_SetClientListening)(iReceiver, iSender, true);
		RETURN_META_VALUE(MRES_SUPERCEDE, true);
	}

	if((PlInfo[iReceiver].iSpeakFlags & SPEAK_LISTENALL) != 0) {
		(g_engfuncs.pfnVoice_SetClientListening)(iReceiver, iSender, true);
		RETURN_META_VALUE(MRES_SUPERCEDE, true);
	}

	RETURN_META_VALUE(MRES_IGNORED, bListen);
}

//Added by BAILOPAN.  ClientKill() forward.
void ClientKill(edict_t *pEntity)
{
	cell iRetVal = 0;
	META_RES result = MRES_IGNORED;
	
	for (AmxCallList::AmxCall* i = clientKill.head; i; i = i->next) {
		AMX_EXEC(i->amx, &iRetVal, i->iFunctionIdx, 1, ENTINDEX(pEntity));
		if (iRetVal & 2) {
			RETURN_META(MRES_SUPERCEDE);
		} else if (iRetVal & 1) {
			result = MRES_SUPERCEDE;
		}
	}

	RETURN_META(result);
}

// This allows us to make the player transparent when in third person (but visable to others).
//(vexd)
int AddToFullPack(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet) {
	if(ent == host) {
		if(FStrEq(STRING(ent->v.classname), "player")) {
			if(PlInfo[ENTINDEX(ent)].iViewType != CAMERA_NONE) {
				ent->v.rendermode = kRenderTransTexture;
				ent->v.renderamt = 100;
				RETURN_META_VALUE(MRES_IGNORED, 0);
			}
		}
	}

	if(FStrEq(STRING(ent->v.classname), "player")) {
		if(PlInfo[ENTINDEX(ent)].iViewType != CAMERA_NONE) {
			ent->v.rendermode = PlInfo[ENTINDEX(ent)].iRenderMode;
			ent->v.renderamt = PlInfo[ENTINDEX(ent)].fRenderAmt;
		}
	}

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

// Check if they are using a model, if so, don't let CS know.
// HACKHACK: this might mess up some stuff with other infobuffers,
// ie you may not see when a modeled player changes thier name, etc.
//(vexd)
void ClientUserInfoChanged(edict_t *pEntity, char *infobuffer) {
	if(PlInfo[ENTINDEX(pEntity)].bModeled && pEntity->v.deadflag == DEAD_NO) {
		RETURN_META(MRES_SUPERCEDE);
	} else {
		RETURN_META(MRES_IGNORED);
	}
}

// This code is to set the model at a specified time. the second part of the code updates the
// SetView camera. (vexd)
void PlayerPostThink(edict_t *pEntity) {
	if((PlInfo[ENTINDEX(pEntity)].bModeled) && (PlInfo[ENTINDEX(pEntity)].fModelSet != 0) && (PlInfo[ENTINDEX(pEntity)].fModelSet < gpGlobals->time)) {
		(g_engfuncs.pfnSetClientKeyValue)(ENTINDEX(pEntity), (g_engfuncs.pfnGetInfoKeyBuffer)(pEntity), "model", PlInfo[ENTINDEX(pEntity)].szModel);
	}

	if(PlInfo[ENTINDEX(pEntity)].pViewEnt) {
		edict_t *pCamEnt = PlInfo[ENTINDEX(pEntity)].pViewEnt;

		MAKE_VECTORS(pEntity->v.v_angle + pEntity->v.punchangle);
		Vector vecSrc	 = pEntity->v.origin + pEntity->v.view_ofs;
		Vector vecAiming = gpGlobals->v_forward;
		TraceResult tr;

		switch(PlInfo[ENTINDEX(pEntity)].iViewType) {
			case CAMERA_3RDPERSON:
				TRACE_LINE(vecSrc, vecSrc - (vecAiming * 128), ignore_monsters, ENT(pEntity), &tr);
				SET_VIEW(pEntity, pCamEnt);
				pCamEnt->v.origin = tr.vecEndPos;
				pCamEnt->v.angles = pEntity->v.v_angle;
				break;
			case CAMERA_UPLEFT:
				TRACE_LINE(vecSrc, vecSrc - ((vecAiming * 32) - ((gpGlobals->v_right * 15) + (gpGlobals->v_up * 15))), ignore_monsters, ENT(pEntity), &tr);
				SET_VIEW(pEntity, pCamEnt);
				pCamEnt->v.origin = tr.vecEndPos;
				pCamEnt->v.angles = pEntity->v.v_angle;
				break;
			case CAMERA_TOPDOWN:
				TRACE_LINE(vecSrc, vecSrc + Vector(0,0,2048), dont_ignore_monsters, ENT(pEntity), &tr);
				SET_VIEW(pEntity, pCamEnt);
				pCamEnt->v.origin = tr.vecEndPos;
				pCamEnt->v.origin.z -= 40;
				pCamEnt->v.angles = Vector(90,pEntity->v.v_angle.y,0);
				break;
			default:
				SET_VIEW(pEntity, pEntity);
				REMOVE_ENTITY(PlInfo[ENTINDEX(pEntity)].pViewEnt);
				PlInfo[ENTINDEX(pEntity)].iViewType = CAMERA_NONE;
				PlInfo[ENTINDEX(pEntity)].pViewEnt = NULL;
				break;
		}
	}
	for (AmxCallList::AmxCall* i = postThink.head; i; i = i->next) {
		AMX_EXEC(i->amx, NULL, i->iFunctionIdx, 1, ENTINDEX(pEntity));
	}
	
	RETURN_META(MRES_IGNORED);
}

// This is called once every server frame. This code resets the lights once every second.
// this is so joining players will see ther correct lighting.
// Also forward, may cause lag, but it is good for checking things.
//(vexd)
void StartFrame() {
	if(!FStrEq((const char *)GlInfo.szLastLights, "")) {
		if(GlInfo.fNextLights < gpGlobals->time) {
			(g_engfuncs.pfnLightStyle)(0, (char *)GlInfo.szLastLights);
			GlInfo.fNextLights = gpGlobals->time + 1;
		}
	}

	for (AmxCallList::AmxCall* i = serverFrame.head; i; i = i->next) {
		AMX_EXEC(i->amx, NULL, i->iFunctionIdx, 0);
	}

	RETURN_META(MRES_IGNORED);
}

//(BAILOPAN) - forward this

void PlayerPreThink(edict_t *pEntity) {
	for (AmxCallList::AmxCall* i = preThink.head; i; i = i->next) {
		AMX_EXEC(i->amx, NULL, i->iFunctionIdx, 1, ENTINDEX(pEntity));
	}
	RETURN_META(MRES_IGNORED);
}

// ClientDisconnect. Reinitialize the PlayerInfo struct for that player.
//(vexd)
void ClientDisconnect(edict_t *pEntity) {
	memset(PlInfo[ENTINDEX(pEntity)].szModel, 0x0, sizeof(PlInfo[ENTINDEX(pEntity)].szModel));
	PlInfo[ENTINDEX(pEntity)].bModeled = false;
	PlInfo[ENTINDEX(pEntity)].fModelSet = 0;
	PlInfo[ENTINDEX(pEntity)].iSpeakFlags = SPEAK_NORMAL;
	PlInfo[ENTINDEX(pEntity)].iViewType = CAMERA_NONE;
	PlInfo[ENTINDEX(pEntity)].iRenderMode = 0;
	PlInfo[ENTINDEX(pEntity)].fRenderAmt = 0;

	int iPlayer = ENTINDEX(pEntity);
	if (iPlayer > 0 && iPlayer < 33) {
		is_PlayerOn[iPlayer] = false;
	}

	RETURN_META(MRES_IGNORED);
}


// pfnTouch, this is a forward that is called whenever 2 entities collide.
void Touch(edict_t *pToucher, edict_t *pTouched) {
	cell iResult;
	META_RES result = MRES_IGNORED;
	for (AmxCallList::AmxCall* i = pfnTouch.head; i; i = i->next) {
		AMX_EXEC(i->amx, &iResult, i->iFunctionIdx, 2, ENTINDEX(pToucher), ENTINDEX(pTouched));
		if (iResult & 2) {
			RETURN_META(MRES_SUPERCEDE);
		} else if (iResult & 1) {
			result = MRES_SUPERCEDE;
		}
	}

	RETURN_META(result);
}

// ClientConnect, reinitialize player info here as well.
//(vexd)
BOOL ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]) {
	memset(PlInfo[ENTINDEX(pEntity)].szModel, 0x0, sizeof(PlInfo[ENTINDEX(pEntity)].szModel));
	PlInfo[ENTINDEX(pEntity)].bModeled = false;
	PlInfo[ENTINDEX(pEntity)].fModelSet = 0;
	PlInfo[ENTINDEX(pEntity)].iSpeakFlags = SPEAK_NORMAL;
	PlInfo[ENTINDEX(pEntity)].iViewType = CAMERA_NONE;
	PlInfo[ENTINDEX(pEntity)].pViewEnt = NULL;
	PlInfo[ENTINDEX(pEntity)].iRenderMode = 0;
	PlInfo[ENTINDEX(pEntity)].fRenderAmt = 0;

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

//(vexd)
void GameInit(void) {
	
}

// make sure that if we currently have an edited light value, to use it.
//(vexd)
void LightStyle(int style, char *val) {
	if(style == 0) {
		memset(GlInfo.szRealLights, 0x0, 128);
		memcpy(GlInfo.szRealLights, val, strlen(val));
	}

	RETURN_META(MRES_IGNORED);
}

// Engine message functions. (vexd)
void MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed) {

	// Reset player model a couple milliseconds after this if they are using an edited model.
	if(msg_type == REG_USER_MSG("ResetHUD", 1)) {
		if(PlInfo[ENTINDEX(ed)].bModeled) {
			PlInfo[ENTINDEX(ed)].fModelSet = gpGlobals->time + AMS_OFFSET;
		}
	}

	// If the message is a blocked one, block it.
	if(GlInfo.iMessageBlock[msg_type]) {
		GlInfo.bBlocking = true;

		if(GlInfo.iMessageBlock[msg_type] == BLOCK_ONCE) {
			GlInfo.iMessageBlock[msg_type] = BLOCK_NOT;
		}

		RETURN_META(MRES_SUPERCEDE);
	} else {
		if (isMsgHooked[msg_type] && !inHookProcess) {
			inHookProcess = msg_type;
			msgd = new MessageInfo(msg_dest, msg_type, pOrigin, ed);
			RETURN_META(MRES_SUPERCEDE);
		}
	}

	RETURN_META(MRES_IGNORED);
}

void MessageEnd(void) {
	cell iResult;
	META_RES result = MRES_IGNORED;

	if(GlInfo.bBlocking) {
		GlInfo.bBlocking = false;
		RETURN_META(MRES_SUPERCEDE);
	}

	int msg_type = inHookProcess;

	if (inHookProcess) {
		for (AmxCallList::AmxCall* i = Msgs.head; i; i = i->next) {
			AMX_EXEC(i->amx, &iResult, i->iFunctionIdx, 1, msg_type);
			if (iResult & 2) {
				RETURN_META(MRES_SUPERCEDE);
			} else if (iResult & 1) {
				result = MRES_SUPERCEDE;
			}
		}
		//clear the message
		inHookProcess = 0;
		if (result != MRES_SUPERCEDE) {	//supercede the message ANYWAY
			msgd->SendMsg();
			RETURN_META(MRES_SUPERCEDE);
		}
		delete msgd;
		msgd = NULL;
	}

	RETURN_META(result);
}

void WriteByte(int iValue) {
	if(GlInfo.bBlocking) {
		RETURN_META(MRES_SUPERCEDE);
	}
	int msg_type = inHookProcess;
	if (msg_type && msgd!=NULL) {
		msgd->AddArg(arg_byte, iValue);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void WriteChar(int iValue) {
	if(GlInfo.bBlocking) {
		RETURN_META(MRES_SUPERCEDE);
	}
	int msg_type = inHookProcess;
	if (msg_type && msgd!=NULL) {
		msgd->AddArg(arg_char, iValue);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void WriteShort(int iValue) {
	if(GlInfo.bBlocking) {
		RETURN_META(MRES_SUPERCEDE);
	}
	int msg_type = inHookProcess;
	if (msg_type && msgd!=NULL) {
		msgd->AddArg(arg_short, iValue);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void WriteLong(int iValue) {
	if(GlInfo.bBlocking) {
		RETURN_META(MRES_SUPERCEDE);
	}
	int msg_type = inHookProcess;
	if (msg_type && msgd!=NULL) {
		msgd->AddArg(arg_long, iValue);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void WriteAngle(float flValue) {
	if(GlInfo.bBlocking) {
		RETURN_META(MRES_SUPERCEDE);
	}
	int msg_type = inHookProcess;
	if (msg_type && msgd!=NULL) {
		msgd->AddArg(arg_angle, flValue);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void WriteCoord(float flValue) {
	if(GlInfo.bBlocking) {
		RETURN_META(MRES_SUPERCEDE);
	}
	int msg_type = inHookProcess;
	if (msg_type && msgd!=NULL) {
		msgd->AddArg(arg_coord, flValue);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void WriteString(const char *sz) {
	if(GlInfo.bBlocking) {
		RETURN_META(MRES_SUPERCEDE);
	}
	int msg_type = inHookProcess;
	if (msg_type && msgd!=NULL) {
		msgd->AddArgString(arg_string, sz);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void WriteEntity(int iValue) {
	if(GlInfo.bBlocking) {
		RETURN_META(MRES_SUPERCEDE);
	}
	int msg_type = inHookProcess;
	if (msg_type && msgd!=NULL) {
		msgd->AddArg(arg_entity, iValue);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ){
	PRECACHE_MODEL("models/rpgrocket.mdl");
 	AMX* amx;
	void* code;
	const char* filename;
	int iFunctionIndex;
	int i=0;
	while ((amx = GET_AMXSCRIPT(i++, &code, &filename)) != 0) {
		if (AMX_FINDPUBLIC(amx, "vexd_pfntouch", &iFunctionIndex) == AMX_ERR_NONE) {
			pfnTouch.put(amx, iFunctionIndex);
		}
		if (AMX_FINDPUBLIC(amx, "pfn_touch", &iFunctionIndex) == AMX_ERR_NONE) {
			pfnTouch.put(amx, iFunctionIndex);
		}
		if (AMX_FINDPUBLIC(amx, "server_frame", &iFunctionIndex) == AMX_ERR_NONE) {
			serverFrame.put(amx, iFunctionIndex);
		}
		if (AMX_FINDPUBLIC(amx, "ServerFrame", &iFunctionIndex) == AMX_ERR_NONE) {
			serverFrame.put(amx, iFunctionIndex);
		}
		if (AMX_FINDPUBLIC(amx, "client_PreThink", &iFunctionIndex) == AMX_ERR_NONE) {
			preThink.put(amx, iFunctionIndex);
		}
		if (AMX_FINDPUBLIC(amx, "client_PostThink", &iFunctionIndex) == AMX_ERR_NONE) {
			postThink.put(amx, iFunctionIndex);
		}
		if (AMX_FINDPUBLIC(amx, "client_kill", &iFunctionIndex) == AMX_ERR_NONE) {
			clientKill.put(amx, iFunctionIndex);
		}
	}

	inHookProcess = 0;
	for (i=0; i<MAX_MESSAGES; i++) {
		isMsgHooked[i] = false;
	}

	for (i=0; i<33; i++) {
		is_PlayerOn[i] = false;
	}

	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate() {
	memset(GlInfo.szLastLights, 0x0, 128);
	memset(GlInfo.szRealLights, 0x0, 128);
	GlInfo.bLights = false;
	GlInfo.fNextLights = 0;

	pfnTouch.clear();
	serverFrame.clear();
	postThink.clear();
	preThink.clear();
	clientKill.clear();
	Msgs.clear();

	int i;
	// Reset message blocks.
	for(i = 0; i < MAX_MESSAGES; i++) {
		GlInfo.iMessageBlock[i] = BLOCK_NOT;
		isMsgHooked[i] = false;
	}

	if (msgd != NULL) {
		delete msgd;
		msgd = NULL;
	}

	RETURN_META(MRES_IGNORED);

}

C_DLLEXPORT int Meta_Query(char *ifvers, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs) {

	gpMetaUtilFuncs=pMetaUtilFuncs;
	*pPlugInfo=&Plugin_info;

	if(strcmp(ifvers, Plugin_info.ifvers)) {

		int mmajor=0, mminor=0, pmajor=0, pminor=0;
		LOG_MESSAGE(PLID, "WARNING: meta-interface version mismatch; requested=%s ours=%s", Plugin_info.logtag, ifvers);
		sscanf(ifvers, "%d:%d", &mmajor, &mminor);
		sscanf(META_INTERFACE_VERSION, "%d:%d", &pmajor, &pminor);

		if(pmajor > mmajor || (pmajor==mmajor && pminor > mminor)) {
			LOG_ERROR(PLID, "metamod version is too old for this plugin; update metamod");
			return(FALSE);

		}

		else if(pmajor < mmajor) {
			LOG_ERROR(PLID, "metamod version is incompatible with this plugin; please find a newer version of this plugin");
			return(FALSE);

		}

		else if(pmajor==mmajor && pminor < mminor)
			LOG_MESSAGE(PLID, "WARNING: metamod version is newer than expected; consider finding a newer version of this plugin");
		else
			LOG_ERROR(PLID, "unexpected version comparison; metavers=%s, mmajor=%d, mminor=%d; plugvers=%s, pmajor=%d, pminor=%d", ifvers, mmajor, mminor, META_INTERFACE_VERSION, pmajor, pminor);

	}

	return(TRUE);

}

static META_FUNCTIONS gMetaFunctionTable;

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs) {

	if(now > Plugin_info.loadable) {

		LOG_ERROR(PLID, "Can't load plugin right now");

		return(FALSE);

	}

	gpMetaGlobals=pMGlobals;
	gMetaFunctionTable.pfnGetEntityAPI2 = GetEntityAPI2;
	gMetaFunctionTable.pfnGetEngineFunctions = GetEngineFunctions;

	memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));

	gpGamedllFuncs=pGamedllFuncs;

	return(TRUE);
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {

	if(now > Plugin_info.unloadable && reason != PNL_CMD_FORCED) {
		LOG_ERROR(PLID, "Can't unload plugin right now");
		return(FALSE);

	}

	return(TRUE);

}

#ifdef __linux__
C_DLLEXPORT void GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals ) {
#else
void WINAPI GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals ) {
#endif

	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;

}

void ClientPutInServer_Post( edict_t *pEntity ) {
	int iPlayer = ENTINDEX(pEntity);
	if (iPlayer > 0 && iPlayer < 33) {
		is_PlayerOn[iPlayer] = true;
	}
	RETURN_META(MRES_IGNORED);
}


DLL_FUNCTIONS gFunctionTable;

C_DLLEXPORT int GetEntityAPI2( DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion ){

	gFunctionTable.pfnGameInit = GameInit;
	gFunctionTable.pfnStartFrame = StartFrame;
	gFunctionTable.pfnTouch = Touch;
	gFunctionTable.pfnServerDeactivate = ServerDeactivate;
	gFunctionTable.pfnClientDisconnect = ClientDisconnect;
	gFunctionTable.pfnServerActivate = ServerActivate;
	gFunctionTable.pfnClientConnect = ClientConnect;
	gFunctionTable.pfnClientDisconnect = ClientDisconnect;
	gFunctionTable.pfnPlayerPostThink = PlayerPostThink;
	gFunctionTable.pfnPlayerPreThink = PlayerPreThink;
	gFunctionTable.pfnClientUserInfoChanged = ClientUserInfoChanged;
	gFunctionTable.pfnAddToFullPack = AddToFullPack;
	gFunctionTable.pfnClientKill = ClientKill;

	if(*interfaceVersion!=INTERFACE_VERSION) {
		LOG_ERROR(PLID, "GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);

	}
	memcpy( pFunctionTable, &gFunctionTable, sizeof( DLL_FUNCTIONS ) );

	return(TRUE);

}

enginefuncs_t meta_engfuncs;

C_DLLEXPORT int GetEngineFunctions(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion ) {

	meta_engfuncs.pfnMessageBegin = MessageBegin;
	meta_engfuncs.pfnMessageEnd = MessageEnd;
	meta_engfuncs.pfnWriteByte = WriteByte;
	meta_engfuncs.pfnWriteChar = WriteChar;
	meta_engfuncs.pfnWriteShort = WriteShort;
	meta_engfuncs.pfnWriteLong = WriteLong;
	meta_engfuncs.pfnWriteAngle = WriteAngle;
	meta_engfuncs.pfnWriteCoord = WriteCoord;
	meta_engfuncs.pfnWriteString = WriteString;
	meta_engfuncs.pfnWriteEntity = WriteEntity;
	meta_engfuncs.pfnLightStyle = LightStyle;
	meta_engfuncs.pfnVoice_SetClientListening = Voice_SetClientListening;

	if(*interfaceVersion!=ENGINE_INTERFACE_VERSION) {
		LOG_ERROR(PLID, "GetEngineFunctions version mismatch; requested=%d ours=%d", *interfaceVersion, ENGINE_INTERFACE_VERSION);
		*interfaceVersion = ENGINE_INTERFACE_VERSION;
		return(FALSE);

	}
	memcpy(pengfuncsFromEngine, &meta_engfuncs, sizeof(enginefuncs_t));

	return(TRUE);

}

DLL_FUNCTIONS gFunctionTable_Post;
C_DLLEXPORT int GetEntityAPI2_Post( DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion ) {
  gFunctionTable_Post.pfnClientPutInServer = ClientPutInServer_Post;

  if(*interfaceVersion!=INTERFACE_VERSION) {
    LOG_ERROR(PLID, "GetEntityAPI2_Post version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
    *interfaceVersion = INTERFACE_VERSION;
    return(FALSE);
  }
  memcpy( pFunctionTable, &gFunctionTable_Post, sizeof( DLL_FUNCTIONS ) );
  return(TRUE);
}

C_DLLEXPORT int AMX_Query(module_info_s** info) {

	*info = &module_info;

	return 1;

}

C_DLLEXPORT int AMX_Attach(pfnamx_engine_g* amxeng,pfnmodule_engine_g* meng) {

	g_engAmxFunc = amxeng;
	g_engModuleFunc = meng;

	if (!gpMetaGlobals)
		REPORT_ERROR( 1 , "[CS STATS] Module is not attached to MetaMod\n");

	ADD_AMXNATIVES( &module_info , Engine_Natives);

	return 1;

}



C_DLLEXPORT int AMX_Detach() {

	return 1;

}

AMX_NATIVE_INFO Engine_Natives[] = {
	{"set_offset_float",	set_offset_float},
	{"set_offset_short",	set_offset_short},
	{"set_offset_char",		set_offset_char},
	{"set_offset",			set_offset},
	{"get_offset_float",	get_offset_float},
	{"get_offset_short",	get_offset_short},
	{"get_offset_char",		get_offset_char},
	{"get_offset",			get_offset},
	{"get_entity_pointer",	get_entity_pointer},

	{"entity_get_float",	entity_get_float},
	{"entity_set_float",	entity_set_float},
	{"entity_set_int",		entity_set_int},
	{"entity_get_int",		entity_get_int},
	{"entity_get_vector",	entity_get_vector},
	{"entity_get_string",	entity_get_string},
	{"entity_get_edict",	entity_get_edict},
	{"entity_get_byte",		entity_get_byte},
	{"entity_set_vector",	entity_set_vector},
	{"entity_set_string",	entity_set_string},
	{"entity_set_edict",	entity_set_edict},
	{"entity_set_byte",		entity_set_byte},
	{"entity_set_origin",	entity_set_origin},
	{"entity_set_model",	entity_set_model},

	{"PointContents",		PointContents},
	{"RadiusDamage",		RadiusDamage},
	{"VelocityByAim",		VelocityByAim},
	{"vector_length",		vector_length},
	{"vector_distance",		vector_distance},
	{"vector_to_angle",		vector_to_angle},
	{"trace_line",			trace_line},
	{"trace_normal",		trace_normal},
	{"halflife_time",		halflife_time},
	{"fake_touch",			fake_touch},
	{"get_grenade_id",		get_grenade_id},

	{"create_entity",		create_entity},
	{"remove_entity",		remove_entity},
	{"find_ent_by_class",	find_ent_by_class},
	{"find_ent_by_classname",	find_ent_by_class}, // just in case anyone likes typing the whole thing
	{"find_ent_by_owner",	find_ent_by_owner},
	{"find_ent_by_target",	find_ent_by_target},
	{"find_ent_by_tname",	find_ent_by_tname},
	{"find_ent_by_model",	find_ent_by_model},
	{"find_ent_in_sphere",	find_ent_in_sphere},
	{"find_sphere_class",	find_sphere_class},
	{"entity_count",		entity_count},
	{"DispatchKeyValue",	DispatchKeyValue},
	{"DispatchSpawn",		DispatchSpawn},

	{"set_msg_block",		set_msg_block},
	{"get_msg_block",		get_msg_block},
	{"set_lights",			set_lights},
	{"set_view",			set_view},
	{"attach_view",			attach_view},
	{"set_speak",			set_speak},
	{"get_speak",			get_speak},

	{"precache_generic",	precache_generic},
	{"register_message",	register_message},
	{"set_msg_arg_float",	set_msg_arg_float},
	{"set_msg_arg_int",		set_msg_arg_int},
	{"set_msg_arg_string",	set_msg_arg_string},
	{"get_msg_arg_float",	get_msg_arg_float},
	{"get_msg_arg_int",		get_msg_arg_int},
	{"get_msg_arg_string",	get_msg_arg_string},
	{"get_msg_args",		get_msg_args},
	{"get_msg_argtype",		get_msg_argtype},
	{"is_valid_ent",		is_valid_ent},
	{"drop_to_floor",		drop_to_floor},
	{"get_info_keybuffer",	get_info_keybuffer},
	{"force_use",			force_use},
	{"get_global_float",	get_global_float},
	{"get_global_int",		get_global_int},
	{"get_global_string",	get_global_string},
	{"get_global_edict",	get_global_edict},
	{"get_global_vector",	get_global_vector},
	{"set_size",			set_size},
	{"get_decal_index",		get_decal_index},
	{"precache_event",		precache_event},
	{"call_think",			call_think},

	{ NULL, NULL }

};