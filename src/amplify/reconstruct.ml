type predicate = Predicate.p

module PredicateSet = Predicate.Set

class type implications =
  object
    method implicands : predicate -> PredicateSet.t
  end

class type logger =
  object
    method logImplications : predicate -> PredicateSet.t -> unit
  end

let doAnalysis (logger : logger) (impls : implications) observedTrue =
  if PredicateSet.is_empty observedTrue then PredicateSet.empty else
  let worklist = Queue.create () in
  let areTrue = ref (PredicateSet.union PredicateSet.empty observedTrue) in
  PredicateSet.iter (fun x -> Queue.push x worklist) observedTrue;

   let analyze item =
     let implicands = (impls#implicands item) in
     let added = PredicateSet.diff implicands !areTrue in 
       logger#logImplications item added;
       PredicateSet.iter (fun x -> Queue.push x worklist) added;
       areTrue := PredicateSet.union !areTrue added
   in
     
   let rec doNext () =
     try 
       let workitem = Queue.pop worklist in
       analyze workitem; doNext ()
     with Queue.Empty -> !areTrue 

  in doNext ()            
