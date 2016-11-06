#include <string>
#include <iostream>
#include <stdlib.h>
#include <getopt.h>
#include "sbo_version.h"
#include "sbo_optarg.h"

string const sbo_optarg_t::usage
{
"Usage: sbosubmit [OPTIONS]\n\n"

"Options are:\n"
"  --address=ADDRESS\n"
"      Allows to specify alternative submission address, e.g. for testing\n"
"      purposes. Default address is set to https://slackbuilds.org/\n"
/*"  --check-only\n"
"      Check the submission files for correctness rather than performing\n"
"      the full submission procedure. The returned values are: 1 for errors,\n"
"      2 for warnings.\n"*/
"  --comment[=COMMENT]\n"
"      Allows to specify the comment for SlackBuild submission. If the COMMENT\n"
"      string is omitted, the comment is taken from the comment.txt file\n"
"      residing in the SlackBuild directory.\n"
"  --email=EMAIL\n"
"      Allows to specify your e-mail address for submission. This is required\n"
"      in the absence of the environment variable SBOSUBMIT_SUBMISSION_EMAIL\n"
"  --help\n"
"      Display usage information and exit.\n"
"  --keywords[=KEYWORDS]\n"
"      Allows to specify the keywords for submission. If the KEYWORDS string\n"
"      omitted, the keywords are taken from the keywords.txt file residing in\n"
"      SlackBuild directory. Keywords are comma-separated words.\n"
"  --verbose\n"
"      Be verbose (quite by default).\n"
"  --version\n"
"      Display version information and exit.\n\n"

"Note:\n"
"  It is perfectly acceptable to specify the same option multiple times.\n"
"  In such case, the last one takes precedence."
};

struct option const sbo_optarg_t::option[] =
{
  { "address",    required_argument, nullptr, sbo_optarg_t::ADDRESS    },
  { "check-only", no_argument,       nullptr, sbo_optarg_t::CHECK_ONLY },
  { "comment",    optional_argument, nullptr, sbo_optarg_t::COMMENT    },
  { "email",      required_argument, nullptr, sbo_optarg_t::EMAIL      },
  { "help",       no_argument,       nullptr, sbo_optarg_t::HELP       },
  { "keywords",   optional_argument, nullptr, sbo_optarg_t::KEYWORDS   },
  { "verbose",    no_argument,       nullptr, sbo_optarg_t::VERBOSE    },
  { "version",    no_argument,       nullptr, sbo_optarg_t::VERSION    },
  { nullptr,      0,                 nullptr, 0                        }
};

/******************************************************************************/
sbo_optarg_t::sbo_optarg_t(int argc, char * const argv[]) :
  checkonly(false), verbose(0)
/******************************************************************************/
{
  int idxi;

  while ((idxi = ::getopt_long(argc, argv, "+", option, nullptr)) != -1)
  {
    opt_idx_t idx = static_cast<opt_idx_t>(idxi);

    switch (idx)
    {
    case ADDRESS:
      address = optarg;
      break;

    case CHECK_ONLY:
      checkonly = true;
      break;

    case COMMENT:
      comment = "@file";
      if (optarg) comment = optarg;
      break;

    case EMAIL:
      email = optarg;
      break;

    case HELP:
      cout << usage << endl;
      exit(EXIT_SUCCESS);

    case KEYWORDS:
      keywords = "@file";
      if (optarg) keywords = optarg;
      break;

    case VERBOSE:
      verbose = 1;
      break;

    case VERSION:
      cout << sbosubmit_version << endl;
      exit(EXIT_SUCCESS);
    }
  }

  if (argc != optind)
  {
    cout << argv[0] << ": " << "incorrect argument: "
         << argv[optind] << endl;
    exit(EXIT_FAILURE);
  }
}
