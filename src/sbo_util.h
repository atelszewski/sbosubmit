#ifndef SBO_UTIL_H_INCLUDED
#define SBO_UTIL_H_INCLUDED

#include <ostream>
#include <string>
#include <vector>

using namespace std;

int cnul_init(void);
void set_verbosity(int const pvl);
int get_verbosity(void);
ostream &imsg(int const pvl = 0);
ostream &wmsg(void);
ostream &emsg(void);
int mktmpfile(string &ppath);
void getcwd(string &pcwd);
void get_cat_prgnam(string const &pcwd, string &pcat, string &pprgnam);
bool is_all_whitespace(string const &pstr);
void trimlr(string &pstr, string const &pwhat = "\f\n\r\t\v ");
void trimr(string &pstr, string const &pwhat = "\f\n\r\t\v ");
void multispace_to_space(string &pstr);
void whitespace_to_space(string &pstr);
size_t bash_split(vector<string> &pvect,
  string const &pstr, string const &pdelim = "\f\n\r\t\v ");

#endif
