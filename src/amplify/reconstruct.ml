type predicate = Predicate.p

module PredicateSet = Predicate.Set

class type implications =
  object
    method implicands : predicate -> PredicateSet.t -> PredicateSet.t
  end

class type logger =
  object
    method logImplications : predicate -> PredicateSet.t -> unit
  end

let doAnalysis logger impls observedTrue notObservedTrue =
  if PredicateSet.is_empty observedTrue then observedTrue else
  if PredicateSet.is_empty notObservedTrue then observedTrue else
  let deduced = ref PredicateSet.empty in
  let worklist = Queue.create () in
  let others = ref (PredicateSet.union notObservedTrue PredicateSet.empty) in
  PredicateSet.iter (fun x -> Queue.push x worklist) observedTrue;

   let analyze item =
     let implicands = (impls#implicands item !others)
     in
       logger#logImplications item implicands;
       PredicateSet.iter (fun x -> Queue.push x worklist) implicands;
       others := PredicateSet.diff !others implicands;
       deduced := PredicateSet.union implicands !deduced
   in
     
   let rec doNext () =
     if PredicateSet.cardinal !others = 0 
     then PredicateSet.union observedTrue !deduced 
     else
       try 
         let workitem = Queue.pop worklist in
         analyze workitem; doNext ()
       with Queue.Empty -> 
         PredicateSet.union observedTrue !deduced 
  in doNext ()            
