#include "TurnFloor.h"
#include "GameUtility.h"
#include "FPSManager.h"

using namespace DX12Library;

ObjModel TurnFloor::modelBox[4];
ObjModel TurnFloor::modelCover[4];

void TurnFloor::StaticInitialize()
{
	//モデル生成
	modelBox[TURNTYPE_LEFT].CreateFromOBJ(MODEL_DIR + "TurnFloor_Left/TurnFloor_Left.obj");
	modelBox[TURNTYPE_RIGHT].CreateFromOBJ(MODEL_DIR + "TurnFloor_Right/TurnFloor_Right.obj");
	modelBox[TURNTYPE_UP].CreateFromOBJ(MODEL_DIR + "TurnFloor_Up/TurnFloor_Up.obj");
	modelBox[TURNTYPE_DOWN].CreateFromOBJ(MODEL_DIR + "TurnFloor_Down/TurnFloor_Down.obj");

	modelCover[TURNTYPE_LEFT].CreatePlaneTex(ONE_CELL_LENGTH, ONE_CELL_LENGTH, MODEL_DIR + "TurnFloor_Left/uv_cover.png");
	modelCover[TURNTYPE_RIGHT].CreatePlaneTex(ONE_CELL_LENGTH, ONE_CELL_LENGTH, MODEL_DIR + "TurnFloor_Right/uv_cover.png");
	modelCover[TURNTYPE_UP].CreatePlaneTex(ONE_CELL_LENGTH, ONE_CELL_LENGTH, MODEL_DIR + "TurnFloor_Up/uv_cover.png");
	modelCover[TURNTYPE_DOWN].CreatePlaneTex(ONE_CELL_LENGTH, ONE_CELL_LENGTH, MODEL_DIR + "TurnFloor_Down/uv_cover.png");
}

void TurnFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	SetStagePos(pos);

	//カバーオブジェクト位置セット
	objCover.Initialize();
	objCover.SetPosition(object.GetPosition() + Vector3(0, ONE_CELL_LENGTH / 2, 0));
	objCover.SetRotation({ 90, 0, 0 });

	UpdateCollision();
}

void TurnFloor::Update()
{
	//上に覆う用のオブジェクトの位置同期
	objCover.SetPosition(object.GetPosition() + Vector3(0, ONE_CELL_LENGTH / 2, 0));
	RollUV();
	object.Update(pIOD[turnType]);
	objCover.Update(pIOD[turnType + 4]);
	UpdateCollision();
}

void TurnFloor::UpdateCollision()
{

}

int TurnFloor::GetFloorType()
{
	int result = -1;
	switch (turnType) {
	case TURNTYPE_LEFT:
		result = FLOORTYPE_TURN_LEFT;
		break;
	case TURNTYPE_RIGHT:
		result = FLOORTYPE_TURN_RIGHT;
		break;
	case TURNTYPE_UP:
		result = FLOORTYPE_TURN_UP;
		break;
	case TURNTYPE_DOWN:
		result = FLOORTYPE_TURN_DOWN;
		break;
	}

	return result;
}

void TurnFloor::SetTurnType(int turnType)
{
	this->turnType = turnType;

	//上と下の時は上下逆にすると正しく表示される
	switch (turnType) {
	case TURNTYPE_LEFT:
	case TURNTYPE_RIGHT:
		break;
	case TURNTYPE_UP:
		object.SetRotation({ 0,180,0 });	break;
	case TURNTYPE_DOWN:
		object.SetRotation({ 0,180,0 });	break;
	}
}

void TurnFloor::RollUV()
{
	//元々のUVに幾分か加算してUVをずらす
	const Vector2 OLD_UV[4] = {
		{0,1}, {0,0}, {1,1}, {1,0} 
	};
	addUV += 0.025f * FPSManager::GetMulAdjust60FPS();
	if (addUV >= 1) { addUV -= 1; }

	//各頂点において
	for (int i = 0; i < 4; i++) {
		modelCover[TURNTYPE_LEFT].SetVertexUV(i, OLD_UV[i] + Vector2(addUV, 0));
		modelCover[TURNTYPE_RIGHT].SetVertexUV(i, OLD_UV[i] + Vector2(-addUV, 0));
		modelCover[TURNTYPE_UP].SetVertexUV(i, OLD_UV[i] + Vector2(0, addUV));
		modelCover[TURNTYPE_DOWN].SetVertexUV(i, OLD_UV[i] + Vector2(0, -addUV));
	}
}
