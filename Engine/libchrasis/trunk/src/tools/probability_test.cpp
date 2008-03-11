#include <chrasis.h>
#include <chrasis/internal.h>

#include "chmlcodec.h"

using namespace std;
using namespace chrasis;

void recognize_all(Character::container & chrs, SQLite::Command & cmd)
{
	int correct = 0, correct2 = 0;

	for (Character::iterator ci = chrs.begin();
	     ci != chrs.end();
	     ++ci)
	{
		Character chr(*ci);
		chr.set_name("");

		ItemPossibilityList likely = recognize(normalize(chr), cmd);
		ItemPossibilityList::const_iterator candidate = likely.begin();

		learn(normalize(*ci), cmd);

		if (candidate->name == ci->get_name())
		{
			cout << "<span style=\"color: black; font-weight: bold\">"
			     << ci->get_name()
			     << "</span>";
			++correct;
			continue;
		}

		if (likely.size() > 2)
		{
			candidate++;
			if (candidate->name == ci->get_name() )
			{
				cout << "<span style=\"color: darkslategrey; font-weight: normal;\">"
				     << ci->get_name()
				     << "</span>";
				++correct2;
				continue;
			}
		}

		cout << "<span style=\"color: red; font-weight: normal"
		     << (likely.size()?"; text-decoration: overline underline":"")
		     << ";\"";
		
		if (likely.size() != 0)
		{
			cout << "title=\"Candidate: " << likely.size() << " ";

			for (ItemPossibilityList::const_iterator i = likely.begin();
			     i != likely.end();
			     ++i)
			{
				cout << i->name << "[" << i->possibility << "] ";
			}

			cout << "\"";
		}

		cout << ">"
		     << ci->get_name()
		     << "</span>";
	}
	cout << " (" << correct << "+" << correct2 << "/" << chrs.size() << ")" << endl;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout	<< "Usage: "
			<< argv[0]
			<< " database.db filename.chml"
			<< endl;
		return 1;
	}

	SQLite::Database db( argv[1] );
	SQLite::Transaction t(db);
	SQLite::Command cmd(t);

	Character::container to_be_tested = read_chml(argv[2]);
	recognize_all(to_be_tested, cmd);

	return 0;
}
