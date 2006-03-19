{
  type predicate = Predicate.p
  exception ParseFailure of string
  exception Eof
}

let compilationUnit = ['0' - '9' 'a' - 'f']
                      ['0' - '9' 'a' - 'f']
(*                      ['0' - '9' 'a' - 'f']
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
*)

let scheme = ['a'-'z' '-']+

let id = ('0' | ['0' - '9']+) 

rule initial impls = shortest 
  | "<implications>\n"			{ implications impls lexbuf }  
  | eof 				{ raise Eof } 
  | _* "\n" 				{ raise (ParseFailure "initial") }
and implications impls = shortest
  | "</implications>\n" 		{ initial impls lexbuf } 
  | (compilationUnit as cU1) 
      "\t" 
      (scheme as scheme1)
      "\t"
      (id as siteId1)
      "\t"
      (id as id1)
      "\t"
      (compilationUnit as cU2)
      "\t"
      (scheme as scheme2)
      "\t"
      (id as siteId2)
      "\t" 
      (id as id2)
      "\n"                              {
       impls#addImplication 
         (Predicate.make cU1 scheme1 (int_of_string siteId1) (int_of_string id1))
         (Predicate.make cU2 scheme2 (int_of_string siteId2) (int_of_string id2));
       implications impls lexbuf } 
           
  | _* "\n" 				{ raise (ParseFailure "implications") }
  
{

  class type implications =
    object
      method addImplication : predicate -> predicate -> unit
      method getImplications : unit -> (predicate * predicate) list
    end

  let readImplications inchannel (impls : implications) =
    let lexbuf = Lexing.from_channel inchannel in

    let readlines () =
      try 
        while true do 
          initial impls lexbuf 
        done 
      with
        Eof -> () 

    in readlines ()
}
