#include <chrasis.h>

#include <algorithm>
#include <iterator>

#include "chmlcodec.h"

using namespace std;
using namespace chrasis;

int main(int argc, char* argv[])
{
	Database db(argv[2]);

	Character::collection all;

	for (int i=1;i<argc;++i)
	{
		Character::collection chars = read_chml(argv[i]);
		std::copy(chars.begin(), chars.end(), back_inserter(all));
	}

	write_chml(all, "all.chml");

	return 0;
}
