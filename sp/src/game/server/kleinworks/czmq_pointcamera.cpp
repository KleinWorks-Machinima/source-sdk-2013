//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "czmq_pointcamera.h"







CzmqPointCamera::CzmqPointCamera(CBaseHandle hEntity)
{
	CBaseEntity* pEnt = gEntList.GetBaseEntity(hEntity);

	if (strcmp(pEnt->GetClassname(), "player") == 0) {

		CBasePlayer* pPlayerEntity = dynamic_cast<CBasePlayer*>(gEntList.GetBaseEntity(hEntity));

		m_bIsPlayerCamera = true;
		mh_parent_entity = hEntity;

		m_ent_type  = int(ENTREC_TYPES::POINT_CAMERA);

		// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

		int ent_name_len = strlen(pPlayerEntity->GetDebugName());
		int ent_modelname_len = strlen(pPlayerEntity->GetModelName().ToCStr());


		char* ent_name_proxystr = new char[ent_name_len + 1];
		char* ent_modelname_proxystr = new char[ent_modelname_len + 1];


		strcpy_s(ent_name_proxystr, ent_name_len + 1, pPlayerEntity->GetDebugName());
		strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, pPlayerEntity->GetModelName().ToCStr());


		m_ent_name = ent_name_proxystr;
		m_ent_model = ent_modelname_proxystr;

		DevMsg(3, "KleinWorks: Camera of player named \"%s\" initialized.\n", pPlayerEntity->GetPlayerName());

		gEntList.AddListenerEntity(this);

		return;
	}
	if (pEnt->GetClassname() == std::string("CPointCamera").c_str()) {

		m_bIsPlayerCamera = false;
		mh_parent_entity = hEntity;
		m_ent_type  = int(ENTREC_TYPES::POINT_CAMERA);


		// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

		int ent_name_len = strlen(pEnt->GetDebugName());
		int ent_modelname_len = strlen(pEnt->GetModelName().ToCStr());


		char* ent_name_proxystr = new char[ent_name_len + 1];
		char* ent_modelname_proxystr = new char[ent_modelname_len + 1];


		strcpy_s(ent_name_proxystr, ent_name_len + 1, pEnt->GetDebugName());
		strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, pEnt->GetModelName().ToCStr());


		m_ent_name = ent_name_proxystr;
		m_ent_model = ent_modelname_proxystr;



		DevMsg(3, "KleinWorks: Point Camera named \"%s\" initialized.", pEnt->GetDebugName());

		gEntList.AddListenerEntity(this);

		return;
	}

	Warning("KleinWorks: ERROR! Something went wrong when trying to create a CzmqPointCamera object on entity named %s! Entity either wasn't a camera of any kind, or something else went wrong. Aborting!\n", pEnt->GetDebugName());

	delete this;

	return;
}


CzmqPointCamera::~CzmqPointCamera()
{
	CzmqBaseEntity::~CzmqBaseEntity();
}





rapidjson::Value CzmqPointCamera::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);


	if (m_bIsPlayerCamera) {
		CBasePlayer* pPlayerEntity = dynamic_cast<CBasePlayer*>(gEntList.GetBaseEntity(mh_parent_entity));

		entData_js.AddMember("ent_name", rapidjson::StringRef(m_ent_name), allocator);


		// have to do this little maneuver to avoid taking a reference of VecToString

		Quaternion eyeQuatAngles;

		int eyePositionStrlen = strlen(VecToString(pPlayerEntity->EyePosition()));
		int eyeAnglesStrlen = strlen(VecToString(pPlayerEntity->EyeAngles()));


		char* eyePosition_proxystr = new char[eyePositionStrlen + 1];
		char* eyeAngles_proxystr = new char[eyeAnglesStrlen + 1];


		strcpy_s(eyePosition_proxystr, eyePositionStrlen + 1, VecToString(pPlayerEntity->EyePosition()));
		strcpy_s(eyeAngles_proxystr, eyeAnglesStrlen + 1, VecToString(pPlayerEntity->EyeAngles()));



		entData_js.AddMember("VEC_ORIGIN",    rapidjson::StringRef(eyePosition_proxystr), allocator);
		entData_js.AddMember("QANGLE_ANGLES", rapidjson::StringRef(eyeAngles_proxystr), allocator);

	}
	else {
		CPointCamera* pPointCameraEntity = dynamic_cast<CPointCamera*>(gEntList.GetBaseEntity(mh_parent_entity));
		
		entData_js.AddMember("ent_name", rapidjson::StringRef(m_ent_name), allocator);


		// have to do this little maneuver to avoid taking a reference of VecToString

		Quaternion eyeQuatAngles;
		QAngle	   eyeEulerAngles = pPointCameraEntity->EyeAngles();

		AngleQuaternion(eyeEulerAngles, eyeQuatAngles);

		// angles MUST be in quaternions, eulers cause a lot of problems
		const char* quatAngleStr = static_cast<const char *>(CFmtStr("(%f, %f, %f, %f)", eyeQuatAngles.w, eyeQuatAngles.x, eyeQuatAngles.y, eyeQuatAngles.z));

		int eyePositionStrlen = strlen(VecToString(pPointCameraEntity->EyePosition()));
		int quatAnglesStrlen = strlen(quatAngleStr);


		char* eyePosition_proxystr = new char[eyePositionStrlen + 1];
		char* eyeQuatAngles_proxystr = new char[quatAnglesStrlen + 1];


		strcpy_s(eyePosition_proxystr, eyePositionStrlen + 1, VecToString(pPointCameraEntity->EyePosition()));
		strcpy_s(eyeQuatAngles_proxystr, quatAnglesStrlen + 1, quatAngleStr);



		entData_js.AddMember("VEC_ORIGIN", rapidjson::StringRef(eyePosition_proxystr), allocator);
		entData_js.AddMember("QANGLE_ANGLES", rapidjson::StringRef(eyeQuatAngles_proxystr), allocator);


	}

	return entData_js;
}

