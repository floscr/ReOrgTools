open Relude.Globals;

module Agendas = {
  module Encode = {
    module E = Json.Encode;

    let make =
      E.(
        ({id, files, fields, timerange}: Types__Agendas.Agenda.t) =>
          object_([
            ("id", string(id)),
            (
              "files",
              files
              |> Array.map(Types__Files__Translators.Identifier.Encode.make)
              |> jsonArray,
            ),
          ])
      );
  };

  module Decode = {
    module D = Decode.AsResult.OfParseError;

    let make = json =>
      D.Pipeline.(
        succeed(Types__Agendas.Agenda.make)
        |> field("id", string)
        |> field(
             "files",
             array(Types__Files__Translators.Identifier.Decode.make),
           )
        /* |> field("fields", array(tuple2(string, string))) */
        /* |> optionalField("timerange", decodeAgendaTimerangeJson) */
        /* |> optionalField("tags", array(decodeFilter)) */
        /* |> optionalField("todos", array(decodeFilter)) */
        /* |> optionalField("reverse", boolean) */
        |> run(json)
      );
  };
};
