#pragma once
#include "CollisionPrimitive.h"
#include "RaycastHit.h"

#include <forward_list>
#include <d3d12.h>

namespace DX12Library {

	class BaseCollider;

	class CollisionManager
	{
	public://静的メンバ関数
		static CollisionManager* GetInstance();

	public://メンバ関数

		/// <summary>
		/// コライダーの追加
		/// </summary>
		/// <param name="collider">コライダー</param>
		inline void AddCollider(BaseCollider* collider) {
			colliders.push_front(collider);
		}

		/// <summary>
		/// コライダーの削除
		/// </summary>
		/// <param name="collider">コライダー</param>
		inline void RemoveCollider(BaseCollider* collider) {
			colliders.remove(collider);
		}

		/// <summary>
		/// 全ての衝突チェック
		/// </summary>
		void CheckAllCollisions();

		/// <summary>
		/// レイキャスト
		/// </summary>
		/// <param name="ray">レイ</param>
		/// <param name="hitInfo">衝突情報</param>
		/// <param name="ignoreCollider">無視するコライダーがあれば指定</param>
		/// <param name="maxDistance">最大距離</param>
		/// <returns>レイが任意のコライダーと交わる場合はtrue、それ以外はfalse</returns>
		bool Raycast(const Ray& ray, RaycastHit* hitInfo = nullptr, const BaseCollider* ignoreCollider = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	private:
		CollisionManager() = default;
		CollisionManager(const CollisionManager&) = delete;
		~CollisionManager() = default;
		CollisionManager& operator=(const CollisionManager&) = delete;
		//コライダーのリスト
		std::forward_list<BaseCollider*> colliders;
	};

}