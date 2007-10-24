#include <chrasis.h>

#include <algorithm>
#include <iterator>

#include "chmlcodec.h"

using namespace std;
using namespace chrasis;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "Usage: " << argv[0] << "infile.chml [infile2.chml ...] outfile.chml" << std::endl;
		return 1;
	}

	Character::collection all;

	for (int i=1 ; i<argc-1 ; ++i)
	{
		Character::collection chars = read_chml(argv[i]);
		std::copy(chars.begin(), chars.end(), back_inserter(all));
	}

	write_chml(all, argv[argc-1]);

	return 0;
}
