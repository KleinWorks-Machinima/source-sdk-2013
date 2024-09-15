//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"

#include "usermessages.h"

#include "czmq_manager_shared.h"

#ifdef CLIENT_DLL

#include "hud_macros.h"
#include "c_baseentity.h"

extern CzmqManager g_C_zmqManager;

#endif // CLIENT_DLL



#ifdef CLIENT_DLL

void __MsgFunc_AddEntToRec(bf_read &msg);

void __MsgFunc_AddEntForwardToRec(bf_read &msg);

void __MsgFunc_RemoveEntForwardFromRec(bf_read &msg);

void __MsgFunc_RemoveEntFromRec(bf_read &msg);

void __MsgFunc_EntRecToggled(bf_read &msg);

void __MsgFunc_EntRecResetSockets(bf_read &msg);

void __MsgFunc_EntRecPrint(bf_read &msg);

void __MsgFunc_EntRecRecordUntil(bf_read &msg);

void __MsgFunc_EntRecDropoutTolerance(bf_read &msg);

void __MsgFunc_EntRecClearSelection(bf_read &msg);


void HookEntRecUserMessages();

#endif // CLIENT_DLL

void RegisterEntRecUserMessages();


void Send_AddEntToRec_Msg(int entry_num, int serial_num);

void Send_AddEntForwardToRec_Msg();

void Send_RemoveEntFromRec_Msg(int serial_num);

void Send_RemoveEntForwardFromRec_Msg();

void Send_EntRecToggled_Msg();

void Send_EntRecResetSockets_Msg();

void Send_EntRecPrint_Msg();

void Send_EntRecRecordUntil_Msg(int value);

void Send_EntRecDropoutTolerance_Msg(int value);

void Send_EntRecClearSelection_Msg();