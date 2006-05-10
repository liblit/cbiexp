let rd = new RunsDirectory.c
let nr = new NumRuns.c rd
let lr = new LogReport.c rd
let sr = new StatsReport.c 

let parsers = [ 
  (rd :> CommandLine.argParser) ; 
  (nr :> CommandLine.argParser) ; 
  (lr :> CommandLine.argParser) ;
  (sr :> CommandLine.argParser) ]

let parser = new CommandLine.parser parsers "logReader"

let doOne logFileName =
  let inchannel = open_in logFileName in
  let count = LogLexer.read inchannel in
  close_in inchannel; count

let doAll () =
  let count = ref 0 in 

  let outchannel = open_out (sr#getName ()) in

  for i = nr#getBeginRuns () to nr#getEndRuns () - 1 do
    let result = doOne (lr#format i) in
    output_string outchannel ((string_of_int result)^"\n");
    count := !count + result
  done;

  output_string outchannel ("Total: "^(string_of_int !count)^"\n");
  close_out outchannel


let main () =
  parser#parseCommandLine ();
  doAll () 

let _ =  main ()
