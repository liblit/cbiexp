#include <stdlib.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "ReportReader.h"
#include "SiteCoords.h"
#include "StaticSiteInfo.h"
#include "classify_runs.h"
#include "utils.h"

using namespace std;

ReportReader::ReportReader(const char* filename)
{
    classify_runs();
    countfile = string(filename);

    ifstream summary;
    summary.exceptions(ios::badbit);
    summary.open(countfile.c_str());
    if (!summary)
    {
      const int code = errno;
      ostringstream message;
      message << "cannot read " << filename << ": " << strerror(code);
      throw runtime_error(message.str());
    }

    //build vector of positions
    while (summary.peek() != EOF)
    {

      positions.push_back(summary.tellg());

      while(summary.peek() != '\n' && summary.peek() != EOF)
      {
        char buf[1024];
        summary.get(buf, 1024);
      }

      if (summary.peek() != EOF) summary.get();

    }
    summary.close();
}

void
ReportReader::read(unsigned runId)
{

  if (!is_srun[runId] && !is_frun[runId])
  {
    ostringstream message;
    message << "Ill-formed run " << runId;
    throw runtime_error(message.str());
  }

  if (runId >= positions.size())
  {
    ostringstream message;
    message << runId << " too large.";
    throw runtime_error(message.str());
  }

  ifstream summary;
  summary.exceptions(ios::badbit);
  summary.open(countfile.c_str());

  // move to correct line position
  summary.seekg(positions[runId]);

  //advanced to correct position, now reading report
  stringbuf runbuf;
  summary.get(runbuf);

  string runinfo = runbuf.str();

  if (runinfo.length() != 0) { //if there is some info for this run

      stringstream line(runinfo);

      bool reached_end  = false;
      do //start reading sites one by one
      {

        stringbuf peek;
        line.get(peek, '\t');
        string info = peek.str();

        if (line.peek() != '\t')
            reached_end = true; //reached end
        else
            line.get(); //throw the '\t' away

        //got site information, ready to handle

        if (info.length() != 0) //if the site had some data
        {

            stringstream site(info);
            stringbuf indexbuf;

            //reading index
            site.get(indexbuf, ':');
            unsigned index = atoi(indexbuf.str().c_str());
            site.get();

            //reading counts
            vector<count_tp> counts;
            do //start reading numbers one by one
            {
              stringbuf itembuf;
              site.get(itembuf, ':');
              site.get();
              char *tail;
              counts.push_back(strtol(itembuf.str().c_str(), &tail, 0));
            } while(site.gcount() > 0);

            //got what we need, now handle
            handleSite(SiteCoords(index), counts);

        }
        else
        {
            reached_end = true;
        }

      } while(reached_end == false);

  }

  summary.close();

}
