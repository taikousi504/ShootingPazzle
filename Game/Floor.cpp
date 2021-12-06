#include "Floor.h"

void Floor::CreateModel(float width, float depth)
{
	model.CreateSquare(width, depth, { 0.7f, 0.7f, 0.7f });
}

void Floor::Initialize()
{
	Object3D::Initialize();
	Object3D::SetObjModel(&model);
	//床なので90度回転
	Object3D::SetRotation(90, 0, 0);

	//当たり判定初期化
	plane.distance = 0;
	plane.normal = { 0,1,0 };	//法線方向は上
}
