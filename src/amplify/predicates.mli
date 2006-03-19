class c :
  Predicate.Set.t ->
    object
      method isTrue : ?curValue : bool -> Predicate.p -> bool
    end
