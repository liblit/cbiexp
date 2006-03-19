module PredicateSet = Predicate.Set

class c ( t : PredicateSet.t )=
  object
    val areTrue =  t

    method isTrue ?(curValue = false) p = 
      if curValue then true else PredicateSet.mem p areTrue 

  end
