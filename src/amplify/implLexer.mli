type predicate = Predicate.p
      
class type implications =
  object
    method addImplication : predicate -> predicate -> unit
    method getImplications : unit -> (predicate * predicate) list
  end

val readImplications : in_channel -> implications -> unit
