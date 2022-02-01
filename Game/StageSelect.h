#pragma once
#include <vector>
#include <string>
#include <Scene.h>
#include "Light.h"
#include "Camera.h"
#include "Stage.h"
#include "UISquareButton.h"
#include "Sprite.h"
#include "Particle2D.h"

class StageSelect :
    public Scene
{
public:
    //ステージ数
    static const int STAGE_COUNT = 6;
    static const std::string STAGE_DIRECTORY;

private:
    //ライト
    Light light;

    //カメラ
    Camera camera;

    //ステージコンテナ
    std::vector<Stage*> stages;

    //現在選択されているステージインデックス
    int nowSelectStageIndex = 1;

    //移動が上向きかどうか
    bool isMoveUp = false;

    //開幕エフェクトタイマー
    Timer firstEffectTimer;

    //選択されているステージが変わった時に開始するタイマー
    Timer changeSelectPosTimer;

    //ゲーム開始エフェクトタイマー
    Timer startGameTimer;

    //繰り返しエフェクトタイマー
    Timer roopEffectTimer;

    //矢印エフェクトタイマー
    Timer arrowTimer;

    //UI 上ボタン
    UISquareButton buttonUp;

    //UI 下ボタン
    UISquareButton buttonDown;

    //UI STARTボタン
    UISquareButton buttonStart;

    //背景(白い丸)
    Sprite sprStageBG;

    //背景(黒)
    Sprite sprBackground;

    //「Stage」文字
    Sprite sprTextStage;

    //ステージ数字
    Sprite sprStageNum[2];

    //前景(白)
    Sprite sprWrite;

    //前景(黒)
    Sprite sprBlack;

    //移動可能なことを表す矢印オブジェクト
    ObjModel modelArrowUp;
    ObjModel modelArrowDown;
    Object3D objArrowUp;
    Object3D objArrowDown;


public:
    StageSelect();
    ~StageSelect();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;

    void UpdateCamera();
    void UpdateTimer();
    void UpdateNowSelect();
    void UpdateStage();
    void UpdateAfterDecided();
    void UpdateStageNumTex();

    void DrawStage();
    void DrawUI();
    void DrawFG();
    void DrawStageNumTex();
};

