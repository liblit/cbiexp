#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <time.h>

#include "../Confidence.h"
#include "../Progress/Bounded.h"

#include "Complex.h"
#include "Conjunction.h"
#include "Disjunction.h"
#include "Predicate.h"

using namespace std;

int Complex::nextIndex = 0;
int Complex::perfectCount = 0;

Complex::Complex(char type_t, Predicate *pred1_t, Predicate *pred2_t)
  :Predicate(nextIndex)
{
  nextIndex ++;
  if(nextIndex == 0)
    cout << "Warning: Complex predicate index overflow\n";

  type = type_t;

  pred1 = pred1_t;
  pred2 = pred2_t;
}

bool
Complex::isInteresting() {
    return initial > pred1->initial && initial > pred2->initial;
}

double
Complex::score() {
  if(tru.failures.count == 0)
    return 0;
  if(increase() < 0 || lowerBound() < 0 || harmonic() < 0)
    return 0;
  return Predicate::score();
}  

double
Complex::lowerBound() const
{
  const double standardError = sqrt(tru.errorPart() + obs.errorPart());
  return increase() - Confidence::critical(99) * standardError;
}


std::string
Complex::bugometerXML() {
  //Returns a string representing a bug-o-meter XML tag.

  ostringstream oss( ostringstream::out );
  oss << "<bug-o-meter true-success=\"" << tru.successes.count
      << "\" true-failure=\"" << tru.failures.count
      << "\" seen-success=\"" << obs.successes.count
      << "\" seen-failure=\"" << obs.failures.count
      << "\" fail=\"" << badness()
      << "\" context=\"" << context()
      << "\" increase=\"" << increase()
      << "\" lower-bound=\"" << lowerBound()
      << "\" log10-true=\"" << log10(double(tru.count()))
      << "\"/>";
  return oss.str();
}

std::vector<unsigned>
Complex::getPredicateList() const
{
  //Returns a vector containing all the indices for the
  //predicates being conjoined to form this one.  For a
  //primitive predicate (not a Complex) this vector has
  //one element.  If printing to an XML file, add one to each
  //element.

  std::vector<unsigned> vect1 = pred1->getPredicateList();
  std::vector<unsigned> vect2 = pred2->getPredicateList();
  std::vector<unsigned> conjVect;

  //Copy the elements into the conjoined array
  for ( unsigned i = 0; i < vect1.size(); i++ ) 
    conjVect.push_back( vect1.at(i) );
  for ( unsigned i = 0; i < vect2.size(); i++ )
    conjVect.push_back( vect2.at(i) );

  return conjVect;
}

std::ostream &
operator<<(std::ostream &out, const Complex &c)
{
  return
    out << *(c.pred1) << " " << (c.pred1)->score() << "\n"
        << *(c.pred2) << " " << (c.pred1)->score();
}

double Complex::max(double a, double b) {
  return (a > b)? a: b;
};

double Complex::min(double a, double b) {
  return (a < b)? a: b;
};

// These methods are not members of class Complex
map<int, set<int> > pairs;
unsigned afterCSURF = 0;
bool atleast_one = false;

void read_pairs() {
  ifstream in("pairs.txt");

  if (!in) {
      const int code = errno;
      cerr << "Cannot read pairs.txt: " << strerror(code) << '\n'; 
      exit(code || 1); 
  }

  int a, b;
  
  while(!in.eof()) {
    in >> a >> b;
    
    if(in.eof())
      break;
    pairs[a].insert(b);
    afterCSURF += 2;
    atleast_one = true;
  }
  in.close();
}

bool isValidPair(int p1, int p2) {
  // If we haven't used csurf
  // Assuming that there will be atleast one pair after CSURF pruning
  if(!atleast_one)
    return true;

  int small = (p1 < p2) ? p1: p2;
  int big = (p1 < p2) ? p2: p1;
  
  if(pairs[small].count(big) > 0)
    return true;
  else
    return false;
}

// Computes complex of predicates of size 2 from input list.  The length of
// the resulting candidate list is bound by the second parameter.
// Only complex predicates with estimates higher than 'lb' are considered
std::list<Complex>
combine(Candidates &candidates, unsigned limit, double lb, FILE * fout) {

  time_t begin, finish; 

  std::list<Complex> result;
  double minMax = lb;
  
  unsigned total = candidates.size() * candidates.size() - candidates.size();
  unsigned prunedC = 0, computedC = 0, interestingC = 0;
  unsigned prunedD = 0, computedD = 0, interestingD = 0;
  
  if(afterCSURF == 0)
    afterCSURF = total;

  if(limit == 0) {
    return result;
  }

  Progress::Bounded progress("Finding conjunctions and disjunctions", total / 2);
  time(&begin);
  
  Candidates::iterator i, j;
  for(i = candidates.begin(); i != candidates.end(); i ++) {
    for(j = candidates.begin(); j != i; j ++) {
      
      progress.step();        
      if(! isValidPair(i->index, j->index))
        continue;
      
      bool skip_conj = false;
      if(result.size() == limit) {
        Conjunction dummy(&*i, &*j, true);
        if(dummy.score() < minMax) {
          prunedC ++;
          skip_conj = true;
        }
      }
      
      if(!skip_conj) {
        computedC ++;
        Conjunction c(&*i, &*j);
        if(c.isInteresting()) {
          result.push_back(c);
          
          interestingC ++;          
          if(result.size() > limit) {
            result.erase(min_element(result.begin(), result.end()));
            minMax = min_element(result.begin(), result.end())->score();
          }
        }
      }
      
      bool skip_disj = false;
      if(result.size() == limit) {
        Disjunction dummy(&*i, &*j, true);
        if(dummy.score() < minMax) {
          prunedD ++;
          skip_disj = true;
        }
      }
      
      if(!skip_disj) {
        computedD ++;
        Disjunction d(&*i, &*j);
        if(d.isInteresting()) {
            result.push_back(d);
            
            interestingD ++;
            if(result.size() > limit) {
              result.erase(min_element(result.begin(), result.end()));
              minMax = min_element(result.begin(), result.end())->score();
            }
        } 
      }
    }
  }
  
  time(&finish);
  time_t exec_time = finish - begin;

  printf("COMBINE:: was able to prune %u conjunctions, %u disjunctions\n", prunedC, prunedD);
  printf("COMBINE:: In all, %u %u complex predicates were interesting\n", interestingC, interestingD);
  printf("COMBINE:: :( Had to compute %u %u complex predicates\n", computedC, computedD);
  
  result.sort();
  result.reverse();

  if ( fout != NULL ) {
    // sanity check
    // Actually, this assertion is false in the second and future iterations of corrective-ranking
    // assert(afterCSURF == prunedC + computedC + prunedD + computedD);

    fprintf(fout, "%u %u\n", total, total - afterCSURF); // Total possible complex predicates
    fprintf(fout, "%u %u %u\n", prunedC, computedC, interestingC);
    fprintf(fout, "%u %u %u\n", prunedD, computedD, interestingD);
    fprintf(fout, "%u\n", exec_time);
    
    if(result.size() > 0)
      fprintf(fout, "%lf %lf\n", result.front().score(), result.back().score());
    else
      fprintf(fout, "-1 -1\n");
  }
  return result;
}

time_t
fast_estimate(Candidates &candidates) {
  time_t begin, finish;
  
  unsigned total = candidates.size() * candidates.size() - candidates.size();
  
  Progress::Bounded progress("Fast estimate", total / 2);
  time(&begin);
  
  Candidates::iterator i, j;
  for(i = candidates.begin(); i != candidates.end(); i ++) {
    for(j = candidates.begin(); j != i; j ++) {
      progress.step();        
      if(! isValidPair(i->index, j->index))
        continue;
      
      Conjunction dummyC(&*i, &*j, true);
      Disjunction dummyD(&*i, &*j, true);
    }
  }
  
  time(&finish);
  time_t exec_time = finish - begin;
  
  printf("FAST ESTIMATE:: Time: %u\n", exec_time);
  return exec_time;
}

std::list<Complex>
combine1(Candidates &candidates, unsigned limit, double lb, FILE * fout) {

  time_t begin, finish; 

  std::list<Complex> result;
  double minMax = lb;
  
  unsigned total = candidates.size() * candidates.size() - candidates.size();
  unsigned prunedC = 0, computedC = 0, interestingC = 0;
  unsigned prunedD = 0, computedD = 0, interestingD = 0;
  unsigned wrongC = 0, wrongD = 0;
  
  if(afterCSURF == 0)
    afterCSURF = total;

  if(limit == 0) {
    return result;
  }

  Progress::Bounded progress("Finding conjunctions and disjunctions", total / 2);
  time(&begin);
  
  Candidates::iterator i, j;
  for(i = candidates.begin(); i != candidates.end(); i ++) {
    for(j = candidates.begin(); j != i; j ++) {
      
      progress.step();        
      if(! isValidPair(i->index, j->index))
        continue;
      
      Conjunction dummyC(&*i, &*j, true);
      Conjunction c(&*i, &*j);
      
      if(dummyC.isInteresting())
        computedC ++;
      else
        prunedC ++;
      
      if(c.score() - dummyC.score() > 0.00001)
        wrongC ++;
        
      if(c.isInteresting()) {
        interestingC ++;
         
        if(result.size() == 0 || result.front().score() < c.score()) {
          result.clear();
          result.push_back(c);
        }
      }
      
      Disjunction dummyD(&*i, &*j, true);
      Disjunction d(&*i, &*j);
      
      if(dummyD.isInteresting())
        computedD ++;
      else
        prunedD ++;
      
      if(d.score() - dummyD.score() > 0.00001)
        wrongD ++;
        
      if(d.isInteresting()) {
        interestingD ++;
         
        if(result.size() == 0 || result.front().score() < d.score()) {
          result.clear();
          result.push_back(d);
        }
      }
    }
  }
  
  time(&finish);
  time_t exec_time = finish - begin;

  printf("COMBINE:: was able to prune %u conjunctions, %u disjunctions\n", prunedC, prunedD);
  printf("COMBINE:: In all, %u %u complex predicates were interesting\n", interestingC, interestingD);
  printf("COMBINE:: :( Had to compute %u %u complex predicates\n", computedC, computedD);
  printf("My Mistakes: %u %u\n", wrongC, wrongD);
  
  result.sort();
  result.reverse();

  if ( fout != NULL ) {
    // sanity check
    assert(afterCSURF == prunedC + computedC + prunedD + computedD);

    fprintf(fout, "%u %u\n", total, total - afterCSURF); // Total possible complex predicates
    fprintf(fout, "%u %u %u\n", prunedC, computedC, interestingC);
    fprintf(fout, "%u %u %u\n", prunedD, computedD, interestingD);
    
    unsigned fe = fast_estimate(candidates);
    fprintf(fout, "%u %u\n", exec_time, fe);
    
    if(result.size() > 0)
      fprintf(fout, "%lf %lf\n", result.front().score(), result.back().score());
    else
      fprintf(fout, "-1 -1\n");
  }
  return result;
}

