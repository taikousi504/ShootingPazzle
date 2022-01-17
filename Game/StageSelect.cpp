#include "StageSelect.h"
#include "Input.h"
#include "GameUtility.h"
#include "Easing.h"
#include "DebugText.h"
#include "SceneManager.h"

const std::string StageSelect::STAGE_DIRECTORY = "StageData/";

StageSelect::StageSelect()
{
	//ステージロード
	for (int i = 0; i < STAGE_COUNT; i++) {
		stages.emplace_back(new Stage());
		stages[i]->LoadStage(STAGE_DIRECTORY + "stage_" + std::to_string(i) + ".spb");
		stages[i]->Initialize();
		stages[i]->SetMasterPosition(Vector3(0, 0, i * 500));

	}

	//UI画像読み込み
	buttonUp.LoadTexture(L"Resources/UI/UI_Arrow_Up.png");
	buttonDown.LoadTexture(L"Resources/UI/UI_Arrow_Down.png");
	buttonStart.LoadTexture(L"Resources/UI/UI_Start.png");

	//スプライト読み込み
	sprStageBG.Initialize();
	sprStageBG.SetTexture(L"Resources/Stage_BG.png");
	sprBackground.Initialize();
	sprBackground.SetTexture(L"Resources/Background.png");
	sprWrite.Initialize();
	sprWrite.SetTexture(L"Resources/Write1280x720.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(L"Resources/Black1280x720.png");
}

StageSelect::~StageSelect()
{
	//解放
	for (int i = 0; i < stages.size(); i++) {
		delete stages[i];
		stages[i] = nullptr;
	}
}

void StageSelect::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//カメラ初期化
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,150, -50 }, 15.0f);
	camera.SetRotation(75, 0, 0);
	//カメラをセット
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(80.0f);
	//ライトをセット
	Object3D::SetLight(&light);

	//フェーズセット
	GameUtility::SetNowPhase(PHASE_STAGESELECT_SELECT);

	//選択インデックス初期化
	nowSelectStageIndex = 1;

	//ステージ初期化

	//UIボタン初期化
	float adjust = 10;
	buttonUp.Initialize({ 0 + adjust, DX12Util::GetWindowHeight() - buttonDown.GetTexSize().y - buttonUp.GetTexSize().y - adjust * 2 });
	buttonDown.Initialize({ 0 + adjust, DX12Util::GetWindowHeight() - buttonDown.GetTexSize().y - adjust });
	buttonStart.Initialize({ DX12Util::GetWindowWidth() - buttonStart.GetTexSize().x - adjust, DX12Util::GetWindowHeight() - buttonStart.GetTexSize().y - adjust });

	//スプライト初期化
	sprBlack.SetColor({ 1,1,1,1 });

	//タイマーセット
	firstEffectTimer.SetTimer(0, 250);
	firstEffectTimer.Start();
	startGameTimer.SetTimer(0, 2000);
}

void StageSelect::Update()
{
	light.Update();
	UpdateCamera();
	UpdateTimer();

	//ステージ決定前処理
	if (startGameTimer.GetIsStart() == false) {
		UpdateNowSelect();
	}
	//ステージ決定後処理
	UpdateAfterDecided();

	UpdateStage();
	UpdateFG();
}

void StageSelect::Draw()
{
	DrawStage();
	DrawUI();
	DrawFG();
}

void StageSelect::UpdateCamera()
{
	//タイマー更新
	changeSelectPosTimer.Update();

	if (startGameTimer.GetIsStart() == false) {
		if (changeSelectPosTimer.GetIsStart()) {
			if (isMoveUp) {
				Vector3 setPos = camera.GetPosition();
				setPos.z = Easing::GetEaseValue(EASE_OUTQUINT, (nowSelectStageIndex - 1) * 500 - 50, nowSelectStageIndex * 500 - 50, changeSelectPosTimer);
				camera.SetPositionAndDistance(setPos, 15.0f);
				light.SetLightTarget({ 0,0,setPos.z + 50 });
				light.CalcLightPos(100.0f);
			}
			else {
				Vector3 setPos = camera.GetPosition();
				setPos.z = Easing::GetEaseValue(EASE_OUTQUINT, (nowSelectStageIndex + 1) * 500 - 50, nowSelectStageIndex * 500 - 50, changeSelectPosTimer);
				camera.SetPositionAndDistance(setPos, 15.0f);
				light.SetLightTarget({ 0,0,setPos.z + 50 });
				light.CalcLightPos(100.0f);
			}
		}
		else {
			Vector3 setPos = { 0,150,(float)nowSelectStageIndex * 500 - 50 };
			camera.SetPositionAndDistance(setPos, 15.0f);
			light.SetLightTarget({ 0,0,setPos.z + 50 });
			light.CalcLightPos(100.0f);
		}
	}
	else {
		Vector3 setPos = { 0,150,(float)nowSelectStageIndex * 500 - 50 };
		float addY = Easing::GetEaseValue(EASE_INBACK, 0, -150, startGameTimer);
		float addZ = Easing::GetEaseValue(EASE_INBACK, 0, 50, startGameTimer);

		setPos += {0, addY, addZ};

		camera.SetPositionAndDistance(setPos, 15.0f);
	}
	camera.Update();
}

void StageSelect::UpdateTimer()
{
	firstEffectTimer.Update();
	startGameTimer.Update();
}

void StageSelect::UpdateNowSelect()
{
	//カーソルを上に
	if (buttonUp.IsReleaseButton()) {
		buttonUp.StartPushedEffect();
		nowSelectStageIndex++;
		if (nowSelectStageIndex >= STAGE_COUNT) { 
			nowSelectStageIndex = STAGE_COUNT - 1;
		}
		else {
			//タイマースタート
			changeSelectPosTimer.SetTimer(0, 1000);
			changeSelectPosTimer.Start();
			//上向き移動
			isMoveUp = true;
		}
	}

	//カーソルを下に
	if (buttonDown.IsReleaseButton()) {
		buttonDown.StartPushedEffect();
		nowSelectStageIndex--;
		if (nowSelectStageIndex < 0) {
			nowSelectStageIndex = 0; 
		}
		else {
			//タイマースタート
			changeSelectPosTimer.SetTimer(0, 1000);
			changeSelectPosTimer.Start();
			//下向き移動
			isMoveUp = false;
		}
	}

	//選択されたステージでゲームスタート
	if (buttonStart.IsReleaseButton()) {
		buttonStart.StartPushedEffect();
		startGameTimer.Start();
	}
}

void StageSelect::UpdateStage()
{
	//ステージ更新
	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Update();
	}
}

void StageSelect::UpdateAfterDecided()
{
	//エフェクト終了後にゲームプレイシーンへ
	if (startGameTimer.GetIsEnd())
	{
		//ステージセット
		GameUtility::SetNowStagePath(STAGE_DIRECTORY + "stage_" + std::to_string(nowSelectStageIndex) + ".spb");
		//シーンを移す
		SceneManager::ChangeScene("GamePlay");
	}
}

void StageSelect::UpdateFG()
{
}

void StageSelect::DrawStage()
{
	sprBackground.DrawBG();
	sprStageBG.DrawBG();

	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Draw();
	}
}

void StageSelect::DrawUI()
{
	buttonUp.Draw();
	buttonDown.Draw();
	buttonStart.Draw();
}

void StageSelect::DrawFG()
{
	//黒
	float alpha = 1.0f - ((float)firstEffectTimer.GetNowTime() / firstEffectTimer.GetEndTime());
	sprBlack.SetColor({ 1,1,1,alpha });
	sprBlack.DrawFG();

	//白
	alpha = Easing::GetEaseValue(EASE_INQUART, 0, 1, startGameTimer);
	sprWrite.SetColor({ 1, 1, 1, alpha });
	sprWrite.DrawFG();
}
