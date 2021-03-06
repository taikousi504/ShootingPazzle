#pragma once
#include "Sound.h"
#include "Timer.h"
#include <vector>
#include <unordered_map>
#include <memory>

class GameSound
{
public:
	struct SoundData
	{
		std::unique_ptr<DX12Library::WaveData> waveData;
		std::unique_ptr<DX12Library::SourceVoice> sourceVoice;
		DX12Library::Timer stopTimer;
		bool isUse3D = true;
	};

private:
	static std::vector<std::unique_ptr<SoundData>> sounds;
	static std::unordered_map<std::wstring, int> indexes;
	static const float MASTER_DISTANCE;

public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 後始末
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// 更新
	/// </summary>
	static void Update();

	/// <summary>
	/// 音声データ追加
	/// </summary>
	/// <param name="path"></param>
	static void AddSound(const std::wstring& path, bool isUse3D = true, bool isLoop = false, float loopStartPos = 0, float loopEndPos = 1000);

	/// <summary>
	/// ロード済音声データを再生 (エミッタ指定なし)
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	static void Play(const std::wstring& name);
	/// <summary>
	/// ロード済音声データを再生
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <param name="emitterPos">音を発する3D空間上での位置</param>
	static void Play(const std::wstring& name, const DX12Library::Vector3& emitterPos);

	/// <summary>
	/// ロード済音声データの再生を止める
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <param name="fadeOutMs">フェードアウトにかける秒数(ms)</param>
	static void Stop(const std::wstring& name, int fadeOutMs = 100);

	/// <summary>
	/// 現在再生中か
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <returns></returns>
	static bool IsPlaying(const std::wstring& name);

	/// <summary>
	/// 音量セット
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <param name="volume">音量倍率</param>
	static void SetVolume(const std::wstring& name, float volume);

	/// <summary>
	/// 3D音響に使うエミッターの位置セット
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <param name="pos">位置</param>
	static void SetPosition(const std::wstring& name, const DX12Library::Vector3& pos);

	/// <summary>
	/// どこまで音を響かせるかの値セット カメラと地面との距離の5倍くらいがよい
	/// </summary>
	/// <param name="distance"></param>
	static void SetDistance(float distance);

	/// <summary>
	/// ロードしてある音声ファイル取得
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	static DX12Library::SourceVoice& GetLoadedSound(const std::wstring& name);

};