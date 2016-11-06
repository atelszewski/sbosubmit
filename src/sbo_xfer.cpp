#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <curl/curl.h>
#include "sbo_util.h"
#include "sbo_classlog.h"
#include "sbo_archive.h"
#include "sbo_comment.h"
#include "sbo_keywords.h"
#include "sbo_html_filter.h"

static int const XFER_TIMEOUT = 20L;
static char const *MAX_FILE_SIZE = "1048576";

static size_t curl_write_wrapper(char *pptr,
  size_t psize, size_t pnmemb, void *puserdata);

/* Late inclusion so that _friend_ declaration works as expected. */
#include "sbo_xfer.h"

/******************************************************************************/
sbo_xfer_t::sbo_xfer_t() :
  sbo_classlog_t("upload"),
  tmpfd(-1)
/******************************************************************************/
{
}

/******************************************************************************/
sbo_xfer_t::~sbo_xfer_t()
/******************************************************************************/
{
  fdclose(tmpfd);
}

/******************************************************************************/
int sbo_xfer_t::upload(sbo_archive_t const &par)
/******************************************************************************/
{
  string tmppath;
  CURL *curl = nullptr;
  CURLcode cres = CURLE_OK;
  CURLFORMcode fres = CURL_FORMADD_OK;
  struct curl_httppost *cpost = nullptr;
  struct curl_httppost *clast = nullptr;
  struct curl_slist *hlist = nullptr;
  sbo_html_filter_t htmlflt;
  int fltres;

  if (par.data == nullptr || par.size == 0U || par.arname.length() == 0U)
  {
    emsg() << "invalid archive." << endl;
    return -1;
  }

  if (url.length() == 0U)
  {
    emsg() << "url must be set." << endl;
    return -1;
  }

  if (category.length() == 0U)
  {
    emsg() << "category must be set." << endl;
    return -1;
  }

  if (submail.length() == 0U)
  {
    emsg() << "submail must be set." << endl;
    return -1;
  }

  if (tags.length() == 0U)
  {
    imsg(1) << "no tags set." << endl;
  }

  if (comments.length() == 0U)
  {
    imsg(1) << "no comment set." << endl;
  }

  tmppath = "/tmp/sbosubmit.XXXXXX";
  if ((tmpfd = mktmpfile(tmppath)) == -1)
  {
    emsg() << "unable to create temporary file." << endl;
    return -1;
  }

  if (fres == CURL_FORMADD_OK) fres =
    curl_formadd(&cpost, &clast,
                 CURLFORM_COPYNAME,    "MAX_FILE_SIZE",
                 CURLFORM_COPYCONTENTS, MAX_FILE_SIZE,
                 CURLFORM_END);
  if (fres == CURL_FORMADD_OK) fres =
    curl_formadd(&cpost, &clast,
                 CURLFORM_COPYNAME,     "userfile",
                 CURLFORM_BUFFER,       par.arname.c_str(),
                 CURLFORM_BUFFERPTR,    par.data,
                 CURLFORM_BUFFERLENGTH, static_cast<long>(par.size),
                 CURLFORM_END);
  if (fres == CURL_FORMADD_OK) fres =
    curl_formadd(&cpost, &clast,
                 CURLFORM_COPYNAME,     "category",
                 CURLFORM_COPYCONTENTS,  category.c_str(),
                 CURLFORM_END);
  if (fres == CURL_FORMADD_OK) fres =
    curl_formadd(&cpost, &clast,
                 CURLFORM_COPYNAME,     "tags",
                 CURLFORM_COPYCONTENTS,  tags.c_str(),
                 CURLFORM_END);
  if (fres == CURL_FORMADD_OK) fres =
    curl_formadd(&cpost, &clast,
                 CURLFORM_COPYNAME,     "submail",
                 CURLFORM_COPYCONTENTS,  submail.c_str(),
                 CURLFORM_END);
  if (fres == CURL_FORMADD_OK) fres =
    curl_formadd(&cpost, &clast,
                 CURLFORM_COPYNAME,     "comments",
                 CURLFORM_COPYCONTENTS,  comments.c_str(),
                 CURLFORM_END);
  if (fres == CURL_FORMADD_OK) fres =
      curl_formadd(&cpost, &clast,
                   CURLFORM_COPYNAME,     "submit",
                   CURLFORM_COPYCONTENTS, "Submit",
                   CURLFORM_END);
  if (fres != CURL_FORMADD_OK)
  {
    emsg() << "curl_formadd(): " << fres << endl;
    fdclose(tmpfd);
    path_unlink(tmppath);
    curl_formfree(cpost);
    cpost = nullptr;
    clast = nullptr;
    return -1;
  }

  curl = curl_easy_init();
  if (curl == nullptr)
  {
    emsg() << "curl_easy_init(): unknown error (out of memory?)."
           << endl;
    fdclose(tmpfd);
    path_unlink(tmppath);
    curl_formfree(cpost);
    cpost = nullptr;
    clast = nullptr;
    return -1;
  }

  if (cres == CURLE_OK) cres =
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, XFER_TIMEOUT);
  if (cres == CURLE_OK) cres = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  if (cres == CURLE_OK) cres = curl_easy_setopt(curl, CURLOPT_HTTPPOST, cpost);
  if (cres == CURLE_OK) cres =
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_wrapper);
  if (cres == CURLE_OK) cres = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

  hlist = curl_slist_append(hlist, "Expect:");
  if (cres == CURLE_OK) cres =
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hlist);

  if (cres == CURLE_OK)
  {
    cres = curl_easy_perform(curl);
  }

  if (cres != CURLE_OK)
  {
    emsg() << curl_easy_strerror(cres) << endl;
    fdclose(tmpfd);
    path_unlink(tmppath);
    curl_easy_cleanup(curl);
    curl = nullptr;
    curl_formfree(cpost);
    cpost = nullptr;
    clast = nullptr;
    curl_slist_free_all(hlist);
    hlist = nullptr;
    return -1;
  }

  fdclose(tmpfd);
  curl_easy_cleanup(curl);
  curl = nullptr;
  curl_formfree(cpost);
  cpost = nullptr;
  clast = nullptr;
  curl_slist_free_all(hlist);
  hlist = nullptr;

  htmlflt.set_filename(tmppath);
  fltres = htmlflt.setup();

  if (fltres == 0)
  {
    cout << "\n========== SERVER REPLY START ==========" << endl;
    fltres = htmlflt.dump();
    cout <<   "=========== SERVER REPLY END ===========\n" << endl;
  }

  if (fltres == 0)
  {
    path_unlink(tmppath);
  }
  else
  {
    wmsg() << "there was an error running HTML filter," << endl;
    wmsg() << "server's reply has been held in: " << tmppath << endl;
  }

  return 0;
}

/******************************************************************************/
void sbo_xfer_t::set_url(string const &purl)
/******************************************************************************/
{
  url = purl;
}

/******************************************************************************/
string const &sbo_xfer_t::get_url(void) const
/******************************************************************************/
{
  return url;
}

/******************************************************************************/
void sbo_xfer_t::set_category(string const &pcat)
/******************************************************************************/
{
  category = pcat;
}

/******************************************************************************/
void sbo_xfer_t::set_tags(sbo_keywords_t const &pkeywords)
/******************************************************************************/
{
  tags = pkeywords.keywords;
}

/******************************************************************************/
void sbo_xfer_t::set_submail(string const &pemail)
/******************************************************************************/
{
  submail = pemail;
}

/******************************************************************************/
void sbo_xfer_t::set_comments(sbo_comment_t const &pcomment)
/******************************************************************************/
{
  comments = pcomment.comment;
}

/******************************************************************************/
size_t sbo_xfer_t::curl_write(char const *pdata, size_t const psize)
/******************************************************************************/
{
  size_t retsize = 0U;
  ssize_t rc;

  do {
    rc = write(tmpfd, &pdata[retsize], psize - retsize);
    if (rc == -1 && errno == EINTR) rc = 0;
    retsize += static_cast<size_t>(rc);
  } while (retsize < psize && rc != -1);

  if (rc == -1) retsize = 0U;

  return retsize;
}

/******************************************************************************/
void sbo_xfer_t::fdclose(int &pfd)
/******************************************************************************/
{
  if (pfd != -1) close(pfd);
  pfd = -1;
}

/******************************************************************************/
void sbo_xfer_t::path_unlink(string const &ppath)
/******************************************************************************/
{
  unlink(ppath.c_str());
}

/******************************************************************************/
static size_t curl_write_wrapper(char *pptr,
  size_t psize, size_t pnmemb, void *puserdata)
/******************************************************************************/
{
  sbo_xfer_t *xfer = static_cast<sbo_xfer_t *>(puserdata);

  return xfer->curl_write(pptr, psize * pnmemb);
}
