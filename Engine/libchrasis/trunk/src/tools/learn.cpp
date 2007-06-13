#include <chrasis.h>
#include <chrasis/internal.h>

#include "chmlcodec.h"

using namespace std;
using namespace chrasis;

int main(int argc, char* argv[])
{
	Database db(argv[2]);

	Character::collection chars = read_chml(argv[1]);

	cout << "Characters in \"" << argv[1] << "\":" << endl;
	for (Character::iterator ci = chars.begin();
	     ci != chars.end();
	     ++ci)
	{
		cout << "Processing char #" << ci - chars.begin()
		     << " (" << ci->get_name() << ")..." << endl;

		learn(normalize(*ci), db);
	}

	return 0;
}
