//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"
#include "czmq_events_shared.h"




rapidjson::Value  EntRecEvent_t::ParseEntEvent(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value ent_event_js;

	ent_event_js.AddMember("event_type", event_type, allocator);
	ent_event_js.AddMember("ent_id", ent_id, allocator);
	if (p_entity != nullptr)
		ent_event_js.AddMember("ent_metadata", p_entity->GetEntityMetaData(allocator), allocator);

	return ent_event_js;
}



rapidjson::Value  EntRecEvent_t::ParseEffectEvent(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value effect_event_js;

	return effect_event_js;
}



rapidjson::Value  EntRecEvent_t::ParseSoundEvent(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value sound_event_js;

	sound_event_js.AddMember("event_type", event_type, allocator);
	sound_event_js.AddMember("ent_id", ent_id, allocator);

	sound_event_js.AddMember("sound_name", rapidjson::StringRef(sound_name), allocator);
	sound_event_js.AddMember("sound_volume", sound_volume, allocator);
	sound_event_js.AddMember("sound_pitch", sound_pitch, allocator);
	sound_event_js.AddMember("sound_time", sound_time, allocator);


	rapidjson::Value sound_origins_js(rapidjson::kObjectType);

	for (auto& vec : sound_origins) {
		rapidjson::Value vec_js(rapidjson::kObjectType);

		vec_js.AddMember("x", vec.x, allocator);
		vec_js.AddMember("y", vec.y, allocator);
		vec_js.AddMember("z", vec.z, allocator);

		sound_origins_js.AddMember("", vec_js, allocator);
	}

	sound_event_js.AddMember("sound_origins", sound_origins_js, allocator);


	return sound_event_js;
}



rapidjson::Value  EntRecEvent_t::ParseEventByType(rapidjson::MemoryPoolAllocator<> &allocator)
{
	if (event_type == ENTREC_EVENT::ENT_CREATED)
		return ParseEntEvent(allocator);

	if (event_type == ENTREC_EVENT::ENT_BROKEN)
		return ParseEntEvent(allocator);

	if (event_type == ENTREC_EVENT::ENT_DELETED)
		return ParseEntEvent(allocator);



	if (event_type == ENTREC_EVENT::EFFECT_CREATED)
		return ParseEffectEvent(allocator);



	if (event_type == ENTREC_EVENT::SOUND_CREATED)
		return ParseSoundEvent(allocator);


	// if none of the above, default to ParseEntEvent
	return ParseEntEvent(allocator);
}