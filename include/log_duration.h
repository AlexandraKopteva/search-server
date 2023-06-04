#pragma once

#include <chrono>
#include <iostream>

#define PROFILE_CONCAT_INTERNAL(X, Y) X##Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x)
#define LOG_DURATION_STREAM(x, out) LogDuration UNIQUE_VAR_NAME_PROFILE(x, out)

class LogDuration {
public:
	// заменим имя типа std::chrono::steady_clock
	// с помощью using для удобства
	using Clock = std::chrono::steady_clock;

	LogDuration() = default;

	LogDuration(const std::string &log_time, std::ostream &out = std::cerr) : log_time_(log_time){
		out_ = &out;
	}

	~LogDuration() {
		using namespace std::chrono;
		using namespace std::literals;

		const auto end_time = Clock::now();
		const auto dur = end_time - start_time_;
		*out_ << log_time_ << "Operation time: " << duration_cast<milliseconds>(dur).count() << " ms" << std::endl;
	}

private:
	std::ostream* out_;
	std::string log_time_;
	const Clock::time_point start_time_ = Clock::now();
};