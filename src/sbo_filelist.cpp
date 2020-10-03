#include <string>
#include <vector>
#include <algorithm>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <regex>
#include "sbo_util.h"
#include "sbo_classlog.h"
#include "sbo_filelist.h"

/* Note: nftw code based on musl libc. */

#define FTW_F   1
#define FTW_D   2
#define FTW_DNR 3
#define FTW_NS  4
#define FTW_SL  5
#define FTW_DP  6
#define FTW_SLN 7

#define FTW_PHYS  1
#define FTW_MOUNT 2
#define FTW_CHDIR 4
#define FTW_DEPTH 8

/******************************************************************************/
sbo_filelist_t::sbo_filelist_t() :
  sbo_classlog_t("find"),
  file_callback(nullptr)
/******************************************************************************/
{
}

/******************************************************************************/
int sbo_filelist_t::find(void)
/******************************************************************************/
{
  if (-1 == nftw(".", 16, FTW_PHYS))
  {
    emsg() << strerror(errno) << endl;
    return -1;
  }

  return 0;
}

/******************************************************************************/
void sbo_filelist_t::sort(void)
/******************************************************************************/
{
  std::sort(result.begin(), result.end());
}

/******************************************************************************/
void sbo_filelist_t::set_file_cb(file_callback_t const pcb)
/******************************************************************************/
{
  file_callback = pcb;
}

/******************************************************************************/
int sbo_filelist_t::nftw(const char *ppath, int pfdlimit, int pflags)
/******************************************************************************/
{
  int r;
  size_t l;
  char pathbuf[PATH_MAX + 1];

  if (pfdlimit <= 0) return 0;

  l = strlen(ppath);
  if (l > PATH_MAX)
  {
    errno = ENAMETOOLONG;
    return -1;
  }

  memcpy(pathbuf, ppath, l + 1);
  r = nftw_do(pathbuf, pfdlimit, pflags, nullptr);

  return r;
}

/******************************************************************************/
int sbo_filelist_t::nftw_do(char *ppath, int pfdlimit,
                            int pflags, struct history *ph)
/******************************************************************************/
{
  size_t l;
  size_t j;
  struct stat st;
  struct history hnew;
  int type;
  int r;
  struct FTW lev;

  l = strlen(ppath);
  j = l && ppath[l - 1] == '/' ? l - 1 : l;

  if ((pflags & FTW_PHYS) ? lstat(ppath, &st) : stat(ppath, &st) < 0)
  {
    if (!(pflags & FTW_PHYS) && errno == ENOENT && !lstat(ppath, &st))
    {
      type = FTW_SLN;
    }
    else if (errno != EACCES)
    {
      return -1;
    }
    else
    {
      type = FTW_NS;
    }
  }
  else if (S_ISDIR(st.st_mode))
  {
    if (access(ppath, R_OK) < 0)
    {
      type = FTW_DNR;
    }
    else if (pflags & FTW_DEPTH)
    {
      type = FTW_DP;
    }
    else
    {
      type = FTW_D;
    }
  }
  else if (S_ISLNK(st.st_mode))
  {
    if (pflags & FTW_PHYS)
    {
      type = FTW_SL;
    }
    else
    {
      type = FTW_SLN;
    }
  }
  else
  {
    type = FTW_F;
  }

  if ((pflags & FTW_MOUNT) && ph && st.st_dev != ph->dev)
  {
    return 0;
  }

  hnew.chain = ph;
  hnew.dev   = st.st_dev;
  hnew.ino   = st.st_ino;
  hnew.level = ph ? ph->level + 1 : 0;
  hnew.base  = static_cast<int>(l + 1);

  lev.level = hnew.level;

  if (ph != nullptr)
  {
    lev.base = ph->base;
  }
  else
  {
    char *name;

    if ((name = strrchr(ppath, '/')))
    {
      lev.base = static_cast<int>(name - ppath);
    }
    else
    {
      lev.base = 0;
    }
  }

  if (!(pflags & FTW_DEPTH) && (r = nftw_cb(ppath, &st, type, &lev)))
  {
    return r;
  }

  for (; ph; ph = ph->chain)
  {
    if (ph->dev == st.st_dev && ph->ino == st.st_ino)
    {
      return 0;
    }
  }

  if ((type == FTW_D || type == FTW_DP) && pfdlimit)
  {
    DIR *d = opendir(ppath);

    if (d)
    {
      struct dirent *de;

      while ((de = readdir(d)))
      {
        if (de->d_name[0] == '.' &&
            (!de->d_name[1] || (de->d_name[1] == '.' && !de->d_name[2])))
        {
          continue;
        }

        if (strlen(de->d_name) >= PATH_MAX - l)
        {
          errno = ENAMETOOLONG;
          closedir(d);
          return -1;
        }

        ppath[j] = '/';
        strcpy(ppath + j + 1, de->d_name);

        if ((r = nftw_do(ppath, pfdlimit - 1, pflags, &hnew)))
        {
          closedir(d);
          return r;
        }
      }

      closedir(d);
    }
    else if (errno != EACCES)
    {
      return -1;
    }
  }

  ppath[l] = '\0';

  if ((pflags & FTW_DEPTH) && (r = nftw_cb(ppath, &st, type, &lev)))
  {
    return r;
  }

  return 0;
}

/******************************************************************************/
int sbo_filelist_t::nftw_cb(const char * const pfpath,
                            const struct stat * const psb, int const
                            ptypeflag, struct FTW * const pftwbuf)
/******************************************************************************/
{
  item_t item;

  (void)pftwbuf;

  if (ptypeflag == FTW_F && (psb->st_mode & S_IFMT) == S_IFREG)
  {
    string fn = pfpath;

    if (fn.length() >= 3U)
    {
      fn = fn.substr(2U);
      if (check_filename(fn))
      {
        item.name = fn;
        item.stat = *psb;
        result.push_back(item);
        if (file_callback) file_callback(fn);
      }
    }
  }

  return 0;
}

/******************************************************************************/
bool sbo_filelist_t::check_filename(string const &pfn) const
/******************************************************************************/
{
  /* Temporary directory within SlackBuild directory. */

  regex const re_tmpdir("^tmp(dir){0,1}(/.*)*$",
    regex_constants::nosubs | regex_constants::egrep);

  /* Qt Creator _Generic project_ file names regex. */

  regex const re_qtcgenproj("^.+\\.(cflags|config|creator|"
    "creator(\\..+){0,1}|cxxflags|files|includes|tasks)$",
    regex_constants::nosubs | regex_constants::egrep);

  if (pfn == "comment.txt"  ||
      pfn == "keywords.txt" ||
      regex_match(pfn, re_tmpdir) ||
      pfn == "get-source.sh" ||
      regex_match(pfn, re_qtcgenproj))
  {
    return false;
  }

  return true;
}
