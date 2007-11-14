#include <chrasis.h>

#include "chmlcodec.h"

using namespace std;
using namespace chrasis;

int main(int argc, char* argv[])
{
	SQLite::Database db(argv[2]);
	SQLite::Command cmd(db);

	Character::container chars = read_chml(argv[1]);

	cout << "Characters in \"" << argv[1] << "\":" << endl;
	for (Character::iterator ci = chars.begin();
	     ci != chars.end();
	     ++ci)
	{
		cout << "Processing char #" << ci - chars.begin()
		     << " (" << ci->get_name() << ")..." << endl;

		Character chr(*ci);
		chr.set_name("");

		ItemPossibility likely = recognize(normalize(chr), cmd);

		std::cout << "likely:" << std::endl;
		for (ItemPossibility::const_iterator it = likely.begin();
		     it != likely.end();
		     ++it)
		{
			std::cout << "\t(" << it->possibility << ") " 
				  << it->id << " [" 
				  << it->name;
			std::cout << ((it->name == ci->get_name()) ? "] *" : "]") ;
			std::cout << std::endl;
		}
	}

	return 0;
}
