#include "BehaviorTreeLogger.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <cassert>
#include <iostream>
#include <format>
#include "Engine/Utilities/Convert.h"
#include "Engine/Utilities/Loader.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

namespace {
constexpr size_t kMaxBehaviorTreeLogFileCount = 20;
}

BehaviorTreeLogger::~BehaviorTreeLogger() {
	std::ofstream logStream(filePath_, std::ios::app);
	logStream << "FINISHED LOG" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeLogger::Init(const std::string& _fileName) {
	try {
		std::filesystem::create_directories("./Project/Logs/BehaviorTree");
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Path1: " << e.path1() << std::endl;
		std::cerr << "Path2: " << e.path2() << std::endl;
	}

	// ファイル数が多くなりすぎないように
	DeleteOldLogFile(kMaxBehaviorTreeLogFileCount);

	std::filesystem::path path(_fileName);
	// 現在時刻を取得
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	// ログ・ファイルの名前にコンマ秒はいらないので削る
	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> nowSeconds
		= std::chrono::time_point_cast<std::chrono::seconds>(now);
	// 日本時間に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSeconds };
	// 年月日の文字列に変更
	std::string dateString = std::format("{:%Y%m%d%H%M%S}", localTime);
	// 時刻を使ってファイル名を決定
	filePath_ = std::string("./Project/Logs/BehaviorTree/") + path.stem().string() + "_" + dateString + ".log";
	// ファイルを作って書き込み準備
	std::ofstream logStream(filePath_);

	logStream << "SUCCESS CREATE LOG" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ logを書き込む
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeLogger::Log(const std::string& _message) {
	std::ofstream logStream(filePath_, std::ios::app);
	logStream << _message;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 警告をだしてログを出す
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeLogger::AssertLog(const std::string& _message) {
	std::ofstream logStream(filePath_, std::ios::app);
	logStream << _message << std::endl;
	OutputDebugStringA(_message.c_str());
	assert(_message.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Logがたくさん残らないように数を制限する
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeLogger::DeleteOldLogFile(size_t max) {
	const std::filesystem::path logDir = "./Project/Logs/BehaviorTree/";

	// ディレクトリが存在しなければ例外を投げる
	if (!std::filesystem::exists(logDir)) {
		std::string message = logDir.string() + "が存在しません";
		AOENGINE::Logger::AssertLog(message);
		return;
	}

	std::vector<std::filesystem::directory_entry> files;

	for (const auto& entry : std::filesystem::directory_iterator(logDir)) {
		if (std::filesystem::is_regular_file(entry)) {
			files.push_back(entry);
		}
	}

	if (files.size() <= max)
		return;

	std::sort(files.begin(), files.end(),
			  [](const auto& a, const auto& b) {
				  return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
			  }
	);

	for (size_t i = 0; i < files.size() - max; ++i) {
		std::error_code ec;
		std::filesystem::remove(files[i], ec);
	}
}
