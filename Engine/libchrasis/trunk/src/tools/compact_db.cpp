#include <chrasis.h>

using namespace chrasis;

// this program should...
//   1. get rid of redundent points in the database
//   2. run "ANALYZE;VACUUM;"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << " <database>" << std::endl;
		return 1;
	}

	Database db(argv[1]);
	Query q(db);

	std::cout << "compact not implemented (yet)!" << std::endl;

	std::cout << "rebuilding index..." << std::endl;
	q.execute("ANALYZE;");

	std::cout << "vacuuming database..." << std::endl;
	q.execute("VACUUM;");

	return 0;
}
