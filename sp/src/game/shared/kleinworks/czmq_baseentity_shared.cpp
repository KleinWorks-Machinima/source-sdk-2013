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




CzmqBaseEntity::CzmqBaseEntity(CBaseHandle hEntity)
{
#ifdef CLIENT_DLL
	CBaseEntity* pEntity = cl_entitylist->GetBaseEntityFromHandle(hEntity);
	const char* modelName = pEntity->GetModelName();

	cl_entitylist->AddListenerEntity(this);
#else
	CBaseEntity* pEntity = gEntList.GetBaseEntity(hEntity);
	const char* modelName = pEntity->GetModelName().ToCStr();

	gEntList.AddListenerEntity(this);
#endif // CLIENT_DLL

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	int ent_name_len = strlen(pEntity->GetDebugName());
	int ent_modelname_len = strlen(modelName);


	char* ent_name_proxystr = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];


	strcpy_s(ent_name_proxystr, ent_name_len + 1, pEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, modelName);




	m_ent_name		 = ent_name_proxystr;
	m_ent_model		 = ent_modelname_proxystr;

	mh_parent_entity = hEntity;
	m_ent_type		 = int(ENTREC_TYPES::BASE_ENTITY);
	m_ent_id		 = hEntity.GetSerialNumber();


	DevMsg(3, "KleinWorks_DEBUG: Entity of class %s initialized.\n", pEntity->GetClassname());

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

	// avoid taking a reference to the ID string by copying it
	int	  id_strlen = strlen(CFmtStr("%d", m_ent_id)) + 1;

	char* ent_id = new char[id_strlen];
	strcpy_s(ent_id, id_strlen, CFmtStr("%d", m_ent_id).String());

	entData_js.AddMember("ent_id", rapidjson::StringRef(ent_id), allocator);

	

	Quaternion entQuatAngle;
	
	AngleQuaternion(pEntity->GetAbsAngles(), entQuatAngle);

	// angles MUST be in quaternions, eulers cause a lot of problems
	const char* quatAngleStr = static_cast<const char *>(CFmtStr("(%f, %f, %f, %f)", entQuatAngle.w, entQuatAngle.x, entQuatAngle.y, entQuatAngle.z));


	// have to do this little maneuver to avoid taking a reference of VecToString
	int absOriginStrlen  = strlen(VecToString(pEntity->GetAbsOrigin()));
	int quatAnglesStrlen = strlen(quatAngleStr);


	char* entAbsOrigin_proxystr = new char[absOriginStrlen + 1];
	char* entQuatAngles_proxystr = new char[quatAnglesStrlen + 1];


	strcpy_s(entAbsOrigin_proxystr, absOriginStrlen + 1, VecToString(pEntity->GetAbsOrigin()));
	strcpy_s(entQuatAngles_proxystr, quatAnglesStrlen + 1, quatAngleStr);

	entData_js.AddMember("VEC_ORIGIN", rapidjson::StringRef(entAbsOrigin_proxystr), allocator);
	entData_js.AddMember("QANGLE_ANGLES", rapidjson::StringRef(entQuatAngles_proxystr), allocator);
	

	return entData_js;
}



rapidjson::Value CzmqBaseEntity::GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entMetaData_js = rapidjson::Value(rapidjson::kObjectType);

	// avoid taking a reference to the ID string by copying it
	int	  id_strlen = strlen(CFmtStr("%d", m_ent_id)) + 1;

	char* ent_id = new char[id_strlen];
	strcpy_s(ent_id, id_strlen, CFmtStr("%d", m_ent_id).String());

	entMetaData_js.AddMember("ent_id", rapidjson::StringRef(ent_id), allocator);

	entMetaData_js.AddMember("ent_name",      rapidjson::StringRef(m_ent_name), allocator);
	entMetaData_js.AddMember("ent_type",      m_ent_type, allocator);
	entMetaData_js.AddMember("ent_modelpath", rapidjson::StringRef(m_ent_model), allocator);

	return entMetaData_js;
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