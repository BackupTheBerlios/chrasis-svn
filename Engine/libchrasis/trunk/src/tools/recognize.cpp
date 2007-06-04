#include <chrasis.h>

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

		Character chr(*ci);
		chr.set_name("");
	
		character_possibility_t likely(recognize(chr, db));

		std::cout << "likely:" << std::endl;
		for (character_possibility_t::iterator it = likely.begin();
		     it != likely.end();
		     ++it)
		{
			std::cout << "\t(" << it->second.first << ") " 
				  << it->second.second << " [" 
				  << it->first;
			std::cout << ((it->second.second == ci->get_name()) ? "] *" : "]") ;
			std::cout << std::endl;
		}
	}

	return 0;
}
