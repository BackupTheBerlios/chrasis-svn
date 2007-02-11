#include "common.h"
#include "recognizer.h"
#include "character.h"
#include "chmlcodec.h"
#include "database.h"
#include "learner.h"

using namespace std;

int main(int argc, char* argv[])
{
	Recognizer &rec = Recognizer::Instance();

	Database db(argv[2]);
	Learner lnr(db);

	Character::collection chars = read_chml(argv[1]);

	cout << "Characters in \"" << argv[1] << "\":" << endl;
	for (Character::iterator ci = chars.begin();
	     ci != chars.end();
	     ++ci)
	{
		cout << "Processing char #" << ci - chars.begin()
		     << " (" << ci->get_name() << ")..." << endl;
	
		lnr.learn(rec.normalize(*ci));
	}

	return 0;
}
