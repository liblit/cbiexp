type predicate = Predicate.p

class type implications =
  object
    method holds : predicate -> predicate -> bool
  end

val doAnalysis : implications -> Predicate.Set.t -> Predicate.Set.t -> Predicate.Set.t
