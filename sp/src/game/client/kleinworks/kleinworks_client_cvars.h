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

#include "czmq_manager.h"


extern CzmqManager g_CzmqManager;



void HandleRecordUntilCvarChanged(IConVar *var, const char *pOldValue, float flOldValue)
{
	int value = ConVarRef(var).GetInt();

	Msg("KleinWorks: Set maximum recorded tick from %d to %d!\n", g_CzmqManager.m_RecordUntil, value);
	g_CzmqManager.m_RecordUntil = value;

}

ConVar cvar_kleinworks_record_rate("kw_entrec_record_until", "0", 0, "If set to a number higher than 0, will automatically stop recordings that reach the number specified. Set to 0 or below to disable.", true, 0, false, 0, &HandleRecordUntilCvarChanged);




void HandleDropoutToleranceChanged(IConVar *var, const char *pOldValue, float flOldValue)
{
	int value = ConVarRef(var).GetInt();

	Msg("KleinWorks: Set EntRec message drop-out tolerance from %d to %d!\n", g_CzmqManager.m_zmq_comms.m_drop_out_tolerance, value);
	g_CzmqManager.m_zmq_comms.m_drop_out_tolerance = value;
}

ConVar cvar_kleinworks_dropout_tolerance("kw_entrec_message_dropout_tolerance", "-1", 0, "Adjusts how many messages without response it takes before EntRec declares that it's peer dropped out and abort data transfer. If set to -1, EntRec will ignore peer drop-outs. Set to -1 by default.", true, -1, false, 0, &HandleDropoutToleranceChanged);





CON_COMMAND(kw_entrec_record, "Toggle for the KleinWorks EntRec system, 1 = recording, 0 = not recording.")
{
	bool recordBool = g_CzmqManager.record_toggle;

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

	// if record value is unchanged, do nothing
	if (recordBool == g_CzmqManager.record_toggle)
		return;


	if (recordBool == true)
	{

		if (g_CzmqManager.m_zmq_comms.m_isDoneTransfering != false) {
			Warning("KleinWorks: ERROR! Previous recording hasn't finished, unable to start new recording! Wait a bit or reset sockets and try again.\n");
			g_CzmqManager.record_toggle = false;
			return;
		}
		Msg("KleinWorks: Attempting to start recording...\n");


		// updating the entity metadata JSON object before starting to record
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		g_CzmqManager.m_entity_metadata_js.Accept(writer);


		DevMsg(3, "Kleinworks_DEBUG: Metadata Message: %s\n", buffer.GetString());

		const char* pEntMetaDataStr = buffer.GetString();

		// Allocate memory for the destination pointers
		char* proxy_sending_metadata = new char[strlen(pEntMetaDataStr) + 1];


		// Copy the strings using std::strcpy
		strcpy_s(proxy_sending_metadata, strlen(pEntMetaDataStr) + 1, pEntMetaDataStr);


		g_CzmqManager.m_zmq_comms.m_sending_metadata = proxy_sending_metadata;



		g_CzmqManager.UpdateSelectedEntities();

		// start transfering the data
		g_CzmqManager.m_zmq_comms.TransferData();

		g_CzmqManager.record_toggle = true;

		g_CzmqManager.record_frame_start = 0;
		g_CzmqManager.record_frame_end = 0;
	}



	if (recordBool == false)
	{
		g_CzmqManager.record_toggle = false;

		if (g_CzmqManager.m_zmq_comms.m_isDoneTransfering != false)
			return;

		Msg("KleinWorks: Ending recording...\n");
		Msg("KleinWorks: Recorded from %d to %d\n", 0, g_CzmqManager.m_zmq_comms.m_OUTPUT_tick_count);

		if (g_CzmqManager.m_zmq_comms.m_isSendingOutput != true)
			return;
		g_CzmqManager.m_zmq_comms.m_isDoneTransfering = true;
	}
}





CON_COMMAND(kw_entrec_reset_sockets, "Disconnects then reconnects all sockets, aborting any active/hanging data transfers.")
{
	Msg("KleinWorks: Reconnecting sockets...\n");

	g_CzmqManager.m_zmq_comms.DisconnectSockets();

	zclock_sleep(200);

	g_CzmqManager.m_zmq_comms.ConnectSockets(5555, 5556);

	Msg("KleinWorks: Reconnected all sockets!\n");
}





extern CBaseEntity *FindEntityForward(CBasePlayer *pMe, bool fHull);

CON_COMMAND(kw_entrec_select, "Select an entity for recording by name. If no arguments are passed, selects any entity under the Player's crosshair.")
{
	CBasePlayer* pPlayer = UTIL_GetCommandClient();

	CBaseEntity* pEntityToSelect = nullptr;

	if (args.ArgC() <= 1 || args.Arg(1) == NULL) 
	{
		if (FindEntityForward(pPlayer, true) != nullptr)
		{
			pEntityToSelect = FindEntityForward(pPlayer, true);
			g_CzmqManager.AddEntityToSelection(pEntityToSelect->GetRefEHandle());

			return;
		}
		Msg("KleinWorks: Found no entities under Player crosshair to add to EntRec selection.\n");
		return;
	}
	
	pEntityToSelect = gEntList.FindEntityByName(NULL, args.Arg(1));

	if (pEntityToSelect != NULL) {
		g_CzmqManager.AddEntityToSelection(pEntityToSelect->GetRefEHandle());

		return;
	}

	Msg("KleinWorks: Found no entities named [%s] to add to EntRec selection.\n", args.Arg(1));
	
}





CON_COMMAND(kw_entrec_print, "Prints the ID's of every entity currently selected for EntRec recording.")
{
	Msg("KleinWorks: Printing all entities from EntRec selection...\n\n");

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
	CBasePlayer* pPlayer = UTIL_GetCommandClient();

	CBaseEntity* pEntityToDeselect = nullptr;

	const char* entToDeselectName = nullptr;

	// if no arguments are passed, try to deselect any entities under Player's crosshair from selection
	if (args.ArgC() <= 1 || args.Arg(1) == NULL)
	{
		if (FindEntityForward(pPlayer, true) == nullptr) {
			Msg("KleinWorks: Found no entities under Player crosshair to remove from EntRec selection.\n");
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
				Msg("KleinWorks: No entity at index [%d] in selected entities list.\n", entIndex);
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

	Msg("KleinWorks: Found no entities named [%s] to remove from EntRec selection.\n", args.Arg(1));
}



CON_COMMAND(kw_entrec_deselect_all, "Removes every entity from EntRec selection.")
{
	Msg("KleinWorks: Attemping to clear EntRec entity selection...\n");

	g_CzmqManager.ClearEntitySelection();

	Msg("KleinWorks: Cleared EntRec entity selection.\n");
}



CON_COMMAND(kw_dbg_skel, "Debug command")
{

	CBasePlayer* pPlayer = UTIL_GetCommandClient();

	CBaseEntity* pEntity = nullptr;

	if (FindEntityForward(pPlayer, true) != nullptr)
	{
		pEntity = FindEntityForward(pPlayer, true);
	}

	if (pEntity->GetBaseAnimating() != nullptr) {


		CzmqBaseSkeletal newSkelEnt = pEntity->GetRefEHandle();

		Msg("KleinWorks: Finished.\n");

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
		

		Msg("KleinWorks: Got base animating, printing details:\n");
		//Msg("GetBoneTransform(1) pszName:[%s]\n", pStudioBone->pszName());
		//Msg("GetBoneTransform(1) parent pszName:[%s]\n", pModelPtr->pBone(pStudioBone->parent)->pszName());
		Msg("GetBoneTransform(1) Pos:[%f, %f, %f]\n", posX, posY, posZ);
		Msg("GetBoneTransform(1) Rot:[%f, %f, %f]\n", rotX, rotY, rotZ);

		*/
		

	}
}