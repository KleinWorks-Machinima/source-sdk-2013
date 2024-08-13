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

#ifdef CLIENT_DLL
#define kleinworks_msg_header "kleinworks_cl:"

#else
#define kleinworks_msg_header "kleinworks_sv:"

#endif // CLIENT_DLL

extern CzmqManager g_CzmqManager;



void h_RecordUntilCvarChanged(IConVar *var, const char *pOldValue, float flOldValue)
{
	int value = ConVarRef(var).GetInt();
	
	Msg(kleinworks_msg_header, " Set maximum recorded tick from %d to %d!\n", g_CzmqManager.m_RecordUntil, value);
	g_CzmqManager.m_RecordUntil = value;

}

void h_DropoutToleranceChanged(IConVar *var, const char *pOldValue, float flOldValue)
{
	int value = ConVarRef(var).GetInt();

	Msg(kleinworks_msg_header, " Set EntRec message drop-out tolerance from %d to %d!\n", g_CzmqManager.m_zmq_comms.m_drop_out_tolerance, value);
	g_CzmqManager.m_zmq_comms.m_drop_out_tolerance = value;
}

ConVar cvar_kleinworks_record_until(
		   /*pName*/	"kw_entrec_record_until",
   /*pDefaultValue*/	"0",
		   /*flags*/	FCVAR_REPLICATED | FCVAR_HIDDEN,
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
			/*flags*/	FCVAR_REPLICATED | FCVAR_HIDDEN,
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

	g_CzmqManager.SetRecording(recordBool);
	cvar_kleinworks_record.SetValue(recordBool);
}





CON_COMMAND(kw_entrec_reset_sockets, "Disconnects then reconnects all sockets, aborting any active/hanging data transfers.")
{
	Msg(kleinworks_msg_header, " Reconnecting sockets...\n");

	g_CzmqManager.m_zmq_comms.DisconnectSockets();

	zclock_sleep(200);

	g_CzmqManager.m_zmq_comms.ConnectSockets(5555, 5556);

	Msg(kleinworks_msg_header, " Reconnected all sockets!\n");
}





extern CBaseEntity *FindEntityForward(CBasePlayer *pMe, bool fHull);

CON_COMMAND(kw_entrec_select, "Select an entity for recording by name. If no arguments are passed, selects any entity under the Player's crosshair.")
{

#ifdef CLIENT_DLL
	CBasePlayer* pPlayer = ToBasePlayer(cl_entitylist->GetBaseEntity(engine->GetLocalPlayer()));
#else
	CBasePlayer* pPlayer = gEntList.GetBaseEntity(hEntity);
#endif // CLIENT_DLL

	CBaseEntity* pEntityToSelect = nullptr;

	if (args.ArgC() <= 1 || args.Arg(1) == NULL)
	{
		if (FindEntityForward(pPlayer, true) != nullptr)
		{
			pEntityToSelect = FindEntityForward(pPlayer, true);
			g_CzmqManager.AddEntityToSelection(pEntityToSelect->GetRefEHandle());

			return;
		}
		Msg(kleinworks_msg_header, " Found no entities under Player crosshair to add to EntRec selection.\n");
		return;
	}
	
#ifdef CLIENT_DLL
	return;

#else
	pEntityToSelect = gEntList.FindEntityByName(NULL, args.Arg(1));

	if (pEntityToSelect != NULL) {
		g_CzmqManager.AddEntityToSelection(pEntityToSelect->GetRefEHandle());

		return;
	}

	Msg(kleinworks_msg_header, " Found no entities named [%s] to add to EntRec selection.\n", args.Arg(1));

#endif // CLIENT_DLL
}





CON_COMMAND(kw_entrec_print, "Prints the ID's of every entity currently selected for EntRec recording.")
{
	Msg(kleinworks_msg_header, " Printing all entities from EntRec selection...\n\n");

	int size = int(g_CzmqManager.m_pSelected_EntitiesList.size());

	for (int i = 0; i < size; i++) {
		CzmqBaseEntity* pEnt = g_CzmqManager.m_pSelected_EntitiesList[i].get();

		const char* pEntName = pEnt->m_ent_name;

		Msg("[%d] # - : name:\"%s\"\n", i, pEntName);
	}
	Msg("\nKleinWorks: Printed all entites from EntRec selection!\n");
}




CON_COMMAND(kw_entrec_deselect, "Removes an entity from EntRec selection by ID.")
{

#ifdef CLIENT_DLL
	CBasePlayer* pPlayer = ToBasePlayer(cl_entitylist->GetBaseEntity(engine->GetLocalPlayer()));
#else
	CBasePlayer* pPlayer = gEntList.GetBaseEntity(hEntity);
#endif // CLIENT_DLL

	CBaseEntity* pEntityToDeselect = nullptr;

	const char* entToDeselectName = nullptr;

	// if no arguments are passed, try to deselect any entities under Player's crosshair from selection
	if (args.ArgC() <= 1 || args.Arg(1) == NULL)
	{
		if (FindEntityForward(pPlayer, true) == nullptr) {
			Msg(kleinworks_msg_header, " Found no entities under Player crosshair to remove from EntRec selection.\n");
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
				Msg(kleinworks_msg_header, " No entity at index [%d] in selected entities list.\n", entIndex);
				return;
			}

			CzmqBaseEntity* pEnt = g_CzmqManager.m_pSelected_EntitiesList[entIndex].get();
			g_CzmqManager.RemoveEntityFromSelection(pEnt);

			return;
		}

		entToDeselectName = args.Arg(1);
	}


	// try to find the entity to deselect by name
	for (int i = 0; i < int(g_CzmqManager.m_pSelected_EntitiesList.size()); i++) {
		CzmqBaseEntity* pEnt = g_CzmqManager.m_pSelected_EntitiesList[i].get();

		DevMsg(4, "KleinWorks_DEBUG: Checking ent #[%d]: ent_name - [%s] for match with arg [%s].\n", i, pEnt->m_ent_name, entToDeselectName);

		if (strcmp(pEnt->m_ent_name, entToDeselectName) == 0) {
			g_CzmqManager.RemoveEntityFromSelection(pEnt);
			return;
		}
	}

	Msg(kleinworks_msg_header, " Found no entities named [%s] to remove from EntRec selection.\n", args.Arg(1));
}



CON_COMMAND(kw_entrec_deselect_all, "Removes every entity from EntRec selection.")
{
	Msg(kleinworks_msg_header, " Attemping to clear EntRec entity selection...\n");

	g_CzmqManager.ClearEntitySelection();

	Msg(kleinworks_msg_header, " Cleared EntRec entity selection.\n");
}



CON_COMMAND(kw_dbg_skel, "Debug command")
{

#ifdef CLIENT_DLL
	CBasePlayer* pPlayer = ToBasePlayer(cl_entitylist->GetBaseEntity(engine->GetLocalPlayer()));
#else
	CBasePlayer* pPlayer = gEntList.GetBaseEntity(hEntity);
#endif // CLIENT_DLL


	CBaseEntity* pEntity = nullptr;

	if (FindEntityForward(pPlayer, true) != nullptr)
	{
		pEntity = FindEntityForward(pPlayer, true);
	}

	if (pEntity->GetBaseAnimating() != nullptr) {


		CzmqBaseSkeletal newSkelEnt = pEntity->GetRefEHandle();

		Msg(kleinworks_msg_header, " Finished.\n");

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