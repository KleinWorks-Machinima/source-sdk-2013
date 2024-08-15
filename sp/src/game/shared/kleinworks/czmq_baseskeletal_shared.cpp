//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\
//
// Purpose: 
// 
// Product Contributors: Barber
//
//===================| PROPERTY OF THE KLEINWORKS™ CORPORTATION®® |===================\\

#include "cbase.h"
#include "czmq_baseskeletal_shared.h"














CzmqBaseSkeletal::CzmqBaseSkeletal(CBaseHandle hEntity)
{
#ifdef CLIENT_DLL
	CBaseAnimating* pSkelEntity = cl_entitylist->GetBaseEntityFromHandle(hEntity)->GetBaseAnimating();
	const char* modelName = pSkelEntity->GetModelName();
	cl_entitylist->AddListenerEntity(this);

#else
	CBaseAnimating* pSkelEntity = gEntList.GetBaseEntity(hEntity)->GetBaseAnimating();
	const char* modelName = pSkelEntity->GetModelName().ToCStr();
	gEntList.AddListenerEntity(this);


#endif // CLIENT_DLL

	// we have to do this to avoid taking a reference to GetDebugName() or GetModelName()

	int ent_name_len = strlen(pSkelEntity->GetDebugName());
	int ent_modelname_len = strlen(modelName);


	char* ent_name_proxystr = new char[ent_name_len + 1];
	char* ent_modelname_proxystr = new char[ent_modelname_len + 1];


	strcpy_s(ent_name_proxystr, ent_name_len + 1, pSkelEntity->GetDebugName());
	strcpy_s(ent_modelname_proxystr, ent_modelname_len + 1, modelName);


	m_ent_name		 = ent_name_proxystr;
	m_ent_model		 = ent_modelname_proxystr;
	mh_parent_entity = hEntity;
	m_ent_type		 = int(ENTREC_TYPES::BASE_SKELETAL);
	m_ent_id		 = hEntity.GetSerialNumber();

	CStudioHdr* pEntModel = pSkelEntity->GetModelPtr();

	// for some reason, the last bone shouldnt be accessable
	// (theres an assert that fails if you try to get it)
	m_numbones = pEntModel->numbones() - 1;

	for (int i = 0; i != m_numbones + 1; i++) {

		char*		boneName	  = new char[strlen(pEntModel->pBone(i)->pszName()) + 1];
		const char* constBoneName = new char[strlen(pEntModel->pBone(i)->pszName()) + 1];

		strcpy_s(boneName, strlen(pEntModel->pBone(i)->pszName()) + 1, pEntModel->pBone(i)->pszName());

		constBoneName = boneName;
		
		mch_bonenames_list.emplace_back(boneName);
	}

	


	DevMsg(3, "KleinWorks: Entity of class %s initialized.\n", pSkelEntity->GetClassname());

}


CzmqBaseSkeletal::~CzmqBaseSkeletal()
{
	CzmqBaseEntity::~CzmqBaseEntity();
}





rapidjson::Value CzmqBaseSkeletal::GetEntityData(rapidjson::MemoryPoolAllocator<> &allocator)
{

#ifdef CLIENT_DLL
	CBaseAnimating* pSkelEntity = cl_entitylist->GetBaseEntityFromHandle(mh_parent_entity)->GetBaseAnimating();
#else
	CBaseAnimating* pSkelEntity = gEntList.GetBaseEntity(mh_parent_entity)->GetBaseAnimating();
#endif // CLIENT_DLL

	CStudioHdr*     pModelPtr   = pSkelEntity->GetModelPtr();

	rapidjson::Value entData_js = rapidjson::Value(rapidjson::kObjectType);

	// avoid taking a reference to the ID string by copying it
	int	  id_strlen = strlen(CFmtStr("%d", m_ent_id)) + 1;

	char* ent_id = new char[id_strlen];
	strcpy_s(ent_id, id_strlen, CFmtStr("%d", m_ent_id).String());

	entData_js.AddMember("ent_id", rapidjson::StringRef(ent_id), allocator);


	// get ang/pos for every bone

	//CStudioHdr* pModelPtr = pSkelEntity->GetModelPtr();

	


	rapidjson::Value boneData_js = rapidjson::Value(rapidjson::kArrayType);
	



	for (int i = 0; i != m_numbones; i++) {
		rapidjson::Value bone_js = rapidjson::Value(rapidjson::kObjectType);

		matrix3x4_t worldMat;
		matrix3x4_t boneToWorld;
		matrix3x4_t localToPose;
		matrix3x4_t localMat;

		mstudiobone_t* boneSelf = pModelPtr->pBone(i);

		pSkelEntity->GetBoneTransform(i, boneToWorld);
		QuaternionMatrix(boneSelf->quat, boneSelf->pos, localToPose);
		
		MatrixMultiply(boneToWorld, localToPose, localMat);

		Vector     bonePosition;
		Quaternion boneQuatAngles;

		MatrixAngles(localMat, boneQuatAngles, bonePosition);


		
		




		const char* bonePosStr = static_cast<const char *>(CFmtStr("(%f, %f, %f)", bonePosition.x, bonePosition.y, bonePosition.z));
		const char* boneQuatAngStr = static_cast<const char *>(CFmtStr("(%f, %f, %f, %f)", boneQuatAngles.w, boneQuatAngles.x, boneQuatAngles.y, boneQuatAngles.z));

		// have to do this little maneuver to avoid taking a reference of the strings

		char* bonePos_proxystr = new char[strlen(bonePosStr) + 1];
		char* boneQuatAng_proxystr = new char[strlen(boneQuatAngStr) + 1];


		strcpy_s(bonePos_proxystr, strlen(bonePosStr) + 1, bonePosStr);
		strcpy_s(boneQuatAng_proxystr, strlen(boneQuatAngStr) + 1, boneQuatAngStr);

		bone_js.AddMember("POS", rapidjson::StringRef(bonePos_proxystr), allocator);
		bone_js.AddMember("ROT", rapidjson::StringRef(boneQuatAng_proxystr), allocator);

		boneData_js.PushBack(bone_js, allocator);
	}

	/*
	for (int i = 0; i != int(pBoneCache->Size()) + 1; i++) {
		rapidjson::Value bone_js = rapidjson::Value(rapidjson::kObjectType);
		
		matrix3x4_t boneToWorld = *pBoneCache->GetCachedBone(i);

		//matrix3x4_t worldToBone;
		//matrix3x4_t local;

		Vector bonePosition;
		QAngle boneAngles;
		Quaternion boneQuatAngles;


		MatrixPosition(boneToWorld, bonePosition);

		MatrixToAngles(boneToWorld, boneAngles);

		AngleQuaternion(boneAngles, boneQuatAngles);


		
		// converting worldspace to bonespace if this isn't the rootbone
		if (i == 0) {

			MatrixPosition(boneToWorld, bonePosition);

			MatrixToAngles(boneToWorld, boneAngles);

			AngleQuaternion(boneAngles, boneQuatAngles);
		}
		else {

			int iParent = pModelPtr->boneParent(i);

			MatrixInvert(boneToWorld[iParent], worldToBone);
			ConcatTransforms(worldToBone, boneToWorld[i], local);

			MatrixPosition(local, bonePosition);

			MatrixToAngles(local, boneAngles);

			AngleQuaternion(boneAngles, boneQuatAngles);
		}
		


		const char* bonePosStr     = static_cast<const char *>(CFmtStr("(%f, %f, %f)",     bonePosition.x, bonePosition.y, bonePosition.z));
		const char* boneQuatAngStr = static_cast<const char *>(CFmtStr("(%f, %f, %f, %f)", boneQuatAngles.w, boneQuatAngles.x, boneQuatAngles.y, boneQuatAngles.z));

		// have to do this little maneuver to avoid taking a reference of the strings

		char* bonePos_proxystr     = new char[strlen(bonePosStr)     + 1];
		char* boneQuatAng_proxystr = new char[strlen(boneQuatAngStr) + 1];


		strcpy_s(bonePos_proxystr,     strlen(bonePosStr) + 1, bonePosStr);
		strcpy_s(boneQuatAng_proxystr, strlen(boneQuatAngStr) + 1, boneQuatAngStr);

		bone_js.AddMember("POS", rapidjson::StringRef(bonePos_proxystr), allocator);
		bone_js.AddMember("ROT", rapidjson::StringRef(boneQuatAng_proxystr), allocator);

		boneData_js.PushBack(bone_js, allocator);
	}
	*/

	entData_js.AddMember("bonedata", boneData_js, allocator);


	return entData_js;
}





rapidjson::Value CzmqBaseSkeletal::GetEntityMetaData(rapidjson::MemoryPoolAllocator<> &allocator)
{
	rapidjson::Value entMetaData_js = rapidjson::Value(rapidjson::kObjectType);

	// avoid taking a reference to the ID string by copying it
	int	  id_strlen		 = strlen(CFmtStr("%d", m_ent_id)) + 1;
	int	  numbone_strlen = strlen(CFmtStr("%d", m_numbones)) + 1;

	char* ent_id = new char[id_strlen];
	char* numbones_str = new char[numbone_strlen];

	strcpy_s(ent_id, id_strlen, CFmtStr("%d", m_ent_id).String());
	strcpy_s(numbones_str, numbone_strlen, CFmtStr("%d", numbones_str).String());

	entMetaData_js.AddMember("ent_id", rapidjson::StringRef(ent_id), allocator);
	entMetaData_js.AddMember("ent_numbones", rapidjson::StringRef(numbones_str), allocator);
	entMetaData_js.AddMember("ent_name", rapidjson::StringRef(m_ent_name), allocator);
	entMetaData_js.AddMember("ent_type", m_ent_type, allocator);
	entMetaData_js.AddMember("ent_modelpath", rapidjson::StringRef(m_ent_model), allocator);

	rapidjson::Value boneData_js = rapidjson::Value(rapidjson::kArrayType);

	for (int i = 0; i != m_numbones + 1; i++) {
		rapidjson::Value bone_js = rapidjson::Value(rapidjson::kObjectType);
		bone_js.AddMember("name", rapidjson::StringRef(mch_bonenames_list[i]), allocator);

		boneData_js.PushBack(bone_js, allocator);
	}

	entMetaData_js.AddMember("bonedata", boneData_js, allocator);

	return entMetaData_js;
}
