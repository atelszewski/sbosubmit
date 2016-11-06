#ifndef SBO_SLACK_DESC_H_INCLUDED
#define SBO_SLACK_DESC_H_INCLUDED

#include <string>
#include "sbo_subfile_base.h"

using namespace std;

class sbo_slack_desc_t :
  public sbo_subfile_base_t
{
public:
  sbo_slack_desc_t();
  void load(string const &pname);

private:
  vector<string> content;
};

#endif
