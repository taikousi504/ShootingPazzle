#include "BreakFloor.h"
#include "GameUtility.h"
#include "GameSound.h"
#include "FPSManager.h"

ObjModel BreakFloor::modelBox;
Particle3D BreakFloor::particle;

void BreakFloor::StaticInitialize()
{
	//モデル生成
	modelBox.CreateFromOBJ(modelDir + "BreakFloor/BreakFloor.obj");

	//パーティクル
	particle.LoadTexture(L"Resources/Particle/Break.png");
	particle.Initialize();
	particle.SetColor({ 0.71f,0.47f, 0.2f, 1 });
}

void BreakFloor::GenerateParticle(const Vector3& pos)
{
	Vector3 generatePos = pos;
	for (int i = 0; i < 5; i++) {
		//上方向にランダムで飛ばす（飛距離は控えめ）
		float x = (float)((rand() % 200 - 100) * 0.01f);
		float y = (float)((rand() % 100) * 0.01f);
		float z = (float)((rand() % 200 - 100) * 0.01f);
		Vector3 vel = Vector3(x, y, z).Normalize() * 0.25f * FPSManager::GetMulAdjust60FPS();
		Vector3 acc =  Vector3(0, -0.01f, 0) * FPSManager::GetMulAdjust60FPS();;
		float startScale = 4.0f;
		float endScale = 0;

		particle.Add(1000, generatePos, vel, acc, startScale, endScale);
	}
}

void BreakFloor::DrawParticle()
{
	particle.Update();
	particle.Draw();
}

void BreakFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();

	isBreak = false;

	objectName = "BreakFloor";
}

void BreakFloor::Update()
{
	//球が自分の上から離れたら破壊
	{
		if (onFloor == false && prevOnFloor == true) {
			Break();
		}
		prevOnFloor = onFloor;
	}

	if (isBreak == false) {
		object.Update(*pIOD);
	}
	UpdateCollision();
}

void BreakFloor::UpdateCollision()
{
}

void BreakFloor::Break()
{
#ifdef BUILD_GAME
	//パーティクルを発生させる
	GenerateParticle(object.GetPosition());

	//効果音再生
	GameSound::Play(L"Break", object.GetPosition());

#endif // BUILD_GAME

	isBreak = true;
	//穴とみなす
	objectName = "HoleFloor";
}
