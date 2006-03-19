type predicate = Predicate.p

module PredicateSet = Predicate.Set

class type implications =
  object
    method holds : predicate -> predicate -> bool
  end

let doAnalysis impls observedTrue notObservedTrue =
  if PredicateSet.is_empty observedTrue then observedTrue else
  if PredicateSet.is_empty notObservedTrue then observedTrue else
  let deduced = ref [] in
  let worklist = Queue.create () in
  let others = ref (PredicateSet.elements notObservedTrue) in
  PredicateSet.iter (fun x -> Queue.push x worklist) observedTrue;

   let analyze item =
     let (implied, notImplied) =
       List.partition (fun x -> impls#holds item x) !others
     in
       List.iter (fun x -> Queue.push x worklist) implied;
       others := notImplied;
       deduced := List.rev_append implied !deduced
   in
     
   let rec doNext () =
     if List.length !others = 0 
     then List.fold_left (fun s x -> PredicateSet.add x s) observedTrue !deduced 
     else
       try 
         let workitem = Queue.pop worklist in
         analyze workitem; doNext ()
       with Queue.Empty -> 
         List.fold_left (fun s x -> PredicateSet.add x s) observedTrue !deduced 
  in doNext ()            
