type predicate = Predicate.p

class c =
  object
    val left : predicate list ref = ref []
    val right : predicate list ref = ref []

    method addPredicate observedTrue p =
      if observedTrue then left := p::!left else right := p::!right

    method getPredicates observedTrue =
      if observedTrue then !left else !right

  end 
