#ifndef SBO_INFO_H_INCLUDED
#define SBO_INFO_H_INCLUDED

#include <string>
#include "sbo_subfile_base.h"

using namespace std;

class sbo_info_t :
  public sbo_subfile_base_t
{
public:
  sbo_info_t();
  void load(string const &pname);

  string prgnam;
  string version;
  string homepage;
  vector<string> download;
  vector<string> checksum;
  vector<string> download64;
  vector<string> checksum64;
  vector<string> requires;
  string maintainer;
  string email;

private:
  vector<string> content;
};

#endif
