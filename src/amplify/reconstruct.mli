type predicate = Predicate.p

class type implications =
  object
    method implicands : predicate -> Predicate.Set.t 
  end

class type logger =
  object
    method logImplications : predicate -> Predicate.Set.t -> unit 
  end

val doAnalysis : logger -> implications -> Predicate.Set.t -> Predicate.Set.t
