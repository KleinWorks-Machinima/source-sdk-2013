//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#pragma once

#include "cbase.h"

#include "ZMQ/srcIPC_EntRec.h"
#include "czmq_baseentity_shared.h"
#include "czmq_pointcamera_shared.h"
#include "czmq_baseskeletal_shared.h"
#include "czmq_events_shared.h"

#include "usermessages.h"
#include "fmtstr.h"

#include <algorithm>
#include <string>


// this is shared so thats why kleinworks/ is specified
#ifdef CLIENT_DLL

#define KW_OUTPUT_PORTNUM 5533
#define KW_INPUT_PORTNUM  5551

#define kleinworks_msg_header "kleinworks_cl"

#include "c_baseplayer.h"
#include "hud_macros.h"

#else

#define KW_OUTPUT_PORTNUM 5577
#define KW_INPUT_PORTNUM  5550

#define kleinworks_msg_header "kleinworks_sv"

#include "player.h"
#include "soundent.h"
#include "soundflags.h"

#endif // CLIENT_DLL







class CzmqManager
{
public:
	CzmqManager();
	~CzmqManager();

	/*======Member-Variables======*/
public:
	std::vector<std::unique_ptr<CzmqBaseEntity>> m_pSelected_EntitiesList;

	std::list<EntRecEvent_t>					 m_ent_events;
	
	
	int	 m_record_until;
	int	 m_start_record_tick;
	int	 m_end_record_tick;
	int  m_last_tick;

	srcIPC::EntRec m_zmq_comms = srcIPC::EntRec(KW_INPUT_PORTNUM, KW_OUTPUT_PORTNUM);

	bool m_record_toggle;

	
	/*======Member-Functions======*/
public:
	void	OnTick();

	void	UpdateSelectedEntities();

	void	RemoveEntityFromSelection(CzmqBaseEntity* pEntity);
	void	RemoveEntityFromSelection(int serialNumber);
	void	AddEntityToSelection(CBaseHandle hEntity);
	void	ClearEntitySelection();

	void	SetRecording(bool value);

	void	HandleSelectedEntityDestroyed(CzmqBaseEntity* pCaller);

	int		AttemptEstablishRecording();


#ifndef CLIENT_DLL
	static void			 OnSoundPlayed(int entindex, const char *soundname, soundlevel_t soundlevel, float flVolume, int iFlags, int iPitch, const Vector *pOrigin, float soundtime, float* soundDuration, CUtlVector< Vector >& soundorigins);
#endif

private:

	rapidjson::Document	 GetEntityMetadata();

	CzmqBaseEntity*		 CreateCzmqEntity(CBaseHandle hEntity);

};

/*
#ifdef CLIENT_DLL

void	RecordedEntityRagdolled(CBaseHandle hParentEntity, CBaseHandle hParentRagdoll);

#endif // CLIENT_DLL
*/

void	OnEntityGibbed(CBaseHandle hParentEntity, CBaseHandle hGib);