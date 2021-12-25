#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "BaseBlock.h"
#include "CollisionManager.h"
#include "Stage.h"

class MyBullet
{
private:
	//壁などに衝突したときに参照する構造体
	struct NextMoveInfo {
		//次フレームの位置
		Vector3 nextPos = {};
		//次フレームの移動量
		Vector3 nextVel = {};
		//次フレームに衝突するか
		bool isCollisionNextFrame = false;
	};

private:
	//半径
	static const float RADIUS;

	//球モデル
	static ObjModel modelSphere;
	//矢印モデル
	static ObjModel modelArrow;

	//座標
	Vector3 position = {};
	//移動量
	Vector3 velocity = {};
	//移動速度
	float speed = 2.0f;
	//射出角度
	float shotAngle = 0;
	//摩擦
	float friction = 0.0005f;
	//重力
	float gravity = 0;
	//射出フラグ
	bool isShoot = false;
	//レイ
	Ray ray;
	//衝突フレームの位置や移動量をまとめた構造体
	NextMoveInfo nextMoveInfo;
	//ステージデータ
	Stage* stage = nullptr;
	//球オブジェクト
	Object3D objSphere;
	//矢印オブジェクト
	Object3D objArrow;

	//エフェクトタイマー
	Timer firstEffectTimer;
	//跳ねるエフェクトの初期Y座標 カメラの位置と同期させる
	float bounceInitPosY = 200;

public:
	static void CreateModel();

public:
	MyBullet() {}

	~MyBullet() {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 出現エフェクト時の更新
	/// </summary>
	void UpdateFirstEffect();

	/// <summary>
	/// クリアエフェクト更新
	/// </summary>
	void UpdateClearEffect();

	/// <summary>
	/// 射出前の更新
	/// </summary>
	void UpdateBeforeShoot();

	/// <summary>
	/// 弾を撃つ位置を決める
	/// </summary>
	void DecideShootPos();

	/// <summary>
	/// 弾を撃つ角度を決める
	/// </summary>
	float DecideShootAngle();

	/// <summary>
	/// 弾をセットされた角度で射出
	/// </summary>
	void Shoot();

	/// <summary>
	/// 弾を動かす
	/// </summary>
	void Move();

	/// <summary>
	/// 摩擦を適用
	/// </summary>
	void ApplyFriction();

	/// <summary>
	/// 重力を適用
	/// </summary>
	void ApplyGravity();

	/// <summary>
	/// 衝突チェック
	/// </summary>
	void CheckCollision();

	/// <summary>
	/// ブロックとの衝突チェック
	/// </summary>
	void CheckBlockCollision();

	/// <summary>
	/// 床との衝突チェック
	/// </summary>
	void CheckFloorCollision();

	/// <summary>
	/// レイの位置方角更新
	/// </summary>
	void UpdateRay();

	/// <summary>
	/// 引数の位置がステージの外にあるか
	/// </summary>
	/// <returns></returns>
	bool IsOutStage(const Vector3& pos);

#pragma region Setter

	/// <summary>
	/// ステージポインタセット
	/// </summary>
	/// <param name="blocks"></param>
	void SetPStage(Stage* stage) { this->stage = stage; }

	/// <summary>
	/// 跳ねるエフェクトの初期Y座標セット
	/// </summary>
	/// <param name="y"> 跳ねるエフェクトの初期Y座標</param>
	void SetBounceInitPosY(float y) { bounceInitPosY = y; }
#pragma endregion

#pragma region Getter

#pragma endregion
};

