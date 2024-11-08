//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
/*

===== tf_client.cpp ========================================================

HL2 client/server game specific stuff

*/

#include "cbase.h"
#include "hl2_player.h"
#include "hl2_gamerules.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "entitylist.h"
#include "physics.h"
#include "game.h"
#include "player_resource.h"
#include "engine/IEngineSound.h"

/*========================*/
/*| KLEINWORKS™ ADDITION |*/


#include "kleinworks/czmq_manager_shared.h"
#include "kleinworks/centrec_interface.h"
#include "kleinworks_cvars.h"


/*| KLEINWORKS™ ADDITION |*/
/*========================*/

#include "tier0/vprof.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void Host_Say(edict_t *pEdict, bool teamonly);

extern CBaseEntity*	FindPickerEntityClass(CBasePlayer *pPlayer, char *classname);
extern bool			g_fGameOver;

/*========================*/
/*| KLEINWORKS™ ADDITION |*/

extern CzmqManager g_CzmqManager;

/*| KLEINWORKS™ ADDITION |*/
/*========================*/


/*
===========
ClientPutInServer

called each time a player is spawned into the game
============
*/
void ClientPutInServer(edict_t *pEdict, const char *playername)
{
	// Allocate a CBasePlayer for pev, and call spawn
	CHL2_Player *pPlayer = CHL2_Player::CreatePlayer("player", pEdict);
	pPlayer->SetPlayerName(playername);

	/*========================*/
	/*| KLEINWORKS™ ADDITION |*/

	// if there isnt an EntRecInterface entity, create one
	CBaseEntity* pEnt = gEntList.FindEntityByClassname(gEntList.FirstEnt(), "entrec_interface");

	if (pEnt == nullptr) {
		Warning("kleinworks_WARNING: No entrec_interface entity was found, creating one now...\n");

		CBaseEntity *pEntrecInterface = dynamic_cast< CBaseEntity * >(CreateEntityByName("entrec_interface"));
		Assert(pEntrecInterface);

		if (DispatchSpawn(pEntrecInterface) == -1)
			Warning("\nkleinworks_ERROR: Failed to create entrec_interface entity!! Use 'ent_create entrec_interface' to spawn one manually.\n");
		else
			pEntrecInterface->Activate();
	}

	/*| KLEINWORKS™ ADDITION |*/
	/*========================*/
}


void ClientActive(edict_t *pEdict, bool bLoadGame)
{
	CHL2_Player *pPlayer = dynamic_cast< CHL2_Player* >(CBaseEntity::Instance(pEdict));
	Assert(pPlayer);

	if (!pPlayer)
	{
		return;
	}

	pPlayer->InitialSpawn();

	if (!bLoadGame)
	{
		pPlayer->Spawn();
	}
}


/*
===============
const char *GetGameDescription()

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
	if (g_pGameRules) // this function may be called before the world has spawned, and the game rules initialized
		return g_pGameRules->GetGameDescription();
	else
		return "Half-Life 2";
}

//-----------------------------------------------------------------------------
// Purpose: Given a player and optional name returns the entity of that 
//			classname that the player is nearest facing
//			
// Input  :
// Output :
//-----------------------------------------------------------------------------
CBaseEntity* FindEntity(edict_t *pEdict, char *classname)
{
	// If no name was given set bits based on the picked
	if (FStrEq(classname, ""))
	{
		return (FindPickerEntityClass(static_cast<CBasePlayer*>(GetContainingEntity(pEdict)), classname));
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Precache game-specific models & sounds
//-----------------------------------------------------------------------------
void ClientGamePrecache(void)
{
	CBaseEntity::PrecacheModel("models/player.mdl");
	CBaseEntity::PrecacheModel("models/gibs/agibs.mdl");
	CBaseEntity::PrecacheModel("models/weapons/v_hands.mdl");

	CBaseEntity::PrecacheScriptSound("HUDQuickInfo.LowAmmo");
	CBaseEntity::PrecacheScriptSound("HUDQuickInfo.LowHealth");

	CBaseEntity::PrecacheScriptSound("FX_AntlionImpact.ShellImpact");
	CBaseEntity::PrecacheScriptSound("Missile.ShotDown");
	CBaseEntity::PrecacheScriptSound("Bullets.DefaultNearmiss");
	CBaseEntity::PrecacheScriptSound("Bullets.GunshipNearmiss");
	CBaseEntity::PrecacheScriptSound("Bullets.StriderNearmiss");

	CBaseEntity::PrecacheScriptSound("Geiger.BeepHigh");
	CBaseEntity::PrecacheScriptSound("Geiger.BeepLow");
}


// called by ClientKill and DeadThink
void respawn(CBaseEntity *pEdict, bool fCopyCorpse)
{
	if (gpGlobals->coop || gpGlobals->deathmatch)
	{
		if (fCopyCorpse)
		{
			// make a copy of the dead body for appearances sake
			((CHL2_Player *)pEdict)->CreateCorpse();
		}

		// respawn player
		pEdict->Spawn();
	}
#ifdef MAPBASE
	else if (g_pGameRules->AllowSPRespawn())
	{
		// In SP respawns, only create corpse if drawing externally
		CBasePlayer *pPlayer = (CBasePlayer*)pEdict;
		if (fCopyCorpse && pPlayer->GetDrawPlayerModelExternally())
		{
			// make a copy of the dead body for appearances sake
			pPlayer->CreateCorpse();
		}

		// respawn player
		pPlayer->Spawn();
	}
#endif
	else
	{       // restart the entire server
		engine->ServerCommand("reload\n");
	}
}

void GameStartFrame(void)
{
	VPROF("GameStartFrame()");
	if (g_fGameOver)
		return;

	gpGlobals->teamplay = (teamplay.GetInt() != 0);
	

	
	/*========================*/
	/*| KLEINWORKS™ ADDITION |*/

	// if we are in the middle of stopping a recording, run anyway
	if (g_CzmqManager.m_zmq_comms.m_isDoneTransfering || g_CzmqManager.m_zmq_comms.m_peerIsDoneTransfering) {

		g_CzmqManager.OnTick();
		return;
	}

	if (engine->IsPaused())
		return;


	if (g_CzmqManager.m_start_record_tick != 0 && gpGlobals->tickcount < g_CzmqManager.m_start_record_tick)
		return;


	if (gpGlobals->tickcount == g_CzmqManager.m_start_record_tick)
		Msg("kleinworks_sv_DEBUG: Start record tick [%d].\n", gpGlobals->tickcount);



	if (g_CzmqManager.m_last_tick < gpGlobals->tickcount) {
		g_CzmqManager.m_last_tick = gpGlobals->tickcount;
		g_CzmqManager.OnTick();
	}

	/*| KLEINWORKS™ ADDITION |*/
	/*========================*/

}

#ifdef HL2_EPISODIC
extern ConVar gamerules_survival;
#endif

//=========================================================
// instantiate the proper game rules object
//=========================================================
void InstallGameRules()
{
#ifdef HL2_EPISODIC
	if (gamerules_survival.GetBool())
	{
		// Survival mode
		CreateGameRulesObject("CHalfLife2Survival");
	}
	else
#endif
	{
		// generic half-life
		CreateGameRulesObject("CHalfLife2");
	}
}

