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

int pow_int(int x, int y)
{
	int a = x;

	if(y == 0) return 1;

	for(int i = 0; i < (y-1); i ++)
	{
		a = a * x;
	}

	return a;
}

int get_number_in_decimal_slot(int value, int decimal_slot)
{
	int a;

	a = value / pow_int(10, decimal_slot);
	a = a % 10;

	return a;
}