let id = ('0' | ['0' - '9']+)
let schemeCode = ['A' - 'Z']

rule initial preds = shortest
  | (schemeCode)
    " "
    (id as index)
    " "
    (id as offset)
    " " 
    (id as predicate)
    _* '\n' { 
      preds#addEntry 
        (int_of_string index) 
        (int_of_string offset)
        (int_of_string predicate); 
      initial preds lexbuf } 
  | eof { }

{

  class type preds =
    object
      method addEntry : int -> int -> int -> unit 
    end

  let read inchannel preds =
    let lexbuf = Lexing.from_channel inchannel in 
    
    let readlines () =
      initial preds lexbuf 

    in readlines ()
}
