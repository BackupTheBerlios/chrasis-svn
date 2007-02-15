#include "common.h"
#include "character.h"
#include "chmlcodec.h"

#include <glibmm.h>

int
main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "usage: " << argv[0] << " <input_dir> <output_dir>" << std::endl;
		return 1;
	}

	std::string idir_s(argv[1]), odir_s(argv[2]);
	Glib::Dir idir(idir_s);
	std::list<std::string> entries (idir.begin(), idir.end());

	for (std::list<std::string>::const_iterator di = entries.begin();
	     di != entries.end();
	     ++di)
	{
		Character::collection chrs;
		if (di->size() > 5 && di->substr(di->size() - 5) == ".chml")
		{
			std::cout << "Processing \"" + *di + "\"" << std::endl;
			// first read the chml
			chrs = read_chml(idir_s + "/" + *di);

			// group them into a map (by name)
			std::map<std::string, Character::collection> m_chrs;
			for (Character::const_iterator ci = chrs.begin();
			     ci != chrs.end();
			     ++ci)
			{
				m_chrs[ ci->get_name() ].push_back(*ci);
			}

			// for each characters, open the corresponding file, push
			// the chars to the back, then write back.
			for (std::map<std::string, Character::collection>::const_iterator mi = m_chrs.begin();
			     mi != m_chrs.end();
			     ++mi)
			{
				std::cout << "\tWriting \"" << mi->first << "\"..." << std::endl;

				Character::collection ochrs;
				if (fexist(odir_s + "/" + mi->first + ".chml"))
					ochrs = read_chml(odir_s + "/" + mi->first + ".chml");

				ochrs.reserve(ochrs.size() + mi->second.size());
				std::copy(mi->second.begin(), mi->second.end(), std::back_inserter(ochrs));

				write_chml(ochrs, odir_s + "/" + mi->first + ".chml");
			}
		}
	}

	//Character::collection chrs = read_chml(argv[1]);

	return 0;
}
