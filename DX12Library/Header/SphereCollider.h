#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include "Vector3.h"

namespace DX12Library {

	/// <summary>
	/// 球衝突判定オブジェクト
	/// </summary>
	class SphereCollider : public BaseCollider, public Sphere
	{
	public:
		SphereCollider(Vector3 offset = { 0,0,0 }, float radius = 1.0f) :
			offset(offset),
			radius(radius)
		{
			// 球形状をセット
			shapeType = COLLISIONSHAPE_SPHERE;
		}

		/// <summary>
		/// 更新
		/// </summary>
		void Update() override;

		inline void SetRadius(float radius) { this->radius = radius; }

	private:
		// オブジェクト中心からのオフセット
		Vector3 offset;
		// 半径
		float radius;
	};
}
