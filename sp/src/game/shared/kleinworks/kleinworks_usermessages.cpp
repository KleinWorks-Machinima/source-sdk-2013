//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"
#include "kleinworks_usermessages.h"


#ifdef CLIENT_DLL



void __MsgFunc_AddEntToRec(bf_read &msg)
{

	int entry_num  = msg.ReadByte();
	int serial_num = msg.ReadByte();

	g_C_zmqManager.AddEntityToSelection(CBaseHandle(entry_num, serial_num));

}

extern CBaseEntity* FindEntityInFrontOfLocalPlayer();

void __MsgFunc_AddEntForwardToRec(bf_read &msg)
{
	CBaseEntity* pEntityToSelect = nullptr;

	if (FindEntityInFrontOfLocalPlayer() != nullptr)
	{
		pEntityToSelect = FindEntityInFrontOfLocalPlayer();

		CBaseHandle hEntity = pEntityToSelect->GetRefEHandle();

		g_C_zmqManager.AddEntityToSelection(hEntity);

		return;
	}

	Msg("kleinworks_cl: Found no entities under Player crosshair to add to EntRec selection.\n");

}

void __MsgFunc_RemoveEntFromRec(bf_read &msg)
{

	int serial_num = msg.ReadByte();

	g_C_zmqManager.RemoveEntityFromSelection(serial_num);

}

void __MsgFunc_RemoveEntForwardFromRec(bf_read &msg)
{

	CBaseEntity* pEntityToDeselect = nullptr;

	const char*  entToDeselectName;

	if (FindEntityInFrontOfLocalPlayer() == nullptr) {
		Msg("kleinworks_cl: Found no entities under Player crosshair to remove from EntRec selection.\n");
		return;
	}

	pEntityToDeselect = FindEntityInFrontOfLocalPlayer();

	entToDeselectName = pEntityToDeselect->GetDebugName();


	// try to find the entity to deselect by name
	for (int i = 0; i < int(g_C_zmqManager.m_pSelected_EntitiesList.size()); i++) {
		CzmqBaseEntity* pEnt = g_C_zmqManager.m_pSelected_EntitiesList[i].get();

		DevMsg(4, "kleinworks_cl_DEBUG: Checking ent #[%d]: ent_name - [%s] for match with arg [%s].\n", i, pEnt->m_ent_name, entToDeselectName);

		if (strcmp(pEnt->m_ent_name, entToDeselectName) == 0) {

			g_C_zmqManager.RemoveEntityFromSelection(pEnt);
			return;
		}
	}

	Msg("kleinworks_cl: Found no entities named [%s] to remove from EntRec selection.\n", entToDeselectName);

}

void __MsgFunc_EntRecToggled(bf_read &msg)
{

	bool recordBool = ConVarRef("kw_entrec_message_record").GetBool();

	g_C_zmqManager.SetRecording(recordBool);

}

void __MsgFunc_EntRecResetSockets(bf_read &msg)
{

	Msg("kleinworks_cl: Reconnecting sockets...\n");

	g_C_zmqManager.m_zmq_comms.DisconnectSockets();

	zclock_sleep(200);

	g_C_zmqManager.m_zmq_comms.ConnectSockets(KW_INPUT_PORTNUM, KW_OUTPUT_PORTNUM);

	Msg("kleinworks_cl: Reconnected all sockets!\n");

}

void __MsgFunc_EntRecPrint(bf_read &msg)
{

	Msg("kleinworks_cl: Printing all entities from EntRec selection...\n\n");

	int size = int(g_C_zmqManager.m_pSelected_EntitiesList.size());

	for (int i = 0; i < size; i++) {
		CzmqBaseEntity* pEnt = g_C_zmqManager.m_pSelected_EntitiesList[i].get();

		const char* pEntName = pEnt->m_ent_name;

		Msg("[%d] # - : name:\"%s\"\n", i, pEntName);
	}
	Msg("\nkleinworks_cl: Printed all entites from EntRec selection!\n");

}

void __MsgFunc_EntRecRecordUntil(bf_read &msg)
{

	int rec_until_val = msg.ReadByte();
	g_C_zmqManager.m_record_until = rec_until_val;

}

void __MsgFunc_EntRecDropoutTolerance(bf_read &msg)
{

	int dropout_tolerance_val = msg.ReadByte();
	g_C_zmqManager.m_zmq_comms.m_drop_out_tolerance = dropout_tolerance_val;

}

void __MsgFunc_EntRecClearSelection(bf_read &msg)
{

	Msg("kleinworks_cl: Attemping to clear EntRec entity selection...\n");

	g_C_zmqManager.ClearEntitySelection();

	Msg("kleinworks_cl: Cleared EntRec entity selection.\n");

}



void HookEntRecUserMessages()
{

	HOOK_MESSAGE(AddEntToRec);
	HOOK_MESSAGE(AddEntForwardToRec);
	HOOK_MESSAGE(RemoveEntFromRec);
	HOOK_MESSAGE(RemoveEntForwardFromRec);
	HOOK_MESSAGE(EntRecToggled);
	HOOK_MESSAGE(EntRecResetSockets);
	HOOK_MESSAGE(EntRecPrint);
	HOOK_MESSAGE(EntRecRecordUntil);
	HOOK_MESSAGE(EntRecDropoutTolerance);
	HOOK_MESSAGE(EntRecClearSelection);

}

#endif // CLIENT_DLL


void RegisterEntRecUserMessages()
{

	usermessages->Register("AddEntToRec", 2);
	usermessages->Register("AddEntForwardToRec", -1);
	usermessages->Register("RemoveEntFromRec", 1);
	usermessages->Register("RemoveEntForwardFromRec", -1);
	usermessages->Register("EntRecToggled", -1);
	usermessages->Register("EntRecResetSockets", -1);
	usermessages->Register("EntRecPrint", -1);
	usermessages->Register("EntRecRecordUntil", 1);
	usermessages->Register("EntRecDropoutTolerance", 1);
	usermessages->Register("EntRecClearSelection", -1);

}


#ifndef CLIENT_DLL

void Send_AddEntToRec_Msg(int entry_num, int serial_num)
{

	CReliableBroadcastRecipientFilter filter;
	
	filter.AddAllPlayers();

	UserMessageBegin(filter, "AddEntToRec");
	WRITE_BYTE(entry_num);
	WRITE_BYTE(serial_num);
	MessageEnd();

}

void Send_AddEntForwardToRec_Msg()
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "AddEntForwardToRec");
	MessageEnd();

}

void Send_RemoveEntFromRec_Msg(int serial_num)
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "RemoveEntFromRec");
	WRITE_BYTE(serial_num);
	MessageEnd();

}

void Send_RemoveEntForwardFromRec_Msg()
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "RemoveEntForwardFromRec");
	MessageEnd();

}

void Send_EntRecToggled_Msg()
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "EntRecToggled");
	MessageEnd();

}

void Send_EntRecResetSockets_Msg()
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "EntRecResetSockets");
	MessageEnd();

}

void Send_EntRecPrint_Msg()
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "EntRecPrint");
	MessageEnd();

}

void Send_EntRecRecordUntil_Msg(int value)
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "EntRecRecordUntil");
	WRITE_BYTE(value);
	MessageEnd();

}

void Send_EntRecDropoutTolerance_Msg(int value)
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "EntRecDropoutTolerance");
	WRITE_BYTE(value);
	MessageEnd();

}

void Send_EntRecClearSelection_Msg()
{

	CReliableBroadcastRecipientFilter filter;

	filter.AddAllPlayers();

	UserMessageBegin(filter, "EntRecClearSelection");
	MessageEnd();

}

#endif // CLIENT_DLL