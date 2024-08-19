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

#include "fmtstr.h"

#include <algorithm>


// this is shared so thats why kleinworks/ is specified
#ifdef CLIENT_DLL

#define KW_OUTPUT_PORTNUM 5533
#define KW_INPUT_PORTNUM  5551

#include "c_baseplayer.h"

#else

#define KW_OUTPUT_PORTNUM 5577
#define KW_INPUT_PORTNUM  5550

#include "player.h"

#endif // CLIENT_DLL





struct EntEvent_t
{
	int				event_type;
	int				ent_id;
	CzmqBaseEntity* p_entity = nullptr;



	rapidjson::Value ParseEvent(rapidjson::MemoryPoolAllocator<> &allocator)
	{
		rapidjson::Value ent_event_js;


		ent_event_js.AddMember("event_type", event_type, allocator);
		ent_event_js.AddMember("ent_id", ent_id, allocator);
		if (p_entity != nullptr)
			ent_event_js.AddMember("ent_metadata", p_entity->GetEntityMetaData(allocator), allocator);

		return ent_event_js;
	}

};







class CzmqManager
{
public:
	CzmqManager();
	~CzmqManager();

	/*======Member-Variables======*/
public:
	std::vector<std::unique_ptr<CzmqBaseEntity>> m_pSelected_EntitiesList;

	std::list<EntEvent_t>						 m_ent_events;
	
	
	int	 m_RecordUntil;
	bool record_toggle;
	int	 record_frame_start;
	int	 record_frame_end;

	
	srcIPC::EntRec m_zmq_comms = srcIPC::EntRec(KW_INPUT_PORTNUM, KW_OUTPUT_PORTNUM);


	
	/*======Member-Functions======*/
public:
	void    OnTick();

	void	UpdateSelectedEntities();

	void	RemoveEntityFromSelection(CzmqBaseEntity* pEntity);
	void	AddEntityToSelection(CBaseHandle hEntity);


	void	ClearEntitySelection();

	void    SetRecording(bool value);

	void	HandleSelectedEntityDestroyed(CzmqBaseEntity* pCaller);

private:

	rapidjson::Document		GetEntityMetadata();

};