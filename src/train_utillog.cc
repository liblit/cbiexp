#include <argp.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ext/hash_map>
#include <vector>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_permute.h>
#include "DatabaseFile.h"
#include "fopen.h"
#include "NumRuns.h"
#include "Progress/Bounded.h"
#include "ReportReader.h"
#include "classify_runs.h"
#include "split_runs.h"
#include "PredCoords.h"
#include "Nonconst_PredStats.h"
#include "UtilLogReg.h"

using namespace std;
using __gnu_cxx::hash_map;

/****************************************************************************
 * Definitions of constants, structs, and classes
 ***************************************************************************/

static double theta0;
static gsl_rng *generator;

struct confusion_matrix {
    unsigned tn;
    unsigned fn;
    unsigned fp;
    unsigned tp;
    confusion_matrix() {
	tn = fn = fp = tp = 0;
    }
};

/****************************************************************
 * Information about non-constant predicates
 ***************************************************************/
struct pred_info_t {
    float mean, var, std;
    count_tp min, max;
    count_tp val;
    double theta;
    pred_info_t () {
	mean = var = std = 0.0;
	min = max = 0;
	val = 0;
	theta = 0.0;
    }
    double init_theta() {
	return (theta = gsl_rng_uniform(generator) - 0.5);
    }	
};

class pred_hash_t : public hash_map<PredCoords, pred_info_t>
{
};

typedef pair<PredCoords,pred_info_t> pred_pair_t;

static pred_hash_t pred_hash;

bool read_nonconst_pred (FILE *fp, PredCoords &pc, pred_info_t &pi) 
{
    unsigned ntallied;
    const int got = fscanf(fp, "%u %u %g %g %Lu %Lu %u\n", &pc.siteIndex,
			   &pc.predicate, &pi.mean, &pi.var, &pi.min, &pi.max, &ntallied);
    
    pi.std = sqrt(pi.var);

    if (got == 7)
	return true;
    else
	return false;
}

void verify_stats () 
{
    for (pred_hash_t::iterator c = pred_hash.begin(); c != pred_hash.end(); c++) {
	const pred_info_t &pi = c->second;
	assert(pi.var > 0.0);
	assert(pi.mean > 0.0);
	assert(pi.max - pi.min > 0);
    }
}

void init_preds ()
{
    // init pred_info
    theta0 = gsl_rng_uniform(generator) - 0.5;
    
    const unsigned num_preds = Nonconst_PredStats::count();
    Progress::Bounded predProg("reading list of non-constant predicates", num_preds);
    FILE * const pfp = fopenRead(Nonconst_PredStats::filename);
    PredCoords pc;
    pred_info_t pi;
    unsigned nread = 0;
    while (read_nonconst_pred(pfp, pc, pi)) {
	predProg.step();
	nread++;
	pi.init_theta();
	pred_pair_t pval;
	pval.first = pc;
	pval.second = pi;
	pred_hash.insert(pval);
    }
    fclose(pfp);

    verify_stats();  // verify that the predicate statistics are valid
    assert(nread == num_preds);
}

/****************************************************************
 * Information about each run
 ***************************************************************/

struct run_info_t {
    pred_hash_t preds;
    int runId;
    run_info_t() {
	preds.clear();
	runId = -1;
    }
};

static run_info_t curr_run;

void
reset_curr_run(unsigned r)
{
    curr_run.preds.clear();
    curr_run.runId = r;
}

class Reader : public ReportReader
{
public:
  Reader(const char* filename) : ReportReader(filename) {} 

protected:
  void handleSite(const SiteCoords &, vector<count_tp> &);
};


void Reader::handleSite(const SiteCoords &coords, vector<count_tp> &counts)
{
    for (unsigned predicate = 0; predicate < counts.size(); ++predicate)
	if (counts[predicate]) {
	    PredCoords pcoords(coords, predicate);
	    pred_info_t pinfo;
	    pinfo.val = counts[predicate];
	    pred_pair_t pval;
	    pval.first = pcoords;
	    pval.second = pinfo;
	    curr_run.preds.insert(pval);
	}
}

/****************************************************************************
 * Update utility-based logistic regression parameters
 ***************************************************************************/
inline double 
get_val(const pred_info_t &pp, const pred_info_t &pi) 
{
 return (double) (pi.val - pp.min)/pp.std;
}

double calc_z () 
{
    double z = theta0;
    for (pred_hash_t::const_iterator c = pred_hash.begin(); c != pred_hash.end(); c++) {
	const PredCoords &pc = c->first;
	const pred_hash_t::iterator found = curr_run.preds.find(pc);
	if (found == curr_run.preds.end())
	    continue;
	else {
	    const pred_info_t &pp = c->second;
	    const pred_info_t &pi = found->second;
	    double val = get_val(pp, pi);
	    z += pp.theta * val;
	}
    }
    return z;
}

double calc_mu (const double z) {
    double mu = 1/ (1+exp(-z));
    assert(!isnan(mu));
    return mu;
}

double calc_ll (const double z) {
    double ll = 0.0;
    if (is_frun[curr_run.runId]) {
	ll = -log1p(exp(-z));
    }
    else if (is_srun[curr_run.runId]) {
	ll = -log1p(exp(z));
    }
    else {
	cerr << "Current run " << curr_run.runId << " is neither a success nor a failure.\n";
	exit(1);
    }

    assert(!isnan(ll));
    return ll;
}

double utillog_update() 
{
    double z = calc_z();
    double mu = calc_mu(z);
    double ll = calc_ll(z);
    double coeff = 0.0;
    double fp_penalty = 0.0;
    if (is_frun[curr_run.runId]) {
	coeff = UtilLogReg::tau * UtilLogReg::delta1 * (1-mu);
    }
    else if (is_srun[curr_run.runId]) {
	if (mu > 0.5)
	    fp_penalty = UtilLogReg::delta3;
	coeff = - UtilLogReg::tau * (UtilLogReg::delta2 * mu + fp_penalty);
    }
    else {
	cerr << "Current run " << curr_run.runId << " is neither a success nor a failure.\n";
	exit(1);
    }

    // update theta parameters
    theta0 += coeff;

    for (pred_hash_t::iterator c = pred_hash.begin(); c != pred_hash.end(); c++) {
	const PredCoords &pc = c->first;
	pred_info_t &pp = c->second;
	// first add the derivative of the l1-norm
	if (pp.theta > 0.0)
	    pp.theta -= UtilLogReg::tau * 0.1;
	else if (pp.theta < 0.0)
	    pp.theta += UtilLogReg::tau * 0.1;
	else
	    pp.theta -= UtilLogReg::tau * 1e-7 * (gsl_rng_uniform(generator) - 0.5);

	const pred_hash_t::iterator found = curr_run.preds.find(pc);
	if (found == curr_run.preds.end())
	    continue;
	else {
	    const pred_info_t &pi = found->second;
	    double val = get_val(pp, pi);
	    pp.theta += coeff * val;
	}
    }
    
    return ll;
}

void utillog_validate(vector<double> &lls, confusion_matrix &cm) {
    cm.tn = cm.tp = cm.fn = cm.fp = 0;

    Reader reader(DatabaseFile::DatabaseName);
    Progress::Bounded valProg ("validating", num_valruns);
    for (unsigned i = 0; i < num_valruns; i++) {
	valProg.step();
	unsigned r = val_runs[i];

        reset_curr_run(r);
	reader.read(r);

        double z = calc_z();
	double mu = calc_mu(z);
	double ll = calc_ll(z);
	lls[i] = ll;

	if (is_frun[curr_run.runId]) {
	    if (mu < 0.5)
		cm.fn++;
	    else if (mu > 0.5)
		cm.tp++;
	    else { // mu == 0.5
		if (gsl_rng_uniform(generator) < 0.5)
		    cm.fn++;
		else 
		    cm.tp++;
	    }
	}
	else if (is_srun[curr_run.runId]) {
	    if (mu < 0.5)
		cm.tn++;
	    else if (mu > 0.5)
		cm.fp++;
	    else { // mu == 0.5
		if (gsl_rng_uniform(generator) < 0.5)
		    cm.tn++;
		else 
		    cm.fp++;
	    }
	}
	else {
	    cerr << "Current run " << curr_run.runId << " is neither a success nor a failure.\n";
	    exit(1);
	}
	
    }
}

/****************************************************************************
 * Print results to file
 ***************************************************************************/

inline ostream &
operator<< (ostream &out, const vector<double> &lls)
{
    for (unsigned i = 0; i < lls.size(); i++) {
	out << lls[i] << ' ';
    }
    out << '\n';
    return out;
}

inline ostream &
operator<< (ostream &out, const confusion_matrix &cm)
{
    out << cm.tn << ' ' << cm.fp << ' ' << cm.fn << ' ' << cm.tp << '\n';
    return out;
}

inline ostream &
operator<< (ostream &out, const pred_hash_t &ph)
{
    for (pred_hash_t::const_iterator c = ph.begin(); c != ph.end(); c++) {
	const PredCoords pc = c->first;
	const pred_info_t pi = c->second;
	out << pc.siteIndex << ' ' << pc.predicate << ' '
	    << pi.theta << '\n';
    }
    return out;
}

void print_results(const vector<double> &train_lls, 
		   const vector<double> &val_lls, 
		   const confusion_matrix &cm)
{
    ofstream trainfp("train_lls.txt", ios_base::app);
    ofstream valfp("val_lls.txt", ios_base::app);
    ofstream cmfp("confmat.txt", ios_base::app);
    ofstream thetafp("theta.txt", ios_base::app);
    ofstream logregfp("logreg.txt");

    trainfp << train_lls;
    valfp << val_lls;
    cmfp << cm;
    thetafp << "-1 -1 " << theta0 << '\n';
    thetafp << pred_hash;
    logregfp << pred_hash;

    trainfp.close();
    valfp.close();
    cmfp.close();
    thetafp.close();
    logregfp.close();
}


/****************************************************************************
 * Processing command line options
 ***************************************************************************/

void process_cmdline(int argc, char** argv)
{
    static const argp_child children[] = {
        { &DatabaseFile::argp, 0, 0, 0 },
        { &NumRuns::argp, 0, 0, 0 },
        { &UtilLogReg::argp, 0, 0, 0 },
        { 0, 0, 0, 0 }
    };

    static const argp argp = {
        0, 0, 0, 0, children, 0, 0
    };

    argp_parse(&argp, argc, argv, 0, 0, 0);
}


/****************************************************************************
 * GSL Random Number Generator
 ***************************************************************************/

void init_gsl_generator ()
{
  FILE * const entropy = fopen("/dev/urandom", "rb");
  if (!entropy) {
    cerr << "Cannot open system entropy" << '\n';
    exit(1);
  }
  if (fread(&gsl_rng_default_seed, sizeof(gsl_rng_default_seed), 1, entropy) != 1) {
    cerr << "Cannot read system entropy" << '\n';
    exit(1);
  }
  fclose(entropy);

  generator = gsl_rng_alloc(gsl_rng_taus);
}

inline void free_gsl_generator ()
{
  gsl_rng_free(generator);
}


/****************************************************************************
 * MAIN
 ***************************************************************************/

int main(int argc, char** argv)
{
    process_cmdline(argc, argv);

    init_gsl_generator();

    classify_runs();

    split_runs();

    init_preds();
    
    // clear the output files first
    ofstream trainfp ("train_lls.txt", ios_base::trunc);
    ofstream valfp ("val_lls.txt", ios_base::trunc);
    ofstream cmfp ("confmat.txt", ios_base::trunc);
    ofstream thetafp ("theta.txt", ios_base::trunc);
    cmfp << "delta1 = " << UtilLogReg::delta1 
         << "; delta2 = " << UtilLogReg::delta2 
         << "; delta3 = " << UtilLogReg::delta3 << endl;
    trainfp.close();
    valfp.close();
    cmfp.close();
    thetafp.close();

    // init gsl permutation
    gsl_permutation * order = gsl_permutation_alloc ((size_t) num_trainruns);
    gsl_permutation_init(order);

    Progress::Bounded epochProg("optimizing logistic regression parameters", UtilLogReg::niters);
    for (unsigned n = 0; n < UtilLogReg::niters; n++) {
	epochProg.step();
	cout << '\n';

	vector<double> train_lls;
	vector<double> val_lls;
	confusion_matrix cm;
	train_lls.resize(num_trainruns);
	val_lls.resize(num_valruns);

	// generate new ordering of training data
	gsl_ran_shuffle (generator, order->data, num_trainruns, sizeof(size_t));
	gsl_permute_uint (gsl_permutation_data(order), &(train_runs.front()), 1, (size_t) num_trainruns);

        Reader reader(DatabaseFile::DatabaseName);
	Progress::Bounded trainProg("reading runs", num_trainruns);
	for (unsigned i = 0; i < num_trainruns; i++) {
	  trainProg.step();
	  unsigned r = train_runs[i];
	  
          reset_curr_run(r);
	  reader.read(r);

	  double ll = utillog_update();
	  train_lls[i] = ll;
	}

	cout << '\n';

	utillog_validate(val_lls, cm);

	gsl_permute_uint_inverse(gsl_permutation_data(order), &(train_runs.front()), 1, (size_t) num_trainruns);
	gsl_permute_inverse(gsl_permutation_data(order), &(train_lls.front()), 1, (size_t) num_trainruns);

	print_results(train_lls, val_lls, cm);
    }


    gsl_permutation_free(order);
    free_gsl_generator();

    return 0;
}


// Local variables:
// c-file-style: "cc-mode"
// End:
