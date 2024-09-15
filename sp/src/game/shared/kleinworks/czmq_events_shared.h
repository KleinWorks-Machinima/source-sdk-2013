//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"

#include "czmq_baseentity_shared.h"







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