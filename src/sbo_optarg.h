#ifndef SBO_OPTARG_H_INCLUDED
#define SBO_OPTARG_H_INCLUDED

#include <string>
#include <getopt.h>

using namespace std;

class sbo_optarg_t
{
public:
  sbo_optarg_t(int argc, char * const argv[]);
  string address;
  bool checkonly;
  string comment;
  string email;
  string keywords;
  int verbose;

private:
  typedef enum
  {
    ADDRESS = 0,
    CHECK_ONLY,
    COMMENT,
    EMAIL,
    HELP,
    KEYWORDS,
    VERBOSE,
    VERSION
  } opt_idx_t;

  static string const usage;
  static struct option const option[];
};

#endif
