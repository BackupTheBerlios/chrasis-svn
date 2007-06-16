#include <chrasis.h>
#include <chrasis/internal.h>

#include "chmlcodec.h"

using namespace std;
using namespace chrasis;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "Usage: " << argv[0] << " file1.chml [file2.chml ...] database.db" << endl;
		return 1;
	}

	Database db( argv[argc-1] );

	Character::collection all_chars;
	for (int i=1;i<argc-1;++i)
	{
		cout << "Reading \"" << argv[i] << "\"..." << endl;
		Character::collection chars = read_chml(argv[i]);
		std::copy(chars.begin(), chars.end(), back_inserter(all_chars));
	}

	for (Character::iterator ci = all_chars.begin();
	     ci != all_chars.end();
	     ++ci)
	{
		cout << "Learning character #" << ci - all_chars.begin()
		     << " (" << ci->get_name() << ")..." << endl;

		learn(normalize(*ci), db);
	}

	return 0;
}
