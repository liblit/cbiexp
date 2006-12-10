#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>

#include "../Confidence.h"

#include "Complex.h"
#include "Conjunction.h"
#include "Disjunction.h"
#include "Predicate.h"

using namespace std;

int Complex::nextIndex = 0;

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


// These members are not members of class Complex
map<int, set<int> > pairs;
void read_pairs() {
  ifstream in("pairs.txt");
  int a, b;
  
  int tmp = 0;
  while(!in.eof()) {
    in >> a >> b;
    pairs[a].insert(b);
    tmp ++;
  }
  in.close();
}

// Read the static site information
bool isValidPair(int p1, int p2) {
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
  std::list<Complex> result;
  double minMax = lb;
  
  unsigned skipped = 0;
  unsigned intr = 0;
  unsigned computed = 0;

  Candidates::iterator i, j;
  for(i = candidates.begin(); i != candidates.end(); i ++) {
    for(j = candidates.begin(); j != i; j ++) {
      
      if(! isValidPair(i->index, j->index))
        continue;
      
      bool skip_conj = false;
      if(result.size() == limit) {
        Conjunction dummy(&*i, &*j, true);
        if(dummy.score() < minMax) {
          skipped ++;
          skip_conj = true;
        }
      }
      
      if(!skip_conj) {
        computed ++;
        Conjunction c(&*i, &*j);
        if(c.isInteresting()) {
          result.push_back(c);
          intr ++;
          
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
          skipped ++;
          skip_disj = true;
        }
      }
      
      if(!skip_disj) {
        computed ++;
        Disjunction d(&*i, &*j);
        if(d.isInteresting()) {
            result.push_back(d);
            
            intr ++;
            if(result.size() > limit) {
            result.erase(min_element(result.begin(), result.end()));
            minMax = min_element(result.begin(), result.end())->score();
            }
        } 
      }
    }
  }
  
  printf("COMBINE:: was able to prune %u conjunctions\n", skipped);
  printf("COMBINE:: In all, %u complex predicates were interesting\n", intr);
  printf("COMBINE:: :( Had to compute %u complex predicates\n", computed);
  
  if ( fout != NULL ) { 
    fprintf(fout, "COMBINE:: %u complex predicates possible\n",
           ( candidates.size() * candidates.size() ) - 2 * candidates.size() );
    fprintf(fout, "COMBINE:: was able to prune %u conjunctions\n", skipped);
    fprintf(fout, "COMBINE:: In all, %u complex predicates were interesting\n", intr);
    if ( result.size() > 0 ) {
      fprintf(fout, "COMBINE:: %u complex predicates returned; scored from %f to %f\n",
      result.size(), result.front().score(), result.back().score() );
    }
    fprintf(fout, "------------------------------------------------------------\n");
  }
  return result;
}

