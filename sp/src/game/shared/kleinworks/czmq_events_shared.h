//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"

#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4003)

#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#pragma warning(pop)

#include "czmq_baseentity_shared.h"

#include <list>


const enum class ENTREC_EVENT : int
{
	ENT_CREATED    = 0xE01,
	ENT_BROKEN     = 0xE02,
	ENT_DELETED    = 0xE03,

	EFFECT_CREATED = 0xE04,

	SOUND_CREATED  = 0xE05,
};




struct EntRecEvent_t
{
	int				event_type;
	int				ent_id;
	int				tick_count;
	CzmqBaseEntity* p_entity = nullptr;

	int					  sound_volume;
	int					  sound_pitch;
	float				  sound_time;
	float				  sound_duration;
	char*			      sound_name;
	Vector				  sound_origin;
	std::list<Vector>     sound_origins;


	static EntRecEvent_t CreateEntCreatedEvent(CzmqBaseEntity* pCreatedEnt, int tickCount);
	static EntRecEvent_t CreateEntDeletedEvent(CzmqBaseEntity* pDeletedEnt, int tickCount);

	rapidjson::Value ParseEntEvent(   rapidjson::MemoryPoolAllocator<> &allocator);
	rapidjson::Value ParseSoundEvent( rapidjson::MemoryPoolAllocator<> &allocator);
	rapidjson::Value ParseEffectEvent(rapidjson::MemoryPoolAllocator<> &allocator);

	rapidjson::Value ParseEventByType(rapidjson::MemoryPoolAllocator<> &allocator);
};