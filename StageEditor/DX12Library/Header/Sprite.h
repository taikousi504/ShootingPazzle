#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <unordered_map>
#include "Vector3.h"
#pragma comment(lib, "d3d12.lib")

class Sprite
{
public:
	using XMFLOAT4 = DirectX::XMFLOAT4;
	//using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

//サブクラス
public:
	struct VertexPosUv

	{
		Vector3 pos;	//xyz座標
		XMFLOAT2 uv;	//uv座標
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		XMFLOAT4 color; // 色 (RGBA)
		XMMATRIX mat;	//3D変換行列
	};

//定数
private:
	static const int spriteSRVCount;

//静的メンバ変数
protected:

	// 頂点数
	static const int vertNum = 4;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> spriteRootSignature;
	//パイプラインステート
	static ComPtr<ID3D12PipelineState> spritePipelineState;
	//射影行列
	static XMMATRIX matProjection;
	//頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> vsBlob;
	//ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> psBlob;
	//エラーオブジェクト
	static ComPtr <ID3DBlob> errorBlob;
	//デスクリプタヒープ
	static ComPtr <ID3D12DescriptorHeap> descHeap;
	//テクスチャバッファ
	static ComPtr <ID3D12Resource> texbuff[];
	//ロードされたテクスチャのファイル名と番号を紐づけるリスト
	static std::unordered_map<const wchar_t*, UINT> loadedTextureList;
	//ロードされたテクスチャ数
	static UINT loadedTextureCount;

//静的メンバ関数
public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	static void FirstInit();

	/// <summary>
	/// スプライトのテクスチャ読み込み処理
	/// </summary>
	/// <param name="texnumber">テクスチャ番号</param>
	/// <param name="filename">ファイルパス</param>
	/// <returns>読みこんだ画像のテクスチャ番号</returns>
	static UINT LoadTexture(const wchar_t* filename);

	/// <summary>
	/// スプライトパイプラインをセット スプライト描画前に必ず書く
	/// </summary>
	static void BeginDraw();

//メンバ関数
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 頂点バッファを更新
	/// </summary>
	void UpdateVertBuff();


//メンバ変数
protected:
	float rotation = 0.0f;		//Z軸回りの回転角
	XMVECTOR position{};			//座標
	XMMATRIX matWorld{};			//ワールド行列
	XMFLOAT4 color = { 1,1,1,1 };	//色
	UINT texNumber;				//テクスチャ番号
	XMFLOAT2 anchorpoint{};		//アンカーポイント
	float width = 0;		//横幅
	float height = 0;		//縦幅
	bool isFlipX = false;		//左右入れ替え
	bool isFlipY = false;		//上下入れ替え
	float tex_x = 0;		//画像切り出しX座標
	float tex_y = 0;		//画像切り出しy座標
	float tex_width = 0;	//画像切り出し横幅
	float tex_height = 0;	//画像切り出し縦幅
	bool isDisplay = true;		//表示状態
	ComPtr<ID3D12Resource> vertBuff;				//頂点バッファ
	ComPtr<ID3D12Resource> constBuff;				//定数バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView{};			//頂点バッファビュー


public:
#pragma region ゲッター
	/// <summary>
	/// 座標を取得する
	/// </summary>
	/// <returns>座標</returns>
	XMFLOAT2 GetPosition()
	{
		return { DirectX::XMVectorGetX(position), DirectX::XMVectorGetY(position) };
	}

	/// <summary>
	/// 色情報を取得する
	/// </summary>
	/// <returns>色情報</returns>
	XMFLOAT4 GetColor() { return color; }

	/// <summary>
	/// Z軸周りの回転角を取得する
	/// </summary>
	/// <returns>Z軸周りの回転角</returns>
	float GetRotation() { return rotation; }

	/// <summary>
	/// スケールを取得する
	/// </summary>
	/// <returns>スケール</returns>
	XMFLOAT2 GetScale() { return { width,height }; }

	/// <summary>
	/// 横反転フラグを取得する
	/// </summary>
	/// <returns>横反転フラグ</returns>
	bool GetIsFlipX() { return isFlipX; }

	/// <summary>
	/// 縦反転フラグを取得する
	/// </summary>
	/// <returns>縦反転フラグ</returns>
	bool GetIsFlipY() { return isFlipY; }

	/// <summary>
	/// 表示フラグを取得する
	/// </summary>
	/// <returns>表示フラグ</returns>
	bool GetIsDisplay() { return isDisplay; }

#pragma endregion

#pragma region セッター
	/// <summary>
	/// 座標をセットする
	/// </summary>
	/// <param name="pos">座標</param>
	void SetPosition(XMFLOAT2 pos);

	/// <summary>
	/// 色情報をセットする
	/// </summary>
	/// <param name="color">色情報</param>
	void SetColor(XMFLOAT4 color);

	/// <summary>
	/// Z軸周りの回転角をセットする
	/// </summary>
	/// <param name="rotation">Z軸周りの回転角</param>
	void SetRotation(float rotation);

	/// <summary>
	/// スケールをセットする
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(XMFLOAT2 scale);

	/// <summary>
	/// 横反転フラグをセットする
	/// </summary>
	/// <param name="isFlipX">横反転フラグ</param>
	void SetIsFlipX(bool isFlipX);

	/// <summary>
	/// 縦反転フラグをセットする
	/// </summary>
	/// <param name="isFlipY">縦反転フラグ</param>
	void SetIsFlipY(bool isFlipY);

	/// <summary>
	/// 表示フラグをセットする
	/// </summary>
	/// <param name="isDisplay">表示フラグ</param>
	void SetIsDisplay(bool isDisplay);

	/// <summary>
	/// 画像をセット
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <param name="loadNewerIfNotFound">画像が見つからなかったときに新しく読み込むかどうか</param>
	void SetTexture(const wchar_t* filename, bool loadNewerIfNotFound = true);

	/// <summary>
	/// 最初に設定されるべき値のセット
	/// </summary>
	/// <param name="posX">X座標</param>
	/// <param name="posY">Y座標</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	void SetInitParams(float posX, float posY, float width, float height);

	/// <summary>
	/// 描画範囲(レクタングル)をセット
	/// </summary>
	/// <param name="tex_x">切り取りx座標</param>
	/// <param name="tex_y">切り取りy座標</param>
	/// <param name="tex_width">切り取り横幅(px)</param>
	/// <param name="tex_height">	切り取り縦幅(px)</param>
	void SetDrawRectangle(float tex_x, float tex_y, float tex_width, float tex_height);

	/// <summary>
	/// アンカーポイントをセットする
	/// </summary>
	/// <param name="anchorpoint">アンカーポイント</param>
	void SetAnchorpoint(XMFLOAT2 anchorpoint);
#pragma endregion
};

/*
	ComPtr<ID3D12RootSignature> Sprite::spriteRootSignature = nullptr;	//ルートシグネチャ
	ComPtr<ID3D12PipelineState> Sprite::spritePipelineState = nullptr;	//パイプラインステート
	ComPtr<ID3D12Resource> Sprite::texbuff = nullptr;					//テクスチャバッファ
	ComPtr<ID3DBlob> Sprite::vsBlob = nullptr; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> Sprite::psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> Sprite::errorBlob = nullptr; // エラーオブジェクト
	ComPtr<ID3D12DescriptorHeap> Sprite::basicDescHeap = nullptr;
	XMMATRIX Sprite::spriteMatProjection;		//射影行列

*/

