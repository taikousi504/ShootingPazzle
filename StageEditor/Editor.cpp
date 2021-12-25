#include <fstream>
#include "Editor.h"
#include "DX12Util.h"
#include "Input.h"
#include "Sound.h"
#include "DebugText.h"
#include "Collision.h"
#include "FPSManager.h"
#include "FbxLoader.h"
#include "ImguiHelper.h"
#include "GameUtility.h"
#include "TurnFloor.h"
#include "SwitchFloor.h"

#include "../DX12Library/imgui/imgui.h"
#include "../DX12Library/imgui/imgui_impl_win32.h"
#include "../DX12Library/imgui/imgui_impl_dx12.h"

void Editor::Initialize()
{
	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 1,-1,0 });
	light.SetLightColor({ 0.8f, 0.8f, 0.8f });
	Object3D::SetLight(&light);

	//カメラ初期化
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,100.0f,0.0f }, 30.0f);
	camera.SetRotation(90, 0, 0);

	//カメラをセット
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//タイマーセット
	timer.SetTimer(0, 1000000);
	timer.Start();

	//ステージ取得
	stage.LoadStage("");
	sliderWidth = stage.stageSize.x;
	sliderDepth = stage.stageSize.y;

	//フェーズ初期化
	GameUtility::SetNowPhase(PHASE_SETPOS);

	//オブジェクト初期化
	squareBlock.Initialize({ 0,0 }, ONE_CELL_LENGTH / 2);
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].Initialize({ 0,0 }, ONE_CELL_LENGTH / 2);
		triangleBlock[i].SetTriangleType(i);
	}
	for (int i = 0; i < _countof(startLane); i++) {
		startLane[i].Initialize(&stage.stageSize);
	}
#pragma region 表示用オブジェクト初期化
	modelNormalFloor.CreateFromOBJ("NormalFloor");
	modelTurnFloor[TURNTYPE_LEFT].CreateFromOBJ("TurnFloor_Left");
	modelTurnFloor[TURNTYPE_RIGHT].CreateFromOBJ("TurnFloor_Right");
	modelTurnFloor[TURNTYPE_UP].CreateFromOBJ("TurnFloor_Up");
	modelTurnFloor[TURNTYPE_DOWN].CreateFromOBJ("TurnFloor_Down");
	modelSwitchFloor[SWITCH_STATE_OFF].CreateFromOBJ("SwitchFloor_OFF");
	modelSwitchFloor[SWITCH_STATE_ON].CreateFromOBJ("SwitchFloor_ON");

	objDispFloor.Initialize();
	objDispFloor.SetObjModel(&modelNormalFloor);
	objDispFloor.SetColor(0.5f, 0.5f, 0.5f, 1);
#pragma endregion
}

void Editor::Update()
{
	// DirectX 毎フレーム処理 ここから

	//入力処理ここから

#pragma region カメラ
#ifdef _DEBUG
	if (isEnteringIOName == false) {
		//カメラ移動
		if (Keyboard::IsKeyPush(DIK_W)) {
			camera.MoveCamera(0, 0, 1, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_S)) {
			camera.MoveCamera(0, 0, -1, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_A)) {
			camera.MoveCamera(-1, 0, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_D)) {
			camera.MoveCamera(1, 0, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_E)) {
			camera.MoveCamera(0, 1, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_X)) {
			camera.MoveCamera(0, -1, 0, false, true, false);
		}

		//カメラ回転
		if (Keyboard::IsKeyPush(DIK_UP)) {
			Vector3 f = { camera.GetRotation().x - 1.5f, camera.GetRotation().y, camera.GetRotation().z };
			camera.SetRotation(f);
		}
		if (Keyboard::IsKeyPush(DIK_DOWN)) {
			Vector3 f = { camera.GetRotation().x + 1.5f, camera.GetRotation().y, camera.GetRotation().z };
			camera.SetRotation(f);
		}
		if (Keyboard::IsKeyPush(DIK_LEFT)) {
			//Shiftが押されていたらZ軸回転
			if (Keyboard::IsKeyPush(DIK_LSHIFT) || Keyboard::IsKeyPush(DIK_RSHIFT)) {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y, camera.GetRotation().z + 1.5f };
				camera.SetRotation(f);
			}
			else {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y - 1.5f, camera.GetRotation().z };
				camera.SetRotation(f);
			}
		}
		if (Keyboard::IsKeyPush(DIK_RIGHT)) {
			if (Keyboard::IsKeyPush(DIK_LSHIFT) || Keyboard::IsKeyPush(DIK_RSHIFT)) {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y, camera.GetRotation().z - 1.5f };
				camera.SetRotation(f);
			}
			else {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y + 1.5f, camera.GetRotation().z };
				camera.SetRotation(f);
			}
		}
	}
#endif
#pragma endregion
#pragma region サウンド

#pragma endregion
#pragma region オブジェクト
	UpdateEdit();
#pragma endregion
#pragma region アップデート
	light.Update();
	camera.Update();
	timer.Update();
	//3Dサウンドで使用するリスナーの位置更新
	Sound::Set3DListenerPosAndVec(camera);

#pragma endregion

#ifdef _DEBUG
	//リセット
	if (Keyboard::IsKeyTrigger(DIK_R) && isEnteringIOName == false) {
		Initialize();
	}

#endif


}

void Editor::Draw()
{
	//背景スプライト描画ここから
	Sprite::BeginDraw();
	//spr->Draw();

	//背景描画ここまで
	DX12Util::ClearDepthBuffer();

	//オブジェクト描画ここから
	Object3D::BeginDraw();
	DrawEdit();

	//オブジェクト描画ここまで

	//前景スプライト描画ここから
	ImguiHelper::Draw();

	//前景スプライト描画ここまで

}

void Editor::UpdateEdit()
{
	UpdateImgui();
	CalcNowCursolPos();
	//ステージ更新
	stage.Update();
	UpdateObject();

	if (mode == MODE_ADD) {
		UpdateAdd();
	}
	else if (mode == MODE_DELETE) {
		UpdateDelete();
	}

}

void Editor::UpdateAdd()
{
	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	if (objectType == OBJECTTYPE_BLOCK) {
		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			stage.AddBlock(nowCursolPos, blockType, breakupCount);
		}	}
	else if (objectType == OBJECTTYPE_FLOOR) {
		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			stage.AddFloor(nowCursolPos, floorType);
		}
	}
}

void Editor::UpdateDelete()
{
	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	if (objectType == OBJECTTYPE_BLOCK) {
		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			stage.DeleteBlock(nowCursolPos);
		}
	}
	else if (objectType == OBJECTTYPE_FLOOR) {
		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			stage.DeleteFloor(nowCursolPos);
		}
	}
}

void Editor::UpdateObject()
{
	UpdateDispObject();

	//ブロック位置をカーソル位置に
	squareBlock.SetStagePos(nowCursolPos);
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].SetStagePos(nowCursolPos);
	}

	float x, z;
	GameUtility::CalcStagePos2WorldPos(nowCursolPos, &x, &z);
	objDispFloor.SetPosition({x, -ONE_CELL_LENGTH / 2, z});

	//breakupCountによってブロックの色を変える
	squareBlock.SetBreakupCount(breakupCount);
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].SetBreakupCount(breakupCount);
	}

	//更新
	UpdateStartLane();
	squareBlock.Update();
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].Update();
	}
	objDispFloor.Update();
}

void Editor::UpdateDispObject()
{
	objDispFloor.SetColor(0.5f, 0.5f, 0.5f, 1);
	switch (floorType) {
	case FLOORTYPE_NORMAL:			objDispFloor.SetObjModel(&modelNormalFloor); break;
	case FLOORTYPE_TURN_LEFT:		objDispFloor.SetObjModel(&modelTurnFloor[TURNTYPE_LEFT]); break;
	case FLOORTYPE_TURN_RIGHT:		objDispFloor.SetObjModel(&modelTurnFloor[TURNTYPE_RIGHT]); break;
	case FLOORTYPE_TURN_UP:			objDispFloor.SetObjModel(&modelTurnFloor[TURNTYPE_UP]); break;
	case FLOORTYPE_TURN_DOWN:		objDispFloor.SetObjModel(&modelTurnFloor[TURNTYPE_DOWN]); break;
	case FLOORTYPE_BREAK:			objDispFloor.SetObjModel(&modelBreakFloor); break;
	case FLOORTYPE_SWITCH_WHITE:	objDispFloor.SetObjModel(&modelSwitchFloor[SWITCH_STATE_OFF]); break;
	case FLOORTYPE_SWITCH_RED:		objDispFloor.SetObjModel(&modelSwitchFloor[SWITCH_STATE_OFF]);
									objDispFloor.SetColor(0.5f, 0.0f, 0.0f, 1); break;
	case FLOORTYPE_SWITCH_BLUE:		objDispFloor.SetObjModel(&modelSwitchFloor[SWITCH_STATE_OFF]);
									objDispFloor.SetColor(0.0f, 0.0f, 0.5f, 1); break;
	case FLOORTYPE_SWITCH_YELLOW:	objDispFloor.SetObjModel(&modelSwitchFloor[SWITCH_STATE_OFF]);
									objDispFloor.SetColor(0.5f, 0.5f, 0.0f, 1); break;
	case FLOORTYPE_SWITCH_GREEN:	objDispFloor.SetObjModel(&modelSwitchFloor[SWITCH_STATE_OFF]);
									objDispFloor.SetColor(0.0f, 0.5f, 0.0f, 1); break;
	}
}

void Editor::UpdateStartLane()
{
	//0...設置されたスタートレーン
	startLane[0].SetPosition(stage.startLaneZ);
	startLane[0].Update();
	//1...カーソル位置に描画されるスタートレーン
	if (mode == MODE_OPTION && optionMode == OPTION_SET_STARTLANE) {
		//ステージ範囲外なら即リターン
		if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
			nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
			return;
		}
		startLane[1].SetPosition(nowCursolPos.y);

		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			stage.startLaneZ = nowCursolPos.y;
		}
	}
	startLane[1].Update();
}

void Editor::DrawEdit()
{
	//ステージ描画
	stage.Draw();

	//スタートレーン描画
	DrawStartLane();

	if (mode == MODE_ADD) {
		//カーソル位置にブロック描画
		if (objectType == OBJECTTYPE_BLOCK) {
			DrawBlock();
		}
		else if (objectType == OBJECTTYPE_FLOOR) {
			DrawFloor();
		}
	}
	else if (mode == MODE_DELETE) {

	}

	stage.EndDraw();
}

void Editor::DrawStartLane()
{
	//0...設置されたスタートレーン
	startLane[0].Draw();

	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	//1...カーソル位置に描画されるスタートレーン
	if (mode == MODE_OPTION && optionMode == OPTION_SET_STARTLANE) {
		startLane[1].Draw();
	}
}

void Editor::DrawBlock()
{
	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	if (blockType == BLOCKTYPE_SQUARE) {
		squareBlock.Draw();
	}
	else if (blockType == BLOCKTYPE_TRIANGLE_NO_LEFTTOP) {
		triangleBlock[TRIANGLETYPE_NO_LEFTTOP].Draw();
	}
	else if (blockType == BLOCKTYPE_TRIANGLE_NO_RIGHTTOP) {
		triangleBlock[TRIANGLETYPE_NO_RIGHTTOP].Draw();
	}
	else if (blockType == BLOCKTYPE_TRIANGLE_NO_LEFTBOTTOM) {
		triangleBlock[TRIANGLETYPE_NO_LEFTBOTTOM].Draw();
	}
	else if (blockType == BLOCKTYPE_TRIANGLE_NO_RIGHTBOTTOM) {
		triangleBlock[TRIANGLETYPE_NO_RIGHTBOTTOM].Draw();
	}	
}

void Editor::DrawFloor()
{
	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	objDispFloor.Draw();
}

void Editor::Save()
{
	std::ofstream file;
	//バイナリモードで開く
	file.open("./StageData/" + ioname + ".spb", std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//ファイルヘッダー
	StageHeader header = {};
	header.width = stage.stageSize.x;
	header.depth = stage.stageSize.y;
	header.startLineZ = stage.startLaneZ;
	header.blockCount = stage.blocks.size();
	header.floorCount = stage.floors.size();

	file.write((char*)&header, sizeof(header));

	//データ部
	for (int i = 0; i < stage.blocks.size(); i++) {
		StageBlock object = {};
		std::string blockType = stage.blocks[i]->GetObjectType();

		if (blockType == "SquareBlock") {
			object.type = 0;
		}
		else if (blockType == "TriangleBlock_0") {
			object.type = 1;
		}
		else if (blockType == "TriangleBlock_1") {
			object.type = 2;
		}
		else if (blockType == "TriangleBlock_2") {
			object.type = 3;
		}
		else if (blockType == "TriangleBlock_3") {
			object.type = 4;
		}

		StageVec2 pos = GameUtility::CalcWorldPos2StagePos(
			stage.blocks[i]->GetPosition().x, stage.blocks[i]->GetPosition().z);
		object.stagePosX = pos.x;
		object.stagePosY = pos.y;
		object.breakupCount = stage.blocks[i]->GetBreakupCount();

		file.write((char*)&object, sizeof(object));
	}

	for (int i = 0; i < stage.floors.size(); i++) {
		StageFloor object = {};
		std::string floorType = stage.floors[i]->GetObjectType();

#pragma region SetType
		if (floorType == "NormalFloor") {
			object.type = 0;
		}
		else if (floorType == "TurnFloor_0") {
			object.type = 1;
		}
		else if (floorType == "TurnFloor_1") {
			object.type = 2;
		}
		else if (floorType == "TurnFloor_2") {
			object.type = 3;
		}
		else if (floorType == "TurnFloor_3") {
			object.type = 4;
		}
		else if (floorType == "BreakFloor") {
			object.type = 5;
		}
		else if (floorType == "SwitchFloor_0") {
			object.type = 6;
		}
		else if (floorType == "SwitchFloor_1") {
			object.type = 7;
		}
		else if (floorType == "SwitchFloor_2") {
			object.type = 8;
		}
		else if (floorType == "SwitchFloor_3") {
			object.type = 9;
		}
		else if (floorType == "SwitchFloor_4") {
			object.type = 10;
		}
#pragma endregion

		StageVec2 pos = GameUtility::CalcWorldPos2StagePos(
			stage.floors[i]->GetPosition().x, stage.floors[i]->GetPosition().z);

		object.stagePosX = pos.x;
		object.stagePosY = pos.y;

		file.write((char*)&object, sizeof(object));
	}

	file.close();
}

void Editor::Load()
{
	//ステージロード
	stage.LoadStage("./StageData/" + ioname + ".spb");
	//ImGuiのスライダーの値を初期化しておく
	sliderWidth = stage.stageSize.x;
	sliderDepth = stage.stageSize.y;
	//スタートレーンモデル再生成
	for (int i = 0; i < _countof(startLane); i++) {
		startLane[i].CreateModel();
	}
}

void Editor::UpdateImgui()
{
	//ImGui
	ImguiHelper::BeginCommand("Settings");
	ImguiHelper::SetWindowSize(250, 720);
	ImguiHelper::SetWindowPos(1280 - 250, 0);
	//Mode
	ImGui::Text("Mode");
	ImGui::RadioButton("Add", &mode, MODE_ADD);
	ImGui::SameLine();
	ImGui::RadioButton("Delete", &mode, MODE_DELETE);
	ImGui::SameLine();
	ImGui::RadioButton("Option", &mode, MODE_OPTION);

	if (mode == MODE_ADD) {
		//ObjectType
		ImGui::Text("ObjectType");
		ImGui::RadioButton("Block", &objectType, OBJECTTYPE_BLOCK);
		ImGui::SameLine();
		ImGui::RadioButton("Floor", &objectType, OBJECTTYPE_FLOOR);

		if (objectType == OBJECTTYPE_BLOCK) {
			ImGui::Text("BlockType");
			ImGui::RadioButton("Square", &blockType, BLOCKTYPE_SQUARE);
			ImGui::RadioButton("Triangle_No_LeftTop", &blockType, BLOCKTYPE_TRIANGLE_NO_LEFTTOP);
			ImGui::RadioButton("Triangle_No_RightTop", &blockType, BLOCKTYPE_TRIANGLE_NO_RIGHTTOP);
			ImGui::RadioButton("Triangle_No_LeftBottom", &blockType, BLOCKTYPE_TRIANGLE_NO_LEFTBOTTOM);
			ImGui::RadioButton("Triangle_No_RightBottom", &blockType, BLOCKTYPE_TRIANGLE_NO_RIGHTBOTTOM);

			static int sliderBreakupCount = 0;
			ImGui::SliderInt("BreakupCount", &sliderBreakupCount, 0, 2);
			breakupCount = sliderBreakupCount;
		}
		else if (objectType == OBJECTTYPE_FLOOR) {
			ImGui::Text("FloorType");
			ImGui::RadioButton("Normal", &floorType, FLOORTYPE_NORMAL);
			ImGui::RadioButton("Turn_Left", &floorType, FLOORTYPE_TURN_LEFT);
			ImGui::RadioButton("Turn_Right", &floorType, FLOORTYPE_TURN_RIGHT);
			ImGui::RadioButton("Turn_Up", &floorType, FLOORTYPE_TURN_UP);
			ImGui::RadioButton("Turn_Down", &floorType, FLOORTYPE_TURN_DOWN);
			ImGui::RadioButton("Break", &floorType, FLOORTYPE_BREAK);
			ImGui::RadioButton("Switch_White", &floorType, FLOORTYPE_SWITCH_WHITE);
			ImGui::RadioButton("Switch_Red", &floorType, FLOORTYPE_SWITCH_RED);
			ImGui::RadioButton("Switch_Blue", &floorType, FLOORTYPE_SWITCH_BLUE);
			ImGui::RadioButton("Switch_Yellow", &floorType, FLOORTYPE_SWITCH_YELLOW);
			ImGui::RadioButton("Switch_Green", &floorType, FLOORTYPE_SWITCH_GREEN);
		}

	}
	else if (mode == MODE_DELETE) {
		//ObjectType
		ImGui::Text("ObjectType");
		ImGui::RadioButton("Block", &objectType, OBJECTTYPE_BLOCK);
		ImGui::SameLine();
		ImGui::RadioButton("Floor", &objectType, OBJECTTYPE_FLOOR);

		if (objectType == OBJECTTYPE_BLOCK) {
			ImGui::NewLine();
			if (ImGui::Button("All Delete")) {
				//全て削除
				for (int i = 0; i < stage.blocks.size(); i++) {
					if (stage.blocks[i]) delete stage.blocks[i];
					stage.blocks.erase(stage.blocks.begin() + i);
					i--;
				}
			}
		}
		else if (objectType == OBJECTTYPE_FLOOR) {
			ImGui::NewLine();
			if (ImGui::Button("All Delete")) {
				//全て削除
				for (int i = 0; i < stage.floors.size(); i++) {
					if (stage.floors[i]) delete stage.floors[i];
					stage.floors.erase(stage.floors.begin() + i);
					i--;
				}
			}
		}
	}
	else if (mode == MODE_OPTION) {
		//ステージサイズ変更
		ImGui::SliderInt("StageWidth", &sliderWidth, 5, 50);
		ImGui::SliderInt("StageDepth", &sliderDepth, 5, 50);
		//大きさが変更されたら地面再生成
		if (sliderWidth != stage.stageSize.x || sliderDepth != stage.stageSize.y) {
			ReCreateStage(sliderWidth, sliderDepth);
		}
		//オプションモード選択
		ImGui::Text("OptionMode");
		ImGui::RadioButton("SetStartLane", &optionMode, OPTION_SET_STARTLANE);
	}

	ImGui::NewLine();

	static char c[128] = "test";
	ImGui::InputText("IO_Name", c, sizeof(c));
	ioname = c;
	isEnteringIOName = ImGui::IsItemActive();

	if (ImGui::Button("Save")) {
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		Load();
	}

	ImguiHelper::EndCommand();
}

void Editor::CalcNowCursolPos()
{
	Vector3 mouse;
	//マウスと地面との当たり判定
	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage.GetFloorCollision(), nullptr, &mouse);
	//DebugText::Print("WorldPos:(" + std::to_string(mouse.x) + ", " + std::to_string(mouse.z) + ")", 0, 0);
	StageVec2 newPos = GameUtility::CalcWorldPos2StagePos(mouse.x, mouse.z);
	nowCursolPos = newPos;

	//DebugText::Print("StagePos:(" + std::to_string(nowCursolPos.x) + ", " + std::to_string(nowCursolPos.y) + ")", 0, 20);
}

void Editor::ReCreateStage(unsigned short width, unsigned short depth)
{
	//まず変動したステージサイズに合わせてオブジェクト移動
	StageVec2 prev = stage.stageSize;
	for (int i = 0; i < stage.blocks.size(); i++) {
		//CalcWorldPos2StagePosでstage.stageSizeが参照されるので変更前の値を代入
		stage.stageSize = prev;
		Vector3 wpos = stage.blocks[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		//SetStagePosで変更後の座標にするために新ステージサイズを代入
		stage.stageSize.x = width;
		stage.stageSize.y = depth;
		stage.blocks[i]->SetStagePos(stagePos);
	}

	for (int i = 0; i < stage.floors.size(); i++) {
		//CalcWorldPos2StagePosでstage.stageSizeが参照されるので変更前の値を代入
		stage.stageSize = prev;
		Vector3 wpos = stage.floors[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		//SetStagePosで変更後の座標にするために新ステージサイズを代入
		stage.stageSize.x = width;
		stage.stageSize.y = depth;
		stage.floors[i]->SetStagePos(stagePos);
	}

	stage.stageSize.x = width;
	stage.stageSize.y = depth;

	//ブロックがステージ外に配置されていたら削除
	for (int i = 0; i < stage.blocks.size(); i++) {

		Vector3 wpos = stage.blocks[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		if (stagePos.x < 0 || stagePos.x >= stage.stageSize.x ||
			stagePos.y < 0 || stagePos.y >= stage.stageSize.y) {
			if (stage.blocks[i]) delete stage.blocks[i];
			stage.blocks.erase(stage.blocks.begin() + i);
			i--;
		}
	}

	//床ブロックがステージ外に配置されていたら削除
	for (int i = 0; i < stage.floors.size(); i++) {

		Vector3 wpos = stage.floors[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		if (stagePos.x < 0 || stagePos.x >= stage.stageSize.x ||
			stagePos.y < 0 || stagePos.y >= stage.stageSize.y) {
			if (stage.floors[i]) delete stage.floors[i];
			stage.floors.erase(stage.floors.begin() + i);
			i--;
		}
	}

	stage.startLaneZ = stage.stageSize.y - 2;

	//スタートレーンモデル再生成
	for (int i = 0; i < _countof(startLane); i++) {
		startLane[i].CreateModel();
	}
}

//bool blnChk = false;
//ImGui::Checkbox("CheckboxTest", &blnChk);
//int radio = 0;
//ImGui::RadioButton("Radio 1", &radio, 0);
//ImGui::SameLine();
//ImGui::RadioButton("Radio 2", &radio, 1);
//ImGui::SameLine();
//ImGui::RadioButton("Radio 3", &radio, 2);
//int nSlider = 0;
//ImGui::SliderInt("Int Slider", &nSlider, 0, 100);
//float fSlider = 0.0f;
//ImGui::SliderFloat("Float Slider", &fSlider, 0.0f, 100.0f);
//float col3[3] = {};
//ImGui::ColorPicker3("ColorPicker3", col3, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
//float col4[4] = {};
//ImGui::ColorPicker4(" ColorPicker4", col4, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | 
//ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

