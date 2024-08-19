//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"
#include "czmq_manager_shared.h"

#ifdef CLIENT_DLL
#define kleinworks_msg_header "kleinworks_cl"

#else
#define kleinworks_msg_header "kleinworks_sv"

#endif // CLIENT_DLL






CzmqManager::CzmqManager()
{

	m_RecordUntil		= 0;

	record_toggle		= false;

	record_frame_start  = 0;
	record_frame_end	= 0;
	
	m_zmq_comms.m_drop_out_tolerance = -1;
	m_zmq_comms.m_pollerTimeout      = 0;

	DevMsg(3, kleinworks_msg_header ": CzmqManager instance initialized.\n");

}

CzmqManager::~CzmqManager()
{
	delete &m_zmq_comms;

}




void CzmqManager::OnTick()
{


	// First, check if we are trying to record
	if (record_toggle != 0) {

		// If we have hit our m_RecordUntil cap, stop recording. If m_RecordUntil is negative then ignore it
		if (m_zmq_comms.m_OUTPUT_tick_count >= m_RecordUntil && m_RecordUntil >= 1) {

			SetRecording(false);

			if (m_zmq_comms.m_isSendingOutput == true)
				m_zmq_comms.m_isDoneTransfering = true;

			return;
		}


		// If we are currently recording, update entities
		if (m_zmq_comms.m_isSendingOutput == true) {
			if (m_zmq_comms.m_OUTPUT_tick_count == 1)
				Msg(kleinworks_msg_header  ": Connection established! Recording started!\n");
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
	if (recordBool == record_toggle)
		return;


	if (recordBool == true)
	{

		if (m_zmq_comms.m_isDoneTransfering != false) {
			Warning("KleinWorks: ERROR! Previous recording hasn't finished, unable to start new recording! Wait a bit or reset sockets and try again.\n");
			record_toggle = false;
			return;
		}
		Msg(kleinworks_msg_header  ": Attempting to start recording...\n");


		// updating the entity metadata before starting to record
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		rapidjson::Document metadata_js = GetEntityMetadata();

		metadata_js.Accept(writer);


		DevMsg(3, kleinworks_msg_header  "_DEBUG: Metadata Message: %s\n", buffer.GetString());

		const char* pEntMetaDataStr = buffer.GetString();

		// Allocate memory for the destination pointers
		char* proxy_sending_metadata = new char[strlen(pEntMetaDataStr) + 1];


		// Copy the strings using std::strcpy
		strcpy_s(proxy_sending_metadata, strlen(pEntMetaDataStr) + 1, pEntMetaDataStr);


		m_zmq_comms.m_sending_metadata = proxy_sending_metadata;



		UpdateSelectedEntities();

		// start transfering the data
		m_zmq_comms.TransferData();

		record_toggle = true;

		record_frame_start = 0;
		record_frame_end = 0;
	}



	if (recordBool == false)
	{
		record_toggle = false;

		if (m_zmq_comms.m_isDoneTransfering != false)
			return;

		Msg(kleinworks_msg_header  ": Ending recording...\n");
		Msg(kleinworks_msg_header  ": Recorded from %d to %d\n", 0, m_zmq_comms.m_OUTPUT_tick_count);

		if (m_zmq_comms.m_isSendingOutput != true)
			return;
		m_zmq_comms.m_isDoneTransfering = true;
	}
}







void CzmqManager::UpdateSelectedEntities()
{
	// create a document representing all of our selected entity's data
	rapidjson::Document entityData_js;
	entityData_js.SetObject();

	rapidjson::MemoryPoolAllocator<> &allocator = entityData_js.GetAllocator();

	entityData_js.AddMember("ent_data", rapidjson::kArrayType, allocator);


	
	// iterate through each entity we have selected
	for (auto& element : m_pSelected_EntitiesList)
	{

		if (!element)
		{ // if a selected entity doesnt exist, print an error message
			Warning("KleinWorks: Something went wrong! At line %d in CzmqManager.cpp, element was a nullptr! Skipping this element!\n", __LINE__ - 3);
			continue;
		}

		
		
		entityData_js["ent_data"].PushBack(element->GetEntityData(allocator), allocator);

	}

	// if there are any entity events, send them
	if (!m_ent_events.empty()) {
		entityData_js.AddMember("ent_events", rapidjson::kArrayType, allocator);

		for (auto& ent_event : m_ent_events) {
			entityData_js["ent_events"].PushBack(ent_event.ParseEvent(allocator), allocator);
		}
	}

	// convert the entity data document into a string
	rapidjson::StringBuffer entDataStrBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(entDataStrBuffer);
	entityData_js.Accept(writer);
	
	// add the stringified entity data document to the current data buffer
	m_zmq_comms.m_sending_data_buffer = zframe_new(entDataStrBuffer.GetString(), strlen(entDataStrBuffer.GetString()));
}





void CzmqManager::AddEntityToSelection(CBaseHandle hEntity)
{
	if (!hEntity.IsValid()) {
		Warning("KleinWorks: ERROR! Something tried to select an invalid entity for recording. Either the entity doesn't exist or it's a Hammer entity.\n");
		return;
	}

#ifdef CLIENT_DLL
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntity(hEntity.GetEntryIndex());
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);
#endif // CLIENT_DLL


	CzmqBaseEntity* zmqEntity = nullptr;
	
	for (auto& element : m_pSelected_EntitiesList)
	{
		if (*element.get() == hEntity) {
			Msg(kleinworks_msg_header  ": Entity [%s] is already in EntRec selection!\n", pEntity->GetDebugName());
			return;
		}
	}
	

	Msg(kleinworks_msg_header  ": adding entity with name [%s] to EntRec selection...\n", pEntity->GetDebugName());

	rapidjson::Value entMetaData_js;
	
	if (strcmp( pEntity->GetClassname(), "player") == 0) {
		CzmqPointCamera* pPointCam = new CzmqPointCamera(hEntity);
		zmqEntity = pPointCam;

		m_pSelected_EntitiesList.emplace_back(std::make_unique<CzmqPointCamera>(*pPointCam));

	}
	else if (strcmp(pEntity->GetClassname(), "npc_metropolice") == 0) {
		CzmqBaseSkeletal* pBaseSkel = new CzmqBaseSkeletal(hEntity);
		zmqEntity = pBaseSkel;

		m_pSelected_EntitiesList.emplace_back(std::make_unique<CzmqBaseSkeletal>(*pBaseSkel));

	}
	else {
		zmqEntity = new CzmqBaseEntity(hEntity);

		m_pSelected_EntitiesList.emplace_back(std::make_unique<CzmqBaseEntity>(std::move(*zmqEntity)));

	}

	__hook(&CzmqBaseEntity::OnParentEntityDestroyed, zmqEntity, &CzmqManager::HandleSelectedEntityDestroyed);



}



void CzmqManager::RemoveEntityFromSelection(CzmqBaseEntity* pEntity)
{
	
	

	for (auto it = m_pSelected_EntitiesList.begin(); it != m_pSelected_EntitiesList.end(); it++) {

		if (strcmp(pEntity->m_ent_name, it->get()->m_ent_name) != 0)
			continue;


		Msg(kleinworks_msg_header  ": Removed entity [%s] from EntRec selection.\n", pEntity->m_ent_name);

		__unhook(&CzmqBaseEntity::OnParentEntityDestroyed, pEntity, &CzmqManager::HandleSelectedEntityDestroyed);

		it->reset();

		m_pSelected_EntitiesList.erase(it);

		return;
	}
}





void CzmqManager::HandleSelectedEntityDestroyed(CzmqBaseEntity* pCaller)
{
	DevMsg(4, kleinworks_msg_header  "_DEBUG: Destruction of entity [%s] detected! Removing entity from EntRec selection...\n", pCaller->m_ent_name);
	__unhook(&CzmqBaseEntity::OnParentEntityDestroyed, pCaller, &CzmqManager::HandleSelectedEntityDestroyed);
	RemoveEntityFromSelection(pCaller);
	return;
}





void CzmqManager::ClearEntitySelection()
{
	DevMsg(4, kleinworks_msg_header  "_DEBUG: [%d]\n", int(m_pSelected_EntitiesList.size()));
	

	int size = int(m_pSelected_EntitiesList.size());

	for (int i = 0; i != size; i++) {

		DevMsg(4, kleinworks_msg_header  "_DEBUG: [%d]\n", i);

		auto it = m_pSelected_EntitiesList.begin();

		CzmqBaseEntity* pEnt = it->get();


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





/*|"Initialization of global CzmqManager instances.."|*/
#ifdef CLIENT_DLL

CzmqManager g_C_zmqManager = CzmqManager();


#else

CzmqManager g_CzmqManager = CzmqManager();

#endif // CLIENT_DLL



