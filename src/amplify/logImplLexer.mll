{
  type predicate = Predicate.p
  exception ParseFailure of string
}

let compilationUnit = ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']

let scheme = ['a'-'z' '-']+

let id = ('0' | ['0' - '9']+) 

rule initial impls = shortest 
  | "{compilationUnit = \""
      (compilationUnit as cU1)
      "\"; scheme = \""
      (scheme as scheme1)
      "\"; site = "
      (id as siteId1)
      "; id = "
      (id as id1)
      "} {compilationUnit = \""
      (compilationUnit as cU2)
      "\"; scheme = \""
      (scheme as scheme2)
      "\"; site = "
      (id as siteId2)
      "; id = "
      (id as id2)
      "}\n" { 
       impls#addImplication 
         (Predicate.make cU1 scheme1 (int_of_string siteId1) (int_of_string id1))
         (Predicate.make cU2 scheme2 (int_of_string siteId2) (int_of_string id2));
       initial impls lexbuf } 
  | eof { } 
  | _* "\n" { raise (ParseFailure "initial") }
  
{

  class type implications =
    object
      method addImplication : predicate -> predicate -> unit
      method getImplications : unit -> (predicate * predicate) list
    end

  let readImplications inchannel (impls : implications) =
    let lexbuf = Lexing.from_channel inchannel in

    let readlines () =
      initial impls lexbuf 

    in readlines ()
}
