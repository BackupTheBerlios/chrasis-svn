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
	Database::OPENDB *odb = db.grabdb();
	Database::Transaction t(*odb);

	int learned_cnt = 0;

	for (int i=1;i<argc-1;++i)
	{
		cout << "Reading \"" << argv[i] << "\"..." << endl;
		Character::container chars = read_chml(argv[i]);

		for (Character::const_iterator ci = chars.begin();
		     ci != chars.end();
		     ++ci)
		{
			cout	<< "\tLearning character #" << ++learned_cnt
				<< " (" << ci->get_name() << ")...";
			learn(normalize(*ci), *odb);
			cout	<< " [OK]" << endl;
		}
	}

	return 0;
}
