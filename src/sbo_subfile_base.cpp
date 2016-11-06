#include <string>
#include <fstream>
#include <string.h>
#include <errno.h>
#include "sbo_classlog.h"
#include "sbo_subfile_base.h"

/******************************************************************************/
bool sbo_subfile_base_t::is_error(bool const pprev) const
/******************************************************************************/
{
  if (pprev) return true;
  return error_flag;
}

/******************************************************************************/
sbo_subfile_base_t::sbo_subfile_base_t(string const &pcname) :
  sbo_classlog_t(pcname),
  error_flag(false)
/******************************************************************************/
{
}

/******************************************************************************/
int sbo_subfile_base_t::file_into_string(string const &ppath, string &pstr)
/******************************************************************************/
{
  ifstream ifile;
  string line;

  ifile.open(ppath, ifstream::in);
  if (!ifile.is_open())
  {
    emsg() << strerror(errno) << ": " << ppath << endl;
    set_error();
    return -1;
  }

  while (getline(ifile, line))
  {
    pstr += line + "\n";
  }

  if (ifile.bad())
  {
    emsg() << strerror(errno) << ": " << ppath << endl;
    ifile.close();
    set_error();
    return -1;
  }

  ifile.close();

  return 0;
}

/******************************************************************************/
int sbo_subfile_base_t::file_into_array(string const &ppath,
                                        vector<string> &parray)
/******************************************************************************/
{
  ifstream ifile;
  string line;

  ifile.open(ppath, ifstream::in);
  if (!ifile.is_open())
  {
    emsg() << strerror(errno) << ": " << ppath << endl;
    set_error();
    return -1;
  }

  while (getline(ifile, line))
  {
    parray.push_back(line);
  }

  if (ifile.bad())
  {
    emsg() << strerror(errno) << ": " << ppath << endl;
    ifile.close();
    set_error();
    return -1;
  }

  ifile.close();

  return 0;
}

/******************************************************************************/
void sbo_subfile_base_t::set_error(void)
/******************************************************************************/
{
  error_flag = true;
}
