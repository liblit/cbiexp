#ifndef INCLUDE_sorts_h
#define INCLUDE_sorts_h

class pred_info;


////////////////////////////////////////////////////////////////////////


namespace Sort
{
  template <class Get>
  class Ascending
  {
  public:
    Ascending()
    {
    }

    bool operator () (const pred_info &a, const pred_info &b)
    {
      return get(a) < get(b);
    }

  private:
    const Get get;
  };


  template <class Get>
  class Descending
  {
  public:
    Descending()
    {
    }

    bool operator () (const pred_info &a, const pred_info &b)
    {
      return get(a) > get(b);
    }

  private:
    const Get get;
  };
}


#endif // !INCLUDE_sorts_h
