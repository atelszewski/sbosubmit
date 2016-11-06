#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include "sbo_util.h"

static int verbose = 0;
static ostream cnul(nullptr);

/******************************************************************************/
int cnul_init(void)
/******************************************************************************/
{
  static ofstream nullf;

  nullf.open("/dev/null");
  if (!nullf.is_open())
  {
    cout << "error: unable to open /dev/null: " << strerror(errno) << endl;
    return -1;
  }

  /* Redirect to /dev/null. */
  cnul.rdbuf(nullf.rdbuf());

  return 0;
}

/******************************************************************************/
void set_verbosity(int const pvl)
/******************************************************************************/
{
  verbose = pvl;
}

/******************************************************************************/
int get_verbosity(void)
/******************************************************************************/
{
  return verbose;
}

/******************************************************************************/
ostream &imsg(int const pvl)
/******************************************************************************/
{
  if (pvl <= verbose)
  {
    cout << "[II] ";
    return cout;
  }

  return cnul;
}

/******************************************************************************/
ostream &wmsg(void)
/******************************************************************************/
{
  cout << "[WW] ";
  return cout;
}

/******************************************************************************/
ostream &emsg(void)
/******************************************************************************/
{
  cout << "[EE] ";
  return cout;
}

/******************************************************************************/
int mktmpfile(string &ppath)
/******************************************************************************/
{
  int rc;
  size_t tmplen;
  char *ctmp;

  if ((tmplen = ppath.length()) == 0U)
  {
    emsg() << "mktmpfile: no path." << endl;
    { rc = -1; goto doreturn; }
  }

  if ((ctmp = static_cast<char *>(malloc(tmplen + 1U))) == nullptr)
  {
    emsg() << "mktmpfile: out of memory." << endl;
    { rc = -1; goto doreturn; }
  }

  strcpy(ctmp, ppath.c_str());

  if ((rc = mkstemp(ctmp)) == -1)
  {
    emsg() << "mktmpfile: " << strerror(errno) << endl;
    free(ctmp);
    ctmp = nullptr;
    { rc = -1; goto doreturn; }
  }

  ppath = ctmp;
  free(ctmp);
  ctmp = nullptr;

doreturn:
  return rc;
}

/******************************************************************************/
void getcwd(string &pcwd)
/******************************************************************************/
{
  char *cwd;

  cwd = ::getcwd(nullptr, 0U);
  if (cwd)
  {
    pcwd = cwd;
    free(cwd);
  }
}

/******************************************************************************/
void get_cat_prgnam(string const &pcwd, string &pcat, string &pprgnam)
/******************************************************************************/
{
  size_t path_len;
  size_t p1;
  size_t p2;

  path_len = pcwd.length();

  p2 = pcwd.rfind('/');
  if (p2 == string::npos || p2 == 0U || (p2 + 1U) >= path_len)
  {
    return;
  }

  pprgnam = pcwd.substr(p2 + 1U);

  p1 = pcwd.rfind('/', p2 - 1U);
  if (p1 == string::npos || (p1 + 1U) == p2) return;

  pcat = pcwd.substr(p1 + 1U, p2 - (p1 + 1));
  std::transform(pcat.begin(), pcat.end(), pcat.begin(), ::tolower);
  pcat[0] = static_cast<char>(::toupper(pcat[0]));
}

/******************************************************************************/
bool is_all_whitespace(string const &pstr)
/******************************************************************************/
{
  return (pstr.find_first_not_of("\f\n\r\t\v ") == string::npos);
}

/******************************************************************************/
void trimlr(string &pstr, string const &pwhat)
/******************************************************************************/
{
  size_t b;
  size_t e;

  if ((b = pstr.find_first_not_of(pwhat)) == string::npos)
  {
    pstr.clear();
    return;
  }

  e = pstr.find_last_not_of(pwhat);
  pstr = pstr.substr(b, e - b + 1U);
}

/******************************************************************************/
void trimr(string &pstr, string const &pwhat)
/******************************************************************************/
{
  size_t e;

  e = pstr.find_last_not_of(pwhat);
  pstr = pstr.substr(0U, e + 1U);
}

/******************************************************************************/
void multispace_to_space(string &pstr)
/******************************************************************************/
{
  string::iterator new_end;

  new_end = unique(pstr.begin(), pstr.end(),
    [](char const lhs, char const rhs) { return lhs == ' ' && lhs == rhs; });
  pstr.erase(new_end, pstr.end());
}

/******************************************************************************/
void whitespace_to_space(string &pstr)
/******************************************************************************/
{
  for (size_t i = 0U; i < pstr.length(); i++)
  {
    if (::isspace(pstr[i])) pstr[i] = ' ';
  }
}

/******************************************************************************/
size_t bash_split(vector<string> &pvect,
  string const &pstr, string const &pdelim)
/******************************************************************************/
{
  size_t bpos = 0U;
  size_t epos;

  do {
    bpos = pstr.find_first_not_of(pdelim, bpos);
    if (bpos == string::npos) break;
    epos = pstr.find_first_of(pdelim, bpos);
    if (epos == string::npos) epos = pstr.length();
    pvect.push_back(pstr.substr(bpos, epos - bpos));
    bpos = epos + 1U;
  } while (1);

  return pvect.size();
}
