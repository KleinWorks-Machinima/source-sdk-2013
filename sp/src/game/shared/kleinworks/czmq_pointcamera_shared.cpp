//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "czmq_pointcamera_shared.h"







CzmqPointCamera::CzmqPointCamera(CBaseHandle hEntity)
{

#ifdef CLIENT_DLL
	CBaseEntity* pEntity  = cl_entitylist->GetBaseEntityFromHandle(hEntity);
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
	m_ent_id		 = hEntity.GetSerialNumber();
	mh_parent_entity = hEntity;
	m_ent_type		 = int(ENTREC_TYPES::POINT_CAMERA);


	if (strcmp(pEntity->GetClassname(), "player") == 0) {

		CBasePlayer* pPlayerEntity = dynamic_cast<CBasePlayer*>(pEntity);

		m_bIsPlayerCamera = true;


		DevMsg(3, "KleinWorks: Camera of player named \"%s\" initialized.\n", pPlayerEntity->GetPlayerName());
	}
	else {

		m_bIsPlayerCamera = false;


		DevMsg(3, "KleinWorks: Point Camera named \"%s\" initialized.", pEntity->GetDebugName());
	}

}


CzmqPointCamera::~CzmqPointCamera()
{
	CzmqBaseEntity::~CzmqBaseEntity();
}





rapidjson::Value CzmqPointCamera::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);

	CBaseEntity* pPointCameraEntity;

#ifdef CLIENT_DLL
	if (m_bIsPlayerCamera) 
		pPointCameraEntity = dynamic_cast<CBasePlayer*>(cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity));
	else
		pPointCameraEntity = dynamic_cast<CPointCamera*>(cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity));
#else
	if (m_bIsPlayerCamera)
		pPointCameraEntity = dynamic_cast<CBasePlayer*>(gEntList.GetBaseEntity(mh_parent_entity));
	else
		pPointCameraEntity = dynamic_cast<CPointCamera*>(gEntList.GetBaseEntity(mh_parent_entity));

#endif // CLIENT_DLL


	// avoid taking a reference to the ID string by copying it
	int	  id_strlen = strlen(CFmtStr("%d", m_ent_id)) + 1;

	char* ent_id = new char[id_strlen];
	strcpy_s(ent_id, id_strlen, CFmtStr("%d", m_ent_id).String());

	entData_js.AddMember("ent_id", rapidjson::StringRef(ent_id), allocator);


	
	// have to do this little maneuver to avoid taking a reference of VecToString

	int eyePositionStrlen = strlen(VecToString(pPointCameraEntity->EyePosition()));
	int quatAnglesStrlen  = strlen(VecToString(pPointCameraEntity->EyeAngles()));


	char* eyePosition_proxystr   = new char[eyePositionStrlen + 1];
	char* eyeQuatAngles_proxystr = new char[quatAnglesStrlen + 1];


	strcpy_s(eyePosition_proxystr, eyePositionStrlen + 1, VecToString(pPointCameraEntity->EyePosition()));
	strcpy_s(eyeQuatAngles_proxystr, quatAnglesStrlen + 1, VecToString(pPointCameraEntity->EyeAngles()));



	entData_js.AddMember("VEC_ORIGIN", rapidjson::StringRef(eyePosition_proxystr), allocator);
	entData_js.AddMember("QANGLE_ANGLES", rapidjson::StringRef(eyeQuatAngles_proxystr), allocator);


	return entData_js;
}

