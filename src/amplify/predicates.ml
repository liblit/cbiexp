module PredicateSet = Predicate.Set

class c ( t : PredicateSet.t )=
  object
    val areTrue =  t

    (* Returns true if the curValue of the predicate is true
    or the predicate is in areTrue. If the file 
    being processed is the same as the file used for the analysis
    then it is safe to make areTrue the set of predicates that are
    changed.*)
    method isTrue curValue p = 
      if curValue then true else PredicateSet.mem p areTrue 

  end
