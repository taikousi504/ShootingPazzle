#include "TriangleBlock.h"
#include "GameUtility.h"

InstancingObjectDraw TriangleBlock::instancingObjectDraw;
ObjModel TriangleBlock::modelTriangle;

void TriangleBlock::CreateModel()
{
	//モデル生成
	modelTriangle.CreateFromOBJ("TriangleBlock");
}

void TriangleBlock::StaticInitialize()
{
	CreateModel();
	instancingObjectDraw.Initialize();
	instancingObjectDraw.SetObjModel(&modelTriangle);
}

void TriangleBlock::Initialize(const StageVec2& pos, float sphereRadius)
{
	//オブジェクト生成
	object.Initialize();

	/*上から見た元の形
	＿＿＿
	|　　／
	|　／
	|／

	*/

	//直方体の周りにつけるカプセル判定 (三角形なので3つ)
	capsule.clear();
	for (int i = 0; i < 3; i++) {
		capsule.push_back({});
		capsule[i].radius = sphereRadius;
	}

	SetStagePos(pos);

	UpdateCollision();
}

void TriangleBlock::SetTriangleType(int shapeType)
{
	this->triangleType = shapeType;

	//タイプによって回転
	switch (this->triangleType) {
	case TRIANGLETYPE_NO_LEFTTOP:
		object.SetRotation({ 0, 180, 0 });
		break;
	case TRIANGLETYPE_NO_RIGHTTOP:
		object.SetRotation({ 0, 270, 0 });
		break;
	case TRIANGLETYPE_NO_LEFTBOTTOM:
		object.SetRotation({ 0, 90, 0 });
		break;
	case TRIANGLETYPE_NO_RIGHTBOTTOM:
		//そのまま
		break;
	}

	UpdateCollision();
}

void TriangleBlock::Update()
{
	object.Update(instancingObjectDraw);
	UpdateCollision();
}

void TriangleBlock::Draw()
{
	instancingObjectDraw.Update();
	instancingObjectDraw.Draw();
}

void TriangleBlock::EndDraw()
{
	instancingObjectDraw.EndDraw();
}

void TriangleBlock::UpdateCollision()
{
	const Vector3& position = object.GetPosition();
	Vector3 topLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//左奥
	Vector3 topRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z + ONE_CELL_LENGTH / 2 };	//右奥
	Vector3 bottomLeft = { position.x - ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//左手前
	Vector3 bottomRight = { position.x + ONE_CELL_LENGTH / 2, position.y, position.z - ONE_CELL_LENGTH / 2 };	//右手前

	//カプセルの値更新
	switch (this->triangleType) {
	case TRIANGLETYPE_NO_LEFTTOP:
		capsule[0].start = topRight;
		capsule[0].end = bottomRight;

		capsule[1].start = bottomRight;
		capsule[1].end = bottomLeft;

		capsule[2].start = bottomLeft;
		capsule[2].end = topRight;
		break;

	case TRIANGLETYPE_NO_RIGHTTOP:
		capsule[0].start = topLeft;
		capsule[0].end = bottomRight;

		capsule[1].start = bottomRight;
		capsule[1].end = bottomLeft;

		capsule[2].start = bottomLeft;
		capsule[2].end = topLeft;
		break;

	case TRIANGLETYPE_NO_LEFTBOTTOM:
		capsule[0].start = topLeft;
		capsule[0].end = topRight;

		capsule[1].start = topRight;
		capsule[1].end = bottomRight;

		capsule[2].start = bottomRight;
		capsule[2].end = topLeft;
		break;

	case TRIANGLETYPE_NO_RIGHTBOTTOM:
		capsule[0].start = topLeft;
		capsule[0].end = topRight;

		capsule[1].start = topRight;
		capsule[1].end = bottomLeft;

		capsule[2].start = bottomLeft;
		capsule[2].end = topLeft;
		break;
	}
}

void TriangleBlock::DecrementBreakupCount()
{
	if (isBreakable == false) {
		return;
	}

	breakupCount--;
	if (breakupCount <= 0) {
		Breakup();
	}
}

void TriangleBlock::Breakup()
{
	//ここに爆発エフェクト入れる

	//残り目標ブロック数を減らす
	pStage->DecrementTargetBlockCount();
}
