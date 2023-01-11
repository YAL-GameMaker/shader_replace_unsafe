#pragma once
#pragma region http://www.gmannickg.com/?p=28

template <typename T>
class auto_cast_wrapper
{
public:
	template <typename R>
	friend auto_cast_wrapper<R> auto_cast(R&& x);

	template <typename U>
	operator U() const
	{
		// doesn't allow downcasts, otherwise acts like static_cast
		// see: http://stackoverflow.com/questions/5693432/making-auto-cast-safe
		return U{ std::forward<T>(mX) };
	}

private:
	auto_cast_wrapper(T&& x) :
		mX(std::forward<T>(x))
	{}

	auto_cast_wrapper(const auto_cast_wrapper& other) :
		mX(std::forward<T>(other.mX))
	{}

	auto_cast_wrapper& operator=(const auto_cast_wrapper&) = delete;

	T&& mX;
};

template <typename R>
auto_cast_wrapper<R> auto_cast(R&& x)
{
	return auto_cast_wrapper<R>(std::forward<R>(x));
}

//

template <typename T>
class unsafe_cast_wrapper
{
public:
	template <typename R>
	friend unsafe_cast_wrapper<R> unsafe_cast(const R& x);

	template <typename U>
	operator U()
	{
		return static_cast<U>(mX);
	}

private:
	unsafe_cast_wrapper(const T& x) :
		mX(x)
	{}

	unsafe_cast_wrapper(const unsafe_cast_wrapper& other) :
		mX(other.mX)
	{}

	// non-assignable
	unsafe_cast_wrapper& operator=(const unsafe_cast_wrapper&);

	const T& mX;
};

template <typename R>
unsafe_cast_wrapper<R> unsafe_cast(const R& x)
{
	return unsafe_cast_wrapper<R>(x);
}

#pragma endregion