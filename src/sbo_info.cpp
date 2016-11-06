#include <string>
#include <vector>
#include "sbo_subfile_base.h"
#include "sbo_info.h"
#include "sbo_util.h"

typedef struct
{
  string name;
  bool can_span;
} info_key_t;

static vector<info_key_t> const key_avail
{
  { "PRGNAM",          false },
  { "VERSION",         false },
  { "HOMEPAGE",        false },
  { "DOWNLOAD",        true  },
  { "MD5SUM",          true  },
  { "DOWNLOAD_x86_64", true  },
  { "MD5SUM_x86_64",   true  },
  { "REQUIRES",        false },
  { "MAINTAINER",      false },
  { "EMAIL",           false }
};

/******************************************************************************/
sbo_info_t::sbo_info_t() :
  sbo_subfile_base_t("info")
/******************************************************************************/
{
}

/******************************************************************************/
void sbo_info_t::load(string const &pname)
/******************************************************************************/
{
  int res;
  size_t ikey = 0U;
  size_t iline = 0U;
  size_t lineno = 1U;

  content.clear();
  imsg(1) << "loading..." << endl;

  res = file_into_array(pname + ".info", content);
  if (res != 0) return;

  /* Parse .info file, dealing with multiline values. In the end,
   * all the multiline values are joined to form single line value. */
  while (ikey < key_avail.size() && iline < content.size())
  {
    string const &key = key_avail[ikey].name;
    string const &line = content[iline];
    size_t endq;
    bool next_span;

    /* If ikey == iline, then the line contains the key name and: full value
     * in case of single line value or beggining of the value in case of
     * multiline value. If ikey < iline, we're dealing with the next line
     * of multiline value. */
    if (ikey == iline)
    {
      size_t const len = key.length();

      /* Check if the key exists and the order of appearance is preserved. */
      if (line.compare(0U, len + 1U, key + "="))
      {
        emsg() << "invalid key name at line " << lineno
               << ", expected: " << key << "=" << endl;
        set_error();
        return;
      }

      /* Check if the opening quotation mark exists. */
      if (line.length() == (len + 1U) || line[len + 1U] != '"')
      {
        emsg() << "invalid character at line " << lineno << ":" << len + 2U
               << ", expected opening quotation mark." << endl;
        set_error();
        return;
      }

      /* Remove key name including '="'. */
      content[iline] = content[iline].substr(len + 2U);
    }

    /* Find closing quotation mark or line continuation mark. */
    endq = line.find_first_of("\"\\", 0U);

    if (endq == string::npos)
    {
      emsg() << "no closing quotation mark or line continuation mark found "
                "at line " << lineno << "." << endl;
      set_error();
      return;
    }

    if (endq != (line.length() - 1U))
    {
      emsg() << "closing quotation mark or line continuation mark must be\n";
      emsg() << "the last character at line " << lineno << "." << endl;
      set_error();
      return;
    }

    if (line[endq] == '\\' && !key_avail[ikey].can_span)
    {
      emsg() << "key " << key << " cannot span multiple lines at line "
             << lineno << "." << endl;
      set_error();
      return;
    }

    if (line[endq] == '\\' && line[endq - 1U] != ' ')
    {
      emsg() << "line continuation mark must be preceded by space at line "
             << lineno << ":" << endq + 1U << "." << endl;
      set_error();
      return;
    }

    /* When dealing with multiline value, concatenate next lines of the value
     * with the line that contains (contained) the key name. */
    if (iline > ikey) content[ikey] += line;

    /* Single line or multiline value? */
    next_span = (line[endq] == '\\');

    /* Remove closing quotation mark or line continuation mark. */
    content[ikey].erase(content[ikey].length() - 1U, 1U);

    /* Remove processed next line in multiline value. */
    if (iline > ikey)
    {
      content.erase(content.begin() + static_cast<ptrdiff_t>(iline));
    }

    /* Determine if the line processed in the next loop run will be new
     * key=value pair or continuation of previous multiline value. */
    if (!next_span)
    {
      ikey++;
      iline = ikey;
    }
    else
    {
      iline = ikey + 1U;
    }

    lineno++;
  }

  if (content.size() == 0U)
  {
    emsg() << "info file has no usable content." << endl;
    set_error();
    return;
  }

  if (content.size() > ikey)
  {
    emsg() << "info file contains more lines than expected. Please make sure\n";
    emsg() << "there is exactly one new line at the end of the file." << endl;
    set_error();
    return;
  }

  prgnam               = content[0];
  version              = content[1];
  homepage             = content[2];
  bash_split(download,   content[3]);
  bash_split(checksum,   content[4]);
  bash_split(download64, content[5]);
  bash_split(checksum64, content[6]);
  bash_split(requires,   content[7]);
  maintainer           = content[8];
  email                = content[9];
}
