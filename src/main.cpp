#include <string>
#include <iostream>
#include <stdlib.h>
#include <curl/curl.h>
#include "sbo_archive.h"
#include "sbo_category.h"
#include "sbo_comment.h"
#include "sbo_env.h"
#include "sbo_filelist.h"
#include "sbo_info.h"
#include "sbo_keywords.h"
#include "sbo_optarg.h"
#include "sbo_readme.h"
#include "sbo_slack_desc.h"
#include "sbo_slackbuild.h"
#include "sbo_util.h"
#include "sbo_xfer.h"

using namespace std;

static void print_file_name(string const &pname);
static void print_ar_file(string const &pname, int64_t const psize);
static void print_ar_add(string const &piname, string const &poname);
static void print_download(sbo_info_t const &pinfo, int const pwhich);
static void print_requires(sbo_info_t const &pinfo);
static void confirm_dialog(void);

/******************************************************************************/
int main(int argc, char * const argv[])
/******************************************************************************/
{
  int res;
  CURLcode cres;
  bool ldres = false;
  sbo_optarg_t const opt(argc, argv);
  sbo_env_t const env;
  string sub_email;
  string cwd;
  string cat;
  string nam;
  sbo_comment_t comment;
  sbo_info_t info;
  sbo_keywords_t keywords;
  sbo_readme_t readme;
  sbo_slack_desc_t desc;
  sbo_slackbuild_t slackbuild;
  sbo_filelist_t fl;
  sbo_archive_t ar;
  sbo_xfer_t xfer;

  res = cnul_init();
  if (res != 0) exit(EXIT_FAILURE);

  set_verbosity(opt.verbose);

  if (opt.comment.length() == 0U)
  {
    imsg(1) << "no submission comment specified." << endl;
  }
  else
  {
    if (opt.comment == "@file")
    {
      imsg(1) << "submission comment to be read from comment.txt" << endl;
    }
    else
    {
      imsg(1) << "submission comment given on the command line." << endl;
    }
  }

  if (opt.email.length() == 0U)
  {
    imsg(1) << "no submission e-mail found on the command line." << endl;
  }
  else
  {
    imsg(1) << "submission e-mail found on the command line: "
            << opt.email << endl;
  }

  if (opt.keywords.length() == 0U)
  {
    imsg(1) << "no submission keywords specified." << endl;
  }
  else
  {
    if (opt.keywords == "@file")
    {
      imsg(1) << "submission keywords to be read from keywords.txt" << endl;
    }
    else
    {
      imsg(1) << "submission keywords given on the command line." << endl;
    }
  }

  if (env.sub_email.length() == 0U)
  {
    imsg(1) << "no submission e-mail found in the environment." << endl;
  }
  else
  {
    imsg(1) << "submission e-mail found in the environment: "
            << sub_email << endl;
  }

  if (env.sub_email.length() == 0U && opt.email.length() == 0U)
  {
    emsg() << "no submission e-mail specified and it is required." << endl;
    exit(EXIT_FAILURE);
  }
  else
  {
    if (opt.email.length() > 0U)
    {
      imsg(1) << "using submission e-mail given on the command line: "
              << opt.email << endl;
      sub_email = opt.email;
    }
    else
    {
      imsg(1) << "using submission e-mail found in the environment: "
              << sub_email << endl;
      sub_email = env.sub_email;
    }
  }

  if (env.info_maint.length() == 0U)
  {
    wmsg() << "no .info maintainer found in the environment," << endl;
    wmsg() << ".info will not be checked against this value." << endl;
  }
  else
  {
    imsg(1) << ".info maintainer found in the environment: "
            << env.info_maint << "," << endl;
    imsg(1) << ".info will be checked against this value." << endl;
  }

  if (env.info_email.length() == 0U)
  {
    wmsg() << "no .info email found in the environment," << endl;
    wmsg() << ".info will not be checked against this value." << endl;
  }
  else
  {
    imsg(1) << ".info email found in the environment: "
            << env.info_email << "," << endl;
    imsg(1) << ".info will be checked against this value." << endl;
  }

  getcwd(cwd);
  imsg() << "CWD: " << cwd << endl;

  get_cat_prgnam(cwd, cat, nam);

  if (cat.length() == 0U || nam.length() == 0U)
  {
    emsg() << "unable to determine category and/or PRGNAM," << endl;
    emsg() << "make sure this program is run from within correct directory."
           << endl;
    exit(EXIT_FAILURE);
  }

  imsg(1) << "category: " << cat << endl;
  imsg(1) << "PRGNAM: " << nam << endl;

  if (!sbo_check_cat(cat))
  {
    emsg() << "unknown category: " << cat << endl;
    exit(EXIT_FAILURE);
  }

  slackbuild.load(nam);
  desc.load(nam);
  info.load(nam);
  readme.load();
  keywords.load(opt.keywords);
  comment.load(opt.comment);

  ldres = slackbuild.is_error(ldres);
  ldres = desc.is_error(ldres);
  ldres = info.is_error(ldres);
  ldres = readme.is_error(ldres);
  ldres = keywords.is_error(ldres);
  ldres = comment.is_error(ldres);
  if (ldres) exit(EXIT_FAILURE);

  imsg() << "searching for files..." << endl;
  if (opt.verbose > 0) fl.set_file_cb(print_file_name);
  res = fl.find();
  if (res == -1) exit(EXIT_FAILURE);
  fl.sort();

  imsg() << "creating archive..." << endl;
  ar.set_prgnam(nam);
  if (opt.verbose > 0) ar.set_add_file_cb(print_ar_add);
  res = ar.create(fl);
  if (res == -1) exit(EXIT_FAILURE);

  if (opt.verbose > 0)
  {
    imsg() << "submission archive listing follows:" << endl;
    ar.set_list_file_cb(print_ar_file);
    res = ar.list();
    if (res == -1) exit(EXIT_FAILURE);
    imsg() << "archive size: " << ar.get_size() << " byte(s)." << endl;
  }

  if (opt.address.length() == 0U)
  {
    xfer.set_url("https://slackbuilds.org/process_submit/");
  }
  else
  {
    xfer.set_url(opt.address);
  }

  imsg() << "submission url: " << xfer.get_url() << endl;

  cout << endl;
  cout << "          SUBMISSION SUMMARY:" << endl;
  cout << "========================================\n" << endl;
  cout << "Category:  "     << cat << endl;
  cout << "PRGNAM:    "     << nam << endl;
  cout << "VERSION:   "     << info.version  << endl;
  cout << "HOMEPAGE:  "     << info.homepage << endl;
  cout << "DOWNLOAD:  ";    print_download(info, 0);
  cout << "DOWNLOAD64:  ";  print_download(info, 1);
  cout << "REQUIRES:    ";  print_requires(info);
  cout << "MAINTAINER:  "   << info.maintainer << endl;
  cout << "EMAIL:       "   << info.email      << endl;
  cout << "\n";
  cout << "Archive name:  " << nam << ".tar.gz" << endl;
  cout << "Archive size:  " << ar.get_size() << endl;
  cout << "Archive listing (uncompressed size):" << endl;
  ar.set_list_file_cb(print_ar_file);
  res = ar.list();
  cout << "e-mail:    " << sub_email << endl;
  cout << "Keywords:  " << keywords  << endl;
  cout << "Comment:   " << comment   << endl;

  confirm_dialog();

  cres = curl_global_init(CURL_GLOBAL_ALL);
  if (cres != CURLE_OK)
  {
    emsg() << curl_easy_strerror(cres) << endl;
    exit(EXIT_FAILURE);
  }

  xfer.set_category(cat);
  xfer.set_tags(keywords);
  xfer.set_submail(sub_email);
  xfer.set_comments(comment);

  imsg() << "uploading archive to remote server..." << endl;
  res = xfer.upload(ar);
  if (res == -1) exit(EXIT_FAILURE);

  imsg() << "Uploading of the submission archive done. Please review" << endl;
  imsg() << "server reply to make sure everything went correctly." << endl;

  return 0;
}

/******************************************************************************/
static void print_file_name(string const &pname)
/******************************************************************************/
{
  cout << "  + " << pname << endl;
}

/******************************************************************************/
static void print_ar_file(string const &pname, int64_t const psize)
/******************************************************************************/
{
  cout << "  + " << pname << " [" << psize << "]" << endl;
}

/******************************************************************************/
static void print_ar_add(string const &piname, string const &poname)
/******************************************************************************/
{
  cout << "  + " << piname << " --> " << poname << endl;
}

/******************************************************************************/
static void print_download(sbo_info_t const &pinfo, int const pwhich)
/******************************************************************************/
{
  vector<string> const *urlsrc = &pinfo.download;
  vector<string> const *sumsrc = &pinfo.checksum;

  if (pwhich != 0)
  {
    urlsrc = &pinfo.download64;
    sumsrc = &pinfo.checksum64;
  }

  if (urlsrc->size() == 0U)
  {
    cout << "none" << endl;
  }
  else
  {
    auto url = urlsrc->begin();
    auto sum = sumsrc->begin();

    cout << endl;

    for ( ; (url != urlsrc->end()) && (sum != sumsrc->end()); ++url, ++sum)
    {
      cout << "  + [" << *sum << "]  " << *url << endl;
    }
  }
}

/******************************************************************************/
static void print_requires(sbo_info_t const &pinfo)
/******************************************************************************/
{
  if (pinfo.requires.size() == 0U)
  {
    cout << "none" << endl;
  }
  else
  {
    cout << endl;

    for (auto item = pinfo.requires.begin();
      item != pinfo.requires.end(); ++item)
    {
      cout << "  + " << *item << endl;
    }
  }
}

/******************************************************************************/
static void confirm_dialog(void)
/******************************************************************************/
{
  string ans;

  cout << "\n"
"    Submission creation complete. Please review all the details and make sure\n"
"    that everything is correct. Type \"yes\" to upload the submission archive\n"
"    or CTRL-C to abort.\n" << endl;

  do {
    cout << "    Type \"yes\" to upload or CTRL-C to abort: ";
    getline(cin, ans);
  } while (ans != "yes");

  cout << endl;
}
