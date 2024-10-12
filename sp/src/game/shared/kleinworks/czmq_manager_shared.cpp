//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"
#include "czmq_manager_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"




CzmqManager::CzmqManager()
{

	m_record_until		= 0;

	m_record_toggle		= false;

	m_start_record_tick = 0;
	m_end_record_tick   = 0;
	m_last_tick			= 0;
	
	m_zmq_comms.m_drop_out_tolerance = -1;
	m_zmq_comms.m_pollerTimeout      = 0;

	DevMsg(3, "%s: CzmqManager instance initialized.\n", kleinworks_msg_header);

}

CzmqManager::~CzmqManager()
{
	delete &m_zmq_comms;

}




void CzmqManager::OnTick()
{

	// First, check if we are trying to record
	if (m_record_toggle != 0) {

		// If we have hit our m_record_until cap, stop recording. If m_record_until is negative then ignore it
		if (m_zmq_comms.m_OUTPUT_tick_count >= m_record_until && m_record_until >= 1) {

			SetRecording(false);

			if (m_zmq_comms.m_isSendingOutput == true)
				m_zmq_comms.m_isDoneTransfering = true;

			return;
		}


		// If we are currently recording, update entities
		if (m_zmq_comms.m_isSendingOutput == true) {
			if (m_zmq_comms.m_OUTPUT_tick_count == 1)
				Msg("%s: Connection established! Recording started!\n", kleinworks_msg_header);
			UpdateSelectedEntities();
		}



		m_zmq_comms.RunFuncLoop();


	}
	// If we aren't trying to record, check if we are trying to end a data transfer
	else if (m_zmq_comms.m_isDoneTransfering || m_zmq_comms.m_peerIsDoneTransfering) {
		m_zmq_comms.RunFuncLoop();
	}




}




void CzmqManager::SetRecording(bool recordBool)
{

	// if record value is unchanged, do nothing
	if (recordBool == m_record_toggle)
		return;


	if (recordBool == true)
	{

		if (m_zmq_comms.m_isDoneTransfering != false) {
			Warning("%s: ERROR! Previous recording hasn't finished, unable to start new recording! Wait a bit or reset sockets and try again.\n", kleinworks_msg_header);
			m_record_toggle = false;
			return;
		}
		Msg("%s: Attempting to start recording...\n", kleinworks_msg_header);


		// updating the entity metadata before starting to record
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		rapidjson::Document metadata_js = GetEntityMetadata();

		metadata_js.Accept(writer);


		DevMsg(3, "%s_DEBUG: Metadata Message: %s\n", kleinworks_msg_header, buffer.GetString());

		const char* pEntMetaDataStr = buffer.GetString();

		// Allocate memory for the destination pointers
		char* proxy_sending_metadata = new char[strlen(pEntMetaDataStr) + 1];


		// Copy the strings using std::strcpy
		strcpy_s(proxy_sending_metadata, strlen(pEntMetaDataStr) + 1, pEntMetaDataStr);


		m_zmq_comms.m_sending_metadata = proxy_sending_metadata;



		UpdateSelectedEntities();

		// start transfering the data
		m_zmq_comms.TransferData();

		m_record_toggle = true;
	}



	if (recordBool == false)
	{
		m_record_toggle = false;

		if (m_zmq_comms.m_isDoneTransfering != false)
			return;

		Msg("%s: Ending recording...\n", kleinworks_msg_header);
		Msg("%s: Recorded from %d to %d\n", kleinworks_msg_header, 0, m_zmq_comms.m_OUTPUT_tick_count);

		if (m_zmq_comms.m_isSendingOutput != true)
			return;
		m_zmq_comms.m_isDoneTransfering = true;

		m_ent_events.clear();
	}
}




int CzmqManager::AttemptEstablishRecording()
{

	if (m_record_toggle == true) {

		if (m_zmq_comms.m_isSendingOutput == true)
			return 0;

		m_zmq_comms.RunFuncLoop();
		return -1;
	}
	else {
		return 0;
	}
}







void CzmqManager::UpdateSelectedEntities()
{
	// create a document representing all of our selected entity's data
	rapidjson::Document entityData_js;
	entityData_js.SetObject();

	rapidjson::MemoryPoolAllocator<> &allocator = entityData_js.GetAllocator();

	entityData_js.AddMember("ent_data", rapidjson::kObjectType, allocator);


	
	// iterate through each entity we have selected
	for (auto iter = m_pSelected_EntitiesList.begin(); iter != m_pSelected_EntitiesList.end(); iter++)
	{
		CzmqBaseEntity* element = iter->get();

		if (!element)
		{ // if a selected entity doesnt exist, print an error message
			Warning("%s: Something went wrong! At line %d in CzmqManager.cpp, element was a nullptr! Skipping this element!\n", kleinworks_msg_header, __LINE__ - 3);
			continue;
		}
		if (!element->IsValid())
		{// if a selected czmq entity's parent entity isn't valid, report and remove it
			DevMsg(4, "%s_DEBUG: Recorded entity with name: [%s] ID: [%d] no longer valid. Destroying...\n", kleinworks_msg_header, element->m_ent_name, element->m_ent_id);
			RemoveEntityFromSelection(element);
			continue;
		}

		char* ent_id_str = new char[strlen(std::to_string(element->m_ent_id).c_str()) + 1];

		strcpy_s(ent_id_str, strlen(std::to_string(element->m_ent_id).c_str()) + 1, std::to_string(element->m_ent_id).c_str());
		
		entityData_js["ent_data"].AddMember(rapidjson::StringRef(ent_id_str), element->GetEntityData(allocator), allocator);

	}

	// if there are any entity events, send them
	if (!m_ent_events.empty()) {
		entityData_js.AddMember("ent_events", rapidjson::kObjectType, allocator);

		for (auto ent_event : m_ent_events) 
			entityData_js["ent_events"].AddMember("", ent_event.ParseEventByType(allocator), allocator);

		// clear the list after parsing all events
		m_ent_events.clear();
	}

	// convert the entity data document into a string
	rapidjson::StringBuffer entDataStrBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(entDataStrBuffer);
	entityData_js.Accept(writer);


	// set the engine tickcount
	m_zmq_comms.m_ENGINE_tick_count = gpGlobals->tickcount;
	
	// add the stringified entity data document to the current data buffer
	m_zmq_comms.m_sending_data_buffer = zframe_new(entDataStrBuffer.GetString(), strlen(entDataStrBuffer.GetString()));
}





void CzmqManager::AddEntityToSelection(CBaseHandle hEntity)
{
	if (!hEntity.IsValid()) {
		Warning("%s: ERROR! Something tried to select an invalid entity for recording. Either the entity doesn't exist or it's a Hammer entity.\n", kleinworks_msg_header);
		return;
	}

#ifdef CLIENT_DLL
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntity(hEntity.GetEntryIndex());
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);

	if (pEntity->IsClient()) {
		Warning("%s: ERROR! Server attempted to select Client-side only entity! Aborting...\n", kleinworks_msg_header);
		return;
	}

#endif // CLIENT_DLL


	CzmqBaseEntity* zmqEntity = nullptr;
	
	for (auto& element : m_pSelected_EntitiesList)
	{
		if (*element.get() == hEntity) {
			Msg("%s: Entity [%s] is already in EntRec selection!\n", kleinworks_msg_header, pEntity->GetDebugName());
			return;
		}
	}
	

	Msg("%s: adding entity with name [%s] to EntRec selection...\n", kleinworks_msg_header, pEntity->GetDebugName());

	rapidjson::Value entMetaData_js;

	zmqEntity = CreateCzmqEntity(hEntity);

	ENTREC_TYPES zmqType = static_cast<ENTREC_TYPES>(zmqEntity->m_ent_type);
	
	if (zmqType == ENTREC_TYPES::BASE_ENTITY) {

		m_pSelected_EntitiesList.emplace_back(std::make_unique<CzmqBaseEntity>(std::move(*zmqEntity)));

	}
	else if (zmqType == ENTREC_TYPES::BASE_SKELETAL) {

		m_pSelected_EntitiesList.emplace_back(std::make_unique<CzmqBaseSkeletal>(*dynamic_cast<CzmqBaseSkeletal*>(zmqEntity)));

	}
	else if (zmqType == ENTREC_TYPES::POINT_CAMERA) {

		m_pSelected_EntitiesList.emplace_back(std::make_unique<CzmqPointCamera>(*dynamic_cast<CzmqPointCamera*>(zmqEntity)));

	}

	__hook(&CzmqBaseEntity::OnParentEntityDestroyed, zmqEntity, &CzmqManager::HandleSelectedEntityDestroyed);

	if (m_record_toggle)
		m_ent_events.push_back(EntRecEvent_t::CreateEntCreatedEvent(zmqEntity, gpGlobals->tickcount));

}



void CzmqManager::RemoveEntityFromSelection(CzmqBaseEntity* pEntity)
{
	
	

	for (auto it = m_pSelected_EntitiesList.begin(); it != m_pSelected_EntitiesList.end(); it++) {

		if (pEntity->m_ent_id != it->get()->m_ent_id)
			continue;


		Msg("%s: Removed entity [%s] from EntRec selection.\n", kleinworks_msg_header, pEntity->m_ent_name);

		if (m_record_toggle)
			m_ent_events.push_back(EntRecEvent_t::CreateEntDeletedEvent(pEntity, gpGlobals->tickcount));

		__unhook(&CzmqBaseEntity::OnParentEntityDestroyed, pEntity, &CzmqManager::HandleSelectedEntityDestroyed);

		it->reset();

		m_pSelected_EntitiesList.erase(it);

		return;
	}
}



void CzmqManager::RemoveEntityFromSelection(int serialNumber)
{



	for (auto it = m_pSelected_EntitiesList.begin(); it != m_pSelected_EntitiesList.end(); it++) {

		if (it->get()->m_ent_id != serialNumber)
			continue;

		CzmqBaseEntity* pEntity = it->get();


		Msg("%s: Removed entity [%s] from EntRec selection.\n", kleinworks_msg_header, pEntity->m_ent_name);

		if (m_record_toggle)
			m_ent_events.push_back(EntRecEvent_t::CreateEntDeletedEvent(pEntity, gpGlobals->tickcount));

		__unhook(&CzmqBaseEntity::OnParentEntityDestroyed, pEntity, &CzmqManager::HandleSelectedEntityDestroyed);

		it->reset();

		m_pSelected_EntitiesList.erase(it);

		return;
	}
}





void CzmqManager::HandleSelectedEntityDestroyed(CzmqBaseEntity* pCaller)
{
	DevMsg(4, "%s_DEBUG: Destruction of entity [%s] detected! Removing entity from EntRec selection...\n", kleinworks_msg_header, pCaller->m_ent_name);
	__unhook(&CzmqBaseEntity::OnParentEntityDestroyed, pCaller, &CzmqManager::HandleSelectedEntityDestroyed);
	RemoveEntityFromSelection(pCaller);
	return;
}





void CzmqManager::ClearEntitySelection()
{
	DevMsg(4, "%s_DEBUG: [%d]\n", kleinworks_msg_header, int(m_pSelected_EntitiesList.size()));
	

	int size = int(m_pSelected_EntitiesList.size());

	for (int i = 0; i != size; i++) {

		DevMsg(4, "%s_DEBUG: [%d]\n", kleinworks_msg_header, i);

		auto it = m_pSelected_EntitiesList.begin();

		CzmqBaseEntity* pEnt = it->get();

		if (m_record_toggle)
			m_ent_events.push_back(EntRecEvent_t::CreateEntDeletedEvent(pEnt, gpGlobals->tickcount));


		__unhook(&CzmqBaseEntity::OnParentEntityDestroyed, pEnt, &CzmqManager::HandleSelectedEntityDestroyed);

		pEnt = NULL;

		it->reset();

		m_pSelected_EntitiesList.erase(it);
	}

}




rapidjson::Document	CzmqManager::GetEntityMetadata()
{
	rapidjson::Document entity_metadata_js;
	entity_metadata_js.SetObject();

	rapidjson::MemoryPoolAllocator<> &allocator = entity_metadata_js.GetAllocator();

	rapidjson::Value ent_metadata(rapidjson::kArrayType);

	ent_metadata.SetObject();
	

	for (auto& ent : m_pSelected_EntitiesList) {

		// avoid taking a reference to the ID string by copying it
		int	  id_strlen = strlen(CFmtStr("%d", ent->m_ent_id)) + 1;

		char* ent_id = new char[id_strlen];
		strcpy_s(ent_id, id_strlen, CFmtStr("%d", ent->m_ent_id).String());

		ent_metadata.AddMember(rapidjson::StringRef(ent_id), ent->GetEntityMetaData(allocator), allocator);
	}





	entity_metadata_js.AddMember("ent_metadata", ent_metadata, allocator);

	return entity_metadata_js;
}




CzmqBaseEntity*	CzmqManager::CreateCzmqEntity(CBaseHandle hEntity)
{

#ifdef CLIENT_DLL
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntity(hEntity.GetEntryIndex());
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);

#endif // CLIENT_DLL

	CzmqBaseEntity* zmqEntity;

	std::string className = pEntity->GetClassname();;

	// this is just a massive annoying list of every possible classname


	// client-side entities have 'class ' at the beginning for some reason
	if (className.substr(0, 6) == "class ") {
		className = className.substr(6);
	}


	/*| Base Entities |*/
	if (className.substr(0, 5) == "item_") {
		zmqEntity = new CzmqBaseEntity(hEntity);

		return zmqEntity;
	}


	/*| Base Skeletals |*/

	if (className.substr(0, 4) == "npc_") {
		zmqEntity = new CzmqBaseSkeletal(hEntity);

		return zmqEntity;
	}
	// client-side NPCs are named differently
	if (className.substr(0, 5) == "C_AI_") {
		zmqEntity = new CzmqBaseSkeletal(hEntity);

		return zmqEntity;
	}

	if (className.find("ragdoll") != std::string::npos || className.find("Ragdoll") != std::string::npos) {
		zmqEntity = new CzmqBaseSkeletal(hEntity, true);


		return zmqEntity;
	}

	if (className == "viewmodel") {
		zmqEntity = new CzmqBaseSkeletal(hEntity);

		return zmqEntity;
	}

	/*| Point Cameras |*/

	if (className == "player") {
		zmqEntity = new CzmqPointCamera(hEntity);

		return zmqEntity;
	}	

	if (className == "point_camera") {
		zmqEntity = new CzmqPointCamera(hEntity);

		return zmqEntity;
	}


	// if entity's classname doesnt match any of the above, just make it a CzmqBaseEntity

	zmqEntity = new CzmqBaseEntity(hEntity);

	return zmqEntity;

}




/*|"Initialization of global CzmqManager instances../"|*/
#ifdef CLIENT_DLL

CzmqManager g_C_zmqManager = CzmqManager();

#define g_zmqManager g_C_zmqManager

void RecordedEntityRagdolled(CBaseHandle hParentEntity, CBaseHandle hParentRagdoll)
{
	int serialNumber = hParentEntity.GetSerialNumber();
	Msg("RecordedEntityRagdolled Called.\n");

	g_zmqManager.RemoveEntityFromSelection(serialNumber);

	g_zmqManager.AddEntityToSelection(hParentRagdoll);
}


#else

CzmqManager g_CzmqManager = CzmqManager();




void CzmqManager::OnSoundPlayed(int entindex, const char *soundname, soundlevel_t soundlevel, float flVolume, int iFlags, int iPitch, const Vector *pOrigin, float soundtime, float* soundDuration, CUtlVector< Vector >& soundorigins)
{
	if (!cvar->FindVar("kw_entrec_record_sounds")->GetBool())
		return;

	if (!g_CzmqManager.m_record_toggle)
		return;

	EntRecEvent_t soundEvent;

	soundEvent.ent_id	  = entindex;
	soundEvent.event_type = static_cast<int>(ENTREC_EVENT::SOUND_CREATED);
	soundEvent.tick_count = gpGlobals->tickcount;


	soundEvent.sound_volume   = flVolume;
	soundEvent.sound_pitch    = iPitch;
	soundEvent.sound_time     = soundtime;
	if (soundDuration != nullptr)
		soundEvent.sound_duration = TIME_TO_TICKS(*soundDuration);
	else
		soundEvent.sound_duration = -1;

	Msg("%s_DEBUG: OnSoundPlayed triggered, sound name = [%s] with duration = [%d].\n", kleinworks_msg_header,  soundname, soundEvent.sound_duration);

	if (pOrigin != nullptr) {
		soundEvent.sound_origin.x = pOrigin->x;
		soundEvent.sound_origin.y = pOrigin->y;
		soundEvent.sound_origin.z = pOrigin->z;
	}
	else {
		soundEvent.sound_origin.x = 0;
		soundEvent.sound_origin.y = 0;
		soundEvent.sound_origin.z = 0;
	}

	for (auto& vec : soundorigins)
		soundEvent.sound_origins.push_back(vec);

	soundEvent.sound_name	 = new char[strlen(soundname) + 1];
	strcpy_s(soundEvent.sound_name, strlen(soundname) + 1, soundname);


	g_CzmqManager.m_ent_events.push_back(soundEvent);
}


#define g_zmqManager g_CzmqManager



#endif // CLIENT_DLL

void OnEntityGibbed(CBaseHandle hParentEntity, CBaseHandle hGib)
{
	for (auto it = g_zmqManager.m_pSelected_EntitiesList.begin(); it != g_zmqManager.m_pSelected_EntitiesList.end(); it++) {

		if (it->get()->mh_parent_entity != hParentEntity)
			continue;

		g_zmqManager.AddEntityToSelection(hGib);

		return;
	}
}



