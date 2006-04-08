open Arg
open CommandLine

class c (rd : RunsDirectory.c) =
  object(self)
    val beginRuns = new box 0 
    val endRuns = new box 0 
    val runsDirectory = rd
    method usage_msg () = "[-begin-runs <begin index>] [-end-runs <end index>]"

    method argActions () =
      [("-begin-runs", Int self#setBeginRuns, "begin index");
       ("-end-runs", Int self#setEndRuns, "end index")]

    method setBeginRuns num =
      beginRuns#set num

    method getBeginRuns () =
      beginRuns# get ()

    method setEndRuns num =
      if num <= 0 
        then raise (Bad "end index must be greater than 0")  
        else endRuns#set num

    method getEndRuns () =
      endRuns#get ()

    method cleanup () =
      if (not (endRuns#isSet ())) then
      let stampFileName = rd#getRoot ()^"/stamp-labels" in
      let inchannel = open_in stampFileName in
      let count = int_of_string (input_line inchannel) in
      if count <= endRuns#get () 
        then raise (Bad ("no runs to read (begin "^
                         (string_of_int (beginRuns#get ()))^
                         " >= end "^
                         (string_of_int (endRuns#get ()))))
        else endRuns#set count 


  end
