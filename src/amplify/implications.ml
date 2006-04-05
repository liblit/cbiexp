type predicate = Predicate.p 

module PredicateSet = Predicate.Set 

module PredicateTable = Predicate.Table

class c =
  object (self)
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

     method toList () =
       PredicateTable.fold 
         (fun k v l -> (k, PredicateSet.fold (fun x l -> x::l) v [])::l) 
         table 
         []

  end
