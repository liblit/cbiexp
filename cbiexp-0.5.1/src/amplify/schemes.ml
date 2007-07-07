module SchemeHash = Hashtbl.Make (
  struct
    type t = string
    let equal = (=)
    let hash = Hashtbl.hash 
  end
)

type schemeInfo = { numValues : int }

let table : schemeInfo SchemeHash.t = SchemeHash.create 17 

let _ = SchemeHash.add table "scalar-pairs" {numValues = 3}

let getNumValues scheme =
  let info = SchemeHash.find table scheme in
  info.numValues

