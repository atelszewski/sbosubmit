#ifndef SBO_SLACKBUILD_H_INCLUDED
#define SBO_SLACKBUILD_H_INCLUDED

#include <string>
#include <vector>
#include "sbo_subfile_base.h"

using namespace std;

class sbo_slackbuild_t :
  public sbo_subfile_base_t
{
public:
  sbo_slackbuild_t();
  void load(string const &pname);

private:
  vector<string> content;
};

#endif
