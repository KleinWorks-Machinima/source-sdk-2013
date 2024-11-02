//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#pragma once


#include "cbase.h"

#include <sstream>
#include <unordered_map>

#include "mathlib/mathlib.h"
#include "bone_setup.h"

#include "kleinworks/czmq_manager_shared.h"
#include "kleinworks/kleinworks_usermessages.h"


extern CzmqManager g_CzmqManager;






void h_RecordUntilCvarChanged(IConVar *var, const char *pOldValue, float flOldValue)
{
	int value = ConVarRef(var).GetInt();
	
	Msg("kleinworks_sv: Set maximum recorded tick from %d to %d!\n", g_CzmqManager.m_record_until, value);
	g_CzmqManager.m_record_until = value;

	Send_EntRecRecordUntil_Msg(value);

}

void h_DropoutToleranceChanged(IConVar *var, const char *pOldValue, float flOldValue)
{
	int value = ConVarRef(var).GetInt();

	Msg("kleinworks_sv: Set EntRec message drop-out tolerance from %d to %d!\n", g_CzmqManager.m_zmq_comms.m_drop_out_tolerance, value);
	g_CzmqManager.m_zmq_comms.m_drop_out_tolerance = value;

	Send_EntRecDropoutTolerance_Msg(value);

}

ConVar cvar_kleinworks_record_until(
		   /*pName*/	"kw_entrec_record_until",
   /*pDefaultValue*/	"0",
		   /*flags*/	FCVAR_REPLICATED,
	 /*pHelpString*/	"If set to a number higher than 0, will automatically stop recordings that reach the number specified. Set to 0 or below to disable.",
			/*bMin*/	true,
			/*fMin*/	0,
			/*bMax*/	false,
			/*fMax*/	0,
		/*callback*/	&h_RecordUntilCvarChanged
);

ConVar cvar_kleinworks_dropout_tolerance(
			/*pName*/	"kw_entrec_message_dropout_tolerance",
	/*pDefaultValue*/	"-1",
			/*flags*/	FCVAR_REPLICATED,
	  /*pHelpString*/	"Adjusts how many messages without response it takes before EntRec declares a drop out and aborts data transfer. If set to -1, EntRec will ignore drop-outs. Set to -1 by default.",
			 /*bMin*/	true,
			 /*fMin*/	-1,
			 /*bMax*/	false,
			 /*fMax*/	0,
		 /*callback*/	&h_DropoutToleranceChanged
);

ConVar cvar_kleinworks_record(
			/*pName*/	"kw_entrec_message_record",
	/*pDefaultValue*/	"0",
			/*flags*/	FCVAR_REPLICATED | FCVAR_HIDDEN,
	  /*pHelpString*/	"",
			 /*bMin*/	true,
			 /*fMin*/	0,
			 /*bMax*/	true,
			 /*fMax*/	1
);

ConVar cvar_kleinworks_record_delay_factor(
			/*pName*/	"kw_entrec_record_delay_factor",
	/*pDefaultValue*/	"5",
			/*flags*/	FCVAR_REPLICATED,
	  /*pHelpString*/	"For synchronizing the Server and Client when a recording starts/ends, EntRec delays starting or ending a recording by x number of ticks.",
			 /*bMin*/	true,
			 /*fMin*/	0,
			 /*bMax*/	false,
			 /*fMax*/	0
);

ConVar cvar_kleinworks_record_sounds(
			/*pName*/	"kw_entrec_record_sounds",
	/*pDefaultValue*/	"0",
			/*flags*/	FCVAR_REPLICATED,
	  /*pHelpString*/	"If set to 1, EntRec will record any sound entities that are created during a recording.",
			 /*bMin*/	true,
			 /*fMin*/	0,
			 /*bMax*/	true,
			 /*fMax*/	1
);




CON_COMMAND(kw_entrec_record, "Toggle for the KleinWorks EntRec system, 1 = recording, 0 = not recording.")
{
	bool recordBool = cvar_kleinworks_record.GetBool();

	// If no arguments are passed, act like a toggle
	if (args.ArgC() <= 1 || args.Arg(1) == NULL)
		recordBool = !recordBool;

	else {

		std::string argStr = args.Arg(1);

		// make sure every character in Arg(1) is a digit
		if (std::all_of(argStr.begin(), argStr.end(), isdigit) != true)
			return;

		int argValue = stoi(argStr);

		if (argValue >= 1)
			recordBool = true;
		else
			recordBool = false;
	}

	if (engine->IsPaused() == true)
		Msg("kleinworks: Recording will %s when game is unpaused.\n", recordBool == true ? "start" : "stop");


	cvar_kleinworks_record.SetValue(recordBool);
}





CON_COMMAND(kw_entrec_reset_sockets, "Disconnects then reconnects all sockets, aborting any active/hanging data transfers.")
{
	Msg("kleinworks_sv: Reconnecting sockets...\n");

	g_CzmqManager.m_zmq_comms.DisconnectSockets();

	zclock_sleep(200);

	g_CzmqManager.m_zmq_comms.ConnectSockets(KW_INPUT_PORTNUM, KW_OUTPUT_PORTNUM);

	Msg("kleinworks_sv: Reconnected all sockets!\n");

	Send_EntRecResetSockets_Msg();

}





CON_COMMAND(kw_entrec_print, "Prints the ID's of every entity currently selected for EntRec recording.")
{
	Msg("kleinworks_sv: Printing all entities from EntRec selection...\n\n");

	int size = int(g_CzmqManager.m_pSelected_EntitiesList.size());

	for (int i = 0; i < size; i++) {
		CzmqBaseEntity* pEnt = g_CzmqManager.m_pSelected_EntitiesList[i].get();

		const char* pEntName = pEnt->m_ent_name;

		Msg("[%d] # - : name:\"%s\"\n", i, pEntName);
	}
	Msg("\nkleinworks_sv: Printed all entites from EntRec selection!\n");

	Send_EntRecPrint_Msg();

}





extern CBaseEntity *FindEntityForward(CBasePlayer *pMe, bool fHull);

CON_COMMAND(kw_entrec_select, "Select an entity for recording by name. If no arguments are passed, selects any entity under the Player's crosshair.")
{

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	CBaseEntity* pEntityToSelect = nullptr;

	if (args.ArgC() <= 1 || args.Arg(1) == NULL)
	{
		Send_AddEntForwardToRec_Msg();

		if (FindEntityForward(pPlayer, true) != nullptr)
		{
			pEntityToSelect = FindEntityForward(pPlayer, true);

			CBaseHandle hEntity = pEntityToSelect->GetRefEHandle();

			g_CzmqManager.AddEntityToSelection(hEntity);

			return;
		}
		Msg("kleinworks_sv: Found no entities under Player crosshair to add to EntRec selection.\n");
		return;
	}
	pEntityToSelect = gEntList.FindEntityByName(NULL, args.Arg(1));

	if (pEntityToSelect != NULL) {

		CBaseHandle hEntity = pEntityToSelect->GetRefEHandle();

		g_CzmqManager.AddEntityToSelection(hEntity);

		Send_AddEntToRec_Msg(hEntity.GetEntryIndex(), hEntity.GetSerialNumber());

		return;
	}

	Msg("kleinworks_sv: Found no entities named [%s] to add to EntRec selection.\n", args.Arg(1));
}




CON_COMMAND(kw_entrec_deselect, "Removes an entity from EntRec selection by ID.")
{

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	CBaseEntity* pEntityToDeselect = nullptr;

	const char* entToDeselectName = nullptr;

	// if no arguments are passed, try to deselect any entities under Player's crosshair from selection
	if (args.ArgC() <= 1 || args.Arg(1) == NULL)
	{
		Send_RemoveEntForwardFromRec_Msg();

		if (FindEntityForward(pPlayer, true) == nullptr) {
			Msg("kleinworks_sv: Found no entities under Player crosshair to remove from EntRec selection.\n");
			return;
		}

		pEntityToDeselect = FindEntityForward(pPlayer, true);

		entToDeselectName = pEntityToDeselect->GetDebugName();
	}
	else {
		std::string argStr = args.Arg(1);
		// if every character in Arg(1) is a digit, try to delete entity at that index
		if (std::all_of(argStr.begin(), argStr.end(), isdigit) == true) {
			int entIndex = stoi(argStr);

			if (int(g_CzmqManager.m_pSelected_EntitiesList.size()) <= entIndex) {
				Msg("kleinworks_sv: No entity at index [%d] in selected entities list.\n", entIndex);
				return;
			}

			CzmqBaseEntity* pEnt = g_CzmqManager.m_pSelected_EntitiesList[entIndex].get();

			Send_RemoveEntFromRec_Msg(pEnt->m_ent_id);

			g_CzmqManager.RemoveEntityFromSelection(pEnt);

			return;
		}

		entToDeselectName = args.Arg(1);
	}


	// try to find the entity to deselect by name
	for (int i = 0; i < int(g_CzmqManager.m_pSelected_EntitiesList.size()); i++) {
		CzmqBaseEntity* pEnt = g_CzmqManager.m_pSelected_EntitiesList[i].get();

		DevMsg(4, "kleinworks_sv_DEBUG: Checking ent #[%d]: ent_name - [%s] for match with arg [%s].\n", i, pEnt->m_ent_name, entToDeselectName);

		if (strcmp(pEnt->m_ent_name, entToDeselectName) == 0) {

			g_CzmqManager.RemoveEntityFromSelection(pEnt);
			return;
		}
	}

	Msg("kleinworks_sv: Found no entities named [%s] to remove from EntRec selection.\n", args.Arg(1));
}



CON_COMMAND(kw_entrec_deselect_all, "Removes every entity from EntRec selection.")
{
	Msg("kleinworks_sv: Attemping to clear EntRec entity selection...\n");

	g_CzmqManager.ClearEntitySelection();

	Msg("kleinworks_sv: Cleared EntRec entity selection.\n");

	Send_EntRecClearSelection_Msg();

}




CON_COMMAND(kw_dbg_skel, "Debug command")
{

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();



	CBaseEntity* pEntity = nullptr;

	if (FindEntityForward(pPlayer, true) != nullptr)
	{
		pEntity = FindEntityForward(pPlayer, true);
	}

	if (pEntity->GetBaseAnimating() != nullptr) {


		CzmqBaseSkeletal newSkelEnt = pEntity->GetRefEHandle();

		Msg("kleinworks_sv: Finished.\n");

		/*
		CBaseAnimating* pAnimating = pEntity->GetBaseAnimating();
		CStudioHdr* pModelPtr = nullptr;
		mstudiobone_t* pStudioBone = nullptr;

		pModelPtr   = pAnimating->GetModelPtr();
		pStudioBone = pModelPtr->pBone(1);

		matrix3x4_t* boneToWorld;
		CBoneCache* pBoneCache;

		pBoneCache = pAnimating->GetBoneCache();

		boneToWorld = pBoneCache->GetCachedBone(1);

		int iParent = pModelPtr->boneParent(1);

		matrix3x4_t worldToBone;
		MatrixInvert(boneToWorld[iParent], worldToBone);

		matrix3x4_t local;
		ConcatTransforms(worldToBone, boneToWorld[1], local);


		Vector bonePosition;
		MatrixPosition(local, bonePosition);

		QAngle boneAngles;
		MatrixToAngles(local, boneAngles);

		float posX = bonePosition.x;
		float posY = bonePosition.y;
		float posZ = bonePosition.z;

		float rotX = boneAngles.x;
		float rotY = boneAngles.y;
		float rotZ = boneAngles.z;


		Msg(kleinworks_msg_header, " Got base animating, printing details:\n");
		//Msg("GetBoneTransform(1) pszName:[%s]\n", pStudioBone->pszName());
		//Msg("GetBoneTransform(1) parent pszName:[%s]\n", pModelPtr->pBone(pStudioBone->parent)->pszName());
		Msg("GetBoneTransform(1) Pos:[%f, %f, %f]\n", posX, posY, posZ);
		Msg("GetBoneTransform(1) Rot:[%f, %f, %f]\n", rotX, rotY, rotZ);

		*/


	}
}
