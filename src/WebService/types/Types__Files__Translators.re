open Relude.Globals;

module Identifier = {
  module Encode = {
    module E = Json.Encode;

    let make =
      E.(
        ({id, workspace}: Types__Files.Identifier.t) =>
          object_([("id", string(id)), ("workspace", int(workspace))])
      );
  };

  module Decode = {
    module D = Decode.AsResult.OfParseError;

    let make = json =>
      D.Pipeline.(
        succeed(Types__Files.Identifier.make)
        |> field("id", string)
        |> field("workspace", intFromNumber)
        |> run(json)
      );
  };
};
