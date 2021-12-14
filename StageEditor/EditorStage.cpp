#include <fstream>

#include "EditorStage.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "NormalFloor.h"
#include "TurnFloor.h"
#include "HoleFloor.h"
#include "GameUtility.h"

EditorStage::~EditorStage()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) delete floors[i];
	}
	floors.clear();
}

void EditorStage::LoadStage(std::string filename)
{
	NormalFloor::StaticInitialize();
	TurnFloor::StaticInitialize();

	//データ初期化
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) delete floors[i];
	}
	floors.clear();

	//GameUtilityにステージサイズを渡す
	GameUtility::SetPStageSize(&stageSize);

	std::ifstream file;
	//バイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//開けなかったらブロックなし床敷き詰めデータを生成
	if (!file.is_open()) {
		stageSize.x = 20;
		stageSize.y = 20;
		startLaneZ = stageSize.y - 2;
		//全マスに床ブロック(ノーマル)
		for (int i = 0; i < stageSize.y; i++) {
			for (int j = 0; j < stageSize.x; j++) {
				StageVec2 pos = { i,j };
				NormalFloor* newFloor = new NormalFloor;
				newFloor->Initialize(pos);
				floors.emplace_back(newFloor);
			}
		}
		//床当たり判定初期化
		floorCollision.distance = 0;
		floorCollision.normal = { 0,1,0 };	//法線方向は上
		return;
	}

	//ステージ情報格納
	StageHeader header;
	file.read((char*)&header, sizeof(header));
	stageSize.x = header.width;
	stageSize.y = header.depth;
	startLaneZ = header.startLineZ;

	//ブロック情報格納
	for (int i = 0; i < header.blockCount; i++) {
		StageBlock object;
		file.read((char*)&object, sizeof(object));

		StageVec2 pos = { object.stagePosX, object.stagePosY };

		//SquareBlock
		if (object.type == 0) {
			SquareBlock* newBlock = new SquareBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock LT
		else if (object.type == 1) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetTriangleType(TRIANGLETYPE_NO_LEFTTOP);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock RT
		else if (object.type == 2) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetTriangleType(TRIANGLETYPE_NO_RIGHTTOP);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock LB
		else if (object.type == 3) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetTriangleType(TRIANGLETYPE_NO_LEFTBOTTOM);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock RB
		else if (object.type == 4) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetTriangleType(TRIANGLETYPE_NO_RIGHTBOTTOM);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
	}

	//床情報格納
	//ステージデータで指定された箇所は指定されたデータを格納、それ以外はノーマルブロック
	for (int i = 0; i < header.floorCount; i++) {
		StageFloor object;
		file.read((char*)&object, sizeof(object));

		StageVec2 pos = { object.stagePosX, object.stagePosY };

		if (object.type == 0) {
			NormalFloor* newFloor = new NormalFloor;
			newFloor->Initialize(pos);
			floors.emplace_back(newFloor);
		}
		else if (object.type == 1) {
			TurnFloor* newFloor = new TurnFloor;
			newFloor->Initialize(pos);
			newFloor->SetTurnType(TURNTYPE_LEFT);
			floors.emplace_back(newFloor);
		}
		else if (object.type == 2) {
			TurnFloor* newFloor = new TurnFloor;
			newFloor->Initialize(pos);
			newFloor->SetTurnType(TURNTYPE_RIGHT);
			floors.emplace_back(newFloor);
		}
		else if (object.type == 3) {
			TurnFloor* newFloor = new TurnFloor;
			newFloor->Initialize(pos);
			newFloor->SetTurnType(TURNTYPE_UP);
			floors.emplace_back(newFloor);
		}
		else if (object.type == 4) {
			TurnFloor* newFloor = new TurnFloor;
			newFloor->Initialize(pos);
			newFloor->SetTurnType(TURNTYPE_DOWN);
			floors.emplace_back(newFloor);
		}
	}

	file.close();

	//床当たり判定初期化
	floorCollision.distance = 0;
	floorCollision.normal = { 0,1,0 };	//法線方向は上
}

void EditorStage::Update()
{
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) floors[i]->Update();
	}

	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Update();
	}
}

void EditorStage::Draw()
{
	NormalFloor::Draw();
	TurnFloor::Draw();

	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Draw();
	}
}

void EditorStage::AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount)
{
	//既にブロックが配置されていたらリターン
	if (CheckExistBlock(stagePos) != -1) {
		return;
	}

	//種類ごとにブロック追加
	if (blockType == BLOCKTYPE_SQUARE) {
		SquareBlock* newBlock = new SquareBlock;
		newBlock->Initialize(stagePos, ONE_CELL_LENGTH / 2);
		newBlock->SetBreakupCount(breakupCount);
		blocks.emplace_back(newBlock);
	}
	else {
		TriangleBlock* newBlock = new TriangleBlock;
		newBlock->Initialize(stagePos, ONE_CELL_LENGTH / 2);
		newBlock->SetBreakupCount(breakupCount);
		if (blockType == BLOCKTYPE_TRIANGLE_NO_LEFTTOP) {
			newBlock->SetTriangleType(TRIANGLETYPE_NO_LEFTTOP);
		}
		else if (blockType == BLOCKTYPE_TRIANGLE_NO_RIGHTTOP) {
			newBlock->SetTriangleType(TRIANGLETYPE_NO_RIGHTTOP);
		}
		else if (blockType == BLOCKTYPE_TRIANGLE_NO_LEFTBOTTOM) {
			newBlock->SetTriangleType(TRIANGLETYPE_NO_LEFTBOTTOM);
		}
		else if (blockType == BLOCKTYPE_TRIANGLE_NO_RIGHTBOTTOM) {
			newBlock->SetTriangleType(TRIANGLETYPE_NO_RIGHTBOTTOM);
		}
		blocks.emplace_back(newBlock);
	}
}

void EditorStage::DeleteBlock(const StageVec2& stagePos)
{
	//ブロックが配置されていなかったらリターン
	int deleteIndex = CheckExistBlock(stagePos);
	if (deleteIndex == -1) {
		return;
	}

	//ブロック削除
	if (blocks[deleteIndex]) delete blocks[deleteIndex];
	blocks.erase(blocks.begin() + deleteIndex);
}

int EditorStage::CheckExistBlock(const StageVec2& stagePos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(stagePos, &x, &z);

	for (int i = 0; i < blocks.size(); i++) {
		if (x == blocks[i]->GetPosition().x && z == blocks[i]->GetPosition().z) {
			return i;
		}
	}

	return -1;
}

void EditorStage::AddFloor(const StageVec2& stagePos, int floorType)
{
	//既に床ブロックが配置されていたらリターン
	if (CheckExistFloor(stagePos) != -1) {
		return;
	}

	//種類ごとに床ブロック追加
	if (floorType == FLOORTYPE_NORMAL) {
		NormalFloor* newFloor = new NormalFloor;
		newFloor->Initialize(stagePos);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_LEFT) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_LEFT);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_RIGHT) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_RIGHT);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_UP) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_UP);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_DOWN) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_DOWN);
		floors.emplace_back(newFloor);
	}
}

void EditorStage::DeleteFloor(const StageVec2& stagePos)
{
	//床ブロックが配置されていなかったらリターン
	int deleteIndex = CheckExistFloor(stagePos);
	if (deleteIndex == -1) {
		return;
	}

	//床ブロック削除
	if (floors[deleteIndex]) delete floors[deleteIndex];
	floors.erase(floors.begin() + deleteIndex);
}

int EditorStage::CheckExistFloor(const StageVec2& stagePos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(stagePos, &x, &z);

	for (int i = 0; i < floors.size(); i++) {
		if (x == floors[i]->GetPosition().x && z == floors[i]->GetPosition().z) {
			return i;
		}
	}

	return -1;
}
