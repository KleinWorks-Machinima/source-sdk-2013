//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "czmq_baseentity_shared.h"

#include "mathlib/mathlib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"




CzmqBaseEntity::CzmqBaseEntity(CBaseHandle hEntity)
{

#ifdef CLIENT_DLL
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntityFromHandle(hEntity);
	

	cl_entitylist->AddListenerEntity(this);
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);
	
	gEntList.AddListenerEntity(this);
#endif // CLIENT_DLL

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	const char* modelname = modelinfo->GetModelName(pEntity->GetModel());
	
	int ent_name_len = strlen(pEntity->GetDebugName());
	int ent_modelname_len = strlen(modelname);

	char* ent_name_proxystr = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];

	strcpy_s(ent_name_proxystr, ent_name_len + 1, pEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, modelname);



	m_ent_name		 = ent_name_proxystr;
	m_ent_model		 = ent_modelname_proxystr;

	mh_parent_entity = hEntity;
	m_ent_type		 = int(ENTREC_TYPES::BASE_ENTITY);
	m_ent_id		 = hEntity.GetSerialNumber();
	m_ent_numbones	 = 0;
	mb_is_gib		 = false;


	DevMsg(3, "kleinworks_DEBUG: Entity of class %s initialized.\n", pEntity->GetClassname());

}


CzmqBaseEntity::CzmqBaseEntity()
{
	mh_parent_entity = nullptr;
	m_ent_type       = NULL;
	m_ent_name       = NULL;
	m_ent_model      = NULL;
}




CzmqBaseEntity::~CzmqBaseEntity()
{
#ifdef CLIENT_DLL
	cl_entitylist->RemoveListenerEntity(this);
#else
	gEntList.RemoveListenerEntity(this);
#endif // CLIENT_DLL
	
	if (mh_parent_entity != NULL) {
		mh_parent_entity.Term();
		mh_parent_entity = NULL;
	}

}



rapidjson::Value CzmqBaseEntity::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{

#ifdef CLIENT_DLL
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity);
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(mh_parent_entity);
#endif // CLIENT_DLL

	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);

	entData_js.AddMember("ent_id", m_ent_id, allocator);

	

	Quaternion entQuatAngle;
	
	AngleQuaternion(pEntity->GetAbsAngles(), entQuatAngle);

	rapidjson::Value ent_pos_js = rapidjson::Value(rapidjson::kObjectType);
	rapidjson::Value ent_rot_js = rapidjson::Value(rapidjson::kObjectType);

	rapidjson::Value ent_origin_js = rapidjson::Value(rapidjson::kObjectType);
	rapidjson::Value ent_quat_js   = rapidjson::Value(rapidjson::kObjectType);


	ent_origin_js.AddMember("x", pEntity->GetAbsOrigin().x, allocator);
	ent_origin_js.AddMember("y", pEntity->GetAbsOrigin().y, allocator);
	ent_origin_js.AddMember("z", pEntity->GetAbsOrigin().z, allocator);

	ent_quat_js.AddMember("w", entQuatAngle.w, allocator);
	ent_quat_js.AddMember("x", entQuatAngle.x, allocator);
	ent_quat_js.AddMember("y", entQuatAngle.y, allocator);
	ent_quat_js.AddMember("z", entQuatAngle.z, allocator);

	ent_pos_js.AddMember("0", ent_origin_js, allocator);
	ent_rot_js.AddMember("0", ent_quat_js, allocator);

	entData_js.AddMember("ent_pos", ent_pos_js, allocator);
	entData_js.AddMember("ent_rot", ent_rot_js, allocator);
	

	return entData_js;
}



rapidjson::Value CzmqBaseEntity::GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entMetaData_js = rapidjson::Value(rapidjson::kObjectType);

	entMetaData_js.AddMember("ent_id", m_ent_id, allocator);
	entMetaData_js.AddMember("ent_type", m_ent_type, allocator);
	entMetaData_js.AddMember("ent_numbones", m_ent_numbones, allocator);

	entMetaData_js.AddMember("ent_name",  rapidjson::StringRef(m_ent_name), allocator);
	entMetaData_js.AddMember("ent_model", rapidjson::StringRef(m_ent_model), allocator);

	return entMetaData_js;
}



bool CzmqBaseEntity::IsValid()
{
	if (!mh_parent_entity.IsValid())
		return false;

	if (mh_parent_entity.Get() == nullptr)
		return false;

#ifdef CLIENT_DLL
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity);
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(mh_parent_entity);
#endif // CLIENT_DLL

	if (pEntity == nullptr)
		return false;
	
	return true;
}




bool CzmqBaseEntity::operator == (const CzmqBaseEntity& other) const
{
	if (mh_parent_entity == other.mh_parent_entity)
		return true;
	else
		return false;
}

bool CzmqBaseEntity::operator == (const CzmqBaseEntity other) const
{
	if (mh_parent_entity == other.mh_parent_entity)
		return true;
	else
		return false;
}

bool CzmqBaseEntity::operator == (const CzmqBaseEntity* other) const
{
	if (mh_parent_entity == other->mh_parent_entity)
		return true;
	else
		return false;
}
/*
bool CzmqBaseEntity::operator ==(const std::unique_ptr<CzmqBaseEntity> other) const
{
	if (other.get() == this)
		return true;
	else
	return false;
}
*/
bool CzmqBaseEntity::operator == (const CBaseHandle other) const
{
	if (mh_parent_entity == other)
		return true;
	else
		return false;
}