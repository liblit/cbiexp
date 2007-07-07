{
  exception ParseFailure of string
  exception Eof
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

rule initial sites = shortest
  | "<sites unit=\""
      (compilationUnit as cU)
      "\" scheme=\""
      (scheme as sch)
      "\">\n"  {sites#addSiteInfo cU sch; skip_sites sites lexbuf}
  | eof        {raise Eof}     
  | _* '\n'    {raise (ParseFailure "initial")} 
and skip_sites l = shortest
  | "</sites>\n"   {initial l lexbuf}
  | _* '\n'        {skip_sites l lexbuf} 

{
  class type sites =
    object
      method addSiteInfo : string -> string -> unit
    end
  
  let readSites inchannel sites =
    let lexbuf = Lexing.from_channel inchannel in

    let readlines () =
      try
        initial sites lexbuf
      with
        Eof -> ()

    in readlines ()
}
