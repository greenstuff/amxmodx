/*
 * TFCX 
 * Copyright (c) 2004 Lukasz Wlasinski
 *
 *
 *    This program is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#ifndef RANK_H
#define RANK_H

#include "amxxmodule.h"
#include "CMisc.h"
#include "CRank.h"

#define RANK_SVERSION "0.1"

#define GET_PLAYER_POINTER(e)   (&players[ENTINDEX(e)])
#define GET_PLAYER_POINTER_I(i) (&players[i])

#ifndef GETPLAYERAUTHID
#define GETPLAYERAUTHID   (*g_engfuncs.pfnGetPlayerAuthId)
#endif


extern AMX_NATIVE_INFO stats_Natives[];
extern AMX_NATIVE_INFO base_Natives[];

extern weaponsVault weaponData[MAX_WEAPONS];
extern traceVault traceData[MAX_TRACE];

typedef void (*funEventCall)(void*);
extern funEventCall modMsgsEnd[MAX_REG_MSGS];
extern funEventCall modMsgs[MAX_REG_MSGS];
extern void (*function)(void*);
extern void (*endfunction)(void*);

extern bool rankBots;

extern bool ignoreDamage;

extern int pdTimerOwner;
extern int pdSentryGunOwner;
extern int pdAmmo[6];

extern cvar_t *tfcstats_maxsize;
extern cvar_t* tfcstats_rank;
extern cvar_t* tfcstats_reset;

extern cvar_t* tfcstats_rankbots;
extern cvar_t* tfcstats_pause;

extern Grenades g_grenades;

extern RankSystem g_rank;
extern CPlayer players[33];
extern CPlayer* mPlayer;
extern int mPlayerIndex;
extern int mState;

extern int iFDamage;
extern int iFDeath;
extern int iFGrenade;

//extern int gmsgCurWeapon;
//extern int gmsgDamage;
//extern int gmsgWeaponList;
//extern int gmsgResetHUD;
//extern int gmsgAmmoX;
//extern int gmsgScoreInfo;
//extern int gmsgAmmoPickup;

void Client_AmmoX(void*);
void Client_CurWeapon(void*);
void Client_Damage(void*);
void Client_Damage_End(void*);
//void Client_WeaponList(void*);
void Client_AmmoPickup(void*);
void Client_ScoreInfo(void*);
void Client_ResetHUD(void*);

bool ignoreBots (edict_t *pEnt, edict_t *pOther = NULL );
bool isModuleActive();

#endif // TFCX_H

