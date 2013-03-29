type predicate = Predicate.p 

module PredicateSet = Predicate.Set 

module PredicateTable = Predicate.Table

class c =
  object
    val table = PredicateTable.create 37
  
    method add (l : predicate) (r : predicate) = 
      try
        let rights = PredicateTable.find table l 
        in 
          let newImpls = PredicateSet.add r rights
          in PredicateTable.replace table l newImpls 
      with
        Not_found -> 
          PredicateTable.add table l (PredicateSet.singleton r)
        
    method holds (left : predicate) (right : predicate) =
      try 
        let rights = PredicateTable.find table left  
        in PredicateSet.mem right rights
      with
        Not_found -> false

    method private rights (left : predicate) =
      try
        PredicateTable.find table left 
      with
        Not_found -> PredicateSet.empty

     method private toList () =
       PredicateTable.fold 
         (fun k v l -> (k, PredicateSet.fold (fun x l -> x::l) v [])::l) 
         table 
         []

  end

class type impliesRelation =
  object
    method add : predicate -> predicate -> unit
    method holds : predicate -> predicate -> bool
    method implicands : predicate -> PredicateSet.t
  end

class type isImpliedByRelation =
  object
    method add : predicate -> predicate -> unit
    method holds : predicate -> predicate -> bool
    method impliers : predicate -> PredicateSet.t
  end

class impliesRelationImpl : impliesRelation = 
  object (self)
    inherit c 
    method implicands l = self#rights l
  end

class impliesSynthRelationImpl : impliesRelation =
  object
    inherit impliesRelationImpl as super

    method! add (l : predicate) (r : predicate) =
      let sl = Predicate.core_to_synth_pred l in
      let sr = Predicate.core_to_synth_pred r in
      super#add sl sr; 

      let (l, r) = Predicate.invert_implication (sl, sr) in 
      super#add l r

  end

class isImpliedByRelationImpl : isImpliedByRelation =
  object (self)
    inherit c
    method impliers l = self#rights l
  end
