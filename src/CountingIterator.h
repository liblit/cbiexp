#ifndef INCLUDE_CountingIterator_h
#define INCLUDE_CountingIterator_h

#include <iterator>


////////////////////////////////////////////////////////////////////////
//
//  An iterator which counts and discards anything assigned into it
//
//  Example use: to compute the size of the intersection of two sets
//  without materializing the intersetion set itself, pass a counting
//  iterator as the result argument to std::set_intersection().
//


template <class Event>
class CountingIterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
  CountingIterator(size_t &count)
    : count(count)
  {
  }

  CountingIterator &operator*()
  {
    return *this;
  }

  CountingIterator &operator++()
  {
    return *this;
  }

  CountingIterator &operator++(int)
  {
    return *this;
  }

  CountingIterator &operator=(const Event &)
  {
    ++count;
    return *this;
  }

private:
  size_t &count;
};


#endif // !INCLUDE_CountingIterator_h
