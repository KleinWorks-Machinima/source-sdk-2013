//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "c_zmq_baseentity.h"

#include "mathlib/mathlib.h"




C_zmqBaseEntity::C_zmqBaseEntity(ClientEntityHandle_t hEntity)
{
	C_BaseEntity* pEntity = cl_entitylist->GetBaseEntityFromHandle(hEntity);

	mh_parent_entity = hEntity;
	m_ent_type		 = int(ENTREC_TYPES::BASE_ENTITY);

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	int ent_name_len = strlen(pEntity->GetDebugName());
	int ent_modelname_len = strlen(pEntity->GetModelName());


	char* ent_name_proxystr      = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];


	strcpy_s(ent_name_proxystr, ent_name_len + 1, pEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, pEntity->GetModelName());

	
	m_ent_name  = ent_name_proxystr;
	m_ent_model = ent_modelname_proxystr;


	DevMsg(3, "KleinWorks_DEBUG: Entity of class %s initialized.\n", pEntity->GetClassname());

	cl_entitylist->AddListenerEntity(this);
}


C_zmqBaseEntity::C_zmqBaseEntity()
{
	mh_parent_entity = nullptr;
	m_ent_type       = NULL;
	m_ent_name       = NULL;
	m_ent_model      = NULL;
}




C_zmqBaseEntity::~C_zmqBaseEntity()
{
	cl_entitylist->RemoveListenerEntity(this);
	
	if (mh_parent_entity != NULL) {
		mh_parent_entity.Term();
		mh_parent_entity = NULL;
	}

}



rapidjson::Value C_zmqBaseEntity::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{

	C_BaseEntity* pEntity = cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity);

	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);

	entData_js.AddMember("ent_name", rapidjson::StringRef(m_ent_name), allocator);

	// have to do this little maneuver to avoid taking a reference of VecToString

	Quaternion entQuatAngle;
	
	AngleQuaternion(pEntity->GetAbsAngles(), entQuatAngle);

	// angles MUST be in quaternions, eulers cause a lot of problems
	const char* quatAngleStr = static_cast<const char *>(CFmtStr("(%f, %f, %f, %f)", entQuatAngle.w, entQuatAngle.x, entQuatAngle.y, entQuatAngle.z));



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



rapidjson::Value C_zmqBaseEntity::GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entMetaData_js = rapidjson::Value(rapidjson::kObjectType);

	entMetaData_js.AddMember("ent_name",      rapidjson::StringRef(m_ent_name), allocator);
	entMetaData_js.AddMember("ent_type",      m_ent_type, allocator);
	entMetaData_js.AddMember("ent_modelpath", rapidjson::StringRef(m_ent_model), allocator);

	return entMetaData_js;
}




bool C_zmqBaseEntity::operator == (const C_zmqBaseEntity& other) const
{
	if (mh_parent_entity == other.mh_parent_entity)
		return true;
	else
		return false;
}

bool C_zmqBaseEntity::operator == (const C_zmqBaseEntity other) const
{
	if (mh_parent_entity == other.mh_parent_entity)
		return true;
	else
		return false;
}

bool C_zmqBaseEntity::operator == (const C_zmqBaseEntity* other) const
{
	if (mh_parent_entity == other->mh_parent_entity)
		return true;
	else
		return false;
}
/*
bool C_zmqBaseEntity::operator ==(const std::unique_ptr<C_zmqBaseEntity> other) const
{
	if (other.get() == this)
		return true;
	else
	return false;
}
*/
bool C_zmqBaseEntity::operator == (const CBaseHandle other) const
{
	if (mh_parent_entity == other)
		return true;
	else
		return false;
}