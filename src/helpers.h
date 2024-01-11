namespace helpers
{
	inline bool isEqual(double x, double y)
	{
		const double epsilon = 1e-5;
		return std::abs(x - y) <= epsilon * std::abs(x);
	}
}