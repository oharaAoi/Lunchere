#pragma once
#include <utility>
#include <memory>
#include <list>
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AOENGINE {

/// <summary>
/// Sceneのレンダリングを行うクラス
/// </summary>
class SceneRenderer :
	public AttributeGui {
public:	// 構造体データ

	/// <summary>
	/// 描画対象ペアのインターフェース
	/// </summary>
	struct IObjectPair {
		virtual ~IObjectPair() = default;
		virtual AOENGINE::ISceneObject* GetSceneObject() = 0;
		virtual const std::string& GetRenderingType() const = 0;
		virtual void SetRenderingType(const std::string& name) = 0;
		virtual int GetRenderQueue() const = 0;
		virtual void SetRenderQueue(int num) = 0;

		virtual bool GetPostDraw() const = 0;
		virtual void SetPostDraw(bool _postDraw) = 0;
	};

	/// <summary>
	/// Objectとレンダリング情報を結びつけたクラス
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	/// <summary>
	/// 描画対象のオブジェクトペア
	/// </summary>
	struct ObjectPair : IObjectPair {
		std::unique_ptr<T> object;
		std::string renderingType;
		std::string objectName;
		int renderQueue = 0;
		bool isPostDraw = false;

		ObjectPair(const std::string& _type, int _renderQueue, bool _isPostDraw, const std::string& _objName, std::unique_ptr<T> _obj)
			: renderingType(_type), renderQueue(_renderQueue), isPostDraw(_isPostDraw), objectName(_objName), object(std::move(_obj)){
		}

		T* GetSceneObject() override { return object.get(); }

		const std::string& GetRenderingType() const override { return renderingType; }
		void SetRenderingType(const std::string& name) override { renderingType = name; }

		int GetRenderQueue() const override { return renderQueue; }
		void SetRenderQueue(int num) override { renderQueue = num; }

		bool GetPostDraw() const override { return isPostDraw; }
		void SetPostDraw(bool _postDraw) override { isPostDraw = _postDraw; }
	};

public:

	SceneRenderer() = default;
	~SceneRenderer() = default;
	SceneRenderer(const SceneRenderer&) = delete;
	const SceneRenderer& operator=(const SceneRenderer&) = delete;

	static SceneRenderer* GetInstance();

public:

	// 終了処理
	void Finalize();
	// 初期化処理
	void Init();
	// 更新処理
	void Update();
	// 後から行う更新
	void PostUpdate();
	// 描画処理
	void Draw() const;
	// Particleなどの描画
	void PostDraw() const;
	// objectの編集
	void EditObject(const ImVec2& windowSize, const ImVec2& imagePos);

	void Debug_Gui() override;

public:

	/// <summary>
	/// Objectを生成する
	/// </summary>
	/// <param name="loadData">: 生成データ</param>
	void CreateObject(AOENGINE::SceneLoader::LevelData* loadData);

	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="objectName">: Objectの名前</param>
	/// <param name="renderingName">: Pipelineの種類</param>
	/// <param name="renderQueue">: レイヤーの値</param>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename T, typename... Args>
	T* AddObject(const std::string& objectName, const std::string& renderingName, int renderQueue = 0, bool isPostDraw = false, Args&&... args) {
		static_assert(std::is_base_of<AOENGINE::ISceneObject, T>::value, "T must derive from ISceneObject");

		auto pair = std::make_unique<ObjectPair<T>>(
			renderingName,
			renderQueue,
			isPostDraw,
			objectName,
			std::make_unique<T>(std::forward<Args>(args)...)
		);
		T* gameObject = static_cast<T*>(pair->object.get());
		pair->object->Init();
		pair->object->SetName(objectName);

		auto& newObject = objectList_.emplace_back(std::move(pair));
		if (isPostDraw) {
			postDrawObjectList_.emplace_back(newObject.get());
		}

		return gameObject;
	}

	/// <summary>
	/// Objectを解放する
	/// </summary>
	/// <param name="objPtr">: 解放するobjectの種類</param>
	void ReleaseObject(AOENGINE::ISceneObject* objPtr);

	/// <summary>
	/// Renderingのタイプを変更する
	/// </summary>
	/// <param name="renderingName">: 変更後のレンダリングのタイプ</param>
	/// <param name="gameObject">: 変更するobjectのポインタ</param>
	void ChangeRenderingType(const std::string& renderingName, AOENGINE::ISceneObject* gameObject);

public:

	template<typename T>
	T* GetGameObject(const std::string& _objName) {
		static_assert(std::is_base_of<AOENGINE::ISceneObject, T>::value, "T must derive from ISceneObject");

		for (auto& pair : objectList_) {
			if (pair->GetSceneObject()->GetName() == _objName) {
				return dynamic_cast<T*>(pair->GetSceneObject());  // 安全にキャスト
			}
		}

		AOENGINE::Logger::Log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		AOENGINE::Logger::Log("[" + _objName + "] not find");
		AOENGINE::Logger::Log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return nullptr;
	}

	template<typename T>
	T* GetGameObject(T* _ptr) {
		for (auto& pair : objectList_) {
			if (pair->GetSceneObject() == _ptr) {
				return dynamic_cast<T*>(pair->GetSceneObject());  // 安全にキャスト
			}
		}
		return nullptr;
	}

	void SetRenderingQueue(const std::string& objName, int num);

private:

	std::list<std::unique_ptr<IObjectPair>> objectList_;
	std::list<IObjectPair*> postDrawObjectList_;
	std::list<std::unique_ptr<IObjectPair>> spriteObjectList_;

	AOENGINE::ParticleManager* particleManager_;
	AOENGINE::GpuParticleManager* gpuParticleManager_;

};
}
