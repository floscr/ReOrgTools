open Relude.Globals;

module R =
  Decode.ParseError.ResultOf({
    type t = [ Decode.ParseError.base | `InvalidFilter];
    let handle = x => (x :> t);
  });

module D = Decode.Base.Make(R.TransformError, R);

let failureToPartialString =
  fun
  | `ExpectedBoolean => "Expected boolean"
  | `ExpectedString => "Expected string"
  | `ExpectedNumber => "Expected number"
  | `ExpectedInt => "Expected int"
  | `ExpectedArray => "Expected array"
  | `ExpectedTuple(size) => "Expected tuple of size " ++ Int.toString(size)
  | `ExpectedObject => "Expected object"
  | `ExpectedValidDate => "Expected a valid date"
  | `ExpectedValidOption => "Expected a valid option"
  | `InvalidFilter => "Invalid Filter parsing";

let failureToString = (v, json) =>
  failureToPartialString(v) ++ " but found " ++ Js.Json.stringify(json);

let failureToDebugString = err =>
  Decode_ParseError.toDebugString(failureToString, err);
