int min_int(int a, int b)
{
	if(a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

int max_int(int a, int b)
{
	if(a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

float min_float(float a, float b)
{
	if(a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

float max_float(float a, float b)
{
	if(a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}


int clamp_int(int value, int upper_bound, int lower_bound)
{
	if(value < lower_bound)
	{
		return lower_bound;
	}

	if(value > upper_bound)
	{
		return upper_bound;
	}

	return value;
}