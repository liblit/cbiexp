#include <argp.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ext/hash_map>
#include "fopen.h"
#include "CompactReport.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "RunsDirectory.h"
#include "SiteCoords.h"
#include "classify_runs.h"
#include "sites.h"
#include "units.h"

using namespace std;
using __gnu_cxx::hash_map;

string sampleRateFile;

/****************************************************************
 * Information about non-constant predicates
 ***************************************************************/

class site_info_t {
private:
  unsigned S, N, Z;
  double a;
  double b;
  double rho;
public:
  site_info_t () {
    S = N = Z = 0;
    a = (numeric_limits<double>::max)();
    b = 0.0;
    rho = 1.0;
  }
  void update(unsigned val);
  void setrho(double r) { rho = r; }
  void setmin(unsigned nruns);
  void print(ostream &out) const;
};

void site_info_t::update (unsigned val)
{
  N += 1;
  S += val;
  a = (a < val) ? a : (double) val;
  b = (b > val) ? b : (double) val;
}

inline void
site_info_t::setmin (unsigned nruns)
{
  Z = nruns - N;
  if (Z > 0)
    a = 0.0;
}

void
site_info_t::print (ostream &out) const
{
  out << S << '\t' << N << '\t' << Z << '\t'
      << a << '\t' << b << '\t' 
      << rho;
}

class SitePair;
typedef site_info_t (SitePair::* SiteInfo);
class SitePair
{
public:
  site_info_t f;
  site_info_t s;

  void update(SiteInfo, unsigned);
};

inline void
SitePair::update(SiteInfo si, unsigned val)
{
  (this->*si).update(val);
}

class site_hash_t : public hash_map<SiteCoords, SitePair> 
{
};

static site_hash_t siteHash;

// This function reads down sampling rates off of file, if the info
// is available.  The non-uniform down-sampling rates file may not
// contain sampling rates for all sites.  (A site is omiited if it
// was not observed in the training runs.)  The default behavior of 
// the decimator is to not do anything; therefore the default down
// sampling rate is 1.
void 
read_rates()
{
  if (sampleRateFile.empty()) {
    return;
  }

  FILE *const rates = fopenRead(sampleRateFile.c_str());
  SiteCoords coords;
  double rho;

  while (true) {
    fscanf(rates, "%u\t%u\t%lg\n", &coords.unitIndex, &coords.siteOffset, &rho);
    if (feof(rates))
      break;

    assert(coords.unitIndex < num_units);
    const unit_t unit = units[coords.unitIndex];
    assert(coords.siteOffset < unit.num_sites);

    site_hash_t::iterator found = siteHash.find(coords);
    if (found == siteHash.end()) {
      cerr << "Site " << coords << " doesn't exist in siteHash.\n";
      exit(1);
    }
    SitePair &sp = found->second;
    sp.f.setrho(rho);
    sp.s.setrho(rho);
  }

  fclose(rates);
}

void set_min()
{
  for (site_hash_t::iterator c = siteHash.begin(); c != siteHash.end(); ++c) {
    SitePair &sp = c->second;
    sp.f.setmin(num_fruns);
    sp.s.setmin(num_sruns);
  }
}

/****************************************************************
 * Information about each run
 ***************************************************************/

class Reader : public ReportReader
{
public:
  Reader(SiteInfo);
  void branchesSite(const SiteCoords &, unsigned, unsigned);
  void gObjectUnrefSite(const SiteCoords &, unsigned, unsigned, unsigned, unsigned);
  void returnsSite(const SiteCoords &, unsigned, unsigned, unsigned);
  void scalarPairsSite(const SiteCoords &, unsigned, unsigned, unsigned);

private:
  void tripleSite(const SiteCoords &, unsigned, unsigned, unsigned) const;
  void obs(const SiteCoords &, unsigned) const;
  const SiteInfo si;
};

inline
Reader::Reader(SiteInfo _si)
    : si(_si)
{
}

inline void
Reader::obs(const SiteCoords &coords, unsigned val) const
{
  const site_hash_t::iterator found = siteHash.find(coords);
  if (found != siteHash.end()) {
    SitePair &sp = found->second;
    sp.update(si,val);
  } else {
    SitePair sp;
    sp.update(si, val);
    siteHash[coords] = sp;
  }
}

void Reader::tripleSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) const
{
    assert(x || y || z);
    obs(coords, x+y+z);
}

inline void
Reader::scalarPairsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) 
{
    tripleSite(coords, x, y, z);
}


inline void
Reader::returnsSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z) 
{
    tripleSite(coords, x, y, z);
}

inline void
Reader::branchesSite(const SiteCoords &coords, unsigned x, unsigned y)
{
    assert(x||y);
    obs(coords, x+y);
}

inline void
Reader::gObjectUnrefSite(const SiteCoords &coords, unsigned x, unsigned y, unsigned z, unsigned w)
{
    assert(x || y || z || w);
    obs(coords, x+y+z+w);
}


/****************************************************************************
 * Print results to file
 ***************************************************************************/

inline ostream &
operator<< (ostream &out, const site_info_t &si)
{
  si.print(out);
  return out;
}

inline ostream &
operator<< (ostream &out, const SitePair &sp)
{
  out << sp.f << '\n' << sp.s;
  return out;
}

inline ostream &
operator<< (ostream &out, const site_hash_t &sh)
{
    for (site_hash_t::const_iterator c = sh.begin(); c != sh.end(); c++) {
	const SiteCoords sc = c->first;
	const SitePair sp = c->second;
	out << sc << '\n' << sp.f << '\n' << sp.s << '\n';
    }
    return out;
}

void print_results()
{
    ofstream parmsfp ("parmstats.txt", ios_base::trunc);
    parmsfp << siteHash;
    parmsfp.close();
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

static const argp_option options[] = {
  {
    "sample-rates",
    'd',
    "FILE",
    0,
    "use \"FILE\" as the downsampling rate for each site",
    0
  },
  { 0, 0, 0, 0, 0, 0 }
};

static int
parseFlag(int key, char *arg, argp_state *)
{
  switch (key)
    {
    case 'd':
      sampleRateFile = arg;
      return 0;
    default:
      return ARGP_ERR_UNKNOWN;
    }
}

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
	{ &CompactReport::argp, 0, 0, 0 },
	{ &NumRuns::argp, 0, 0, 0 },
	{ &RunsDirectory::argp, 0, 0, 0 },
	{ 0, 0, 0, 0 }
    };

    static const argp argp = {
	options, parseFlag, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}


/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    classify_runs();

    Progress::Bounded prog("Reading runs and collecting sufficient stats", NumRuns::count());
    for (unsigned r = NumRuns::begin; r < NumRuns::end; ++r) {
      prog.step();
      SiteInfo si = 0;
      if (is_srun[r])
	si = &SitePair::s;
      else if (is_frun[r])
        si = &SitePair::f;
      else
	continue;
  
      Reader(si).read(r);
    }

    read_rates();

    set_min();

    print_results();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
