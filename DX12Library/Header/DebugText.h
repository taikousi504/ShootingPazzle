#pragma once
#include "Sprite.h"
#include <string>

namespace DX12Library {

	class DebugText
	{
	public:
		static const int maxCharCount = 256;	//最大文字数
		static const int fontWidth = 9;			//フォント画像内1文字分の横幅
		static const int fontHeight = 18;		//フォント画像内1文字分の縦幅
		static const int fontLineCount = 14;	//フォント画像内1行分の文字列
		static UINT debugTextTexNumber;

		/// <summary>
		/// デバッグテキストを出力
		/// </summary>
		/// <param name="text">テキスト</param>
		/// <param name="x">x座標</param>
		/// <param name="y">y座標</param>
		/// <param name="scale">スケール</param>
		static void Print(const std::string& text, float x, float y, float scale = 1.0f);

		/// <summary>
		/// デバッグテキストの初期化処理
		/// </summary>
		/// <param name="texfilename">デバッグテキストに使用するテクスチャファイルパス</param>
		static void Initialize(const std::string& texfilename);
		static void SetDebugTextTexNumber(const int debugTextTexNumber);

		/// <summary>
		/// デバッグテキストを出力
		/// </summary>
		/// <param name="text">整数</param>
		/// <param name="x">x座標</param>
		/// <param name="y">y座標</param>
		/// <param name="scale">スケール</param>
		static void Print(int text, float x, float y, float scale = 1.0f);

		/// <summary>
		/// デバッグテキストを出力
		/// </summary>
		/// <param name="text">数値(float)</param>
		/// <param name="x">x座標</param>
		/// <param name="y">y座標</param>
		/// <param name="scale">スケール</param>
		static void Print(float text, float x, float y, float scale = 1.0f);

		/// <summary>
		/// デバッグテキストを出力
		/// </summary>
		/// <param name="text">数値(Vector2)</param>
		/// <param name="x">x座標</param>
		/// <param name="y">y座標</param>
		/// <param name="scale">スケール</param>
		static void Print(const Vector2& text, float x, float y, float scale = 1.0f);

		/// <summary>
		/// デバッグテキストを出力
		/// </summary>
		/// <param name="text">数値(Vector3)</param>
		/// <param name="x">x座標</param>
		/// <param name="y">y座標</param>
		/// <param name="scale">スケール</param>
		static void Print(const Vector3& text, float x, float y, float scale = 1.0f);

		/// <summary>
		/// デバッグテキストを出力
		/// </summary>
		/// <param name="text">数値(Vector4)</param>
		/// <param name="x">x座標</param>
		/// <param name="y">y座標</param>
		/// <param name="scale">スケール</param>
		static void Print(const Vector4& text, float x, float y, float scale = 1.0f);


		/// <summary>
		/// デバッグテキストを描画
		/// </summary>
		static void DrawAll();

		static UINT GetDebugTextTexNumber() { return debugTextTexNumber; }

	private:
		static Sprite sprites[maxCharCount];
		static int spriteIndex;
	};

}