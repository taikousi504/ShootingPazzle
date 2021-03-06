#include <d3dcompiler.h>
#include <string>
#include <d3dx12.h>
#include <DirectXTex.h>
#include <wrl.h>
#include "Sprite.h"
#include "DX12Util.h"
#include "DrawManager.h"
#include "Archive.h"
#include "Encorder.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DX12Library;
using namespace DirectX;
using namespace Microsoft::WRL;

ComPtr<ID3D12RootSignature> Sprite::spriteRootSignature = {};	//ルートシグネチャ
ComPtr<ID3D12PipelineState> Sprite::spritePipelineState[PIPELINE_COUNT] = {};	//パイプラインステート
XMMATRIX Sprite::matProjection{};		//射影行列
ComPtr <ID3D12DescriptorHeap> Sprite::descHeap = nullptr;
const int Sprite::spriteSRVCount = 512;
ComPtr <ID3D12Resource> Sprite::texbuff[Sprite::spriteSRVCount];	//テクスチャバッファ
std::unordered_map<std::string, UINT> Sprite::loadedTextureList;
UINT Sprite::loadedTextureCount = 0;


void Sprite::FirstInit()
{
	HRESULT result = S_FALSE;

	//各シェーダファイルの読み込みとコンパイル
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	bool isLoadedArchiveVS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/SpriteVertexShader.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/SpriteVertexShader.hlsl"));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // インクルード可能にする
				"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
				0,
				&vsBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchiveVS = true;
			}
		}
	}

	if (isLoadedArchiveVS == false) {
		result = D3DCompileFromFile(
			L"Shader/SpriteVertexShader.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);
	}

	if (FAILED(result)) {
		// errorBlob からエラー内容を string 型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ピクセルシェーダの読み込みとコンパイル
	bool isLoadedArchivePS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/SpritePixelShader.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/SpritePixelShader.hlsl"));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // インクルード可能にする
				"main", "ps_5_0",    // エントリーポイント名、シェーダーモデル指定
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
				0,
				&psBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchivePS = true;
			}
		}
	}

	if (isLoadedArchivePS == false) {
		result = D3DCompileFromFile(
			L"Shader/SpritePixelShader.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);
	}

	if (FAILED(result)) {
		// errorBlob からエラー内容を string 型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//頂点レイアウト配列の宣言と、各種項目の設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0},

		{"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0},

	};

	//グラフィックスパイプラインの各ステージの設定をする構造体を用意する。
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//頂点シェーダ、ピクセルシェーダをパイプラインに設定
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());


	//サンプルマスクとラスタライザステートの設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	//ラスタライズステート
	//標準的な設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//一旦標準値をセット
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;				//カリングしない

	//デプスステンシルステートの設定
	//標準的な設定(深度テストを行う、書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	//一旦標準値をセット
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//常に上書きルール
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状を三角形に設定
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	gpipeline.NumRenderTargets = 1; // 描画対象は 1 つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255 指定の RGBA
	gpipeline.SampleDesc.Count = 1; // 1 ピクセルにつき 1 回サンプリング


	//デスクリプタテーブルの設定
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //t0 レジスタ

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2];
	//定数用
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)
	//テクスチャ用
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//サンプラーの設定
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(
		0, D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR);

	//ルートシグネチャの生成
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr <ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	//ルートシグネチャの生成
	result = DX12Util::GetDevice()->CreateRootSignature(0, rootSigBlob.Get()->GetBufferPointer(), rootSigBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(spriteRootSignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = spriteRootSignature.Get();

	for (int i = 0; i < PIPELINE_COUNT; i++) {
		//ブレンドステートの設定
		// レンダーターゲットのブレンド設定(8 個あるがいまは一つしか使わない)
		D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定

		blenddesc.BlendEnable = true; // ブレンドを有効にする

		blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // 加算

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う

		switch (i) {
		case SPRITE_BLENDMODE_NORMAL:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case SPRITE_BLENDMODE_ADD:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case SPRITE_BLENDMODE_SUB:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		case SPRITE_BLENDMODE_MUL:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		case SPRITE_BLENDMODE_SCREEN:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case SPRITE_BLENDMODE_REVERSE:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		}

		//透明部分の深度値書き込み設定
		gpipeline.BlendState.AlphaToCoverageEnable = false;

		// ブレンドステートに設定する
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		//パイプラインステートの生成
		result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(spritePipelineState[i].ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(result));
	}

	//スプライト用デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = DX12Util::GetDevice()->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf())
	);

	//行列初期化
	matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight(), 0.0f, 0.0f, 1.0f
	);
}

void Sprite::Initialize()
{
	HRESULT result;

	//頂点データ
	VertexPosUv vertices[] = {
		//	 x		y	  z		u	  v
		{{	0.0f,100.0f,0.0f},{0.0f,1.0f}},	//左下
		{{	0.0f,  0.0f,0.0f},{0.0f,0.0f}},	//左上
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},	//右下
		{{100.0f,  0.0f,0.0f},{1.0f,0.0f}},	//右上

	};

	//頂点バッファ生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf())
	);

	//頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * vertNum;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	//定数バッファの生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf())
	);

	//定数バッファにデータを転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = Vector4(1, 1, 1, 1);		//色指定 (RGBA)
	constMap->mat = XMMatrixOrthographicOffCenterLH(
		0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight(), 0.0f, 0.0f, 1.0f);	//平行投影行列の合成
	constBuff->Unmap(0, nullptr);
}

UINT Sprite::LoadTexture(const std::string& filepath)
{
	HRESULT result = S_FALSE;

	////すでにテクスチャが読み込まれている場合はリターン
	//if (spriteTexbuff[loadedTextureList[filename]] != nullptr) {
	//	return;
	//}

	//WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchimg{};

	//まずアーカイブ読み込み
	bool isLoaded = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(filepath, &size);
		if (pData != nullptr) {
			result = LoadFromWICMemory(
				pData, size, WIC_FLAGS_FORCE_RGB,
				&metadata, scratchimg);
			if (FAILED(result)) {
				return result;
			}

			isLoaded = true;
		}
	}
	//アーカイブファイルから読み取れなかったら直接読み取る
	if (isLoaded == false) {
		result = LoadFromWICFile(
			Encorder::StrToWstr(filepath).c_str(), WIC_FLAGS_FORCE_RGB,
			&metadata, scratchimg);
		if (FAILED(result)) {
			return result;
		}
	}

	const Image* img = scratchimg.GetImage(0, 0, 0);	//生データ抽出

	//テクスチャバッファの生成
	//GPU側にテクスチャバッファを作成する
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	result = DX12Util::GetDevice()->CreateCommittedResource(	//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(texbuff[loadedTextureCount].ReleaseAndGetAddressOf())
	);

	//データ転送
	//テクスチャバッファにデータ転送
	result = texbuff[loadedTextureCount]->WriteToSubresource(
		0,
		nullptr,	//全領域にコピー
		img->pixels,	//元データアドレス
		(UINT)img->rowPitch,	//1ラインサイズ
		(UINT)img->slicePitch	//全サイズ
	);

	//実際にシェーダーリソースビューを生成する
	//シェーダーリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	DX12Util::GetDevice()->CreateShaderResourceView(
		texbuff[loadedTextureCount].Get(),	//ビューと関連付けるバッファ
		&srvDesc,	//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeap->GetCPUDescriptorHandleForHeapStart(),
			loadedTextureCount,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	//リストに追加
	loadedTextureList.emplace(filepath, loadedTextureCount);

	loadedTextureCount++;

	return loadedTextureCount - 1;
}

void Sprite::BeginDraw()
{
	//ルートシグネチャの設定
	DX12Util::GetCmdList()->SetGraphicsRootSignature(spriteRootSignature.Get());

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	DX12Util::GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//パイプラインステートの設定
	DX12Util::GetCmdList()->SetPipelineState(spritePipelineState[0].Get());

	//プリミティブ形状を設定
	DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::DrawPrivate()
{
	if (!isDisplay)
		return;

	//前回の描画に使用されたパイプラインのインデックスと今回使うものが違うなら
	//ここで切り替え
	static int prevPipelineIndex = 0;
	if (pipelineIndex != prevPipelineIndex) {
		//パイプラインステートの設定
		DX12Util::GetCmdList()->SetPipelineState(spritePipelineState[pipelineIndex].Get());
	}


	//ワールド行列の更新
	matWorld = XMMatrixIdentity();
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));
	matWorld *= XMMatrixTranslation(position.x, position.y, 0);

	//行列の転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->mat = matWorld * matProjection;		//行列の合成
	constBuff->Unmap(0, nullptr);

	//頂点バッファをセット
	DX12Util::GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファをセット
	DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());


	//シェーダリソースビューをセット
	DX12Util::GetCmdList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	//描画コマンド
	DX12Util::GetCmdList()->DrawInstanced(4, 1, 0, 0);

	prevPipelineIndex = pipelineIndex;
}

void Sprite::SetAnchorpoint(const Vector2& anchorpoint)
{
	this->anchorpoint = anchorpoint;
	UpdateVertBuff();
}

void Sprite::SetIsDisplay(const bool isDisplay)
{
	this->isDisplay = isDisplay;
}


void Sprite::SetTexture(const std::string& filename, const bool loadNewerIfNotFound)
{
	if (loadedTextureList.find(filename) != loadedTextureList.end())
	{
		texNumber = loadedTextureList.find(filename)->second;
	}
	else
	{
		if (loadNewerIfNotFound == false) {
			return;
		}

		texNumber = LoadTexture(filename);
	}

	//画像初期スケール代入
	D3D12_RESOURCE_DESC resDesc =
		texbuff[texNumber]->GetDesc();

	if (width == 0) {
		width = resDesc.Width;
	}
	if (height == 0) {
		height = resDesc.Height;
	}
	if (tex_width == 0) {
		tex_width = resDesc.Width;
	}
	if (tex_height == 0) {
		tex_height = resDesc.Height;
	}

	size.x = width;
	size.y = height;

	UpdateVertBuff();
}

void Sprite::SetPosition(const Vector2& pos)
{
	position = pos;
	UpdateVertBuff();
}

void Sprite::SetColor(const Vector4& color)
{
	this->color = color;
	UpdateVertBuff();
}

void Sprite::SetRotation(const float rotation)
{
	this->rotation = rotation;
	UpdateVertBuff();
}

void Sprite::SetSize(const Vector2& size)
{
	this->size = size;
	UpdateVertBuff();
}

void Sprite::SetIsFlipX(const bool isFlipX)
{
	this->isFlipX = isFlipX;
	UpdateVertBuff();
}

void Sprite::SetIsFlipY(const bool isFlipY)
{
	this->isFlipY = isFlipY;
	UpdateVertBuff();
}

void Sprite::SetDrawRectangle(const float tex_x, const float tex_y, const float tex_width, const float tex_height)
{
	this->tex_x = tex_x;
	this->tex_y = tex_y;
	this->tex_width = tex_width;
	this->tex_height = tex_height;

	size.x = tex_width;
	size.y = tex_height;

	UpdateVertBuff();

}

void Sprite::UpdateVertBuff()
{
	HRESULT result = S_FALSE;

	//頂点データ
	VertexPosUv vertices[] = {
		//	 x		y	  z		u	  v
		{{	0.0f,100.0f,0.0f},{0.0f,1.0f}},	//左下
		{{	0.0f,  0.0f,0.0f},{0.0f,0.0f}},	//左上
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},	//右下
		{{100.0f,  0.0f,0.0f},{1.0f,0.0f}},	//右上

	};


	//画像の大きさから表示サイズを設定
	//左下、左上、右下、右上
	enum { LB, LT, RB, RT };
	float left = (0.0f - anchorpoint.x) * size.x;
	float right = (1.0f - anchorpoint.x) * size.x;
	float top = (0.0f - anchorpoint.y) * size.y;
	float bottom = (1.0f - anchorpoint.y) * size.y;

	if (isFlipX)
	{
		//左右入れ替え
		left = -left;
		right = -right;
	}

	if (isFlipY)
	{
		//上下入れ替え
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,	bottom,	0.0f };	//左下
	vertices[LT].pos = { left,	top,	0.0f };	//左上
	vertices[RB].pos = { right,	bottom,	0.0f };	//右下
	vertices[RT].pos = { right,	top,	0.0f };	//右上

	//テクスチャ情報取得
	if (texbuff[texNumber])
	{
		D3D12_RESOURCE_DESC resDesc =
			texbuff[texNumber]->GetDesc();

		float tex_left = tex_x / resDesc.Width;
		float tex_right = (tex_x + tex_width) / resDesc.Width;
		float tex_top = tex_y / resDesc.Height;
		float tex_bottom = (tex_y + tex_height) / resDesc.Height;

		vertices[LB].uv = { tex_left,	tex_bottom};	//左下
		vertices[LT].uv = { tex_left,	tex_top};	//左上
		vertices[RB].uv = { tex_right,	tex_bottom};	//右下
		vertices[RT].uv = { tex_right,	tex_top};	//右上
	}


	//頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

}

void Sprite::DrawBG()
{
	DrawManager::AddDrawListBG(DRAW_MANAGER_OBJECT_TYPE_SPRITE, this);
}

void Sprite::DrawFG()
{
	DrawManager::AddDrawList(DRAW_MANAGER_OBJECT_TYPE_SPRITE, this);
}

void Sprite::SetInitParams(float posX, float posY, float width, float height)
{
	SetPosition({posX, posY});
	SetSize({ width, height });
}

