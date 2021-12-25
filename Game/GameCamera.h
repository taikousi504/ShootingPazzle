#pragma once
#include <Camera.h>
#include "GameUtility.h"
#include "Timer.h"

class GameCamera :
    public Camera
{
private:
    //出現エフェクト
    Timer effectTimer;

    //エフェクト前位置
    Vector3 beforeFirstEffectPos;
    //エフェクト後位置
    Vector3 afterFirstEffectPos;
    //エフェクト前回転量
    Vector3 beforeFirstEffectRot;
    //エフェクト後回転量
    Vector3 afterFirstEffectRot;

public:
    void Initialize() override;

    void Update() override;

    /// <summary>
    /// 出現エフェクト
    /// </summary>
    void UpdateFirstEffect();

    /// <summary>
    /// クリアエフェクト
    /// </summary>
    void UpdateClearEffect();

    /// <summary>
    /// 射出後のパラメータでカメラセット
    /// </summary>
    void SetCameraParamAfterShoot();

    /// <summary>
    /// ステージの大きさからカメラ位置をセット
    /// </summary>
    /// <param name="stageSize">ステージの大きさ</param>
    /// <returns>セットされたY座標</returns>
    float SetPosFromStageSize(const StageVec2& stageSize);

    /// <summary>
    /// エフェクトタイマーをスタートさせる
    /// </summary>
    /// <param name="start"></param>
    /// <param name="end"></param>
    /// <param name="speed"></param>
    void StartEffectTimer(int start, int end, float speed = 1.0);
};

