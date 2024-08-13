//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\


#include "cbase.h"
#include "c_zmq_manager.h"








C_zmqManager::C_zmqManager()
{

	m_RecordUntil		= 0;

	record_toggle		= false;

	record_frame_start  = 0;
	record_frame_end	= 0;
	
	m_zmq_comms.m_drop_out_tolerance = -1;
	m_zmq_comms.m_pollerTimeout      = 0;



	//======-Json Object Initialization-======\\

	
	m_entity_metadata_js.SetObject();


	rapidjson::Value entArray(rapidjson::kArrayType);


	m_entity_metadata_js.AddMember("EntList", entArray, m_entity_metadata_js.GetAllocator());


}

C_zmqManager::~C_zmqManager()
{
	delete &m_zmq_comms;

}




void C_zmqManager::OnTick()
{


	// First, check if we are trying to record
	if (record_toggle != 0) {

		// If we have hit our m_RecordUntil cap, stop recording. If m_RecordUntil is negative then ignore it
		if (m_zmq_comms.m_OUTPUT_tick_count >= m_RecordUntil && m_RecordUntil >= 1) {

			engine->ClientCmd("kw_entrec_record 0\n");

			Msg("KleinWorks: Ending recording...\n");
			Msg("KleinWorks: Recorded from %d to %d\n", 0, m_zmq_comms.m_OUTPUT_tick_count);

			if (m_zmq_comms.m_isSendingOutput == true)
				m_zmq_comms.m_isDoneTransfering = true;

			return;
		}


		// If we are currently recording, update entities
		if (m_zmq_comms.m_isSendingOutput == true) {
			if (m_zmq_comms.m_OUTPUT_tick_count == 1)
				Msg("KleinWorks: Connection established! Recording started!\n");
			UpdateSelectedEntities();
		}



		m_zmq_comms.RunFuncLoop();


	}
	// If we aren't trying to record, check if we are trying to end a data transfer
	else if (m_zmq_comms.m_isDoneTransfering || m_zmq_comms.m_peerIsDoneTransfering) {
		m_zmq_comms.RunFuncLoop();
	}




}







void C_zmqManager::UpdateSelectedEntities()
{
	// create a document representing all of our selected entity's data
	rapidjson::Document entityData_js;
	entityData_js.SetObject();

	entityData_js.AddMember("EntList", rapidjson::kArrayType, entityData_js.GetAllocator());



	
	// iterate through each entity we have selected
	for (auto& element : m_pSelected_EntitiesList)
	{
		if (!element)
		{ // if a selected entity doesnt exist, print an error message
			Warning("KleinWorks: Something went wrong! At line %d in C_zmqManager.cpp, element was a nullptr! Skipping this element!\n", __LINE__ - 3);
			continue;
		}

		
		
		entityData_js["EntList"].PushBack(element->GetEntityData(entityData_js.GetAllocator()), entityData_js.GetAllocator());

	}
	

	// convert the entity data document into a string
	rapidjson::StringBuffer entDataStrBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(entDataStrBuffer);
	entityData_js.Accept(writer);

	// add the stringified entity data document to the current data buffer
	m_zmq_comms.m_sending_data_buffer = zframe_new(entDataStrBuffer.GetString(), strlen(entDataStrBuffer.GetString()));
}





void C_zmqManager::AddEntityToSelection(CBaseHandle hEntity)
{
	if (!hEntity.IsValid()) {
		Warning("KleinWorks: ERROR! Something tried to select an invalid entity for recording. Either the entity doesn't exist or it's a Hammer entity.\n");
		return;
	}
	C_BaseEntity* pEntity = cl_entitylist->GetBaseEntityFromHandle(hEntity);
	


	C_zmqBaseEntity* zmqEntity = nullptr;
	
	for (auto& element : m_pSelected_EntitiesList)
	{
		if (*element.get() == hEntity) {
			Msg("KleinWorks: Entity [%s] is already in EntRec selection!\n", pEntity->GetDebugName());
			return;
		}
	}
	

	Msg("KleinWorks: adding entity with name [%s] to EntRec selection...\n", pEntity->GetDebugName());

	rapidjson::Value entMetaData_js;
	
	if (strcmp( pEntity->GetClassname(), "player") == 0) {
		C_zmqPointCamera* pPointCam = new C_zmqPointCamera(hEntity);
		zmqEntity = pPointCam;

		m_pSelected_EntitiesList.emplace_back(std::make_unique<C_zmqPointCamera>(*pPointCam));

		entMetaData_js = pPointCam->GetEntityMetaData(m_entity_metadata_js.GetAllocator());
	}
	else if (strcmp(pEntity->GetClassname(), "npc_metropolice") == 0) {
		C_zmqBaseSkeletal* pBaseSkel = new C_zmqBaseSkeletal(hEntity);
		zmqEntity = pBaseSkel;

		m_pSelected_EntitiesList.emplace_back(std::make_unique<C_zmqBaseSkeletal>(*pBaseSkel));

		entMetaData_js = pBaseSkel->GetEntityMetaData(m_entity_metadata_js.GetAllocator());
	}
	else {
		zmqEntity = new C_zmqBaseEntity(hEntity);

		m_pSelected_EntitiesList.emplace_back(std::make_unique<C_zmqBaseEntity>(std::move(*zmqEntity)));

		entMetaData_js = zmqEntity->GetEntityMetaData(m_entity_metadata_js.GetAllocator());
	}

	__hook(&C_zmqBaseEntity::OnParentEntityDestroyed, zmqEntity, &C_zmqManager::HandleSelectedEntityDestroyed);

	

	// add the entity to the entity metadata JSON object
	m_entity_metadata_js["EntList"].PushBack(entMetaData_js, m_entity_metadata_js.GetAllocator());

}



void C_zmqManager::RemoveEntityFromSelection(C_zmqBaseEntity* pEntity)
{
	
	

	for (auto it = m_pSelected_EntitiesList.begin(); it != m_pSelected_EntitiesList.end(); it++) {

		if (strcmp(pEntity->m_ent_name, it->get()->m_ent_name) != 0)
			continue;

		// remove the entity from the entity metadata JSON object
		for (auto entity = m_entity_metadata_js["EntList"].Begin(); entity != m_entity_metadata_js["EntList"].End(); entity++) {

			if (strcmp(entity->FindMember("ent_name")->value.GetString(), pEntity->m_ent_name) == 0) {
				m_entity_metadata_js["EntList"].Erase(entity);
				DevMsg(4, "KleinWorks_DEBUG: Removed entity [%s] from metadata_js.\n", pEntity->m_ent_name);
				break;
			}
			
		}

		Msg("KleinWorks: Removed entity [%s] from EntRec selection.\n", pEntity->m_ent_name);

		__unhook(&C_zmqBaseEntity::OnParentEntityDestroyed, pEntity, &C_zmqManager::HandleSelectedEntityDestroyed);

		it->reset();

		m_pSelected_EntitiesList.erase(it);

		return;
	}
}





void C_zmqManager::HandleSelectedEntityDestroyed(C_zmqBaseEntity* pCaller)
{
	DevMsg(4, "KleinWorks_DEBUG: Destruction of entity [%s] detected! Removing entity from EntRec selection...\n", pCaller->m_ent_name);
	__unhook(&C_zmqBaseEntity::OnParentEntityDestroyed, pCaller, &C_zmqManager::HandleSelectedEntityDestroyed);
	RemoveEntityFromSelection(pCaller);
	return;
}





void C_zmqManager::ClearEntitySelection()
{
	DevMsg(4, "KleinWorks_DEBUG: [%d]\n", int(m_pSelected_EntitiesList.size()));
	

	int size = int(m_pSelected_EntitiesList.size());

	for (int i = 0; i != size; i++) {

		DevMsg(4, "KleinWorks_DEBUG: [%d]\n", i);

		auto it = m_pSelected_EntitiesList.begin();

		C_zmqBaseEntity* pEnt = it->get();


		// remove the entity from the entity metadata JSON object
		for (int i = 0; i != int(m_entity_metadata_js["EntList"].Size()); i++) {

			auto entity = m_entity_metadata_js["EntList"].Begin();

			if (strcmp(entity->FindMember("ent_name")->value.GetString(), pEnt->m_ent_name) == 0) {
				m_entity_metadata_js["EntList"].Erase(entity);
				DevMsg(4, "KleinWorks_DEBUG: Removed entity [%s] from metadata_js.\n", pEnt->m_ent_name);
				break;
			}

		}


		__unhook(&C_zmqBaseEntity::OnParentEntityDestroyed, pEnt, &C_zmqManager::HandleSelectedEntityDestroyed);

		pEnt = NULL;

		it->reset();

		m_pSelected_EntitiesList.erase(it);
	}

}





/*|"Initialization of global C_zmqManager instance.."|*/
C_zmqManager g_C_zmqManager = C_zmqManager();




