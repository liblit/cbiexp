type predicate = Predicate.p 

class type impliesRelation =
  object
    method add : predicate -> predicate -> unit
    method holds : predicate -> predicate -> bool
    method implicands : predicate -> Predicate.Set.t
  end

class impliesRelationImpl : impliesRelation

class impliesSynthRelationImpl : impliesRelation

class type isImpliedByRelation =
  object
    method add : predicate -> predicate -> unit
    method holds : predicate -> predicate -> bool
    method impliers : predicate -> Predicate.Set.t
  end

class isImpliedByRelationImpl : isImpliedByRelation 
