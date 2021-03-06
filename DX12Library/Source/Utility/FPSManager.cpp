#include "FPSManager.h"
#include "DebugText.h"

using namespace DX12Library;

double FPSManager::FRAME_TIME = 0.0f;
std::chrono::steady_clock::time_point FPSManager::start;
std::chrono::steady_clock::time_point FPSManager::end;

double FPSManager::nowFPS = 60;
bool FPSManager::isAdjust = false;
bool FPSManager::isPrintFPS = false;

void FPSManager::SetFPS(double fps, bool isPrintFPS)
{
	if (fps > 0) {
		FRAME_TIME = 1.0 / fps;
		isAdjust = true;
	}
	else {
		isAdjust = false;
	}
	FPSManager::isPrintFPS = isPrintFPS;
	start = std::chrono::steady_clock::now();
}

void FPSManager::Update()
{
	static int count = 0;
	static double totalDeltaTime = 0;

	auto end = std::chrono::steady_clock::now();
	double deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0f;

	totalDeltaTime += deltaTime;
	count++;

	if (isAdjust) {

		if (deltaTime < FRAME_TIME)
		{
			DWORD sleepTime = static_cast<DWORD>((FRAME_TIME - deltaTime) * 1000);
			totalDeltaTime += FRAME_TIME - deltaTime;
			timeBeginPeriod(1);
			Sleep(sleepTime);
			timeEndPeriod(1);
		}
	}


	if (totalDeltaTime >= 1.0) { // 経過時間が0より大きい(こうしないと下の計算でゼロ除算になると思われ)
		nowFPS = (double)count / totalDeltaTime; // 平均fpsを計算
		count = 0;
		totalDeltaTime = 0;
	}
	//出力
	if (isPrintFPS) {
		DebugText::Print("FPS:" + std::to_string(FPSManager::GetFPS()), 0, 0);
	}

	start = std::chrono::steady_clock::now();
}
