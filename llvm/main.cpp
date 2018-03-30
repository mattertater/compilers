

/*
extern "C" int string_len(const char* str);
extern "C" int string_cmp(const char* str1, const char* str2);
extern "C" const char* string_chr(const char* str, char c);
extern "C" char* string_cpy(char* dest, const char* src);
extern "C" int mul_add(int x, int y, int z);
*/

int string_len(const char* str)
{
	int i = 0;

	while (str[i] != 0)
	{
		++i;
	}

	return i;
}

int main()
{
	string_len("dicks");

	return 0;
}