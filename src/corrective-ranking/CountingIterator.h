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
  CountingIterator(size_t &counter);

  CountingIterator &operator*();
  CountingIterator &operator++();
  CountingIterator &operator++(int);

  CountingIterator &operator=(const Event &);

private:
  size_t &counter;
};


////////////////////////////////////////////////////////////////////////


template <class Event>
CountingIterator<Event>::CountingIterator(size_t &counter)
  : counter(counter)
{
}


template <class Event>
CountingIterator<Event> &
CountingIterator<Event>::operator*()
{
  return *this;
}


template <class Event>
CountingIterator<Event> &
CountingIterator<Event>::operator++()
{
  return *this;
}


template <class Event>
CountingIterator<Event> &
CountingIterator<Event>::operator++(int)
{
  return *this;
}


template <class Event>
CountingIterator<Event> &
CountingIterator<Event>::operator=(const Event &)
{
  ++counter;
  return *this;
}


#endif // !INCLUDE_CountingIterator_h
