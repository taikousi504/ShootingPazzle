#include "BaseBlock.h"
#include "GameUtility.h"
#include "Easing.h"

Stage* BaseBlock::pStage = nullptr;

void BaseBlock::SetStagePos(const StageVec2& pos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(pos, &x, &z);

	object.SetPosition({ x, ONE_CELL_LENGTH / 2, z });
}

void BaseBlock::SetBreakupCount(unsigned short breakupCount)
{
	this->breakupCount = breakupCount;
	//最初から0の時は壊れないブロック
	isBreakable = breakupCount > 0;
}

void BaseBlock::SetBlockColor(int blockColor)
{
	this->blockColor = blockColor;

	//色設定
	if (blockColor == BLOCK_COLOR_BLACK) {
		object.SetColor({ 0.5f, 0.5f, 0.5f, 1 });
	}
	else if (blockColor == BLOCK_COLOR_RED) {
		object.SetColor({ 1, 0, 0, 1 });
	}
	else if (blockColor == BLOCK_COLOR_BLUE) {
		object.SetColor({ 0, 0, 1, 1 });
	}
	else if (blockColor == BLOCK_COLOR_YELLOW) {
		object.SetColor({ 1, 1, 0, 1 });
	}
	else if (blockColor == BLOCK_COLOR_GREEN) {
		object.SetColor({ 0, 1, 0, 1 });
	}
}

void BaseBlock::UpdateFirstEffect(const Timer& timer)
{
	//初回だけエフェクトの種類決め
	if (firstEffectType == -1) {
		//easeOut系の中からランダムで。(1,4,7,10,...)
		firstEffectType = rand() % 7;
		firstEffectType = 3 * firstEffectType + 1;
	}
	if (firstEffectEndTime == -1) {
		firstEffectEndTime = 1700 + rand() % 200;
	}
	
	double y = Easing::GetEaseValue(firstEffectType, 300, ONE_CELL_LENGTH / 2, timer, 500, firstEffectEndTime);

	Vector3 nPos = object.GetPosition();
	object.SetPosition({ nPos.x, (float)y, nPos.z });
}

void BaseBlock::UpdateClearEffect(const Timer& timer)
{
	//初回だけエフェクトのスタート時間決め
	if (clearEffectStartTime == -1) {
		clearEffectStartTime = 1000 + rand() % 1500;
	}

	//タイマーの値がclearEffectStartTimeを超えていたらブロックを落とす
	if ((double)timer.GetNowTime() >= clearEffectStartTime) {
		Vector3 nPos = object.GetPosition();
		float sub = (timer.GetNowTime() - clearEffectStartTime) * 0.01f;
		object.SetPosition({ nPos.x, nPos.y - sub, nPos.z });
	}
}
