type predicate = Predicate.p

class type implications =
  object
    method implicands : predicate -> Predicate.Set.t 
  end

val doAnalysis : implications -> Predicate.Set.t -> Predicate.Set.t
