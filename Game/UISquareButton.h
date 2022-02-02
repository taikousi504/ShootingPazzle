#pragma once
#include "Sprite.h"
#include "Timer.h"
#include <string>

/// <summary>
/// 四角形ボタンUIクラス
/// </summary>
class UISquareButton
{
private:
	//ボタンテクスチャ
	Sprite textureOn;
	Sprite textureOff;
	Sprite textureAdd;

	Vector2 pos;
	Vector2 size;

	Timer pushedEffectTimer;

	bool isEnable = true;

	bool prevOverlap = false;

public:

	void LoadTexture(const std::wstring& texturePath);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="texturePath">画像パス 画像は左半分にoff、右半分にon時の画像をつなげたものを使用/param>
	/// <param name="pos">座標</param>
	void Initialize(const Vector2& pos);

	/// <summary>
	/// 描画 各種更新も兼ねる
	/// </summary>
	void Draw();

	/// <summary>
	/// 押されたときのエフェクト開始
	/// </summary>
	void StartPushedEffect();

	/// <summary>
	/// マウスカーソルがボタンに重なっているかを返す
	/// </summary>
	/// <returns></returns>
	bool IsOverlapMouseCursol();

	/// <summary>
	/// ボタンが押されているかを返す
	/// </summary>
	/// <returns></returns>
	bool IsPushButton();

	/// <summary>
	/// ボタンを押した瞬間か返す
	/// </summary>
	/// <returns></returns>
	bool IsTriggerButton();

	/// <summary>
	/// ボタンが押された状態から離されたかどうか返す
	/// </summary>
	/// <returns></returns>
	bool IsReleaseButton();

	/// <summary>
	/// サウンド更新
	/// </summary>
	void UpdateSound();

	void SetPosition(const Vector2& pos);

	void SetColor(const Vector4& color);

	/// <summary>
	/// カーソルとボタンが重なったとき、テクスチャを変化させるか
	/// </summary>
	/// <param name="flag"></param>
	void SetIsEnable(bool flag) {
		if (isEnable) {
			UpdateSound();
		}
		isEnable = flag;
	}

	const Vector2& GetPosition() { return pos; }

	const Vector2& GetTexSize() { return size; }
};

