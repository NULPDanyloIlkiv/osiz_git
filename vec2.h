#pragma once

struct vec2i; struct vec2f;

struct vec2i {
	int x, y;

	vec2i(void);
	vec2i(int val);
	vec2i(int _x, int _y);

	vec2i& operator=(int val);

	operator vec2f(void);

	vec2i operator+(vec2i const& vec);
	vec2i operator-(vec2i const& vec);

	vec2i operator*(vec2i const& vec);
	vec2i operator/(vec2i const& vec);



	vec2i& operator+=(vec2i const& vec);
	vec2i& operator-=(vec2i const& vec);

	vec2i& operator*=(vec2i const& vec);
	vec2i& operator/=(vec2i const& vec);

	vec2i operator-(void);



	bool operator==(vec2i const& vec);
	bool operator!=(vec2i const& vec);
};

struct vec2f {
	float x, y;

	vec2f(void);
	vec2f(float val);
	vec2f(float _x, float _y);

	vec2f& operator=(float val);

	operator vec2i(void);

	vec2f operator+(vec2f const& vec);
	vec2f operator-(vec2f const& vec);

	vec2f operator*(vec2f const& vec);
	vec2f operator/(vec2f const& vec);



	vec2f& operator+=(vec2f const& vec);
	vec2f& operator-=(vec2f const& vec);

	vec2f& operator*=(vec2f const& vec);
	vec2f& operator/=(vec2f const& vec);

	vec2f operator-(void);



	bool operator==(vec2f const& vec);
	bool operator!=(vec2f const& vec);
};
