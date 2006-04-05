class c :
  Predicate.Set.t ->
    object
      method isTrue : bool -> Predicate.p -> bool
    end
