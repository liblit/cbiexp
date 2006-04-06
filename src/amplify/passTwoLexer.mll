{
  type predicate = Predicate.p
  exception ParseFailure of string
  exception Eof

  class type predicates =
    object
      method isTrue : bool -> predicate -> bool
    end

  let mkLine (preds : predicates) (v0,p0) (v1,p1) (v2,p2) =
    (if preds#isTrue v0 p0 then "1\t" else "0\t")^ 
    (if preds#isTrue v1 p1 then "1\t" else "0\t")^
    (if preds#isTrue v2 p2 then "1\n" else "0\n")
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

let nonZeroCount = ['1' - '9'] ['0' - '9']* 

rule initial preds outchannel = shortest 
  | ("<report id=\"samples\">\n") as lxm	{ 
      output_string outchannel lxm;
      report preds outchannel lexbuf }  
  | ("<report id=\"" _* "\">\n") as lxm  	{ 
      output_string outchannel lxm;
      report_skip preds outchannel lexbuf }
  | (eof) as lxm			{ 
      output_string outchannel lxm; 
      raise Eof }
  | (_* '\n') as lxm  				{ raise (ParseFailure "initial") }
and report preds outchannel = shortest
  | ("</report>\n") as lxm			{ 
      output_string outchannel lxm; 
      initial preds outchannel lexbuf } 
  | ("<samples unit=\"" 
      (compilationUnit as cU) 
      "\" scheme=\"scalar-pairs\">\n") as lxm	{ 
       output_string outchannel lxm;
       scalar_pairs preds cU 0 outchannel lexbuf }
  | ("<samples unit=\""
      compilationUnit
      "\" scheme=\""
      ['a'-'z' '-']+ 
      "\">\n") as lxm			{ 
      output_string outchannel lxm;
      sites_skip preds outchannel lexbuf }
  | _* '\n'				{ raise (ParseFailure "report") }
and report_skip preds outchannel = shortest 
  | ("</report>\n") as lxm		{ 
      output_string outchannel lxm; 
      initial preds outchannel lexbuf }
  | (_* '\n') as lxm			{ 
      output_string outchannel lxm;
      report_skip preds outchannel lexbuf }
and scalar_pairs preds cU siteId outchannel = shortest
  | ("</samples>\n") as lxm		{ 
      output_string outchannel lxm;
      report preds outchannel lexbuf }
  | '0' '\t' '0' '\t' '0' '\n'          { 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      output_string outchannel 
        (mkLine preds (false, (mkP 0)) (false, (mkP 1)) (false, (mkP 2)));
      scalar_pairs preds cU (siteId + 1) outchannel lexbuf } 
  | '0' '\t' '0' '\t' nonZeroCount '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      output_string outchannel 
        (mkLine preds (false, (mkP 0)) (false, (mkP 1)) (true, (mkP 2)));
      scalar_pairs preds cU (siteId + 1) outchannel lexbuf }
  | '0' '\t' nonZeroCount '\t' '0' '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      output_string outchannel 
        (mkLine preds (false, (mkP 0)) (true, (mkP 1)) (false, (mkP 2)));
      scalar_pairs preds cU (siteId + 1) outchannel lexbuf } 
  | '0' '\t' nonZeroCount '\t' nonZeroCount '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      output_string outchannel 
        (mkLine preds (false, (mkP 0)) (true, (mkP 1)) (true, (mkP 2)));
      scalar_pairs preds cU (siteId + 1) outchannel lexbuf } 
  | nonZeroCount '\t' '0' '\t' '0' '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      output_string outchannel 
        (mkLine preds (true, (mkP 0)) (false, (mkP 1)) (false, (mkP 2)));
      scalar_pairs preds cU (siteId + 1) outchannel lexbuf } 
  | nonZeroCount '\t' '0' '\t' nonZeroCount '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      output_string outchannel 
        (mkLine preds (true, (mkP 0)) (false, (mkP 1)) (true, (mkP 2)));
      scalar_pairs preds cU (siteId + 1) outchannel lexbuf } 
  | nonZeroCount '\t' nonZeroCount '\t' '0' '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      output_string outchannel 
        (mkLine preds (true, (mkP 0)) (true, (mkP 1)) (false, (mkP 2)));
      scalar_pairs preds cU (siteId + 1) outchannel lexbuf } 
  | nonZeroCount '\t' nonZeroCount '\t' nonZeroCount '\n'		{ 
      let mkP = Predicate.make cU "scalar-pairs" siteId in
      output_string outchannel 
        (mkLine preds (true, (mkP 0)) (true, (mkP 1)) (true, (mkP 2)));
      scalar_pairs preds cU (siteId + 1) outchannel lexbuf } 
and sites_skip preds outchannel = shortest
  | ("</samples>\n") as lxm		{ 
      output_string outchannel lxm;
      report preds outchannel lexbuf }
  | (_* '\n') as lxm			{ 
      output_string outchannel lxm; 
      sites_skip preds outchannel lexbuf } 

{
  let updatePredicates inchannel outchannel (preds : predicates) =
    let lexbuf = Lexing.from_channel inchannel in

    let updatelines () =
      try 
        while true do
          initial preds outchannel lexbuf 
        done
      with
        Eof -> ()

    in updatelines ()
}
