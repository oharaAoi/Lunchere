#pragma once
#include <vector>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/Rigging/Skeleton.h"

namespace AOENGINE {

/// <summary>
/// IKなどで使用する
/// </summary>
class EndEffector {
public:

	/// <summary>
	/// joints情報
	/// </summary>
	struct JointsData {
		Math::Vector3& pos;
		Math::Quaternion rotate;
		int32_t index;
	};

public:

	EndEffector() = default;
	~EndEffector() = default;

public:

	// 更新
	void Update(const Math::Matrix4x4& _worldMat);

public:

	void SetChain(const std::vector<int32_t>& _chain) { chain_ = _chain; }

	void SetSkeleton(AOENGINE::Skeleton* _skeleton) { pSkeleton_ = _skeleton; }

	void SetTargetPosition(const Math::Vector3& _pos) { targetPos_ = _pos; }

private:

	AOENGINE::Skeleton* pSkeleton_;
	std::vector<int32_t> chain_;
	std::vector<Math::Vector3> positions_;
	std::vector<JointsData> jointsData_;

	Math::Vector3 targetPos_;

};

}

using AOENGINE::EndEffector;

