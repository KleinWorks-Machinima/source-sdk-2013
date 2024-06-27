//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "czmq_baseskeletal.h"






CzmqBaseSkeletal::CzmqBaseSkeletal(CBaseHandle hEntity)
{
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);


	mh_parent_entity = hEntity;
	m_ent_type  = int(ENTREC_TYPES::BASE_SKELETAL);
	

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	int ent_name_len = strlen(pEntity->GetDebugName());
	int ent_modelname_len = strlen(pEntity->GetModelName().ToCStr());


	char* ent_name_proxystr = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];


	strcpy_s(ent_name_proxystr, ent_name_len + 1, pEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, pEntity->GetModelName().ToCStr());


	m_ent_name = ent_name_proxystr;
	m_ent_model = ent_modelname_proxystr;


	DevMsg(3, "KleinWorks: Entity of class %s initialized.\n", pEntity->GetClassname());

	gEntList.AddListenerEntity(this);
}


CzmqBaseSkeletal::~CzmqBaseSkeletal()
{
	CzmqBaseEntity::~CzmqBaseEntity();
}





rapidjson::Value CzmqBaseSkeletal::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	return rapidjson::Value();
}

