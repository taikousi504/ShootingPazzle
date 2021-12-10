#pragma once
#include "BaseFloor.h"
class NormalFloor :
    public BaseFloor
{
private:
	static ObjModel modelBox;

public:
	static void CreateModel();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const StageVec2& pos) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	void UpdateCollision() override;


	virtual std::string GetObjectType() override { return "NormalFloor"; }
};

