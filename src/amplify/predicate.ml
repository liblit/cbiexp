type p = { compilationUnit : string; scheme : string; site : int; id : int }

let make cU sc si i = {compilationUnit = cU; scheme = sc; site = si; id = i}

let string_of_p p = 
  p.compilationUnit^":"^
  p.scheme^":"^
  (string_of_int p.site)^":"^
  (string_of_int p.id)

module Set = Set.Make (
  struct 
    type t = p
    let compare l r = Pervasives.compare l r
  end )

module Table = Hashtbl.Make (
  struct
    type t = p

    let equal k1 k2 = 
      k1 = k2

    let hash k = Hashtbl.hash k 

  end )
