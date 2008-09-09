#include <cassert>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>
#include <set>
#include <sstream>
#include <stdexcept>
#include "CompactReport.h"
#include "ReportReader.h"
#include "SiteCoords.h"
#include "StaticSiteInfo.h"
#include "classify_runs.h"
#include "utils.h"

using namespace std;

ReportReader::ReportReader(const char* filename)
{
    summary.exceptions(ios::badbit);
    summary.open(filename);
    if (!summary)
    {
      const int code = errno;
      ostringstream message;
      message << "cannot read " << filename << ": " << strerror(code);
      throw runtime_error(message.str());
    }
    linepos = 0;
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

  if (linepos == runId); //at line position we want to read
  else //must reposition
  {
    summary.seekg(0); //go to start of file and read until linepos is runId
    linepos = 0;
    char buf[1024];
    int lastchar = 0;
    do //keep reading lines until runId is linepos or EOF reached
    {
      if (lastchar == EOF)
      {
          ostringstream message;
          message << "runId " << runId << " exceeds number of runs";
          throw runtime_error(message.str());
      }
      do
      {
         summary.get(buf, 1024);
      } while(summary.gcount() > 0);
      linepos++;
      lastchar = summary.get();
    } while (linepos < runId);
  }

  //advanced to correct position, now reading report

  bool reached_end  = false;
  do //start reading sites one by one
  {

    //reading up to first tab
    stringbuf peek;
    summary.get(peek, '\t');

    string info = peek.str();

    //if there's a newline in data need to back up
    if (info.find_first_of("\n") != string::npos)
    {
      stringbuf buf;
      //all this casting is to help gcc disambiguate seekg call
      summary.seekg((streampos)((int)summary.tellg() - (int)summary.gcount()));
      if (summary.peek() != '\n')
      {
        summary.get(buf, '\n');
        info = buf.str();
      }
      else
      {
        info = "";
      }
      reached_end = true;
    }

    //consume the remaining character whether tab or newline
    summary.get();

    //got site information, ready to handle

    if (info.length() != 0) //if there was some data for this run
    {

        stringstream line(info);
        stringbuf indexbuf;
 
        //reading index
        line.get(indexbuf, ':');
        unsigned index = atoi(indexbuf.str().c_str());
        line.get();
 
        //reading counts
        vector<count_tp> counts;
        do //start reading numbers one by one
        {
          stringbuf itembuf;
          line.get(itembuf, ':');
          line.get();
          char *tail;
          counts.push_back(strtol(itembuf.str().c_str(), &tail, 0));
        } while(line.gcount() > 0);
 
        //got what we need, now handle
        handleSite(SiteCoords(index), counts);

    }

  } while(reached_end == false && summary.gcount() != 0);

  linepos++;

}
