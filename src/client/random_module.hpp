#pragma once

struct RandomModule 
{
	std::default_random_engine random_engine;

	RandomModule() {
		random_engine.seed(std::random_device{}());
	}
	
	auto & get_random_engine() {
		return random_engine;
	}
	
	float random_float(float x, float y) {
		static std::uniform_real_distribution<float> d{};

		using parm_t = decltype(d)::param_type;
		return d(get_random_engine(), parm_t{x, y});
	}

	int8_t random_int8(int8_t x, int8_t y) {
		static std::uniform_int_distribution<int8_t> d{};

		using parm_t = decltype(d)::param_type;
		return d(get_random_engine(), parm_t{x, y});
	}

	
};






