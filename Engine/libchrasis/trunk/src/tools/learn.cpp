#include "chrasis.h"

using namespace std;
using namespace chrasis;

int main(int argc, char* argv[])
{
	Database db(argv[2]);

	prepare_database(db);

	Learner lnr(db);

	Character::collection chars = read_chml(argv[1]);

	cout << "Characters in \"" << argv[1] << "\":" << endl;
	for (Character::iterator ci = chars.begin();
	     ci != chars.end();
	     ++ci)
	{
		cout << "Processing char #" << ci - chars.begin()
		     << " (" << ci->get_name() << ")..." << endl;
	
		lnr.learn(*ci);
	}

	return 0;
}
